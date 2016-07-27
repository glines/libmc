#version 130

flat in vec3 color;

// TODO: Write a function for Phong lighting

void main() {
  gl_FragColor = vec4(color, 1.0);
}
