const Pointer = require('./build/Release/pointer.node')
const PointerN = require('./build/Release/tablet.node')
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

  x = reportBuffer[this.settings.xBufferPositions[0]] | (reportBuffer[this.settings.xBufferPositions[1]] << 8)
  y = reportBuffer[this.settings.yBufferPositions[0]] | (reportBuffer[this.settings.yBufferPositions[1]] << 8)

  // scale the values to monitor resolution
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

  // if using touch, check for pen first
  // if (reportBuffer[1] > 0x00) {
  //   inRange = true
  // } else {
  //   inRange = false
  // }

  if (x === 0 && y === 0) {
    return
  }

  Pointer.setPointerPosition(xS + this.monitorConfig.xOffset, yS)

  switch (reportBuffer[this.settings.bBufferPositions[0]] & 0x07) {
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

function standardVirtualBufferParser(reportBuffer) {
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

  if (reportBuffer[1] > 0x00) {
    inRange = true
  } else {
    inRange = false
  }

  if (x === 0 && y === 0) {
    return
  }

  Pointer.setPointerPosition(xS + this.monitorConfig.xOffset, yS)

  switch (reportBuffer[this.settings.bBufferPositions[0]] & 0x07) {
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

// calculate an extra position value between the two most recent position reports from the buffer,
function doubleReportBufferParser(reportBuffer) {
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

  switch (reportBuffer[this.settings.bBufferPositions[0]] & 0x07) {
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

  switch (reportBuffer[this.settings.bBufferPositions[0]] & 0x07) {
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

function touchBufferParser(reportBuffer, tablet) {
  if (inRange) return

  if (tablet.settings.name === 'Wacom PTH-460') {
    let x = reportBuffer[4] | (reportBuffer[5] << 8)
    let y = reportBuffer[6] | (reportBuffer[7] << 8)

    let xScale = tablet.monitorConfig.width / 6399
    let yScale = tablet.monitorConfig.height / 3999

    let xS = x * xScale
    let yS = y * yScale
  } else {
    let x = reportBuffer[4] << 8
    let y = reportBuffer[5] << 8

    let xScale = tablet.monitorConfig.width / 65280
    let yScale = tablet.monitorConfig.height / 65280

    let xS = x * xScale
    let yS = y * yScale
  }
}

function initXPointer() {
  return Pointer.initDisplay()
}

function initUinput(devName, xMax, yMax, isPressure) {
  return Pointer.initUinput(devName, xMax, yMax, isPressure)
}

function initRead(devPath, devName, xMax, yMax, left, top, xScale, yScale, xBufferPositions, yBufferPositions, bBufferPositions) {
  return PointerN.initRead(devPath, devName, xMax, yMax, left, top, xScale, yScale, xBufferPositions, yBufferPositions, bBufferPositions)
}

function averagePosition(positionBufferArr, amountOfPositions, currentPositionPrio) {
  let sum = 0
  let latest = positionBufferArr.length - 1

  if (positionBufferArr.length >= amountOfPositions) {
    // let s = 0
    // for (let y = 0; y < 5; y++) {
    //   s += Math.abs(positionBufferArr[latest - y] - positionBufferArr[latest - (y + 1)])
    // }

    // if (isNaN(s) || s > 65) {
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
  standardAvgBufferParser,
  initXPointer,
  initUinput,
  initRead,
  standardVirtualBufferParser,
  Pointer,
  touchBufferParser,
}

// let pressure = reportBuffer[6] | (reportBuffer[7] << 8)
// if (reportBuffer[0] === 17) {
//   // touch wheel
// }
