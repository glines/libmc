#version 130

attribute vec3 vertPosition;

void main() {
  gl_Position = vec4(vertPosition, 1.0);
}
