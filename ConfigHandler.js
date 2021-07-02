const { readFileSync, writeFileSync } = require('fs')

class ConfigHandler {
  constructor() {}

  writeConfigSync(tabletConfig, configObject) {
    const configs = this.readConfigSync()

    for (let i = 0; i < configs.length; i++) {
      if (configs[i].name === tabletConfig.name) {
        configs[i] = tabletConfig
      }
    }

    writeFileSync('./configs.json', JSON.stringify(configs), (err) =>
      err ? console.log('Error writing config', err) : console.log('Successfully wrote config to file')
    )
  }

  readConfigSync(tabletName) {
    return JSON.parse(
      readFileSync('./configs.json', (err) => {
        if (err) {
          console.log('Error when reading file')
        }
      })
    )
  }
}

module.exports = ConfigHandler
