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

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "terrainMesh.h"

#include "lodTree.h"

namespace mc { namespace samples { namespace terrain {
  LodTree::LodTree()
  {
    // The root of the tree always straddles the origin. Since the center of
    // the root node is the origin, the root node precisely divides space up
    // into the eight octants, and no node besides the root can occupy more
    // than one octant.
    //
    // The initial root block is positioned at (-1, -1, -1) at level of detail
    // 1 such that it contains the cubical volume from (-1, -1, -1) to
    // (1, 1, 1).
    Coordinates block;
    block.x = -1;
    block.y = -1;
    block.z = -1;
    m_root = std::shared_ptr<Node>(
        new Node(block, 1, nullptr));
  }

  void LodTree::m_grow(const Coordinates &block, int lod) {
    assert(!m_root->contains(block, lod));  // We shouldn't need to check twice
    do {
      assert(m_root->lod() < 31);  // Don't get carried away
      // We double the size of the root node along each dimension. Since the
      // root node divides the octants of 3D space at the origin, no child of
      // the root node can occupy more than one octant. Thus, Growing the root
      // node is as simple as adding the root node's children to even larger
      // children and decreasing the LOD of the root node.
      m_root->m_lod += 1;
      m_root->m_block.x = m_root->m_block.y = m_root->m_block.z
        = -(1 << (m_root->lod() - 1));
      for (int i = 0; i < 8; ++i) {
        auto oldChild = m_root->child(i);
        if (!oldChild)
          continue;
        Coordinates newBlock;
        m_root->m_childIndexBlock(i, &newBlock);
        auto newChild = std::shared_ptr<Node>(
            new Node(newBlock, m_root->lod() - 1, m_root.get()));
        m_root->m_children[i] = newChild;
        int index = newChild->m_childIndexContainingBlock(oldChild->block());
        newChild->m_children[index] = oldChild;
        oldChild->m_parent = newChild.get();
      }
    } while (!m_root->contains(block, lod));
  }

  std::shared_ptr<LodTree::Node> LodTree::getNode(
      const LodTree::Coordinates &block, int lod)
  {
    if (!m_root) {
      // Since the octree is empty, we can add this node as the root node. When
      // more nodes are added, the octree growing algorithms will naturally
      // adjust the root node of the tree.
      m_root = std::shared_ptr<Node>(
          new Node(block, lod, nullptr));
    }
    if (!m_root->contains(block, lod)) {
      // The given block and lod are not contained in the octree root, so we
      // must grow the octree and give it a new root node.
      this->m_grow(block, lod);
    }
    if (lod == m_root->lod()) {
      // The node we are adding happens to be the root node
      assert(memcmp(&block, &m_root->block(), sizeof(block)) == 0);
      return m_root;
    }
    // The getChild() method for LodTree::Node objects will automatically
    // create the node if it does not exist yet.
    auto node = m_root->getChild(block, lod);
    assert(node);
    return node;
  }

  LodTree::Node::Node(const Coordinates &block, int lod, Node *parent)
    : m_block(block), m_lod(lod), m_parent(parent)
  {
  }

  glm::vec3 LodTree::Node::pos() {
    glm::vec3 result;
    result.x = (float)(m_block.x * TerrainMesh::BLOCK_SIZE) *
      TerrainMesh::VOXEL_DELTA;
    result.y = (float)(m_block.y * TerrainMesh::BLOCK_SIZE) *
      TerrainMesh::VOXEL_DELTA;
    result.z = (float)(m_block.z * TerrainMesh::BLOCK_SIZE) *
      TerrainMesh::VOXEL_DELTA;
    return result;
  }

  float LodTree::Node::size() {
    return (float)((1 << m_lod) * TerrainMesh::BLOCK_SIZE) *
      TerrainMesh::VOXEL_DELTA;
  }

  int LodTree::Node::m_childIndexContainingBlock(const Coordinates &block) {
    // We assume that the given block is contained in one of our child nodes.
    // We can find the index of that child node by leveraging the fact that
    // nodes at each level of detail are aligned to powers of 2.
    //
    // For level of detail n, exclunding level of detail 0, the nth lowest
    // order bit in the coordinate integer determines the position of this
    // block within this node, and thus determines the child index. Thus, for
    // level of detail 0, the block must exist within this node (i.e. block ==
    // m_block) since this node cannot have any children. For level of detail 1,
    // the 1st lowest order bit determines the position, which makes sense
    // because our nodes are 2^1 blocks cubed = 8 blocks. For level of detail
    // 2, the 2nd bit determines the positon, etc.
    //
    // It should be noted that this scheme works equally well with negative
    // integers, since two's complement integers use the same arithmetic
    // operation for addition for both negative and positive integers.
    int index = 0;
#define INDEX_FROM_BLOCK(i, b) \
    i |= ((b.x >> (m_lod - 1)) & 1) << 0; \
    i |= ((b.y >> (m_lod - 1)) & 1) << 1; \
    i |= ((b.z >> (m_lod - 1)) & 1) << 2;
    if (m_parent == nullptr) {
      // The root node needs special treatment because it straddles the origin
      Coordinates adjBlock;
      assert(m_block.x == -(1 << (m_lod - 1)));
      assert(m_block.y == -(1 << (m_lod - 1)));
      assert(m_block.z == -(1 << (m_lod - 1)));
      adjBlock.x = block.x - m_block.x;
      adjBlock.y = block.y - m_block.y;
      adjBlock.z = block.z - m_block.z;
      INDEX_FROM_BLOCK(index, adjBlock);
    } else {
      INDEX_FROM_BLOCK(index, block);
    }

    // If this assert fails, it is probable that the block passed is not
    // contained by this node, i.e. garbage in garbage out.
    assert(index >= 0 && index < 8);

    return index;
  }

  void LodTree::Node::m_childIndexBlock(
      int index, Coordinates *block)
  {
    // We determine the absolute block position for our child at the given
    // index. This calculation depends on this node's absolute block position
    // as well as the level of detail.
    block->x = m_block.x + (index & (1 << 0) ? 1 << (m_lod - 1) : 0);
    block->y = m_block.y + (index & (1 << 1) ? 1 << (m_lod - 1) : 0);
    block->z = m_block.z + (index & (1 << 2) ? 1 << (m_lod - 1) : 0);
  }

  std::shared_ptr<LodTree::Node> LodTree::Node::m_createChild(
      int index)
  {
    assert(!m_children[index]);  // Don't overwrite any children
    assert(m_lod > 0);  // Level of detail 0 cannot have children
    // Calculate the block position of our child node
    Coordinates block;
    m_childIndexBlock(index, &block);
    m_children[index] = std::shared_ptr<Node>(
        new Node(
          block,  // block position
          m_lod - 1,  // level of detail
          this  // parent
          ));
    return m_children[index];
  }

  std::shared_ptr<LodTree::Node> LodTree::Node::nextSibling() const {
    if (!m_parent)
      return nullptr;  // The root node has no siblings
    int i = 0;
    // Determine this node's index in it's parent's children array
    // NOTE: This index could also be determined from the level of
    // detail and voxel block coordinates
    for (; i < 8; ++i) {
      if (m_parent->m_children[i].get() == this) {
        ++i;
        break;
      }
    }
    // Look for our next sibling
    for (; i < 8; ++i) {
      if (m_parent->m_children[i]) {
        return m_parent->m_children[i];
      }
    }
    // This node is the last sibling; it has no next sibling
    return nullptr;
  }

  bool LodTree::Node::contains(
      const Coordinates &block, int lod) const
  {
    // Make sure this block/lod combination is valid
    assert((((1 << lod) - 1) & labs(block.x)) == 0);
    assert((((1 << lod) - 1) & labs(block.y)) == 0);
    assert((((1 << lod) - 1) & labs(block.z)) == 0);
    if (lod > m_lod) {
      // The target is larger than this node
      return false;
    } else if (lod == m_lod) {
      // The target is the same size as this node
      if (memcmp(&m_block, &block, sizeof(block)) != 0) {
        return false;
      }
    } else {
      // The target is smaller than this node and might be contained within
      // this node
      if (block.x < m_block.x)
        return false;
      if (block.y < m_block.y)
        return false;
      if (block.z < m_block.z)
        return false;
      if (block.x >= m_block.x + (1 << m_lod))
        return false;
      if (block.y >= m_block.y + (1 << m_lod))
        return false;
      if (block.z >= m_block.z + (1 << m_lod))
        return false;
    }
    return true;
  }

  std::shared_ptr<LodTree::Node> LodTree::Node::getChild(
      const Coordinates &block, int lod)
  {
    assert(this->contains(block, lod));
    assert(lod < m_lod);
    // Determine which of our children this block belongs to
    int index = this->m_childIndexContainingBlock(block);
    std::shared_ptr<Node> child = m_children[index];
    if (!child) {
      // Create children as necessary
      child = m_createChild(index);
    }
    if (child->m_lod == lod) {
      assert(memcmp(&child->m_block, &block, sizeof(block)) == 0);
      // The base case, where our child is the node we are looking for
      return child;
    }
    // Recursive call to find the node
    return child->getChild(block, lod);
  }

  std::shared_ptr<LodTree::Node> LodTree::Node::getRelativeNode(
      const Coordinates &offset)
  {
    // TODO
    return nullptr;
  }

  LodTree::Node::Iterator &LodTree::Node::Iterator::operator++() {
    // Look for our first child
    for (int i = 0; i < 8; ++i) {
      std::shared_ptr<Node> child = m_current->m_children[i];
      if (child) {
        m_current = child.get();
        return *this;
      }
    }
    // Check for a parent
    Node *parent = m_current->parent();
    if (!parent) {
      // We're done
      // NOTE: This only happens with an octree consisting of a single node
      m_current = nullptr;
      return *this;
    }
    // Look for our next sibling
    std::shared_ptr<Node> sibling = m_current->nextSibling();
    if (sibling) {
      m_current = sibling.get();
      return *this;
    }
    // Recursively look for siblings of our parent
    do {
      std::shared_ptr<Node> uncle = parent->nextSibling();
      if (uncle) {
        m_current = uncle.get();
        return *this;
      }
      parent = parent->parent();
    } while (parent);
    // We recursed past the root node, so we're done
    m_current = nullptr;
    return *this;
  }
} } }
