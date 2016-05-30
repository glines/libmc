#ifndef MCXX_MESH_H_
#define MCXX_MESH_H_

extern "C" {
#include <mc/mesh.h>
}

namespace mc {
  class Mesh {
    private:
      const mcMesh *m_internal;
    public:
      Mesh();
      Mesh(const Mesh &mesh);
      Mesh(const mcMesh *mesh);
  };
}

#endif
