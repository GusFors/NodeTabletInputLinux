if (process.argv.includes('-r')) {
  let x = 76
  let y = 32.75
  let width = 90
  let height = 50.625

  let left = (x - width / 2) * 100
  let right = (x + width / 2) * 100
  let top = (y - height / 2) * 100
  let bottom = (y + height / 2) * 100

  console.log('left: ' + left)
  console.log('right ' + right)
  console.log('top: ' + top)
  console.log('bottom: ' + bottom)
} else {
  let left = 3100
  let right = 12100
  let top = 744
  let bottom = 5806
  let width = right - left
  let height = bottom - top

  let x = (left + width / 2) / 100
  let y = (top + height / 2) / 100
  width = width / 100
  height = height / 100

  console.log('width ' + width)
  console.log('height ' + height)
  console.log('x: ' + x)
  console.log('y: ' + y)
}
