const HID = require('node-hid')
const Tablet = require('./Tablet')

let isForcedProportions = process.argv.includes('-f')
let isFastLogging = process.argv.includes('-l')
let isDraftLog = process.argv.includes('-d')
let isExit = process.argv.includes('-t')
let isAvg = process.argv.includes('-s')

if (process.argv.includes('-de')) {
  let devices = HID.devices()
  console.log(devices)
  process.exit()
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
})()
