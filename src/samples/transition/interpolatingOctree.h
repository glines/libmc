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

#ifndef MC_TRANSITION_INTERPOLATING_OCTREE_H_
#define MC_TRANSITION_INTERPOLATING_OCTREE_H_

#include <GL/glew.h>
#include <mcxx/scalarField.h>

#include "../common/octree.h"
#include "../common/sceneObject.h"

namespace mc { namespace samples { namespace transition {
  class InterpolatingNode : public OctreeNode<InterpolatingNode> {
    private:
      float m_samples[8];

      bool m_isValid();

    public:
      InterpolatingNode(
          InterpolatingNode *parent,
          int index);

      /**
       * Returns in world coordinates the position of this octree node's
       * volume. The position is given relative to the corner of the octree
       * node's volume with the lowest coordinate value. Octree coordinate
       * units are converted directly to world space units with a 1 to 1 ratio.
       *
       * \return Position in world coordinates where this node's volume is
       * located.
       */
      glm::vec3 worldPos() const;

      /**
       * Returns the size of a side of this node in terms of world space unit
       * length. The nodes are cube volumes, so each side has the same length.
       *
       * Since nodes double in size as one traverses up the octree, this size
       * depends on the octree level of the node. The octree is addressed with
       * integer octree coordinates, and one unit length in octree coordinates
       * in treated as one unit length in world space units.
       *
       * \return Size of one side of this node in terms of world space units.
       */
      float size() const;

      /**
       * Sets the sample at the given sample index of this node and recursively
       * updates the adjacent and child nodes that share this sample.
       *
       * \param value The value to set the sample.
       * \param sampleIndex The index of the sample within this node.
       */
      void setSample(float value, int sampleIndex = 0);

      /**
       * Returns the sample value for this node at the given sample index.
       *
       * \param sampleIndex The index of the sample to retrieve.
       * \return The sample value.
       */
      float sample(int sampleIndex) const { return m_samples[sampleIndex]; }
  };

  class InterpolatingOctree :
    public Octree<InterpolatingNode>,
    public SceneObject,
    public ScalarField
  {
    private:
      typedef struct WireframeVertex {
        float pos[3];
        float color[3];
      } WireframeVertex;
      typedef struct BillboardVertex {
        float pos[3];
        float tex[2];
      } BillboardVertex;
      GLuint m_cubeWireframeVertices, m_cubeWireframeIndices,
             m_billboardVertices, m_billboardIndices;

      float m_defaultSample;

      void m_generateCubeWireframe();
      void m_generateBillboard();
      void m_drawOctreeWireframe(
          const glm::mat4 &modelWorld,
          const glm::mat4 &worldView,
          const glm::mat4 &projection) const;
      void m_drawSamplePoints(
          const glm::mat4 &modelWorld,
          const glm::mat4 &worldView,
          const glm::mat4 &projection) const;

    public:
      /**
       * Constructs an interpolating octree object where the given sample value
       * is assumed for unspecified samples. The default, and recommended
       * value, for this sample is a positive value, and thus outside of the
       * volume contained by the isosurface.
       *
       * \fixme The defaultSample value is not actually used to set the defalut
       * samples of nodes at this time. It is hard-coded to 1.0f in the
       * InterpolatingNode class.
       */
      InterpolatingOctree(
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat(),
          float defaultSample = 1.0f);

      void setSample(const OctreeCoordinates &pos, float value);

      float operator()(float x, float y, float z);

      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug);
  };
} } }

#endif
