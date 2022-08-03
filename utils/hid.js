const fs = require('fs/promises')

const getTabletHidrawPath = async () => {
  let hidrawDirs = await fs.readdir('/sys/class/hidraw')

  let foundTablets = []
  for (let i = 0; i < hidrawDirs.length; i++) {
    let currentDevice = await fs.readFile(`/sys/class/hidraw/${hidrawDirs[i]}/device/uevent`, { encoding: 'utf8' })
    let r = currentDevice.split('\n')
    let isTablet = false
    let tablet = {}

    for (let y = 0; y < r.length; y++) {
      let rInfo = r[y]
      let devName
      if (rInfo.includes('0000056A')) {
        isTablet = true
        break
      }
    }

    if (isTablet) {
      tablet.info = currentDevice
      tablet.hidpath = hidrawDirs[i]
      foundTablets.push(tablet)
    }
  }

  console.log(foundTablets)

  return '/dev/' + foundTablets[0].hidpath
}
if (require.main === module) {
  getTabletHidrawPath()
}

module.exports = getTabletHidrawPath
