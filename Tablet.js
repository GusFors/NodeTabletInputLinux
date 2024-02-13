const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./configs/ConfigHandler')
const deviceDetector = new DeviceDetector('/mmConfigs.json')
const Display = require('./build/Release/display.node')
const fs = require('fs')
const EventEmitter = require('node:events')
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
  initRead,
  standardVirtualBufferParser,
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
    console.log(this.monitorConfig)
    console.log(this.tabletHID.rawInfo)
    console.log('xScale:', this.xScale, 'yScale:', this.yScale)

    // ignores other options
    if (parserSettings.isNative) {
      console.log('reading native hidraw ')
      initRead(
        await this.tabletHID.rawInfo.hidpath,
        await this.settings.name,
        this.monitorConfig.xTotalWidth,
        this.monitorConfig.xTotalHeight,
        this.settings.left,
        this.settings.top,
        this.xScale,
        this.yScale,
        this.settings.xBufferPositions[0],
        this.settings.yBufferPositions[0]
      )
      return
    } else {
      console.log('using node parsers with readstreams are currently broken, using interval as workaround')
      //  process.exit()
      this.tabletHID.buffer.close()
      this.tabletHID.buffer = new EventEmitter()
      let buffer = Buffer.alloc(16)
      let fd = fs.openSync(this.tabletHID.rawInfo.hidpath, 'r')

      setInterval(() => {
        fs.readSync(fd, buffer, { length: 16 })
        this.tabletHID.buffer.emit('data', buffer)
      }, 1)
    }

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
    } else if (parserSettings.isPressure) {
      this.parser = pressureBufferParser.bind(this)
      this.tabletHID.buffer.on('data', this.parser)
    } else if (parserSettings.isVirtual) {
      console.log('using virtualBufferParser')
      this.parser = standardVirtualBufferParser.bind(this)
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
    this.tabletHID.buffer.close()
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
