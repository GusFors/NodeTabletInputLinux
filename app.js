let isExit = process.argv.includes('-e')
let isAutomaticRestart = process.argv.includes('-r')
let isDoubleReport = process.argv.includes('-d')
let isVirtualDevice = process.argv.includes('-v')
let isListDevices = process.argv.includes('-de')
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
  DetectedTablet.simpleTabletInput({ isVirtual: isVirtualDevice, isNative, isDoubleReport }, isExit)
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
