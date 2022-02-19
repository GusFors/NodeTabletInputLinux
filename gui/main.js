const wsConnection = new WebSocket('ws://localhost:4000')
const topInput = document.querySelector('#top')
const bottomInput = document.querySelector('#bottom')
const rightInput = document.querySelector('#right')
const leftInput = document.querySelector('#left')
const model = document.querySelector('#model')

wsConnection.addEventListener('open', (wsEvent) => {
  wsConnection.send(JSON.stringify({ msg: 'Basic html client connected' }))
  wsConnection.send(JSON.stringify({ msg: 'Test data from client', left: 4050, right: 11150, top: 2278, bottom: 6272 }))
})

wsConnection.addEventListener('message', (wsEvent) => {
  const data = JSON.parse(wsEvent.data)
  console.log('Data:', data)

  if (data.settings) {
    topInput.value = data.settings.top
    bottomInput.value = data.settings.bottom
    rightInput.value = data.settings.right
    leftInput.value = data.settings.left
    model.innerText += ' ' + data.settings.name
  }
})

document.querySelector('#apply').onclick = (event) => {
  wsConnection.send(
    JSON.stringify({
      id: 'applyArea',
      top: parseInt(topInput.value),
      bottom: parseInt(bottomInput.value),
      left: parseInt(leftInput.value),
      right: parseInt(rightInput.value),
    })
  )
}
