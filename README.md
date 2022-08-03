Node console app for tablet input. Just made for fun, some stuff might be broken.

1. Install with `npm install`
2. If it doesn't build the native modules automatically run `node-gyp rebuild`
3. Set tablets area in `configs/mmConfigs.json`, optionally run `node utils/converters.js` to convert from/to mm values, see comments in `utils/converters.js` for examples.
4. Run with `node app.js` or `npm start`

#### Tried tablets

- PTH-460
- CTL-480
- CTH-480
- CTH-472
