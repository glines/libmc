#include <mc/vector.h>

mcVec3 mcVec3_lerp(const mcVec3 *u, const mcVec3 *v, float a) {
  mcVec3 result;
  result.x = (1.0f - a) * u->x + a * v->x;
  result.y = (1.0f - a) * u->y + a * v->y;
  result.z = (1.0f - a) * u->z + a * v->z;
  return result;
}
