const ConfigHandler = require('./configs/ConfigHandler')
const fs = require('fs/promises')
const fsStream = require('fs')

class DeviceDetector {
  constructor(configPath = '/mmConfigs.json') {
    this.configs = new ConfigHandler().readConfigSync(configPath)
  }

  async getConfig() {
    return new Promise(async (resolve, reject) => {
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

  async getTabletHidInfo() {
    return new Promise(async (resolve, reject) => {
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
          let deviceInfo = currentDevice.split('\n')
          let sObj = {}

          for (let j = 0; j < deviceInfo.length; j++) {
            let kv = deviceInfo[j].split('=')
            if (kv[0]) sObj[kv[0]] = kv[1]

            if (kv[0] === 'HID_ID') {
              let ids = kv[1].split(':')
              let conv = parseInt(ids[2], 16)
              tablet.productId = conv
            }
          }

          tablet.rawInfo = sObj
          tablet.hidpath = '/dev/' + hidrawDirs[i]
          foundTablets.push(tablet)
        }
      }

      if (foundTablets.length < 1) {
        reject('No tablets could be detected')
      }
      resolve(foundTablets)
    })
  }

  async getHidInfo() {
    let hidrawDirs = await fs.readdir('/sys/class/hidraw')
    let devices = []
    
    for (let i = 0; i < hidrawDirs.length; i++) {
      let currentDevice = await fs.readFile(`/sys/class/hidraw/${hidrawDirs[i]}/device/uevent`, { encoding: 'utf8' })
      let devObj = {}

      let deviceInfo = currentDevice.split('\n')
      let sObj = {}

      for (let j = 0; j < deviceInfo.length; j++) {
        let kv = deviceInfo[j].split('=')
        if (kv[0]) sObj[kv[0]] = kv[1]

        if (kv[0] === 'HID_ID') {
          let ids = kv[1].split(':')
          let conv = parseInt(ids[2], 16)
          devObj.productId = conv
        }
      }

      devObj.rawInfo = sObj
      devObj.hidpath = '/dev/' + hidrawDirs[i]
      devices.push(devObj)
    }
    return devices
  }

  async getTabletHidBuffer() {
    return new Promise(async (resolve, reject) => {
      try {
        const detectedTablets = await this.getTabletHidInfo()
        let deviceBuffers = []

        for (let i = 0; i < detectedTablets.length; i++) {
          let currentPath = detectedTablets[i].hidpath
          deviceBuffers.push(fsStream.createReadStream(currentPath))
          deviceBuffers[i].on('data', (buffer) => {
            // console.log(buffer, 'testbuffer' + i)
            if (buffer.length > 31) {
              console.log('try holding pen closer to tablet')
              return
            } else {
              for (let y = 0; y < deviceBuffers.length; y++) {
                if (y !== i) {
                  deviceBuffers[y].close()
                  deviceBuffers[y].push(null)
                  deviceBuffers[y].read(0)
                }
              }
              resolve({ buffer: deviceBuffers[i], rawInfo: detectedTablets[i] })
            }
          })
        }
      } catch (error) {
        console.log('error:', error)
      }
    })
  }

  async tabletDetector() {
    // let allDevices = HID.devices()
    let wacDevices = await this.getTabletHidInfo()
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
}

module.exports = DeviceDetector
