/*
 * Copyright (c) 2016 Jonathan Glines
 * Jonathan Glines <jonathan@glines.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef MCXX_MESH_H_
#define MCXX_MESH_H_

/**
 * \addtogroup libmcxx
 * @{
 */

/** \file mcxx/mesh.h
 *
 * This header contains the C++ wrapper around the mcMesh data structure from
 * libmc.
 */

extern "C" {
#include <mc/mesh.h>
}

namespace mc {
  /**
   * The Mesh class is a C++ wrapper for the mcMesh data structure from the C API
   * of libmc.
   */
  class Mesh {
    private:
      const mcMesh *m_internal;
    public:
      Mesh();
      Mesh(const Mesh &mesh);
      Mesh(const mcMesh *mesh);

      unsigned int numVertices() const { return m_internal->numVertices; }
      const mcVertex &vertex(unsigned int i) const { return m_internal->vertices[i]; }

      unsigned int numFaces() const { return m_internal->numFaces; }
      const mcFace &face(unsigned int i) const { return m_internal->faces[i]; }

      unsigned int numIndices() const { return m_internal->numIndices; }

      bool isTriangleMesh() const { return m_internal->isTriangleMesh; }
  };
}

/** @} */

#endif
