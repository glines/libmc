#include "../../include/mcxx/mesh.h"

namespace mc {
  Mesh::Mesh() : m_internal(nullptr) {
  }

  Mesh::Mesh(const Mesh &mesh) {
  }

  Mesh::Mesh(const mcMesh *mesh) : m_internal(mesh) {
  }
}
