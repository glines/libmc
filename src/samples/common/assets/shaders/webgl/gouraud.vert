attribute vec3 vertPosition;
attribute vec3 vertNormal;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat4 modelViewProjection;
uniform mat4 normalTransform;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

varying vec3 color;

void main() {
  // FIXME: transform the vertex normal by the inverse of the transpose of... something
  vec4 eyeNormal = normalize(normalTransform * vec4(vertNormal, 0.0));
  vec4 eyeCoordinates = modelView * vec4(vertPosition, 1.0);
  vec3 lightVector = normalize(lightPosition - vec3(eyeCoordinates));

  // Diffuse shading
  // TODO: add surface diffuse reflectivity
  float ambient = 0.4;
  color = lightIntensity * max(dot(lightVector, vec3(eyeNormal)), 0.0)
          + ambient;

  gl_Position = modelViewProjection * vec4(vertPosition, 1.0);
}
