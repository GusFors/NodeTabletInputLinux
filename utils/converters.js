const { getArgValue } = require('./args')

// example: node utils/converters.js -x 76 -y 36.5625 -w 100 -h 56.25 -c 100 -m
// -c 200 for PTH tablets
function mmToWac(config, isLog = false) {
  let convObj = {}

  let x = config ? config.xOffset : getArgValue('-x', 'number')
  let y = config ? config.yOffset : getArgValue('-y', 'number')
  let width = config ? config.width : getArgValue('-w', 'number')
  let height = config ? config.height : getArgValue('-h', 'number')

  let c = config ? config.convM : getArgValue('-c', 'number', 100)

  let left = (x - width / 2) * c
  let right = (x + width / 2) * c
  let top = (y - height / 2) * c
  let bottom = (y + height / 2) * c

  convObj.left = left
  convObj.right = right
  convObj.top = top
  convObj.bottom = bottom

  if (isLog) {
    console.log('"left": ' + left + ',')
    console.log('"right": ' + right + ',')
    console.log('"top": ' + top + ',')
    console.log('"bottom": ' + bottom + ',')
  }

  return { ...config, ...convObj }
}

// example: node utils/converters.js -l 2600 -r 12600 -t 843.75 -b 6468.75 -c 100
// -c 200 for PTH tablets
function wacToMm(config, isLog = false) {
  let left = config ? config.left : getArgValue('-l', 'number')
  let right = config ? config.right : getArgValue('-r', 'number')
  let top = config ? config.top : getArgValue('-t', 'number')
  let bottom = config ? config.bottom : getArgValue('-b', 'number')
  let width = right - left
  let height = bottom - top

  let c = config ? config.convM : getArgValue('-c', 'number', 100)

  let x = (left + width / 2) / c
  let y = (top + height / 2) / c
  width = width / c
  height = height / c

  if (isLog) {
    console.log('"width": ' + width + ',')
    console.log('"height": ' + height + ',')
    console.log('"x": ' + x + ',')
    console.log('"y": ' + y + ',')
  }
  return
}

if (require.main === module) {
  if (getArgValue('-m', 'boolean')) {
    mmToWac(null, true)
  } else {
    wacToMm(null, true)
  }
}

module.exports = { mmToWac }
