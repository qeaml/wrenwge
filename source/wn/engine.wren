// Base class for a state.
class State {
  construct new() {
    assert(false, "Do not instantiate State directly.")
  }

  init() {}
  on(evt) {}
  tick(delta) {}
  render() {}
}

// Base class for a sub-state.
class SubState {
  construct new() {
    assert(false, "Do not instantiate SubState directly.")
  }

  on(evt) {}
  tick() {}
  render() {}
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

  foreign nqTexture(name, out)
}

// from <nwge/render/Texture.hpp>
foreign class Texture {
  construct new() {}
}

// from <nwge/render/*.hpp>
class Render {
  foreign static clear(r, g, b)
  static clear() { clear(0.3, 0.3, 0.3) }
  foreign static color(r, g, b, a)
  static color(r, g, b) { color(r, g, b, 1.0) }
  static color() { color(1.0, 1.0, 1.0, 1.0) }
  foreign static rect(x, y, z, width, height)
  static rect(x, y, z, width, height, texture) {
    rect(x, y, z, width, height, texture, 0.0, 0.0, 1.0, 1.0)
  }
  foreign static rect(x, y, z, width, height, texture, texX, texY, texW, texH)
  foreign static square(x, y, z, size)
  static square(x, y, z, size, texture) {
    square(x, y, z, size, texture, 0.0, 0.0, 1.0, 1.0)
  }
  foreign static square(x, y, z, size, texture, texX, texY, texW, texH)
  foreign static line(x1, y1, z1, x2, y2, z2, width)
  foreign static text(x, y, z, text, height)
}
