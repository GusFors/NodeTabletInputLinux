const HID = require('node-hid')
const ConfigHandler = require('./configs/ConfigHandler')

class DeviceDetector {
  constructor(configPath) {
    this.configs = new ConfigHandler().readConfigSync(configPath)
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

  getConfig() {
    return new Promise((resolve, reject) => {
      let wacomDevices = HID.devices().filter((device) => device.vendorId === 1386)
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
}

module.exports = DeviceDetector
