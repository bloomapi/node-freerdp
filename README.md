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

    npm install node-freerdp

_Note, this is a work in progress_

The below example is the spec we are working towards. As of right now, only new session, bitmap events, and keyboard scancodes are working.

### Example Usage

For the time being, this is the only documentation available.

    var freerdp = require('freerdp');

    var session = new freerdp.Session({
      host: 'my.host',
      username: 'myuser',
      password: 'mypass',
      domain: 'mydomain', // optional
      port: 3389, // optional
    });

    session.on('connect', function () {
      console.log('connected');

      var x = 10, y = 20, button = 'left', isPressed = true; // button can be left, right, middle
      session.sendPointerEvent(x, y, button, isPressed);

      var code = 0x23; // letter 'H'
      var specialKey = false; // set to true if you're sending the host key or others
      session.sendKeyEventScancode(code, isPressed);

      setTimeout(function () {
        session.close(); // end session
      }, 1000);
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
