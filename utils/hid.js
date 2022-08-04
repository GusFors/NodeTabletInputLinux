const fs = require('fs/promises')

// move to DeviceDetector?
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
      // console.log(currentDevice)
      // let param = new URLSearchParams(currentDevice)
      // let convObj = Object.fromEntries(param.entries())
      // console.log(convObj)
      let deviceInfo = currentDevice.split('\n')
      // console.log(tablet.info)
      let sObj = {}
      for (let j = 0; j < deviceInfo.length; j++) {
        let kv = deviceInfo[j].split('=')
        if (kv[0]) sObj[kv[0]] = kv[1]
        // console.log(kv)
        if (kv[0] === 'HID_ID') {
          let ids = kv[1].split(':')
          let conv = parseInt(ids[2], 16)
          sObj.PRODUCT_ID = ids[2]
          // console.log(conv)
        }
      }
      // console.log(sObj)
      tablet.info = sObj
      // console.log(tablet.info[1].split('='))
      tablet.hidpath = hidrawDirs[i]

      foundTablets.push(tablet)
    }
  }

  console.log(foundTablets)
  // console.log(foundTablets[0].)
  return '/dev/' + foundTablets[0].hidpath
}
if (require.main === module) {
  getTabletHidInfo()
}

module.exports = getTabletHidInfo
