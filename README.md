Node-FreeRDP
============

Node.JS addon for libfreerdp.

### Dependencies

This requires the installation of libfreerdp 1.1.x.

On Mac:

    brew tap untoldone/homebrew-x11
    brew install untoldone/homebrew-x11/freerdp --devel

On Ubuntu (from 15.04 forward -- earlier Ubuntus used freerdp 1.0.x which wont work):

    sudo apt install freerdp libfreerdp-dev

### Installation

    npm install freerdp

### Features

This is based on FreeRDP. Any standard features of FreeRDP are supported as a result. Many
flags you see on the command line version of xfreerdp are easily added to the current codebase
if a desired functionality is missing from this Node.js wrapper library. This includes things
such as Network Level Authentication (NLA) and clipboard redirection.

### Example Usage

For the time being, this is the only API documentation available.

    var freerdp = require('freerdp');

    var session = new freerdp.Session({
      host: 'my.host',
      username: 'myuser',
      password: 'mypass',
      domain: 'mydomain', // optional
      port: 3389, // optional
      width: 1366, // optional
      height: 768, // optional
      certIgnore: true // optional
    });

    session.on('connect', function () {
      console.log('connected');

      var x = 10, y = 20; // if x / y are null, will not move mouse
      session.sendPointerEvent(x, y, {
        pressLeft: true // Other options are pressMiddle, pressRight, releaseLeft|Middle|Right
      });

      var code = 0x23; // letter 'H'
      var isPressed = true;
      session.sendKeyEventScancode(code, isPressed);

      setTimeout(function () {
        session.close(); // end session
      }, 50000);
    });

    session.on('bitmap', function (bitmap) {
      console.log(`bitmap at ${bitmap.x}, ${bitmap.y}, of dimensions ${bitmap.w}, ${bitmap.h}`);
      // bitmap.data contains RGBA buffer where each pixel is of bitmap.bytesPerPixel size
    });

    session.on('error', function (err) {
      
    });

    session.on('close', function () {
      console.log('connection closed');
    });

    session.connect();

Example of writing a current session screenshot to a png and pasting text

Install extra dependency for example with `npm install canvas`

    var freerdp = require('freerdp');
    var Canvas = require('canvas');
    var fs = require('fs');

    var canvas = new Canvas(1366, 768),
        ctx = canvas.getContext('2d');

    var session = new freerdp.Session({
      host: 'my.host',
      username: 'myuser',
      password: 'mypass',
      domain: 'mydomain', // optional
      port: 3389, // optional
      width: 1366, // optional
      height: 768, // optional
      certIgnore: true // optional
    });

    session.on('connect', function () {
      setInterval(function () {
        var b = canvas.toBuffer();
        fs.writeFileSync('screenshot.png', b, 'binary');
      }, 1000);

      setTimeout(function () {
        // Set contents of local clipboard
        session.setClipboard("Hello World\n");
      }, 2500);

      setTimeout(function () {
        // Press paste hotkey sequence to initialize paste, pulling
        // the contents of local clipboard and pasting on the remote system.
        // Note this only works if the current focus supports this hotkey sequence
        // (focused on notepad, for example)
        session.sendKeyEventScancode(0x001d, true); // ctrl
        session.sendKeyEventScancode(0x002F, true); // v
        session.sendKeyEventScancode(0x002F, false);
        session.sendKeyEventScancode(0x001d, false);
      }, 3000);
    })

    session.on('bitmap', function (bitmap) {
      var imageData = ctx.createImageData(bitmap.w, bitmap.h);
      var dest = new Uint8ClampedArray(bitmap.w * bitmap.h * bitmap.bpp);
      var size = bitmap.w * bitmap.h * bitmap.bpp;
      // ARGB to RGBA
      for (var i = 0; i < bitmap.h; i++)
      {
        for (var j = 0; j < bitmap.w * 4; j += 4)
        {
          // ARGB <-> ABGR
          dest[(i * bitmap.bpp * bitmap.w) + j + 0] = bitmap.buffer[(i * bitmap.bpp * bitmap.w) + j + 2];
          dest[(i * bitmap.bpp * bitmap.w) + j + 1] = bitmap.buffer[(i * bitmap.bpp * bitmap.w) + j + 1];
          dest[(i * bitmap.bpp * bitmap.w) + j + 2] = bitmap.buffer[(i * bitmap.bpp * bitmap.w) + j + 0];
          dest[(i * bitmap.bpp * bitmap.w) + j + 3] = bitmap.buffer[(i * bitmap.bpp * bitmap.w) + j + 3];
        }
      }

      imageData.data.set(dest);

      ctx.putImageData(imageData, bitmap.x, bitmap.y);
    });

    setTimeout(function () { }, 10000); // prevent process from exiting

### Roadmap

* Add better error handling
* There's almost certainly memory issues with the current implementation... investigate and fix if needed
* Find ways to funnel messages printed by libfreerdp to stdout to events
