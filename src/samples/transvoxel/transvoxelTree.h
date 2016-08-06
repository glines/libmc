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

#ifndef MC_SAMPLES_TRANSVOXEL_TRANSVOXEL_TREE_H_
#define MC_SAMPLES_TRANSVOXEL_TRANSVOXEL_TREE_H_

#include <glm/glm.hpp>

#include "../common/octree.h"

namespace mc { namespace samples { namespace transvoxel {
  class Frustum;
  class TransvoxelNode : public OctreeNode<TransvoxelNode> {
    private:
      bool m_isVisible;
    public:
      TransvoxelNode(TransvoxelNode *parent, int index);

      bool isVisible() const { return m_isVisible; }
      void setVisible(bool flag) { m_isVisible = flag; }

      glm::vec3 worldSpacePos() const;
      glm::vec3 worldSpaceCenter() const;
      float size() const;

      bool contains(const glm::vec3 &point) const;
  };

  class TransvoxelTree : public Octree<TransvoxelNode> {
    public:
      TransvoxelTree();
  };
} } }

#endif
