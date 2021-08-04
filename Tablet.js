const HID = require('node-hid')
const robot = require('robotjs')
const DeviceDetector = require('./DeviceDetector')
const ConfigHandler = require('./ConfigHandler')
const deviceDetector = new DeviceDetector()

class Tablet {
  constructor() {
    this.tabletHID = null
    this.settings = null
    this.xScale = null
    this.yScale = null
    this.monitorResolution = robot.getScreenSize()
  }

  async simpleTabletInput(isRestart) {
    if (isRestart && this.tabletHID !== null) {
      this.tabletHID.pause()
      this.tabletHID = null
    }

    this.tabletHID = new HID.HID(await deviceDetector.getPath())
    this.settings = await deviceDetector.getConfig()

    console.log('Getting input from', this.settings.name)

    this.xScale = 2560 / ((this.settings.right - this.settings.left) / this.settings.multiplier)
    this.yScale = this.monitorResolution.height / ((this.settings.bottom - this.settings.top) / this.settings.multiplier)

    robot.setMouseDelay(0)

    let x
    let y
    let xS
    let yS
    let isClick = false

    if (this.settings.name === 'Wacom PTH-460') {
      this.tabletHID.on('data', (reportData) => {
        // console.log(reportData)

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

        x === 0 && y === 0 ? false : robot.moveMouse(xS + 2560, yS)

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
      })
    } else {
      this.tabletHID.on('data', (reportData) => {
        if (reportData[0] !== 2) {
          return
        }

        x = reportData[2] | (reportData[3] << 8)
        y = reportData[4] | (reportData[5] << 8)

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

        x === 0 && y === 0 ? false : robot.moveMouse(xS + 2560, yS)

        switch (reportData[1]) {
          case 241:
            if (isClick === false) {
              isClick = true
              robot.mouseToggle('down', 'left')
            }
            break
          case 242:
            if (!isClick) {
              isClick = true
              robot.mouseClick('left')
            }
            break
          case 244:
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
        // xBuffer.length = 0
        // yBuffer.length = 0
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
