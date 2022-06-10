// let isForcedProportions = process.argv.includes('-f')
// let isFastLogging = process.argv.includes('-l')
// let isDraftLog = process.argv.includes('-d')
let isExit = process.argv.includes('-t')
let isAvg = process.argv.includes('-s')
let isWebSocket = process.argv.includes('-w')
let isAutomaticRestart = process.argv.includes('-r')
let isDoubleReport = process.argv.includes('-d')
let isVirtualDevice = process.argv.includes('-v')

const Tablet = require('./Tablet')

if (process.argv.includes('-de')) {
  const HID = require('node-hid')
  let devices = HID.devices()
  console.log(devices)
  process.exit()
}

if (isExit) {
  setTimeout(() => {
    process.exit()
  }, 10000)
}

let isRunning = false

const run = async () => {
  const DetectedTablet = new Tablet()

  if (isAvg) {
    DetectedTablet.tabletInput(false, { isVirtual: isVirtualDevice })
    console.log('Using avg position')
  } else if (isDoubleReport) {
    DetectedTablet.simpleTabletInput(false, { isDoubleReport: true, isVirtual: isVirtualDevice })
    console.log('Using double report position')
  } else {
    DetectedTablet.simpleTabletInput(false, { isVirtual: isVirtualDevice })
    console.log('Using raw position')
  }

  isRunning = true

  // start in forked process?, alternatively start gtk instead
  if (isWebSocket) {
    const { spawn } = require('child_process')
    const WebSocketServer = require('ws').WebSocketServer
    const wss = new WebSocketServer({ port: 4000 })
    console.log('starting ws server..')

    spawn('xdg-open', [__dirname + '/gui/index.html'])

    wss.on('connection', (ws) => {
      ws.send(JSON.stringify({ msg: 'init from console', settings: DetectedTablet.settings }))

      ws.on('message', (clientData) => {
        let data = JSON.parse(clientData)
        console.log(data)

        if (data.id === 'applyArea') {
          DetectedTablet.settings.top = data.top
          DetectedTablet.settings.bottom = data.bottom
          DetectedTablet.settings.left = data.left
          DetectedTablet.settings.right = data.right

          DetectedTablet.updateScale()
          console.log(DetectedTablet.settings)
        }
      })
    })
  }
}
run()

let restartInterval
// a bit buggy, TODO close virtual device on error
process.on('uncaughtException', function (error) {
  console.log('Crashed with error: ', error.message)
  isRunning = false

  if (isAutomaticRestart) {
    restartInterval = setInterval(() => {
      if (!isRunning) {
        console.log('trying to restart.. \n')
        run()
        clearInterval(restartInterval)
      }
    }, 1000)
  }
  // setInterval(() => {
  //   run()
  // }, 1000)
})
