var rdp = require('./build/Release/node-freerdp');
var EventEmitter = require('events');

// This is from include/freerdp/input.h to simplify addon code
const PTR_FLAGS_WHEEL           = 0x0200
const PTR_FLAGS_WHEEL_NEGATIVE  = 0x0100
const PTR_FLAGS_MOVE            = 0x0800
const PTR_FLAGS_DOWN            = 0x8000
const PTR_FLAGS_BUTTON1         = 0x1000 /* left */
const PTR_FLAGS_BUTTON2         = 0x2000 /* right */
const PTR_FLAGS_BUTTON3         = 0x4000 /* middle */
const WheelRotationMask         = 0x01FF

class Session extends EventEmitter {
  constructor(options) {
    super();
    this.host = options.host;
    this.username = options.username;
    this.password = options.password;
    this.domain = options.domain;
    this.port = options.port || 3389;
    this.width = options.width || 1366;
    this.height = options.height || 768;
    this.bitsPerPixel = 24;
    this.certIgnore = options.certIgnore;
  }

  sendKeyEventScancode(code, pressed) {
    rdp.sendKeyEventScancode(this._sessionIndex, code, pressed);
  }

  sendPointerEvent(x, y, options) {
    var flags = 0;

    x = x || 0;
    y = y || 0;

    if (options.pressLeft) flags |= PTR_FLAGS_BUTTON1 | PTR_FLAGS_DOWN;
    if (options.pressMiddle) flags |= PTR_FLAGS_BUTTON3 | PTR_FLAGS_DOWN;
    if (options.pressRight) flags |= PTR_FLAGS_BUTTON2 | PTR_FLAGS_DOWN;

    if (options.releaseLeft) flags |= PTR_FLAGS_BUTTON1;
    if (options.releaseMiddle) flags |= PTR_FLAGS_BUTTON3;
    if (options.releaseRight) flags |= PTR_FLAGS_BUTTON2;

    if (x !== null && y !== null && flags == 0) {
      flags |= PTR_FLAGS_MOVE;
    }

    rdp.sendPointerEvent(this._sessionIndex, flags, x, y);
  }

  setClipboard(val) {
    rdp.setClipboard(this._sessionIndex, val);
  }

  close() {
    rdp.close(this._sessionIndex);
  }

  _eventHandler(event, args) {
    args.unshift(event);
    this.emit.apply(this, args);
  }

  connect() {
    var params = [];

    params.push(`/v:${this.host}`);
    params.push(`/u:${this.username}`);
    params.push(`/p:${this.password}`);
    params.push(`/w:${this.width}`);
    params.push(`/h:${this.height}`);
    params.push(`/bpp:${this.bitsPerPixel}`);

    params.push('+clipboard');

    if (this.certIgnore) {
      params.push('/cert-ignore');
    }

    if (this.domain) {
      params.push(`/d:${this.domain}`);
    }

    if (this.port) {
      params.push(`/port:${this.port}`);
    }

    this._sessionIndex = rdp.connect(params, this._eventHandler.bind(this));
  }
}

module.exports = Session;
