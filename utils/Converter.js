let c = 100 // 100 for most, 200 for PTH?

if (process.argv.includes('-r')) {
  let x = 76
  let y = 36.5625
  let width = 123
  let height = 69.1875

  let left = (x - width / 2) * c
  let right = (x + width / 2) * c
  let top = (y - height / 2) * c
  let bottom = (y + height / 2) * c

  console.log('left: ' + left)
  console.log('right ' + right)
  console.log('top: ' + top)
  console.log('bottom: ' + bottom)
} else {
  let left = 5472
  let right = 24928
  let top = 1078
  let bottom = 12022
  let width = right - left
  let height = bottom - top

  let x = (left + width / 2) / c
  let y = (top + height / 2) / c
  width = width / c
  height = height / c
 
  console.log('width ' + width)
  console.log('height ' + height)
  console.log('x: ' + x)
  console.log('y: ' + y)
}
