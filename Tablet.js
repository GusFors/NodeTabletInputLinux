const HID = require('node-hid')
const robot = require('robotjs')
const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./ConfigHandler')
const deviceDetector = new DeviceDetector()
const Pointer = require('./build/Release/pointer.node')
const Display = require('./build/Release/display.node')
// const { spawn } = require('child_process')

class Tablet {
  constructor() {
    this.tabletHID = null
    this.settings = null
    this.xScale = null
    this.yScale = null
    this.monitorResolution = { height: 0, width: 0 }
  }

  async simpleTabletInput(isRestart) {
    if (isRestart && this.tabletHID !== null) {
      this.tabletHID.pause()
      this.tabletHID = null
    }

    this.tabletHID = new HID.HID(await deviceDetector.getPath())
    this.settings = await deviceDetector.getConfig()

    console.log('Getting input from', this.settings.name)
    this.monitorResolution.width = Display.getPrimaryMonitorWidth()
    this.monitorResolution.height = Display.getPrimaryMonitorHeight()
    this.xScale = this.monitorResolution.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorResolution.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    // can also remove the delay by manually removing sleep events in the robotjs modules c/c++ files and then rebuilding with node-gyp
    robot.setMouseDelay(0)
    robot.setKeyboardDelay(0)

    let x
    let y
    let xS
    let yS
    let isClick = false
    let executionTimes = []
    let previousTouchWheelValue = 0
    let heightLimit = this.monitorResolution.height
    let widthLimit = this.monitorResolution.width
    let xOffset = Display.getPrimaryMonitorXoffset()

    if (this.settings.name !== 'Wacom PTH-460') {
      console.log('Total X screen width: ' + Display.getDisplaysTotalWidth())
      console.log('Number of active monitors: ' + Display.getNumberOfMonitors())
      console.log('Assumed primary monitor xOffset: ' + xOffset)
      console.log('Assumed primary monitor yOffset: ' + Display.getPrimaryMonitorYoffset())
      console.log('Assumed primary monitor width: ' + Display.getPrimaryMonitorWidth())

      this.tabletHID.on('data', (reportData) => {
        // prevent setting cursor if no pen currently detected
        if (reportData[0] !== 0x02) {
          return
        }

        x = reportData[2] | (reportData[3] << 8)
        y = reportData[4] | (reportData[5] << 8)

        xS = (x - this.settings.left) * this.xScale
        yS = (y - this.settings.top) * this.yScale

        if (xS > this.monitorResolution.width) {
          xS = this.monitorResolution.width
        }

        if (xS < 0) {
          xS = 0
        }

        if (yS > this.monitorResolution.height) {
          yS = this.monitorResolution.height
        }

        if (yS < 0) {
          yS = 0
        }

        if (x === 0 && y === 0) {
          return
        }

        // add offset to xS since in this case main monitor is not the leftmost monitor
        Pointer.setPointer(xS + xOffset, yS)
        // robot.moveMouse(xS + xOffset, yS)

        // different pens can have different button/click values, try and make it pen agnostic
        switch (reportData[1] & 0x07) {
          case 0x01:
            if (isClick === false) {
              isClick = true
              robot.mouseToggle('down', 'left')
            }
            break

          case 0x04:
            if (!isClick) {
              isClick = true
              robot.mouseClick('right')
            }
            break

          default:
            if (isClick) {
              isClick = false
              robot.mouseToggle('up', 'left')
            }
        }
        // console.log(reportData.readUInt32LE(0) & 0x3FFFFFFF);
        // console.log((reportData[1] & 0x7));
      })
    } else {
      // try and make use of the wheel of the pro tablet
      this.tabletHID.on('data', (reportData) => {
        const t0 = performance.now()

        // TODO fix continuous scrolling issues
        if (reportData[0] === 17) {
          console.log(reportData[4] - 127)

          if (reportData[4] === 127) {
            previousTouchWheelValue = 0
            return
          }

          if (reportData[4] < previousTouchWheelValue) {
            robot.keyTap('audio_vol_up')
          } else {
            robot.keyTap('audio_vol_down')
          }

          previousTouchWheelValue = reportData[4]
        }

        if (reportData[0] != 16) {
          return
        }

        x = reportData[2] | (reportData[3] << 8)
        y = reportData[5] | (reportData[6] << 8)

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

        if (x === 0 && y === 0) {
          return
        }

        Pointer.setPointer(xS + 2560, yS)

        switch (reportData[1]) {
          case 97:
            if (isClick === false) {
              isClick = true

              robot.mouseToggle('down', 'left')
            }
            break
          case 98:
            if (!isClick) {
              isClick = true

              robot.mouseClick('left')
            }
            break
          case 100:
            if (!isClick) {
              isClick = true

              robot.mouseClick('right')
            }
            break
          default:
            if (isClick) {
              isClick = false

              robot.mouseToggle('up', 'left')
            }
        }
        const t1 = performance.now()
        executionTimes.push(t1 - t0)
      })
    }
    return 0
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
    this.xScale = this.monitorResolution.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorResolution.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)
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

    this.xScale = this.monitorResolution.width / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorResolution.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    robot.setMouseDelay(0)

    let intervalData = []
    let x
    let y
    let xS
    let yS
    let isClickHold = false

    const xBuffer = []
    const yBuffer = []

    this.tabletHID.on('data', (reportData) => {
      intervalData[0] = reportData

      if (reportData[0] != 2) {
        return
      }

      if (reportData[9] === 0) {
        return false
      }

      x = reportData[3] | (reportData[4] << 8)
      y = reportData[5] | (reportData[6] << 8)

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

      xBuffer.push(xS)
      yBuffer.push(yS)

      // pressure
      if (reportData[7] > 0) {
        if (isClickHold === false) {
          robot.mouseToggle('down', 'left')
          isClickHold = true
        }
      }

      if (reportData[7] === 0) {
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

// const child = spawn('xrandr')

// child.stdout.on('data', (data) => {
//   console.log(`child stdout:\n${data}`)
// }) // worst case find primary with xrandr?
