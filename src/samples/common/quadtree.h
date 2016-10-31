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

#ifndef MC_SAMPLES_COMMON_QUADTREE_H_
#define MC_SAMPLES_COMMON_QUADTREE_H_

namespace mc { namespace samples {
  typedef struct QuadtreeCoordinates {
    int x, y;
  } QuadtreeCoordinates;

  // NOTE: Curiously recurring template paramater pattern used here
  template <class N>
  class QuadtreeNode {
    private:
      N *m_parent;
      std::shared_ptr<N> m_children[4];
      QuadtreeCoordinates m_pos;
      int m_level, m_index;

      void m_childIndexToPos(int index, QuadtreeCoordinates *pos);
      std::shared_ptr<N> m_createChild(int index);
    public:
      // FIXME: This is public because we can no longer friend the quadtree
      // class.
      int m_childIndexContainingPos(const QuadtreeCoordinates &pos) const;

      class Iterator {
        friend QuadtreeNode;
        private:
          const N *m_current;

          Iterator(const N *current) : m_current(current) {}
        public:
          /**
           * Overloaded pre-increment operator so that this iterator class can
           * be used as an iterator. This method moves the iterator to the next
           * node in the quadtree. Node ordering is carried out depth-first.
           */
          Iterator &operator++();

          const N &operator*() {
            return *m_current;
          }

          bool operator==(const Iterator &other) {
            return this->m_current == other.m_current;
          }

          bool operator!=(const Iterator &other) {
            return !(*this == other);
          }
        private:
      };

      QuadtreeNode(N *parent, int index);

      /**
       * Returns the position of this node in quadtree coordinates. The position
       * of a node is measured relative to the corner of the node volume with
       * the lowest coordinate value.
       *
       * \return Quadtree coordinates of this node's position.
       */
      const QuadtreeCoordinates &pos() const { return m_pos; }
      void setPos(const QuadtreeCoordinates &pos) {
        memcpy(&m_pos, &pos, sizeof(pos));
      }

      /** Returns the quadtree level of this node. The quadtree level with the
       * highest resolution is level 0, and greater values of the quadtree level
       * have lower resolution with larger nodes. The quadtree level thus
       * determines the size of the nodes, with smaller levels having smaller
       * nodes further down the quadtree.
       *
       * \return The quadtree level of this node.
       */
      int level() const { return m_level; }
      void setLevel(int level) { m_level = level; }

      /**
       * This method accesses the child of this node at the given index.  Since
       * it is not required for the quadtree to be complete, it is possible that
       * this method will return a null pointer indicating no child nodes at
       * the given index.
       *
       * \param index Index of the child node to access.
       * \return Shared pointer to the child of this node at the given index.
       */
      std::shared_ptr<N> child(int index) const {
        return m_children[index];
      }
      void setChild(int index, std::shared_ptr<N> child) {
        m_children[index] = child;
      }

      void setIndex(int index) { m_index = index; }

      std::shared_ptr<N> nextSibling() const;

       bool hasChild(int index) const {
        return m_children[index].get() != nullptr;
      }

       /**
        * Returns the child node at the given child index. If no child
        * exists at the given index yet, then a child is created at that
        * index before it is returned.
        *
        * \param index Index of the child node to return.
        * \return Shared pointer to the child at the given index.
        */
      std::shared_ptr<N> getChild(int index);

      /**
       * Return the descendant node at the given quadtree coordinates with the
       * given quadtree level. If such a node does not already exist, then it
       * is created.
       *
       * \param pos The coordinates of the node to look for.
       * \param level The quadtree level to look for the node.
       * \return Pointer to the node at the given quadtree coordinates and
       * quadtree level.
       */
      std::shared_ptr<N> getDescendant(
          const QuadtreeCoordinates &pos, int level);

      /**
       * Determine if the volume with the given position and quadtree level
       * would be contained within this node.
       *
       * \param pos The quadtree coordinates to check.
       * \param level The quadtree level to check.
       * \return True if the volume with the given position and quadtree level
       * wolud be contained within this node.
       */
      bool contains(const QuadtreeCoordinates &pos, int level) const;

      /**
       * Returns the node at the same level of the quadtree as this node at the
       * given offset. This offset is given in terms of nodes at this level of
       * the quadtree, i.e. an offset of 2 would refer to a node 2 nodes over,
       * not two quadtree units over.
       *
       * If the node at the offset does not exist, then a null pointer is
       * returned.
       *
       * \param node The originating node of which to find a relative node.
       * \param offset The offset given in terms of nodes at the same level of
       * the quadtree adjacent to the given node node. 
       * \return Pointer to the node at the offset relative to this node.
       */
      N *relativeNode(const QuadtreeCoordinates &offset);

      /**
       * Returns an iterator starting at this quadtree node for iterating over
       * this node and all of its children.
       *
       * If this node is a null pointer, which signifies an empty quadtree,
       * then this method returns an iterator referencing a null pointer as
       * well.
       *
       * \return An iterator refering to this node.
       */
      Iterator begin() const {
        return Iterator(static_cast<const N*>(this));
      }

      /**
       * Returns an iterator representing the end of the subtree defined by
       * this node.
       *
       * The iterator returned by this method actually points to the parent of
       * this node, but since this node might be the root of the quadtree it is
       * important not to iterate past the iterator returned by this method.
       * Iterating past this iterator is undefined behavior.
       */
      Iterator end() const {
        return Iterator(this->parent());
      }

      /**
       * \return A pointer to the parent node of this node.
       */
      N *parent() const { return m_parent; }
      void setParent(N* parent) { m_parent = parent; }

      /**
       * This method is used to check whether or not this node's quadtree
       * position is aligned to the lattice grid for this node's level in the
       * quadtree. All nodes (other than the root node, which straddles the
       * origin) should be aligned to the quadtree lattice grid in order for
       * the quadtree to work correctly.
       *
       * \return True if this node is aligned to the lattice grid for this
       * node's level in the quadtree, false otherwise.
       */
      bool isAligned() const;
  };

  template <class N>
    class Quadtree {
      public:
      private:
        std::shared_ptr<N> m_root;

        void m_grow(
            const QuadtreeCoordinates &pos,
            int level);
        void m_growRoot();
        void m_alignPosToLevel(
            const QuadtreeCoordinates &pos,
            int level,
            QuadtreeCoordinates *alignedPos);

      public:
        /**
         * Constructs an empty quadtree structure.
         */
        Quadtree();

        std::shared_ptr<N> root() const { return m_root; }

        void growRoot() { m_growRoot(); }

        /**
         * This method looks for an quadtree node that is located at the given
         * coordinates. Node coordinates are given by the corner of the node
         * volume. More than one node may be located at this position, so this
         * method returns the highest node in the quadtree with this position,
         * such that all other nodes with the same position are children of the
         * returned node.
         *
         * If no such node exists, then it is created.
         *
         * \param pos The position of the node to find in quadtree coordinates.
         * \param lod The quadtree level at which the node resides, with the
         * default being the very bottom of the quadtree for the highest
         * resolution.
         */
        std::shared_ptr<N> getNode(
            const QuadtreeCoordinates &pos, int level = 0);

        /**
         * Returns the node at the same level of detail as the given node at
         * the given offset. This offset is given in terms of nodes at this
         * level of detail, i.e. an offset of 2 would refer to a node 2 nodes
         * over, not two quadtree units over. If the node at the offset does
         * not exist, then it and its parent nodes are created.
         *
         * \param node The originating node of which to find a relative node.
         * \param offset The offset given in terms of nodes at the same level of
         * detail adjacent to the given node node. 
         * \return Shared pointer to the node at the offset relative to this
         * node.
         */
        std::shared_ptr<N> getRelativeNode(
            const N &node, const QuadtreeCoordinates &offset);

        /**
         * Returns an iterator starting at the quadtree root for iterating over
         * the quadtree. This method simply leverages the N::begin() method on
         * the root node, which implements the desired behavior.
         *
         * \return Iterator at the begining of the quadtree.
         */
        typename N::Iterator begin() const {
          assert(m_root);  // We always have a root node
          return m_root->begin();
        }

        /**
         * Returns an iterator signifying the end of the quadtree. This
         * iterator always contains a null pointer, since the root node does
         * not have any parent node. This method simply leverages the N::end()
         * method on the root node, which implements the desired behavior.
         *
         * \return Iterator at the end of the quadtree.
         */
        typename N::Iterator end() const {
          assert(m_root);  // We always have a root node
          return m_root->end();
        }
    };

  template <class N>
    void QuadtreeNode<N>::m_childIndexToPos(
        int index,
        QuadtreeCoordinates *pos)
    {
      // We determine the absolute quadtree position for our child at the given
      // index. This calculation depends on this node's absolute quadtree
      // position as well as its quadtree level.
      pos->x = m_pos.x + (index & (1 << 0) ? 1 << (m_level - 1) : 0);
      pos->y = m_pos.y + (index & (1 << 1) ? 1 << (m_level - 1) : 0);
    }

  template <class N>
    std::shared_ptr<N> QuadtreeNode<N>::m_createChild(
        int index)
    {
      assert(!m_children[index]);  // Don't overwrite any children
      assert(m_level > 0);  // Quadtree level 0 cannot have children
      m_children[index] = std::shared_ptr<N>(
          new N(
            static_cast<N*>(this),  // parent
            index  // index
            ));
      return m_children[index];
    }

  template <class N>
    int QuadtreeNode<N>::m_childIndexContainingPos(
        const QuadtreeCoordinates &pos) const
    {
      // We assume that the given position is contained in one of our child
      // nodes.  We can find the index of that child node by leveraging the
      // fact that nodes at each quadtree level are aligned to powers of 2.
      //
      // For quadtree level n, exclunding quadtree level 0, the nth lowest
      // order bit in the coordinate integer determines the position of this
      // block within this node, and thus determines the child index.
      // Furthermore, for quadtree level 0, the node must exist within this
      // node (i.e. pos == m_pos) since this node cannot have any children at
      // quadtree level 0. For quadtree level 1, the 1st lowest order bit
      // determines the position, which makes sense because our nodes are 2^1
      // quadtree units cubed = 8 unit cubes.  For level of detail 2, the 2nd
      // bit determines the positon, etc.
      //
      // It should be noted that this scheme works equally well with negative
      // integers, since two's complement integers use the same arithmetic
      // operation for addition for both negative and positive integers.
      int index = 0;
#define INDEX_FROM_BLOCK(i, b) \
      i |= ((b.x >> (m_level - 1)) & 1) << 0; \
      i |= ((b.y >> (m_level - 1)) & 1) << 1;
      if (m_parent == nullptr) {
        // The root node needs special treatment because it straddles the
        // origin
        QuadtreeCoordinates adjPos;
        assert(m_pos.x == -(1 << (m_level - 1)));
        assert(m_pos.y == -(1 << (m_level - 1)));
        adjPos.x = pos.x - m_pos.x;
        adjPos.y = pos.y - m_pos.y;
        INDEX_FROM_BLOCK(index, adjPos);
      } else {
        INDEX_FROM_BLOCK(index, pos);
      }

      // If this assert fails, it is probable that the position passed is not
      // contained by this node, i.e. garbage in garbage out.
      assert(index >= 0 && index < 4);

      return index;
    }

  template <class N>
    typename QuadtreeNode<N>::Iterator &QuadtreeNode<N>::Iterator::operator++() {
      // Look for our first child
      for (int i = 0; i < 4; ++i) {
        auto child = m_current->m_children[i];
        if (child) {
          m_current = child.get();
          return *this;
        }
      }
      // Check for a parent
      auto *parent = m_current->parent();
      if (!parent) {
        // We're done
        // NOTE: This only happens with an quadtree consisting of a single node
        m_current = nullptr;
        return *this;
      }
      // Look for our next sibling
      auto sibling = m_current->nextSibling();
      if (sibling) {
        m_current = sibling.get();
        return *this;
      }
      // Recursively look for siblings of our parent
      do {
        auto uncle = parent->nextSibling();
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

  template <class N>
    QuadtreeNode<N>::QuadtreeNode(N *parent, int index)
      : m_parent(parent), m_index(index)
    {
      if (m_parent == nullptr) {
        // The root node is given a starting level of 1 such that it is large
        // enough to stradle the origin and divide the space into the four
        // quadrants.
        m_level = 1;
        // The root node's initial position is such that it straddles the
        // origin.
        m_pos.x = m_pos.y = -1;
        // The root node is given a pseudo index of -1, since it is not the
        // child of any node.
        m_index = -1;
      } else {
        assert(m_parent->m_level > 0);  // Level 0 nodes cannot have children
        // Compute the quadtree level and position for this node
        m_level = m_parent->m_level - 1;
        m_parent->m_childIndexToPos(m_index, &m_pos);
      }
    }

  template <class N>
    std::shared_ptr<N> QuadtreeNode<N>::nextSibling() const {
      if (!m_parent)
        return nullptr;  // The root node has no siblings
      // Determine this node's index in it's parent's children array
      // NOTE: This index could also be determined (possibly more efficiently)
      // from the absolute quadtree position and quadtree level
      // NOTE: This index is actually stored in the node!
#ifndef NDEBUG
      for (int i = 0; i < 4; ++i) {
        if (m_parent->m_children[i].get() == this) {
          assert(i == m_index);
          break;
        }
      }
#endif
      // Look for our next sibling
      for (int i = m_index + 1; i < 4; ++i) {
        if (m_parent->m_children[i]) {
          return m_parent->m_children[i];
        }
      }
      // This node is the last sibling; it has no next sibling
      return nullptr;
    }

  template <class N>
    std::shared_ptr<N> QuadtreeNode<N>::getChild(
        int index)
    {
      assert(m_level != 0);
      auto child = m_children[index];
      if (!child) {
        // Create children as necessary
        child = m_createChild(index);
      }
      return child;
    }

  template <class N>
    std::shared_ptr<N> QuadtreeNode<N>::getDescendant(
        const QuadtreeCoordinates &pos, int level)
    {
      assert(this->contains(pos, level));
      assert(level < m_level);
      // Determine which of our children this block belongs to
      int index = this->m_childIndexContainingPos(pos);
      auto child = this->getChild(index);
      if (child->m_level == level) {
        assert(memcmp(&child->m_pos, &pos, sizeof(pos)) == 0);
        // The base case, where our child is the node we are looking for
        return child;
      }
      // Recursive call to find the node
      return child->getDescendant(pos, level);
    }

  template <class N>
    bool QuadtreeNode<N>::contains(
        const QuadtreeCoordinates &pos,
        int level) const
    {
      // Make sure this position/level combination is aligned with the quadtree
      // coordinate lattice
      assert((pos.x & ((1 << level) - 1)) == 0);
      assert((pos.y & ((1 << level) - 1)) == 0);
      if (level > m_level) {
        // The target is larger than this node
        return false;
      } else if (level == m_level) {
        // The target is the same size as this node
        if (memcmp(&m_pos, &pos, sizeof(pos)) != 0) {
          return false;
        }
      } else {
        // The target is smaller than this node and might be contained within
        // this node
        if (pos.x < m_pos.x)
          return false;
        if (pos.y < m_pos.y)
          return false;
        /* FIXME: Bug here, should probably be pos.x + (1 << level) */
        if (pos.x >= m_pos.x + (1 << m_level))
          return false;
        /* FIXME: Bug here, should probably be pos.y + (1 << level) */
        if (pos.y >= m_pos.y + (1 << m_level))
          return false;
      }
      return true;
    }

  template <class N>
    N *QuadtreeNode<N>::relativeNode(
        const QuadtreeCoordinates &offset)
    {
      if (m_parent == nullptr) {
        if (offset.x == 0 && offset.y == 0) {
          return static_cast<N*>(this);
        }
        // The root node has no relatives
        return nullptr;
      }
      assert(this->isAligned());
      // Traverse up the quadtree until we find a node that straddles the
      // border between this node and the node we are looking for
      QuadtreeCoordinates offsetNodePos;
      offsetNodePos.x = m_pos.x + (offset.x * (1 << m_level));
      offsetNodePos.y = m_pos.y + (offset.y * (1 << m_level));
      auto parent = m_parent;
      while (parent && !parent->contains(offsetNodePos, m_level)) {
        parent = parent->m_parent;
      }
      if (parent == nullptr)
        return nullptr;
      // Traverse the quadtree downwards until we find the node
      int index = parent->m_childIndexContainingPos(offsetNodePos);
      auto child = parent->m_children[index];
      while (child && child->m_level > this->m_level) {
        index = child->m_childIndexContainingPos(offsetNodePos);
        child = child->m_children[index];
      }
      if (!child)
        return nullptr;
      assert(memcmp(&child->m_pos, &offsetNodePos, sizeof(offsetNodePos)) == 0);
      assert(child->m_level == this->m_level);
      return child.get();
    }

  template <class N>
    bool QuadtreeNode<N>::isAligned() const {
      if (m_pos.x & ((1 << m_level) - 1))
        return false;
      if (m_pos.y & ((1 << m_level) - 1))
        return false;
      return true;
    }

  template <class N>
    void Quadtree<N>::m_growRoot() {
      // FIXME: This is the most ugly code in the Quadtree class, because it
      // heavily alters the internal structure of the root node and its
      // immediate children.
      assert(m_root->level() < 31);  // Don't get carried away
      // We double the size of the root node along each dimension. Since the
      // root node divides the quadrants of 2D space at the origin, no child of
      // the root node can occupy more than one quadrant. Thus, Growing the
      // root node is as simple as adding the root node's children to even
      // larger children and increasing the level of the root node.
      m_root->setLevel(m_root->level() + 1);
      QuadtreeCoordinates newRootPos;
      newRootPos.x = newRootPos.y
        = -(1 << (m_root->level() - 1));
      m_root->setPos(newRootPos);
      for (int i = 0; i < 4; ++i) {
        auto oldChild = m_root->child(i);
        if (!oldChild)
          continue;
        auto newChild = std::shared_ptr<N>(
            new N(m_root.get(), i));
        m_root->setChild(i, newChild);
        int index = newChild->m_childIndexContainingPos(oldChild->pos());
        newChild->setChild(index, oldChild);
        oldChild->setParent(newChild.get());
        oldChild->setIndex(index);
      }
    }

  template <class N>
    void Quadtree<N>::m_grow(
        const QuadtreeCoordinates &pos,
        int level)
    {
      fprintf(stderr,
          "m_grow called,\n"
          "  level: %d, pos: (%d, %d)\n"
          "  m_root->pos(): (%d, %d)\n"
          "  m_root->level(): %d\n",
          level,
          pos.x,
          pos.y,
          m_root->pos().x,
          m_root->pos().y,
          m_root->level());
      // Just dobule check that we actually need to grow
      assert(!m_root->contains(pos, level));
      do {
        m_growRoot();
      } while (!m_root->contains(pos, level));
    }

  template <class N>
    void Quadtree<N>::m_alignPosToLevel(
        const QuadtreeCoordinates &pos,
        int level,
        QuadtreeCoordinates *alignedPos)
    {
      assert(level >= 0);
      assert(level < sizeof(int) * 8 - 1);
      alignedPos->x = (pos.x >> level) << level;
      alignedPos->y = (pos.y >> level) << level;
    }

  template <class N>
    Quadtree<N>::Quadtree() {
      // The root of the tree always straddles the origin. Since the center of
      // the root node is the origin, the root node precisely divides space up
      // into the four quadrants, and no node besides the root can occupy more
      // than one quadrant.
      //
      // The initial root block is positioned at (-1, -1) at the quadtree level
      // of 1 such that it contains the area from (-1, -1) to (1, 1).
      QuadtreeCoordinates pos;
      pos.x = -1;
      pos.y = -1;
      /* FIXME: I don't even use pos here for some reason... */
      m_root = std::shared_ptr<N>(
          new N(
            nullptr,  // parent
            -1  // index
            ));
    }

  template <class N>
    std::shared_ptr<N> Quadtree<N>::getNode(
        const QuadtreeCoordinates &pos, int level)
    {
      // Make sure the position is aligned to the quadtree coordinate lattice
      // for the given quadtree level
      QuadtreeCoordinates alignedPos;
      m_alignPosToLevel(pos, level, &alignedPos);
      if (!m_root->contains(alignedPos, level)) {
        // The given position and level are not contained in the quadtree root,
        // so we must grow the quadtree and give it a new root node.
        this->m_grow(alignedPos, level);
      }
      if (level == m_root->level()) {
        // The node we are retrieving happens to be the root node
        assert(memcmp(&alignedPos, &m_root->pos(), sizeof(alignedPos)) == 0);
        return m_root;
      }
      // The getDescendant() method for QuadtreeNode objects will automatically
      // create the descendant node if it does not exist yet.
      auto node = m_root->getDescendant(alignedPos, level);
      assert(node);
      return node;
    }

  template <class N>
    std::shared_ptr<N> Quadtree<N>::getRelativeNode(
        const N &node, const QuadtreeCoordinates &offset)
    {
      // This method uses the existing machinery to find nodes by starting from
      // the top of the quadtree and traversing it downwards. If the quadtree
      // is aligned properly, we should find the node we are looking for.
      assert(node.isAligned());
      QuadtreeCoordinates relativePos;
      relativePos.x = node.pos().x + offset.x * (1 << node.level());
      relativePos.y = node.pos().y + offset.y * (1 << node.level());
      auto result = this->getNode(relativePos, node.level());
      assert(result->isAligned());
      return result;
    }
} }

#endif
