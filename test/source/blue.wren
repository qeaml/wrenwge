import "engine" for State, SubState, Render, KeyBind, Bundle, AudioSource, AudioBuffer
import "green" for GreenSubState
import "red" for HalfRedSubState

class BlueState is State {
  construct new() {
    _bundle = Bundle.new("test-assets")

    _audioBuffer = AudioBuffer.new()
    _audioSource = AudioSource.new()
    _audioSource.buffer(_audioBuffer)
    _bundle.nqAudio("Yay.mp3", _audioBuffer)

    _greenBind = KeyBind.new("test.green", "g") {
      SubState.push(GreenSubState.new())
    }
    _purpleBind = KeyBind.new("test.purple", "p") {
      SubState.push(HalfRedSubState.new(), {"renderParent": true})
    }
    _playBind = KeyBind.new("test.play", "space") {
      _audioSource.play()
    }
  }

  render() {
    Render.clear([0.0, 0.0, 1.0])
    Render.color()
    Render.text([0.1, 0.1, 0.1], "Welcome to the blue state.", 0.05)
    Render.text([0.1, 0.15, 0.1], "Press %(_greenBind.key()) to become green.", 0.025)
    Render.text([0.1, 0.175, 0.1], "Press %(_purpleBind.key()) to become purple.", 0.025)
    Render.text([0.1, 0.2, 0.1], "Press %(_playBind.key()) to play a sound.", 0.025)
  }
}
