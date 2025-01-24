import "engine" for SubState, Render, KeyBind, Console

class GreenSubState is SubState {
  construct new() {
    _leaveBind = KeyBind.new("test.leave", "q") {
      SubState.pop()
    }
  }

  on(evt) {
    if(evt.isMouseUp) {
      Console.print("You have clicked in the Green Sub-State.")
    }
  }

  render() {
    Render.clear([0.0, 1.0, 0.0])
    Render.color([0.0])
    Render.text([0.1, 0.1, 0.1], "Welcome to the green sub-state.", 0.05)
    Render.text([0.1, 0.15, 0.1], "Press %(_leaveBind.key()) to leave.", 0.025)
  }
}
