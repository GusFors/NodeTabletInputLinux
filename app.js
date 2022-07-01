let isExit = process.argv.includes('-e')
let isAvg = process.argv.includes('-s')
let isWebSocket = process.argv.includes('-w')
let isAutomaticRestart = process.argv.includes('-r')
let isDoubleReport = process.argv.includes('-d')
let isVirtualDevice = process.argv.includes('-v')
let isNewConfig = process.argv.includes('-c')
let isListDevices = process.argv.includes('-de')
let isTouch = process.argv.includes('-t')

if (isListDevices) {
  const HID = require('node-hid')
  let devices = HID.devices()

  for (let i = 0; i < devices.length; i++) {
    if (devices[i].vendorId === 1386) {
      console.log(devices[i])
    }
  }

  process.exit()
}

if (isExit) {
  setTimeout(() => {
    process.exit()
  }, 10000)
}

const Tablet = require('./Tablet')
// console.log(process.pid)

let isRunning = false

const run = async () => {
  const DetectedTablet = new Tablet()

  if (isAvg) {
    DetectedTablet.simpleTabletInput({ isVirtual: isVirtualDevice, isAvg: true, isDoubleReport: true, isTouch })
    console.log('Using avg position')
  } else if (isDoubleReport) {
    DetectedTablet.simpleTabletInput({ isDoubleReport: true, isVirtual: isVirtualDevice, isTouch })
    console.log('Using double report position')
  } else {
    DetectedTablet.simpleTabletInput({ isVirtual: isVirtualDevice, isNewConfig, isTouch })
    console.log('Using raw position')
  }

  isRunning = true

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
// buggy, doesn't properly close unused tablets
process.on('uncaughtException', function (error) {
  console.log('Crashed with error: ', error.message)
  isRunning = false

  console.log(error)

  if (isAutomaticRestart) {
    restartInterval = setInterval(() => {
      if (!isRunning) {
        console.log('trying to restart.. \n')
        run()
        clearInterval(restartInterval)
      }
    }, 1000)
  }
})
