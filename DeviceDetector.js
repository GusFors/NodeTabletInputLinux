const HID = require('node-hid')
const ConfigHandler = require('./configs/ConfigHandler')
const fs = require('fs/promises')

class DeviceDetector {
  constructor(configPath) {
    this.configs = new ConfigHandler().readConfigSync(configPath)
    // console.log(this.configs)
  }

  tabletDetector() {
    let allDevices = HID.devices()
    let wacDevices = allDevices.filter((device) => device.vendorId === 1386)
    let tabletMatches = []
    let tabletName

    for (let i = 0; i < wacDevices.length; i++) {
      for (let x = 0; x < this.configs.length; x++) {
        if (this.configs[x].productId === wacDevices[i].productId) {
          tabletMatches.push(wacDevices[i])
          tabletName = this.configs[x].name
        }
      }
    }
    return tabletMatches
  }

  async getPath() {
    return new Promise((resolve, reject) => {
      this.tryReadDevice(0, { resolve: resolve, reject, reject }, this.tabletDetector())
    })
  }

  getName() {
    return new Promise((resolve, reject) => {
      let wacomDevices = HID.devices().filter((device) => device.vendorId === 1386)
      this.configs.forEach((config) => {
        wacomDevices.forEach((device) => {
          if (config.productId === device.productId) {
            return resolve(config.name)
          }
        })
      })
      return reject('Could not find or get any tablet name')
    })
  }

  async getConfig() {
    return new Promise(async (resolve, reject) => {
      // let wacomDevices = HID.devices().filter((device) => device.vendorId === 1386)
      let wacomDevices = await this.getTabletHidInfo()
      this.configs.forEach((config) => {
        wacomDevices.forEach((device) => {
          if (config.productId === device.productId) {
            return resolve(config)
          }
        })
      })
    })
  }

  tryReadDevice(i, promise, dataReadArray) {
    // keep looping until cancelled or succesful, some tablets might not get detected until pen is in range
    if (i === dataReadArray.length) {
      i = 0
    }

    if (dataReadArray.length === 0) {
      return promise.reject('No devices to read')
    }
    // try open one of the possible paths
    let tabletDevice = new HID.HID(dataReadArray[i].path)
    console.log('\nSuccess reading device with path', dataReadArray[i].path)

    return promise.resolve(dataReadArray[i].path)
  }

  async getTabletHidInfo() {
    let hidrawDirs = await fs.readdir('/sys/class/hidraw')

    let foundTablets = []
    for (let i = 0; i < hidrawDirs.length; i++) {
      let currentDevice = await fs.readFile(`/sys/class/hidraw/${hidrawDirs[i]}/device/uevent`, { encoding: 'utf8' })
      let r = currentDevice.split('\n')
      let isTablet = false
      let tablet = {}

      for (let y = 0; y < r.length; y++) {
        let rInfo = r[y]
        if (rInfo.includes('0000056A')) {
          isTablet = true
          break
        }
      }

      if (isTablet) {
        // console.log(currentDevice)
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
            tablet.productId = conv
            // console.log(conv)
          }
        }
        // console.log(sObj)
        tablet.rawInfo = sObj
        // console.log(tablet.info[1].split('='))
        tablet.hidpath = '/dev/' + hidrawDirs[i]

        foundTablets.push(tablet)
      }
    }

    // console.log(foundTablets)
    // console.log(foundTablets[0].)
    //  return '/dev/' + foundTablets[0].hidpath
    return foundTablets
  }
}

module.exports = DeviceDetector
