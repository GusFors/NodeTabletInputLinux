#!/usr/bin/env gjs

// const Gtk = imports.gi.Gtk
imports.gi.versions.Gtk = '3.0'
const { Gio, Gtk } = imports.gi

let app = new Gtk.Application({ application_id: 'org.gtk.ExampleApp' })

let entrys = []

app.connect('activate', () => {
  let win = new Gtk.Window({ application: app, defaultHeight: 600, defaultWidth: 800, borderWidth: 4 })
  const mainBox = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL })

  const rowOneContainer = new Gtk.Box({ orientation: Gtk.Orientation.HORIZONTAL, spacing: 4 })
  const rowTwoContainer = new Gtk.Box({ orientation: Gtk.Orientation.HORIZONTAL, spacing: 4 })

  const entryBoxTop = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL })
  // entryBoxTop.set_baseline_position(0)
  const labelTop = new Gtk.Label({
    useMarkup: true,
    label: '<span>Top</span>',
  })

  const entryTop = new Gtk.Entry({
    buffer: new Gtk.EntryBuffer(),
  })

  entrys.push(entryTop)

  entryBoxTop.add(labelTop)
  entryBoxTop.add(entryTop)

  const entryBoxBottom = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL })

  const labelBottom = new Gtk.Label({
    useMarkup: true,
    label: '<span>Bottom</span>',
  })

  const entryBottom = new Gtk.Entry({
    buffer: new Gtk.EntryBuffer(),
  })
  entrys.push(entryBottom)
  entryBoxBottom.add(labelBottom)
  entryBoxBottom.add(entryBottom)

  const entryBoxLeft = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL })
  const labelLeft = new Gtk.Label({
    useMarkup: true,
    label: '<span>Left</span>',
  })
  const entryLeft = new Gtk.Entry({
    buffer: new Gtk.EntryBuffer(),
  })
  entrys.push(entryLeft)
  entryBoxLeft.add(labelLeft)
  entryBoxLeft.add(entryLeft)

  const entryBoxRight = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL })
  const labelRight = new Gtk.Label({
    useMarkup: true,
    label: '<span>Right</span>',
  })
  const entryRight = new Gtk.Entry({
    buffer: new Gtk.EntryBuffer(),
  })
  entrys.push(entryRight)
  entryBoxRight.add(labelRight)
  entryBoxRight.add(entryRight)

  const buttonBox = new Gtk.Box({ orientation: Gtk.Orientation.HORIZONTAL })

  const button = new Gtk.Button({
    label: 'Apply',
    borderWidth: 4,
  })

  button.connect('clicked', () => {
    for (let i = 0; i < entrys.length; i++) {
      log('Log entry: ' + entrys[i].get_buffer().text)
    }
   
  })

  buttonBox.add(button)

  rowOneContainer.add(entryBoxTop)
  rowOneContainer.add(entryBoxBottom)

  rowTwoContainer.add(entryBoxLeft)
  rowTwoContainer.add(entryBoxRight)

  mainBox.add(rowOneContainer)
  mainBox.add(rowTwoContainer)
  mainBox.add(buttonBox)

  win.add(mainBox)
  win.show_all()
})

app.run([])
