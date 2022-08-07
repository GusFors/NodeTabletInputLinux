const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./configs/ConfigHandler')
const deviceDetector = new DeviceDetector('/mmConfigs.json')
const Display = require('./build/Release/display.node')
const fs = require('fs')
const { mmToWac } = require('./utils/converters')
const {
  standardBufferParser,
  doubleReportBufferParser,
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  Pointer,
  touchBufferParser,
  pressureBufferParser,
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

  async simpleTabletInput(parserSettings = { isDoubleReport: false, isAvg: false, isVirtual: false, isTouch: false, isPressure: false }) {
    console.log(parserSettings)

    this.tabletHID = await deviceDetector.getTabletHidBuffer()
    // this.tabletHID.buffer = fs.createReadStream('/dev/hidraw7')

    this.settings = mmToWac(await deviceDetector.getConfig())
    console.log('\nGetting input from', this.settings.name)

    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    console.log('Total X screen width: ' + this.monitorConfig.xTotalWidth)
    console.log('Total X screen height: ' + this.monitorConfig.xTotalHeight)
    console.log('Number of active monitors: ' + this.monitorConfig.numOfMonitors)
    console.log('Assumed primary monitor xOffset: ' + this.monitorConfig.xOffset)
    console.log('Assumed primary monitor yOffset: ' + this.monitorConfig.yOffset)
    console.log('Assumed primary monitor width: ' + this.monitorConfig.width + '\n')

    console.log(this.settings)
    console.log(this.tabletHID.rawInfo)

    // init the pointer and display before setting pointer positions and clicks
    initXPointer()

    if (parserSettings.isVirtual) {
      let uiDevice = initUinput(await this.settings.name, this.monitorConfig.xTotalWidth, this.monitorConfig.xTotalHeight, parserSettings.isPressure)
      console.log('Created uinput device:', uiDevice)
      Pointer.setPointerPosition = Pointer.setPointerPosition = Pointer['setUinputPointer']
    } else {
      Pointer.setPointerPosition = Pointer.setPointerPosition = Pointer['setPointer']
    }

    if (parserSettings.isAvg) {
      console.log('Using standardAvgBufferParser')
      this.parser = standardAvgBufferParser.bind(this)
      this.tabletHID.buffer.on('data', this.parser)
    } else if (parserSettings.isDoubleReport) {
      console.log('Using doubleReportBufferParser')
      this.parser = doubleReportBufferParser.bind(this)
      this.tabletHID.buffer.on('data', this.parser)
    } else if (parserSettings.isNewConfig) {
      console.log('Using newConfig')
      this.parser = standardBufferParser.bind(this)
      this.tabletHID.buffer.on('data', this.parser)
    } else if (parserSettings.isPressure) {
      this.parser = pressureBufferParser.bind(this)
      this.tabletHID.buffer.on('data', this.parser)
    } else {
      console.log('Using standardBufferParser')
      this.parser = standardBufferParser.bind(this)
      this.tabletHID.buffer.on('data', this.parser)
    }

    if (parserSettings.isTouch && this.settings.touch) {
      let tabletPath = this.tabletHID.rawInfo.hidpath
      let assumedTouchPath = `/dev/hidraw${parseInt(tabletPath.replace(/\D/g, '')) + 1}`

      let touchBuffer = fs.createReadStream(assumedTouchPath).on('error', (err) => {
        console.log('Could not open stream, tablet might not support touch, wrong hidraw was opened or other permission error. Error:', err)
      })
      touchBuffer.on('data', (buffer) => {
        touchBufferParser(buffer, this)
      })
      console.log('Using touchBufferParser')
    }
  }

  closeTablet() {
    this.tabletHID.buffer.pause()
    this.tabletHID.buffer = null
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
