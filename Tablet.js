const HID = require('node-hid')
const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./configs/ConfigHandler')
const deviceDetector = new DeviceDetector('/mmConfigs.json')
const Display = require('./build/Release/display.node')
const { mmToWac } = require('./utils/converters')
const {
  standardBufferParser,
  doubleReportBufferParser,
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  Pointer,
  touchBufferParser,
} = require('./Parsers')

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
      xTotalWidth: Display.getDisplaysTotalWidth(),
      xTotalHeight: Display.getDisplaysTotalHeight(),
    }
    this.parser = null
  }

  async simpleTabletInput(parserSettings = { isDoubleReport: false, isAvg: false, isVirtual: false, isNewConfig: false, isTouch: false }) {
    console.log(parserSettings)

    this.tabletHID = new HID.HID(await deviceDetector.getPath())
    this.settings = mmToWac(await deviceDetector.getConfig())
    console.log('Getting input from', this.settings.name)

    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    console.log('Total X screen width: ' + this.monitorConfig.xTotalWidth)
    console.log('Total X screen height: ' + this.monitorConfig.xTotalHeight)
    console.log('Number of active monitors: ' + this.monitorConfig.numOfMonitors)
    console.log('Assumed primary monitor xOffset: ' + this.monitorConfig.xOffset)
    console.log('Assumed primary monitor yOffset: ' + this.monitorConfig.yOffset)
    console.log('Assumed primary monitor width: ' + this.monitorConfig.width + '\n')

    console.log(this.settings)

    // init the pointer and display before setting pointer positions and clicks
    initXPointer() // optionally run when clicks by uinput are implemented

    if (parserSettings.isVirtual) {
      let uiDevice = initUinput(await this.settings.name, this.monitorConfig.xTotalWidth, this.monitorConfig.xTotalHeight)
      console.log('Created uinput device:', uiDevice)
      Pointer.setPointerPosition = Pointer.setPointerPosition = Pointer['setUinputPointer']
    } else {
      Pointer.setPointerPosition = Pointer.setPointerPosition = Pointer['setPointer']
    }

    if (parserSettings.isAvg) {
      console.log('Using standardAvgBufferParser')
      this.parser = standardAvgBufferParser.bind(this)
      this.tabletHID.on('data', this.parser)
    } else if (parserSettings.isDoubleReport) {
      console.log('Using doubleReportBufferParser')
      this.parser = doubleReportBufferParser.bind(this)
      this.tabletHID.on('data', this.parser)
    } else if (parserSettings.isNewConfig) {
      console.log('Using newConfig')
      this.parser = standardBufferParser.bind(this)
      this.tabletHID.on('data', this.parser)
    } else {
      console.log('Using standardBufferParser')
      this.parser = standardBufferParser.bind(this)
      this.tabletHID.on('data', this.parser)
    }

    if (parserSettings.isTouch) {
      let tabletPath = await deviceDetector.getPath()
      let assumedTouchPath = `/dev/hidraw${parseInt(tabletPath.replace(/\D/g, '')) + 1}`

      const fs = require('fs')
      let touchBuffer = fs.createReadStream(assumedTouchPath)
      touchBuffer.on('data', (buffer) => {
        //  console.log(chunk[4])
        touchBufferParser(buffer, this)
      })
    }
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
}

module.exports = Tablet
