const HID = require('node-hid')
const Tablet = require('./Tablet')

let isForcedProportions = process.argv.includes('-f')
let isFastLogging = process.argv.includes('-l')
let isDraftLog = process.argv.includes('-d')
let isExit = process.argv.includes('-t')

if (process.argv.includes('-de')) {
  let devices = HID.devices()
  console.log(devices)
  process.exit()
}

;(async () => {
  const DetectedTablet = new Tablet()
  DetectedTablet.simpleTabletInput()
})()
