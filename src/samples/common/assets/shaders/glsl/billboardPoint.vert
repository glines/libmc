#version 130

attribute vec3 vertPosition;
attribute vec2 vertTexCoord;

uniform mat4 modelView;
uniform mat4 projection;

varying vec2 texCoord;

void main() {
  gl_Position = projection * modelView * vec4(vertPosition, 1.0);
  texCoord = vertTexCoord;
}
