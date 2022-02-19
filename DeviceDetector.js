const HID = require('node-hid')
const ConfigHandler = require('./configs/ConfigHandler')

class DeviceDetector {
  constructor() {
    this.configs = new ConfigHandler().readConfigSync()
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

  getPath() {
    return new Promise((resolve, reject) => {
      this.tryReadDevice(0, resolve, this.tabletDetector())
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

  tryReadDevice(i, promiseResolve, dataReadArray) {
    if (i === dataReadArray.length) {
      i = 0
    }

    let tabletDevice = new HID.HID(dataReadArray[i].path)
    tabletDevice.read((err, data) => {
      if (err) {
        console.log('Unable to read device, trying next.. ', err)
      }

      // TODO check why CTH-480 can have different buffer lengths
      if (data && data.length !== 64) {
        clearTimeout(tryReadTimeout)
        tabletDevice.close()
        console.log('\nSuccess reading device')

        return promiseResolve(dataReadArray[i].path)
      }
    })

    let tryReadTimeout = setTimeout(() => {
      tabletDevice.close()
      if (i === dataReadArray.length - 1) {
        this.tryReadDevice(0, promiseResolve, dataReadArray)
      } else {
        this.tryReadDevice(i + 1, promiseResolve, dataReadArray)
      }
    }, 200)
  }
}

module.exports = DeviceDetector
