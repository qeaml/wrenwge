import "engine" for State, Render

class BlueState is State {
  construct new() {}

  render() {
    Render.clear([0.0, 0.0, 1.0])
    Render.color()
    Render.text([0.1, 0.1, 0.1], "Welcome to the blue state.", 0.05)
  }
}
