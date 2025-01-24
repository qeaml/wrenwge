import "engine" for Render, Texture

class TextureAtlas {
  construct new(texture, count) {
    _texture = texture
    _count = count
  }

  texCoord(loc, size) {
    return [
      loc[0] / _count[0],
      loc[1] / _count[1],
      size[0] / _count[0],
      size[1] / _count[1]
    ]
  }
}
