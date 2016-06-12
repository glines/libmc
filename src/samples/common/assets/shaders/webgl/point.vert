#version 130

attribute vec3 vertColor;
attribute vec3 vertPosition;

uniform mat4 modelView;
uniform mat4 projection;

varying vec3 color;

void main() {
  gl_Position = projection * modelView * vec4(vertPosition, 1.0);
  color = vertColor;
}
