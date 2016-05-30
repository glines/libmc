#ifndef MCXX_ISOSURFACE_BUILDER_H_
#define MCXX_ISOSURFACE_BUILDER_H_

#include <vector>

extern "C" {
#include <mc/isosurfaceBuilder.h>
}

namespace mc {
  class Mesh;
  class ScalarField;
  class IsosurfaceBuilder {
    private:
      mcIsosurfaceBuilder m_internal;
      std::vector<Mesh*> m_meshes;
    public:
      IsosurfaceBuilder();
      ~IsosurfaceBuilder();

      const Mesh *buildIsosurface(
          mcScalarField sf,
          mcAlgorithmFlag algorithm,
          void *args = nullptr
          );

      const Mesh *buildIsosurface(
          const ScalarField &sf,
          mcAlgorithmFlag algorithm);
  };
}

#endif
