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
    // keep looping until cancelled or succesful, some tablets don't get detected until pen is in range
    if (i === dataReadArray.length) {
      i = 0
    }
    // try open one of the possible paths
    let tabletDevice = new HID.HID(dataReadArray[i].path)

    // if it can't be read, try the next device path
    tabletDevice.read((err, data) => {
      if (err) {
        console.log('Unable to read device, trying next.. ', err)
      }

      // TODO, CTH-480 can have different buffer lengths
      if (data && data.length !== 64) {
        // stop looping
        clearTimeout(tryReadTimeout)
        tabletDevice.close()

        // resolve the promise that got passed down from getPath
        console.log('\nSuccess reading device with path', dataReadArray[i].path)
        return promiseResolve(dataReadArray[i].path)
      }
    })

    // try next path after timeout, might not work properly if set too low
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
