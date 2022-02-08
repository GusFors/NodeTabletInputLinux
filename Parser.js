const Pointer = require('./build/Release/pointer.node')
const robot = require('robotjs')
robot.setMouseDelay(0)
robot.setKeyboardDelay(0)

let x
let y
let xS
let yS
let isClick = false
let previousTouchWheelValue = 0
let setPositionInterval = false

function standardBufferParser(reportBuffer, tablet) {
  if (reportBuffer[0] !== 0x02) {
    return
  }

  // get x and y position from the tablet buffer
  x = reportBuffer[2] | (reportBuffer[3] << 8)
  y = reportBuffer[4] | (reportBuffer[5] << 8)

  // scale the values to monitor resolution
  xS = (x - tablet.settings.left) * tablet.xScale
  yS = (y - tablet.settings.top) * tablet.yScale

  // safety checks to keep cursor on primary monitor
  if (xS > tablet.monitorConfig.width) {
    xS = tablet.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > tablet.monitorConfig.height) {
    yS = tablet.monitorConfig.height
  }

  if (yS < 0) {
    yS = 0
  }

  if (x === 0 && y === 0) {
    return
  }

  // add offset to xS since in my case the main monitor is not the leftmost monitor
  Pointer.setPointer(xS + tablet.monitorConfig.xOffset, yS)

  // different pens can have different button/click values, try and make it pen agnostic
  switch (reportBuffer[1] & 0x07) {
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
}

function proBufferParser(reportBuffer, tablet) {
  if (reportBuffer[0] === 17) {
    console.log(reportBuffer[4] - 127)

    // try and make use of the wheel of the pro tablet
    if (reportBuffer[4] === 0x7f) {
      previousTouchWheelValue = 0
      return
    }

    if (reportBuffer[4] < previousTouchWheelValue) {
      robot.keyTap('audio_vol_up')
    } else {
      robot.keyTap('audio_vol_down')
    }
    previousTouchWheelValue = reportBuffer[4]
  }

  if (reportBuffer[0] != 0x10) {
    return
  }

  x = reportBuffer[2] | (reportBuffer[3] << 8)
  y = reportBuffer[5] | (reportBuffer[6] << 8)

  xS = (x - tablet.settings.left) * tablet.xScale
  yS = (y - tablet.settings.top) * tablet.yScale

  if (xS > tablet.monitorConfig.width) {
    xS = tablet.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > tablet.monitorConfig.height) {
    yS = tablet.monitorConfig.height
  }

  if (yS < 0) {
    yS = 0
  }

  if (x === 0 && y === 0) {
    return
  }

  Pointer.setPointer(xS + tablet.monitorConfig.xOffset, yS)

  switch (reportBuffer[1] & 0x07) {
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
}

module.exports = { standardBufferParser, proBufferParser }