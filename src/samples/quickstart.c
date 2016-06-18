#include <mc.h>

float sphere(float x, float y, float z) {
  static const float r = 1.0;
  return x*x + y*y + z*z - r*r;
}

int main(int argc, char **argv) {
  mcIsosurfaceBuilder ib;
  mcIsosurfaceBuilder_init(&ib);
  mcIsosurfaceBuilder_isosurfaceFromField(
      sphere);
}
