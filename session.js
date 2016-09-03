var rdp = require('./build/release/node-freerdp');
var EventEmitter = require('events');

class Session extends EventEmitter {
  constructor(options) {
    super();
    this.host = options.host;
    this.username = options.username;
    this.password = options.password;
    this.domain = options.domain;
    this.port = options.port;
  }

  sendKeyEventScancode(code, pressed) {
    rdp.sendKeyEventScancode(this._sessionIndex, code, pressed);
  }

  sendPointerEvent(x, y, button, isPressed) {
    rdp.sendMouseEvent(this._sessionIndex, x, y, button, pressed);
  }

  close() {
    rdp.closeConnection(this._sessionIndex);
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
    params.push(`/w:1366`);
    params.push(`/h:768`);
    params.push(`/bpp:32`);

    if (this.domain) {
      params.push(`/d:${this.domain}`);
    }

    if (this.port) {
      params.push(`/d:${this.port}`);
    }

    this._sessionIndex = rdp.connect(params, this._eventHandler.bind(this));

    console.log(this._sessionIndex);
    console.log('connected');
  }
}

module.exports = Session;
