let isExit = process.argv.includes('-e')
let isAvg = process.argv.includes('-s')
let isWebSocket = process.argv.includes('-w')
let isAutomaticRestart = process.argv.includes('-r')
let isDoubleReport = process.argv.includes('-d')
let isVirtualDevice = process.argv.includes('-v')
let isNewConfig = process.argv.includes('-c')
let isListDevices = process.argv.includes('-de')
let isTouch = process.argv.includes('-t')
let isPressure = process.argv.includes('-p')
let isNative = process.argv.includes('-n')

const DeviceDetector = require('./DeviceDetector')
const Tablet = require('./Tablet')

if (isListDevices) {
  ;(async () => {
    const d = new DeviceDetector()
    let devices = await d.getHidInfo()

    for (let i = 0; i < devices.length; i++) {
      console.log(devices[i])
    }
    process.exit()
  })()
}

let isRunning = false
const run = async () => {
  const DetectedTablet = new Tablet()
  DetectedTablet.simpleTabletInput({ isVirtual: isVirtualDevice, isNewConfig, isTouch, isPressure, isNative, isDoubleReport })
  isRunning = true
}
run()

if (isExit) {
  setTimeout(() => {
    process.exit()
  }, 10000)
}
let restartInterval
// might not properly close unused tablets
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
