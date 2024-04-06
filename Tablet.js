const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./configs/ConfigHandler')
const deviceDetector = new DeviceDetector('/mmConfigs.json')
const Display = require('./build/Release/display.node')
const fs = require('fs')
const EventEmitter = require('node:events')
const { spawn } = require('node:child_process')
const { mmToWac } = require('./utils/converters')
const {
  standardBufferParser,
  doubleReportBufferParser,
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  Pointer,
  touchBufferParser,
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

  async simpleTabletInput(
    parserSettings = { isDoubleReport: false, isAvg: false, isVirtual: false, isTouch: false, isPressure: false },
    detached = true
  ) {
    console.log(parserSettings)

    this.tabletHID = await deviceDetector.getTabletHidBuffer()
    this.settings = mmToWac(await deviceDetector.getTabletConfig(await this.tabletHID.rawInfo.productId))
    console.log('\nGetting input from', this.settings.name)

    this.xScale = this.monitorConfig.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorConfig.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    console.log(this.settings)
    console.log(this.monitorConfig)
    console.log(this.tabletHID.rawInfo)

    // ignores other options
    if (parserSettings.isNative) {
      console.log('reading native hidraw ')
      if (detached) {
        let inputProcess = spawn(
          './native_modules/tabletinput',
          [
            await this.tabletHID.rawInfo.hidpath,
            await this.settings.name,
            this.settings.left,
            this.settings.top,
            this.settings.right,
            this.settings.bottom,
            this.settings.xBufferPositions[0],
            this.settings.yBufferPositions[0],
          ],
          { detached: true, stdio: 'ignore' }
        )
        inputProcess.unref()
        process.exit()
      }
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
      this.tabletHID.buffer.close()
      this.tabletHID.buffer = new EventEmitter()
      let buffer = Buffer.alloc(16)
      let fd = fs.openSync(this.tabletHID.rawInfo.hidpath, 'r')

      setInterval(() => {
        fs.readSync(fd, buffer, { length: 16 })
        this.tabletHID.buffer.emit('data', buffer)
      }, 1)
    }

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
    // Display.closeDisplay()
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
