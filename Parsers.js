const Pointer = require('./build/Release/pointer.node')

let x
let y
let xS
let yS
let isClick = false

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

  // extra checks to keep cursor on primary monitor
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
        Pointer.mouseLeftClickDown()
      }
      break

    case 0x04:
      if (!isClick) {
        isClick = true
        Pointer.mouseRightClickDown()
        Pointer.mouseRightClickUp()
      }
      break

    default:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

function standardVirtualBufferParser(reportBuffer, tablet) {
  if (reportBuffer[0] !== 0x02) {
    return
  }

  x = reportBuffer[2] | (reportBuffer[3] << 8)
  y = reportBuffer[4] | (reportBuffer[5] << 8)

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

  Pointer.setUinputPointer(xS + tablet.monitorConfig.xOffset, yS)

  switch (reportBuffer[1] & 0x07) {
    case 0x01:
      if (isClick === false) {
        isClick = true
        Pointer.mouseLeftClickDown()
      }
      break

    case 0x04:
      if (!isClick) {
        isClick = true
        Pointer.mouseRightClickDown()
        Pointer.mouseRightClickUp()
      }
      break

    default:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

function proBufferParser(reportBuffer, tablet) {
  if (reportBuffer[0] === 17) {
    // if touch wheel
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
        Pointer.mouseLeftClickDown()
      }
      break

    case 0x04:
      if (!isClick) {
        isClick = true
        Pointer.mouseRightClickDown()
        Pointer.mouseRightClickUp()
      }
      break

    default:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

let isInp = true
let xInp = []
let yInp = []

// experimental, calculate an extra position value between the two most recent position reports from the tablet buffer,
function doubleReportBufferParser(reportBuffer, tablet) {
  if (reportBuffer[0] !== 0x02) {
    // xInp = []
    // yInp = []
    return
  }

  x = reportBuffer[2] | (reportBuffer[3] << 8)
  y = reportBuffer[4] | (reportBuffer[5] << 8)

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

  xInp.push(xS)
  yInp.push(yS)

  if (isInp) {
    if (xInp.length > 1) {
      Pointer.setPointer(xInp[xInp.length - 2] + tablet.monitorConfig.xOffset, yInp[yInp.length - 2])

      setTimeout(() => {
        Pointer.setPointer(
          (xInp[xInp.length - 1] + xInp[xInp.length - 2]) / 2 + tablet.monitorConfig.xOffset,
          (yInp[yInp.length - 1] + yInp[yInp.length - 2]) / 2
        )
      }, 3.76)
    }
  } else {
    Pointer.setPointer(xS + tablet.monitorConfig.xOffset, yS)
  }

  switch (reportBuffer[1] & 0x07) {
    case 0x01:
      if (isClick === false) {
        isClick = true
        Pointer.mouseLeftClickDown()
      }
      break

    case 0x02:
      if (isClick === false) {
        isClick = true
        isInp = !isInp
        console.log('toggled inp', isInp)
        xInp = []
        yInp = []
      }
      break

    case 0x04:
      if (!isClick) {
        isClick = true
        Pointer.mouseRightClickDown()
        Pointer.mouseRightClickUp()
      }
      break

    default:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

let xBuffer = []
let yBuffer = []
const avgPositionStrength = 4
const currentPositionStrength = 3

function standardAvgBufferParser(reportBuffer, tablet) {
  if (reportBuffer[0] !== 0x02) {
    return
  }

  // get x and y position from the tablet buffer
  x = reportBuffer[2] | (reportBuffer[3] << 8)
  y = reportBuffer[4] | (reportBuffer[5] << 8)

  // scale the values to monitor resolution
  xS = (x - tablet.settings.left) * tablet.xScale
  yS = (y - tablet.settings.top) * tablet.yScale

  // extra checks to keep cursor on primary monitor
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
    xBuffer = []
    yBuffer = []
    return
  }

  xBuffer.push(xS)
  yBuffer.push(yS)

  // add offset to xS since in this case the main monitor is not the leftmost monitor
  Pointer.setPointer(
    averagePosition(xBuffer, avgPositionStrength, currentPositionStrength) + tablet.monitorConfig.xOffset,
    averagePosition(yBuffer, avgPositionStrength, currentPositionStrength)
  )

  // different pens can have different button/click values, try and make it pen agnostic
  switch (reportBuffer[1] & 0x07) {
    case 0x01:
      if (isClick === false) {
        isClick = true
        Pointer.mouseLeftClickDown()
      }
      break

    case 0x04:
      if (!isClick) {
        isClick = true
        Pointer.mouseRightClickDown()
        Pointer.mouseRightClickUp()
      }
      break

    default:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

function initXPointer() {
  return Pointer.initDisplay()
}

function initUinput(devName, xMax, yMax) {
  return Pointer.initUinput(devName, xMax, yMax)
}

// just trying stuff, experimental
function averagePosition(positionBufferArr, amountOfPositions, currentPositionPrio) {
  let sum = 0
  let latest = positionBufferArr.length - 1

  // wait until there are enough values, prevent cursor getting stuck at display border
  if (positionBufferArr.length >= amountOfPositions) {
    for (let i = positionBufferArr.length; i > positionBufferArr.length - amountOfPositions; i--) {
      // if position difference is over a certain value, get a smoothed position
      if (Math.abs(positionBufferArr[latest] - positionBufferArr[i]) > 300) {
        sum += (positionBufferArr[i - 1] + positionBufferArr[latest] + positionBufferArr[i + 1]) / 3
      } else {
        sum += positionBufferArr[i - 1]
      }
    }
    // how much the most recent position value should be prioritized when calculating average position
    for (let y = 0; y < currentPositionPrio; y++) {
      sum += positionBufferArr[latest]
      amountOfPositions++
    }
    return sum / amountOfPositions
  } else {
    return positionBufferArr[latest]
  }
}

module.exports = {
  standardBufferParser,
  doubleReportBufferParser,
  proBufferParser,
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  standardVirtualBufferParser,
}

// let positions = []
// let offset = 2560
// let inRange = false

// let setPositionInterval = false
// optionally set cursor position every x ms instead of on data report cb
// setInterval(() => {
//   if (inRange) {
//     Pointer.setPointer(xS + offset, yS)
//   }
// }, 1)

// let xT = xS + tablet.monitorConfig.xOffset
// let yT = yS
// setTimeout(() => {
//   Pointer.setPointer(xT, yT)
// }, 40)

// let previousTouchWheelValue = 0
// const robot = require('robotjs')
// robot.setMouseDelay(0)
// robot.setKeyboardDelay(0)

// console.log(reportBuffer[4] - 127)
// // try and make use of the wheel of the pro tablet, buggy for now
// if (reportBuffer[4] === 0x7f) {
//   previousTouchWheelValue = 0
//   return
// }
// if (reportBuffer[4] < previousTouchWheelValue) {
//   robot.keyTap('audio_vol_up')
// } else {
//   robot.keyTap('audio_vol_down')
// }
// previousTouchWheelValue = reportBuffer[4]

// console.log(xInp[xInp.length - 2])
// console.log((xInp[xInp.length - 1] + xInp[xInp.length - 2]) / 2)
// console.log(xInp[xInp.length - 1])