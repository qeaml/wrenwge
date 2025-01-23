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
