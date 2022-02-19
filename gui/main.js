const wsConnection = new WebSocket('ws://localhost:4000')

wsConnection.addEventListener('open', (wsEvent) => {
  wsConnection.send(JSON.stringify({ msg: 'Basic html client connected' }))
  wsConnection.send(JSON.stringify({ msg: 'Test data from client', left: 4050, right: 11150, top: 2278, bottom: 6272 }))
})

wsConnection.addEventListener('message', (wsEvent) => {
  console.log('Data:', wsEvent.data)
})
