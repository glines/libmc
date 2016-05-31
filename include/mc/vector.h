#ifndef MC_VECTOR_H_
#define MC_VECTOR_H_

typedef struct mcVec3 {
  float x, y, z;
} mcVec3;

mcVec3 mcVec3_lerp(const mcVec3 *u, const mcVec3 *v, float a);

#endif
