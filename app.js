let isExit = process.argv.includes('-e')
let isAvg = process.argv.includes('-s')
let isAutomaticRestart = process.argv.includes('-r')
let isDoubleReport = process.argv.includes('-d')
let isVirtualDevice = process.argv.includes('-v')
let isListDevices = process.argv.includes('-de')
let isTouch = process.argv.includes('-t')
let isPressure = process.argv.includes('-p')
let isNative = process.argv.includes('-n')

const DeviceDetector = require('./DeviceDetector')
const Tablet = require('./Tablet')

if (isListDevices) {
  ;(async () => {
    const d = new DeviceDetector()
    let devices = await d.getTabletHidInfo(true)
  })()
}

let isRunning = false
const run = async () => {
  const DetectedTablet = new Tablet()
  DetectedTablet.simpleTabletInput({ isVirtual: isVirtualDevice, isTouch, isPressure, isNative, isDoubleReport }, isExit)
  isRunning = true
}
run()

let restartInterval

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
