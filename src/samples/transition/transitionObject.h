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

#ifndef MC_SAMPLES_TRANSITION_TRANSITION_OBJECT_H_
#define MC_SAMPLES_TRANSITION_TRANSITION_OBJECT_H_

#include <GL/glew.h>

#include "../common/meshObject.h"

namespace mc { namespace samples {
  class ShaderProgram;
  namespace transition {
    /**
     * This scene object class demonstrates the transition between voxel meshes
     * of different resolutions. This is particularly useful when developing
     * algorithms such as Eric Lengyel's Transvoxel algorithm.
     *
     * Since voxel transitions involve much more complicated scenarios than with
     * ordinary marching cubes, the TransitionObject class cannot be configured
     * with a simple bitwise specification of the scalar field function as with
     * the CubeObject class. Instead, the TransitionObject class uses an octree
     * specification for representing the scalar field function that models the
     * different transition scenarios. Corners of each node in the octree are
     * given scalar field values, typically -1.0f or 1.0f.  These values are
     * interpolated within the volume of the octree in order to calculate the
     * scalar field function values.
     */
    class TransitionObject : public MeshObject {
      private:
        typedef struct {
          float pos[3];
          float color[3];
        } WireframeVertex;
        typedef struct {
          float pos[3];
          float tex[3];
        } BillboardVertex;

        GLuint m_billboardVertices, m_billboardIndices,
               m_transitionCellWireframeVertices,
               m_transitionCellWireframeIndices;
        int m_cell;

        void m_generateBillboard();
        void m_generateTransitionCellWireframe();

        void m_update();

        void m_drawSamplePoint(
            float x, float y, float z,
            float value,
            std::shared_ptr<ShaderProgram> shader,
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;
        void m_drawSamplePoints(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;
        void m_drawTransitionCellWireframe(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;
      public:
        TransitionObject(
            int cell = 0x001,
            const glm::vec3 &position = glm::vec3(0.0f),
            const glm::quat &orientation = glm::quat());

        /**
         * \return The transition cell configuration that this transition
         * object represents.
         */
        int cell() const { return m_cell; }
        /**
         * Sets the transition cell configuration that this transition object
         * represents and updates the mesh and samples drawn accordingly.
         *
         * \param cell The transition cell configuration to set.
         */
        void setCell(int cell);

        void draw(const glm::mat4 &modelWorld,
            const glm::mat4 &worldView, const glm::mat4 &projection,
            float alpha, bool debug);
    };
  }
} }

#endif
