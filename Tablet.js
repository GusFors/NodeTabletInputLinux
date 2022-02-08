const HID = require('node-hid')
const robot = require('robotjs')
const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./ConfigHandler')
const deviceDetector = new DeviceDetector()
const Display = require('./build/Release/display.node')
const { standardBufferParser, proBufferParser } = require('./Parser')

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

    // can also remove the delay by manually removing sleep events in the robotjs modules c/c++ files and then rebuilding with node-gyp
    robot.setMouseDelay(0)
    robot.setKeyboardDelay(0)

    console.log('Total X screen width: ' + this.monitorConfig.width)
    console.log('Number of active monitors: ' + this.monitorConfig.numOfMonitors)
    console.log('Assumed primary monitor xOffset: ' + this.monitorConfig.xOffset)
    console.log('Assumed primary monitor yOffset: ' + this.monitorConfig.yOffset)
    console.log('Assumed primary monitor width: ' + this.monitorConfig.width)

    if (this.settings.name !== 'Wacom PTH-460') {
      this.tabletHID.on('data', (reportBuffer) => {
        standardBufferParser(reportBuffer, this)
      })
    } else {
      this.tabletHID.on('data', (reportBuffer) => {
        proBufferParser(reportBuffer, this)
      })
    }
    return 1
  }

  averagePosition(bufferArray) {
    let sum = 0
    for (let i = 0; i < bufferArray.length; i++) {
      sum += bufferArray[i]
    }
    return sum / bufferArray.length
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

  async tabletInput(isRestart) {
    if (isRestart && this.tabletHID !== null) {
      this.tabletHID.pause()
      this.tabletHID = null
    }

    this.tabletHID = new HID.HID(await deviceDetector.awaitPath())
    this.settings = await deviceDetector.getConfig()

    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    robot.setMouseDelay(0)
    robot.setKeyboardDelay(0)

    let intervalData = []
    let x
    let y
    let xS
    let yS
    let isClickHold = false

    const xBuffer = []
    const yBuffer = []

    this.tabletHID.on('data', (reportBuffer) => {
      intervalData[0] = reportBuffer

      if (reportBuffer[0] != 2) {
        return
      }

      if (reportBuffer[9] === 0) {
        return false
      }

      x = reportBuffer[3] | (reportBuffer[4] << 8)
      y = reportBuffer[5] | (reportBuffer[6] << 8)

      xS = (x - this.settings.left) * this.xScale
      yS = (y - this.settings.top) * this.yScale

      if (xS > 2560) {
        xS = 2560
      }

      if (xS < 0) {
        xS = 0
      }

      if (yS > 1440) {
        yS = 1440
      }

      if (yS < 0) {
        yS = 0
      }

      // add offset to xS since in this case main monitor is not the leftmost monitor
      Pointer.setPointer(xS + 2560, yS)

      // pressure
      if (reportBuffer[7] > 0) {
        if (isClickHold === false) {
          robot.mouseToggle('down', 'left')
          isClickHold = true
        }
      }

      if (reportBuffer[7] === 0) {
        isClickHold = false
        robot.mouseToggle('up', 'left')
      }

      x === 0 && y === 0 ? false : robot.moveMouse(this.averagePosition(xBuffer.slice(-5, -1)), this.averagePosition(yBuffer.slice(-5, -1)))
      setTimeout(() => {
        xBuffer.shift()
      }, 1000)
    })
    return intervalData
  }
}

module.exports = Tablet
