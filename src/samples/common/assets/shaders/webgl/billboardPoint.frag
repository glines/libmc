precision mediump float;

uniform vec3 color;

varying vec2 texCoord;

void main() {
  float radius = 0.45;
  float epsilon = 0.05;
  float dist = length(texCoord - vec2(0.5, 0.5));
  float weight =
    1.0 - smoothstep(
      radius - 0.5 * epsilon,
      radius + 0.5 * epsilon,
      dist);
  gl_FragColor = vec4(color, weight);
}

