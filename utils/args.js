'use strict'

function getArgValue(key, type, defaultValue) {
  let args = process.argv.slice(2)

  let kIndex = args.findIndex((arg) => {
    return arg.includes(key)
  })

  let value = args[kIndex + 1]

  if (type === 'boolean') {
    if (value !== 'false') {
      return args.includes(key)
    }
    return false
  }

  if (type === 'number') {
    let convert = parseFloat(value)

    if (typeof convert !== 'number' || Number.isNaN(convert)) {
      if (defaultValue) {
        return defaultValue
      }

      throw new Error(`Argument value ${key} is not of type ${type}, was instead given ${typeof value} '${value}'`)
    }

    value = parseFloat(value)
    value = Number.isNaN(value) ? 'Nan' : value
  }

  return value
}

module.exports = { getArgValue }
