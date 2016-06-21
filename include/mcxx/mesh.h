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

      /**
       * Constructs an empty mesh.
       *
       * \todo I'm not sure if this constructor is being used.
       */
      Mesh();

      /**
       * Copy constructor for a mesh object.
       */
      Mesh(const Mesh &mesh);

      /**
       * Constructor which wraps an mcMesh structure.
       */
      Mesh(const mcMesh *mesh);

      /**
       * Returns the number of vertices in this mesh.
       */
      unsigned int numVertices() const { return m_internal->numVertices; }
      /**
       * Accesses the mesh vertex at the given vertex index.
       */
      const mcVertex &vertex(unsigned int i) const { return m_internal->vertices[i]; }

      /**
       * Returns the number of faces in this mesh.
       */
      unsigned int numFaces() const { return m_internal->numFaces; }
      /**
       * Accesses the mesh face at the given face index.
       */
      const mcFace &face(unsigned int i) const { return m_internal->faces[i]; }

      /**
       * Returns the total number of vertex indices used in the mesh faces.
       * This value is useful for quickly determining the number of indices
       * needed for indexed mesh rendering.
       *
       * \return The total number of vertex indices referred by faces in this
       * mesh.
       */
      unsigned int numIndices() const { return m_internal->numIndices; }

      /**
       * Returns true if all of the faces in this mesh refer to exactly three
       * vertex indices each. This category of triangle mesh is ideal for
       * drawing with many raster graphics API's.
       *
       * \return Whether or not this mesh is a triangle mesh.
       */
      bool isTriangleMesh() const { return m_internal->isTriangleMesh; }
  };
}

/** @} */

#endif
