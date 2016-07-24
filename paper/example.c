#include <mcxx/mcxx.h>
#include <cmath>

float terrain(float x, float y, float z) {
  return z - cos(x) * sin(y);
}

int main() {
  mc::IsosurfaceBuilder ib;
  auto mesh = ib.buildIsosurface(
    terrain,  // scalar field
    MC_NIELSON_DUAL,  // algorithm
    10, 10, 10,  // resolution
    mc::Vec3(-1.0f, -1.0f, -1.0f),  // min
    mc::Vec3(1.0f, 1.0f, 1.0f)  // max
    );
  // ... use the mesh ...
  return 0;
}
