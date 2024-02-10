// if (isWebSocket) {
//   const { spawn } = require('child_process')
//   const WebSocketServer = require('ws').WebSocketServer
//   const wss = new WebSocketServer({ port: 4000 })
//   console.log('starting ws server..')
//   spawn('xdg-open', [__dirname + '/gui/index.html'])
//   wss.on('connection', (ws) => {
//     ws.send(JSON.stringify({ msg: 'init from console', settings: DetectedTablet.settings }))
//     ws.on('message', (clientData) => {
//       let data = JSON.parse(clientData)
//       console.log(data)
//       if (data.id === 'applyArea') {
//         DetectedTablet.settings.top = data.top
//         DetectedTablet.settings.bottom = data.bottom
//         DetectedTablet.settings.left = data.left
//         DetectedTablet.settings.right = data.right
//         DetectedTablet.updateScale()
//         console.log(DetectedTablet.settings)
//       }
//     })
//   })
// }
