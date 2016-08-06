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

#include "common.h"
#include "transvoxelTree.h"

namespace mc { namespace samples { namespace transvoxel {
  TransvoxelNode::TransvoxelNode(TransvoxelNode *parent, int index)
    : OctreeNode<TransvoxelNode>(parent, index), m_isVisible(false)
  {
  }

  glm::vec3 TransvoxelNode::worldSpacePos() const {
    /* Compute the world space position of this node */
    glm::vec3 result;
    result.x = (float)(this->pos().x * BLOCK_SIZE) * VOXEL_DELTA;
    result.y = (float)(this->pos().y * BLOCK_SIZE) * VOXEL_DELTA;
    result.z = (float)(this->pos().z * BLOCK_SIZE) * VOXEL_DELTA;
    /*
    fprintf(stderr,
        "BLOCK_SIZE: %d\n"
        "VOXEL_DELTA: %g\n"
        "this->pos(): (%d, %d, %d)",
        BLOCK_SIZE,
        VOXEL_DELTA,
        this->pos().x,
        this->pos().y,
        this->pos().z);
    fprintf(stderr, "worldSpacePos: (%d, %d, %d)\n",
        result.x,
        result.y,
        result.z);
        */
    return result;
  }

  glm::vec3 TransvoxelNode::worldSpaceCenter() const {
    glm::vec3 result;
    result.x = this->worldSpacePos().x + 0.5f * this->size();
    result.y = this->worldSpacePos().y + 0.5f * this->size();
    result.z = this->worldSpacePos().z + 0.5f * this->size();
    return result;
  }

  float TransvoxelNode::size() const {
    /* Compute the world space size of this node */
    return (float)((1 << this->level()) * BLOCK_SIZE) * VOXEL_DELTA;
  }

  bool TransvoxelNode::contains(const glm::vec3 &point) const {
    /* Determine if the given point is contained within the world space volume
     * of this node */
#define CHECK_AXIS(axis) \
    do { \
      if (point.axis < this->worldSpacePos().axis) \
        return false; \
      if (point.axis > this->worldSpacePos().axis + this->size()) \
        return false; \
    } while (0)
    CHECK_AXIS(x);
    CHECK_AXIS(y);
    CHECK_AXIS(z);
    return true;
  }

  TransvoxelTree::TransvoxelTree() {
  }
} } }
