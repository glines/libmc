#version 130

uniform vec3 color;

varying vec2 texCoord;

void main() {
  float radius = 0.35;
  float border = 0.1;
  vec3 borderColor = vec3(0.4588f, 0.3686f, 0.3686f);
  float epsilon = 0.05;
  float dist = length(texCoord - vec2(0.5, 0.5));
  /*
  float weight =
    1.0 - smoothstep(
      radius - 0.5 * epsilon,
      radius + 0.5 * epsilon,
      dist);
      */
  if (dist > radius)
    discard;
  if (dist > radius - border)
    gl_FragColor = vec4(borderColor, 1.0f);
  else
    gl_FragColor = vec4(color, 1.0f);
}
