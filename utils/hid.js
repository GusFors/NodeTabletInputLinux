const fs = require('fs/promises')

const getTabletHidInfo = async () => {
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
      let deviceInfo = currentDevice.split('\n')

      let sObj = {}
      for (let j = 0; j < deviceInfo.length; j++) {
        let kv = deviceInfo[j].split('=')
        if (kv[0]) sObj[kv[0]] = kv[1]
        // console.log(kv)
        if (kv[0] === 'HID_ID') {
          let ids = kv[1].split(':')
          let conv = parseInt(ids[2], 16)
          sObj.PRODUCT_ID = ids[2]
        }
      }

      tablet.info = sObj

      tablet.hidpath = hidrawDirs[i]

      foundTablets.push(tablet)
    }
  }

  console.log(foundTablets)

  return '/dev/' + foundTablets[0].hidpath
}
if (require.main === module) {
  getTabletHidInfo()
}

module.exports = getTabletHidInfo
