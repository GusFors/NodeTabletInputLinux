const { readFileSync, writeFileSync } = require('fs')
const path = require('path')

class ConfigHandler {
  constructor() {}

  writeConfigSync(tabletConfig, configObject) {
    const configs = this.readConfigSync()

    for (let i = 0; i < configs.length; i++) {
      if (configs[i].name === tabletConfig.name) {
        configs[i] = tabletConfig
      }
    }

    writeFileSync(__dirname + '/configs.json', JSON.stringify(configs), (err) =>
      err ? console.log('Error writing config', err) : console.log('Successfully wrote config to file')
    )
  }

  readConfigSync(tabletName) {
    console.log(__dirname)
    return JSON.parse(
      readFileSync(__dirname + '/configs.json', (err) => {
        if (err) {
          console.log('Error when reading file')
        }
      })
    )
  }
}

module.exports = ConfigHandler
