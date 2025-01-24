import "engine" for SubState, Render

class HalfRedSubState is SubState {
  construct new() {}

  render() {
    Render.color([1, 0, 0, 0.5])
    Render.rect([0, 0, 0.01], [1, 1])
  }
}