const HID = require('node-hid')
const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./configs/ConfigHandler')
const deviceDetector = new DeviceDetector()
const Display = require('./build/Release/display.node')
const { standardBufferParser, proBufferParser, standardAvgBufferParser, initPointer } = require('./Parser')

class Tablet {
  constructor() {
    this.tabletHID = null
    this.settings = null
    this.xScale = null
    this.yScale = null
    this.monitorConfig = {
      height: Display.getPrimaryMonitorHeight(),
      width: Display.getPrimaryMonitorWidth(),
      xOffset: Display.getPrimaryMonitorXoffset(),
      yOffset: Display.getPrimaryMonitorYoffset(),
      numOfMonitors: Display.getNumberOfMonitors(),
    }
  }

  async simpleTabletInput(isRestart) {
    if (isRestart && this.tabletHID !== null) {
      this.tabletHID.pause()
      this.tabletHID = null
    }

    this.tabletHID = new HID.HID(await deviceDetector.getPath())
    this.settings = await deviceDetector.getConfig()
    console.log('Getting input from', this.settings.name)

    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    console.log('Total X screen width: ' + this.monitorConfig.width)
    console.log('Number of active monitors: ' + this.monitorConfig.numOfMonitors)
    console.log('Assumed primary monitor xOffset: ' + this.monitorConfig.xOffset)
    console.log('Assumed primary monitor yOffset: ' + this.monitorConfig.yOffset)
    console.log('Assumed primary monitor width: ' + this.monitorConfig.width)

    // init the X display and pointer
    initPointer()

    // TODO read directly from hidraw instead of node-hid?
    // const fs = require('fs')
    // let rawHidPath = fs.createReadStream('/dev/hidraw6')
    // rawHidPath.on('data', (chunk) => {
    //   standardBufferParser(chunk, this)
    //   // console.log(chunk)
    // })

    if (this.settings.name !== 'Wacom PTH-460') {
      this.tabletHID.on('data', (reportBuffer) => {
        standardBufferParser(reportBuffer, this)
        // rps++
      })
    } else {
      this.tabletHID.on('data', (reportBuffer) => {
        proBufferParser(reportBuffer, this)
        // rps++
      })
    }
    return 1
  }

  closeTablet() {
    this.tabletHID.pause()
    this.tabletHID = null
  }

  updateScale() {
    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)
  }

  saveSettings() {
    new ConfigHandler().writeConfigSync(this.settings)
  }

  // TODO
  async tabletInput(isRestart) {
    if (isRestart && this.tabletHID !== null) {
      this.tabletHID.pause()
      this.tabletHID = null
    }

    this.tabletHID = new HID.HID(await deviceDetector.getPath())
    this.settings = await deviceDetector.getConfig()
    console.log('Getting input from', this.settings.name)

    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    console.log('Total X screen width: ' + this.monitorConfig.width)
    console.log('Number of active monitors: ' + this.monitorConfig.numOfMonitors)
    console.log('Assumed primary monitor xOffset: ' + this.monitorConfig.xOffset)
    console.log('Assumed primary monitor yOffset: ' + this.monitorConfig.yOffset)
    console.log('Assumed primary monitor width: ' + this.monitorConfig.width)

    initPointer()
    this.tabletHID.on('data', (reportBuffer) => {
      standardAvgBufferParser(reportBuffer, this)
    })

    return 1
  }
}

module.exports = Tablet

// setInterval(() => {
//   this.tabletHID.read((err, reportBuffer) => {
//     standardBufferParser(reportBuffer, this)
//     //  console.log(data)
//     rps++
//   })
// }, 1)

// let execTimes = []
//     const t0 = performance.now()
//     standardBufferParser(reportBuffer, this)
//     const t1 = performance.now()
//     execTimes.push(t1 - t0)
//     setInterval(() => {
//       let sum = 0
//       for (let i = 0; i < execTimes.length; i++) {
//         sum += execTimes[i]
//       }
//       console.log(sum / execTimes.length)
//       execTimes = []
//     }, 5000)

// let rps = 0
// setInterval(() => {
//   console.log(rps)
//   rps = 0
// }, 1000)

// console.log(this.tabletHID.setNonBlocking(1))
