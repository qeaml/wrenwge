import "engine" for State, Console, Render, Bundle, Texture, KeyBind
import "atlas" for TextureAtlas
import "blue" for BlueState

class InitState is State {
  construct new() {
    _timer = 0.0
    _announced = false

    _bundle = Bundle.new("sample")
    _texture = Texture.new()
    _bundle.nqTexture("fen.jpg", _texture)

    _tilesTexture = Texture.new()
    _bundle.nqTexture("tiles.png", _tilesTexture)
    _tiles = TextureAtlas.new(_tilesTexture, [16, 16])

    _red = false
    _redBind = KeyBind.new("test.red", "r") {
      Console.print("Red key pressed.")
      _red = true
    }
    _redBind.onRelease {
      Console.print("Red key released.")
      _red = false
    }
    _blueBind = KeyBind.new("test.blue", "b") {
      State.swap(BlueState.new())
    }
    _exitBind = KeyBind.new("test.exit", "escape") {
      return false
    }
  }

  on(evt) {
    if(evt.isMouseDown) {
      Console.print("Mouse pressed at %(evt.clickPos)")
    }
    if(evt.isMouseUp) {
      Console.print("Mouse released as %(evt.clickPos)")
    }
  }

  tick(delta) {
    _timer = _timer + delta
    if(_timer >= 3.0 && !_announced) {
      Console.print("3 seconds have passed")
      _announced = true
    }
  }

  render() {
    if(_red) {
      Render.clear([1.0, 0.0, 0.0])
    } else {
      Render.clear([0.0, 0.0, 0.0])
    }

    Render.color([1.0, 0.5, 0.5])
    Render.line([0.1, 0.15, 0.1], [0.9, 0.15, 0.1], 1)
    Render.color([0.5, 1.0, 0.5])
    Render.square([0.875, 0.125, 0.2], 0.05)
    Render.color()
    Render.square([0.5, 0.5, 0.5], 0.5, _texture)
    Render.rect([0.25, 0.75, 0.5], [0.5, 0.1], _texture)

    Render.rect([0.5, 0.9, 0.1], [0.2, 0.1], _tilesTexture, _tiles.texCoord([0, 14], [4, 2]))

    Render.color()
    Render.text([0.1, 0.1, 0.1], "Hello, world of Wren!", 0.05)
    Render.text([0.1, 0.151, 0.1], "You launched the game %(_timer) seconds ago.", 0.025)
    Render.text([0.1, 0.176, 0.1], "Press %(_redBind.key()) to change the background color.", 0.025)
    Render.text([0.1, 0.202, 0.1], "Press %(_blueBind.key()) to enter the blue state.", 0.025)
    Render.text([0.1, 0.227, 0.1], "Press %(_exitBind.key()) to exit the game.", 0.025)
  }
}
