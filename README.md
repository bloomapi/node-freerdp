Node-FreeRDP
============

Node.JS addon for libfreerdp.

### Dependencies

This requires the installation of libfreerdp 1.1.x.

On Mac:

    brew tap untoldone/homebrew-x11
    brew install untoldone/homebrew-x11/freerdp --devel

On Ubuntu (from 15.04 forward -- earlier Ubuntus used freerdp 1.0.x which wont work):

    sudo apt install freerdp

### Installation

    npm install freerdp

### Example Usage

For the time being, this is the only documentation available.

    var freerdp = require('freerdp');

    var session = new freerdp.Session({
      host: 'my.host',
      username: 'myuser',
      password: 'mypass',
      domain: 'mydomain', // optional
      port: 3389, // optional
      width: 1366, // optional
      height: 768 // optional
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
      console.log(`bitmap at ${bitmap.x}, ${bitmap.y}, of dimensions ${bitmap.width}, ${bitmap.height}`);
      // bitmap.data contains RGBA buffer where each pixel is of bitmap.bytesPerPixel size
    });

    session.on('error', function (err) {
      
    });

    session.on('close', function () {
      console.log('connection closed');
    });

    session.connect();

Example of writing a current session screenshot to a png

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
      height: 768 // optional
    });

    session.on('connect', function () {
      setInterval(function () {
        var b = canvas.toBuffer();
        fs.writeFileSync('screenshot.png', b, 'binary');
      }, 1000);
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

* Add Clipboard support
* Add better error handling
* There's almost certainly memory issues with the current implementation... investigate and fix if needed
* Find ways to funnel messages printed by libfreerdp to stdout to events
