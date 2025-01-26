// Base class for a state.
class State {
  construct new() {}
  init() {}
  on(evt) {}
  tick(delta) {}
  render() {}

  foreign static swap(state)
}

// Base class for a sub-state.
class SubState {
  construct new() {}
  on(evt) {}
  tick(delta) {}
  render() {}

  foreign static push(subState, options)
  static push(subState) {
    push(subState, {})
  }
  foreign static swap(subState, options)
  static swap(subState) {
    swap(subState, {})
  }
  foreign static pop()
  foreign static clear()
}

class Event {
  construct mouseMotion(from, to, delta) {
    _type = 2
    _from = from
    _to = to
    _delta = delta
  }
  construct mouseDown(pos, button) {
    _type = 3
    _pos = pos
    _button = button
  }
  construct mouseUp(pos, button) {
    _type = 4
    _pos = pos
    _button = button
  }
  construct mouseScroll(amt) {
    _type = 5
    _amt = amt
  }

  isMouseMotion { _type == 2 }
  isMouseDown { _type == 3 }
  isMouseUp { _type == 4 }
  isMouseScroll { _type == 5 }

  motionFrom { _from }
  motionTo { _to }
  motionDelta { _delta }

  clickPos { _pos }
  clickLMB { _button == 1 }
  clickMMB { _button == 2 }
  clickRMB { _button == 3 }

  scrollAmt { _amt }
}

// from <nwge/audio/Buffer.hpp>
foreign class AudioBuffer {
  construct new() {}
}

// from <nwge/audio/Source.hpp>
foreign class AudioSource {
  construct new() {}

  foreign buffer(buf)
  foreign play()
}

// from <nwge/bind.hpp>
foreign class KeyBind {
  construct new(name, key, callback) {}
  foreign key()
  foreign onRelease(callback)
}

// from <nwge/console.hpp>
class Console {
  foreign static clear()
  foreign static nextLine()
  foreign static print(color, message)
  static print(message) { print(0, message) }
  static note(message) { print(4, message) }
  static warn(message) { print(1, message) }
  static error(message) { print(2, message) }
}

// from <nwge/data/bundle.hpp>
foreign class Bundle {
  construct new(name) {}

  foreign nqTexture(name, texture)
  foreign nqAudio(name, buffer)
}

// from <nwge/json/*>
class JSON {
  foreign static parse(source)
  foreign static encode(value)
}

// from <nwge/render/Texture.hpp>
foreign class Texture {
  construct new() {}
}

// from <nwge/render/*>
class Render {
  foreign static clear(color)
  static clear() { clear([]) }

  foreign static color(color)
  static color() { color([]) }

  foreign static rect(pos, extents)
  foreign static rect(pos, extents, texture, texCoord)
  static rect(pos, extents, texture) {
    rect(pos, extents, texture, [0.0, 0.0, 1.0, 1.0])
  }

  foreign static square(pos, size)
  foreign static square(pos, size, texture, texCoord)
  static square(pos, size, texture) {
    square(pos, size, texture, [0.0, 0.0, 1.0, 1.0])
  }

  foreign static line(posA, posB, width)

  foreign static text(pos, text, height)
}