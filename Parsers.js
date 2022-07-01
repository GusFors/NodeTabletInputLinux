const Pointer = require('./build/Release/pointer.node')
Pointer.setPointerPosition = null

let x
let y
let xS
let yS
let isClick = false
let inRange = false

function standardBufferParser(reportBuffer) {
  if (reportBuffer[0] > 0x10) {
    return
  }

  // get x and y position from the buffer, get the indexes from the tablets config file
  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  // scale the values to monitor resolution
  xS = (x - this.settings.left) * this.xScale
  yS = (y - this.settings.top) * this.yScale

  // extra checks to keep cursor on primary monitor
  if (xS > this.monitorConfig.width) {
    xS = this.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > this.monitorConfig.height) {
    yS = this.monitorConfig.height
  }

  if (yS < 0) {
    yS = 0
  }

  if (x === 0 && y === 0) {
    return
  }

  // add offset to xS since in my case the main monitor is not the leftmost monitor
  Pointer.setPointerPosition(xS + this.monitorConfig.xOffset, yS)

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

  // used when touch is active
  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true
}

function standardVirtualBufferParser(reportBuffer) {
  if (reportBuffer[0] > 0x10) {
    return
  }

  // if (reportBuffer[0] !== 0x02) {
  //   return
  // }

  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  xS = (x - this.settings.left) * this.xScale
  yS = (y - this.settings.top) * this.yScale

  if (xS > this.monitorConfig.width) {
    xS = this.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > this.monitorConfig.height) {
    yS = this.monitorConfig.height
  }

  if (yS < 0) {
    yS = 0
  }

  if (x === 0 && y === 0) {
    return
  }

  Pointer.setPointerPosition(xS + this.monitorConfig.xOffset, yS)

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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true
}

function proBufferParser(reportBuffer) {
  if (reportBuffer[0] === 17) {
    // if touch wheel
  }

  if (reportBuffer[0] != 0x10) {
    return
  }

  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  xS = (x - this.settings.left) * this.xScale
  yS = (y - this.settings.top) * this.yScale

  if (xS > this.monitorConfig.width) {
    xS = this.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > this.monitorConfig.height) {
    yS = this.monitorConfig.height
  }

  if (yS < 0) {
    yS = 0
  }

  if (x === 0 && y === 0) {
    return
  }

  Pointer.setPointerPosition(xS + this.monitorConfig.xOffset, yS)

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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true
}

let xInp = []
let yInp = []
let isInp = true

// experimental, calculate an extra position value between the two most recent position reports from the buffer,
function doubleReportBufferParser(reportBuffer) {
  if (reportBuffer[0] > 0x10) {
    return
  }

  // if (reportBuffer[0] !== 0x02) {
  //   // xInp = []
  //   // yInp = []
  //   return
  // }

  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  xS = (x - this.settings.left) * this.xScale
  yS = (y - this.settings.top) * this.yScale

  if (xS > this.monitorConfig.width) {
    xS = this.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > this.monitorConfig.height) {
    yS = this.monitorConfig.height
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
      Pointer.setPointerPosition(xInp[xInp.length - 2] + this.monitorConfig.xOffset, yInp[yInp.length - 2])

      setTimeout(() => {
        Pointer.setPointerPosition(
          (xInp[xInp.length - 1] + xInp[xInp.length - 2]) / 2 + this.monitorConfig.xOffset,
          (yInp[yInp.length - 1] + yInp[yInp.length - 2]) / 2
        )
      }, 1000 / this.stdRps / 2)
    }
  } else {
    Pointer.setPointerPosition(xS + this.monitorConfig.xOffset, yS)
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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true
}

let xBuffer = []
let yBuffer = []
const avgPositionStrength = 8
const currentPositionStrength = 3

function standardAvgBufferParser(reportBuffer, isDouble = true) {
  if (reportBuffer[0] > 0x10) {
    return
  }

  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  xS = (x - this.settings.left) * this.xScale
  yS = (y - this.settings.top) * this.yScale

  if (xS > this.monitorConfig.width) {
    xS = this.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > this.monitorConfig.height) {
    yS = this.monitorConfig.height
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

  if (isDouble && xBuffer.length > 1) {
    setTimeout(() => {
      xBuffer.push((xBuffer[xBuffer.length - 1] + xBuffer[xBuffer.length - 2]) / 2)
      yBuffer.push((yBuffer[yBuffer.length - 1] + yBuffer[yBuffer.length - 2]) / 2)

      Pointer.setPointerPosition(
        averagePosition(xBuffer, avgPositionStrength, currentPositionStrength) + this.monitorConfig.xOffset,
        averagePosition(yBuffer, avgPositionStrength, currentPositionStrength)
      )
    }, 1000 / this.stdRps / 2)
  }

  Pointer.setPointerPosition(
    averagePosition(xBuffer, avgPositionStrength, currentPositionStrength) + this.monitorConfig.xOffset,
    averagePosition(yBuffer, avgPositionStrength, currentPositionStrength)
  )

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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true
}

// experimental
function touchBufferParser(reportBuffer, tablet) {
  if (inRange) return

  console.log(reportBuffer)

  if (tablet.settings.name === 'Wacom PTH-460') {
    let x = reportBuffer[4] | (reportBuffer[5] << 8)
    let y = reportBuffer[6] | (reportBuffer[7] << 8)

    let xScale = tablet.monitorConfig.width / 6399
    let yScale = tablet.monitorConfig.height / 3999

    let xS = x * xScale
    let yS = y * yScale

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

    Pointer.setPointerPosition(xS + tablet.monitorConfig.xOffset, yS)

    switch (reportBuffer[3]) {
      case 0x01:
        if (isClick === false) {
          isClick = true
          Pointer.mouseLeftClickDown()
        }
        break

        case 0x00:
        if (isClick) {
          isClick = false
          Pointer.mouseLeftClickUp()
        }
    }
  } else {
    let x = reportBuffer[4] << 8
    let y = reportBuffer[5] << 8

    let xScale = tablet.monitorConfig.width / 65280
    let yScale = tablet.monitorConfig.height / 65280

    let xS = x * xScale
    let yS = y * yScale

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

    Pointer.setPointerPosition(xS + tablet.monitorConfig.xOffset, yS)
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
      // if position difference is over a certain value, increase the latest position amount
      if (Math.abs(positionBufferArr[latest] - positionBufferArr[latest - amountOfPositions]) > 70) {
        sum += (positionBufferArr[i - 1] + positionBufferArr[latest] + positionBufferArr[latest] + positionBufferArr[latest]) / 4
      } else if (Math.abs(positionBufferArr[latest] - positionBufferArr[i]) > 30) {
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

function simpleBufferParser(reportBuffer, xBufferPositions, yBufferPositions) {
  if (reportBuffer[0] > 0x10) {
    return
  }

  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  xS = (x - this.settings.left) * this.xScale
  yS = (y - this.settings.top) * this.yScale

  if (xS > this.monitorConfig.width) {
    xS = this.monitorConfig.width
  }

  if (xS < 0) {
    xS = 0
  }

  if (yS > this.monitorConfig.height) {
    yS = this.monitorConfig.height
  }

  if (yS < 0) {
    yS = 0
  }

  if (x === 0 && y === 0) {
    return
  }

  Pointer.setPointerPosition(xS + 2560, yS)

  return { x, y }
}

module.exports = {
  standardBufferParser,
  doubleReportBufferParser,
  proBufferParser,
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  standardVirtualBufferParser,
  Pointer,
  touchBufferParser,
}
