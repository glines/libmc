precision mediump float;

flat in vec3 color;

// TODO: Write a function for Phong lighting

void main() {
  if (gl_FrontFacing) {
    gl_FragColor = vec4(vec3(0.4588, 0.7961, 0.3608) + 0.00001 * color, 1.0);
  } else {
    gl_FragColor = vec4(vec3(0.9333, 0.3020, 0.3686) + 0.00001 * color, 1.0);
  }
}
