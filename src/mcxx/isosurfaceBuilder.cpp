#include "../../include/mcxx/isosurfaceBuilder.h"
#include "../../include/mcxx/mesh.h"
#include "../../include/mcxx/scalarField.h"

namespace mc {
  IsosurfaceBuilder::IsosurfaceBuilder() {
  }

  IsosurfaceBuilder::~IsosurfaceBuilder() {
    for (auto mesh : m_meshes) {
      delete mesh;
    }
  }

  const Mesh *IsosurfaceBuilder::buildIsosurface(
      mcScalarField sf,
      mcAlgorithmFlag algorithm,
      void *args)
  {
    const mcMesh *m = mcIsosurfaceBuilder_isosurfaceFromField(
        &m_internal, sf, algorithm);
    Mesh *mesh = new Mesh(m);
    m_meshes.push_back(mesh);
    return mesh;
  }

  const Mesh *IsosurfaceBuilder::buildIsosurface(
      const ScalarField &sf,
      mcAlgorithmFlag algorithm)
  {
  }
}
