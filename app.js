const HID = require('node-hid')
const Tablet = require('./Tablet')
const { spawn } = require('child_process')

let isForcedProportions = process.argv.includes('-f')
let isFastLogging = process.argv.includes('-l')
let isDraftLog = process.argv.includes('-d')
let isExit = process.argv.includes('-t')
let isAvg = process.argv.includes('-s')
let isWebSocket = process.argv.includes('-w')

if (process.argv.includes('-de')) {
  let devices = HID.devices()
  console.log(devices)
  process.exit()
}

if (isExit) {
  setTimeout(() => {
    process.exit()
  }, 10000)
}

;(async () => {
  const DetectedTablet = new Tablet()

  if (isAvg) {
    DetectedTablet.tabletInput()
    console.log('using avg position')
  } else {
    DetectedTablet.simpleTabletInput()
    console.log('using raw position')
  }

  // start in forked process?
  if (isWebSocket) {
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
})()
