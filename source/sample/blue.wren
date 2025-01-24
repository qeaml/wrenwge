import "engine" for State, SubState, Render, KeyBind
import "green" for GreenSubState

class BlueState is State {
  construct new() {
    _greenBind = KeyBind.new("test.green", "g") {
      SubState.push(GreenSubState.new())
    }
  }

  render() {
    Render.clear([0.0, 0.0, 1.0])
    Render.color()
    Render.text([0.1, 0.1, 0.1], "Welcome to the blue state.", 0.05)
    Render.text([0.1, 0.15, 0.1], "Press %(_greenBind.key()) to become green.", 0.025)
  }
}
