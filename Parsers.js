const Pointer = require('./build/Release/pointer.node')
const PointerN = require('./build/Release/tablet.node')
Pointer.setPointerPosition = null

let x
let y
let xS
let yS
let isClick = false
let inRange = false

// console.log(Array.from(reportBuffer[1].toString(2)[5]))

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

  // used when touch is active
  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true

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

    case 0x00:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

function pressureBufferParser(reportBuffer) {
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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true

  if (x === 0 && y === 0) {
    return
  }

  let pressure = reportBuffer[6] | (reportBuffer[7] << 8)
  let mouseClick = -1
  switch (reportBuffer[1] & 0x07) {
    case 0x01:
      if (isClick === false) {
        isClick = true
        mouseClick = 1
        //Pointer.uMouseLeftClickDown()
      }
      break

    // case 0x04:
    //   if (!isClick) {
    //     isClick = true
    //     Pointer.uMouseRightClickDown()
    //     Pointer.uMouseRightClickUp()
    //   }
    //   break

    case 0x00:
      if (isClick) {
        isClick = false
        mouseClick = 0
        // Pointer.uMouseLeftClickUp()
      } else {
        mouseClick = -1
      }
  }
  // console.log(mouseClick)
  Pointer.setUinputPointer(xS + this.monitorConfig.xOffset, yS, pressure, mouseClick)
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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true

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

    case 0x00:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true

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

    case 0x00:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true

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

    case 0x00:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

let xBuffer = []
let yBuffer = []
const avgPositionStrength = 5
const currentPositionStrength = 2

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

  if (reportBuffer[1] === 0x00) inRange = false
  if (reportBuffer[1] > 0x00) inRange = true

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

    case 0x00:
      if (isClick) {
        isClick = false
        Pointer.mouseLeftClickUp()
      }
  }
}

// experimental
function touchBufferParser(reportBuffer, tablet) {
  if (inRange) return

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

function initUinput(devName, xMax, yMax, isPressure) {
  return Pointer.initUinput(devName, xMax, yMax, isPressure)
}
// TODO read directly instead of passing through node
function initRead(devPath, devName, xMax, yMax, left, top, xScale, yScale, xOffset, yOffset, width, height, xBufferPositions, yBufferPositions) {
  return PointerN.initRead(
    devPath,
    devName,
    xMax,
    yMax,
    left,
    top,
    xScale,
    yScale,
    xOffset,
    yOffset,
    width,
    height,
    xBufferPositions,
    yBufferPositions
  )
}

// just trying stuff, experimental
function averagePosition(positionBufferArr, amountOfPositions, currentPositionPrio) {
  let sum = 0
  let latest = positionBufferArr.length - 1

  // wait until there are enough values, prevent cursor getting stuck at display border
  if (positionBufferArr.length >= amountOfPositions) {
    // let s = 0
    // for (let y = 0; y < 5; y++) {
    //   s += Math.abs(positionBufferArr[latest - y] - positionBufferArr[latest - (y + 1)])
    // }

    // if (isNaN(s) || s > 65) {
    //    console.log('vrooom ', s)
    //   let l = positionBufferArr[latest]
    //   // for (let j = 0; j < avgPositionStrength; j++) {
    //   //   positionBufferArr[latest - j] = (positionBufferArr[latest - j] - l) / 2
    //   // }

    //   return l
    // }

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

module.exports = {
  standardBufferParser,
  pressureBufferParser,
  doubleReportBufferParser,
  proBufferParser,
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  initRead,
  standardVirtualBufferParser,
  Pointer,
  PointerN,
  touchBufferParser,
}

// const INPUTS = {
// BTN_LEFT: 0x110,
// BTN_RIGHT: 0x111,
// }

// switch (reportBuffer[1] & 0x07) {
//   case 0x01:
//     if (isClick === false) {
//       isClick = true
//       Pointer.uMouseLeftClickDown()
//     }
//     break

//   case 0x04:
//     if (!isClick) {
//       isClick = true
//       Pointer.uMouseRightClickDown()
//       Pointer.uMouseRightClickUp()
//     }
//     break

//  case 0x00:
//     if (isClick) {
//       isClick = false
//       Pointer.uMouseLeftClickUp()
//     }
// }
