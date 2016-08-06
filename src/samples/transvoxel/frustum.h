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

#include <glm/glm.hpp>

extern "C" {
#include <mc/algorithms/common/cube_definitions.h>
}

namespace mc { namespace samples {
  class PerspectiveCamera;
  namespace transvoxel {
    class TransvoxelNode;
    class Frustum {
      private:
        glm::mat4 m_worldView, m_projection, m_viewWorld, m_wvp, m_pvw;
        float m_focalLength, m_aspect, m_fovy;
      public:
        enum class FrustumPlane {
          NEAR = MC_CUBE_FACE_FRONT,
          RIGHT = MC_CUBE_FACE_LEFT,
          TOP = MC_CUBE_FACE_TOP,
          BOTTOM = MC_CUBE_FACE_BOTTOM,
          LEFT = MC_CUBE_FACE_RIGHT,
          FAR = MC_CUBE_FACE_BACK,
        };

        Frustum(const PerspectiveCamera &camera, float aspect);

        /**
         * This method returns the position in world space coordinates of the
         * frustum vertex with the given index. These frustum vertices are
         * enumerated in the same order that cube sample indices use within
         * libmc.
         *
         * \param index The index of the frustum vertex.
         * \return Position in world space coordinates of the frustum vertex with
         * the given index.
         */
        glm::vec3 vertexPosition(int index) const;

        bool testVisibility(const TransvoxelNode &node) const;
        float projectedSize(const TransvoxelNode &node) const;
    };
  }
} }
