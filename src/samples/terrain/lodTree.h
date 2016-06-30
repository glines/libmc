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

#ifndef MC_SAMPLES_TERRAIN_LOD_TREE_H_
#define MC_SAMPLES_TERRAIN_LOD_TREE_H_

#include <memory>
#include <glm/glm.hpp>

namespace mc { namespace samples { namespace terrain {
  class TerrainMesh;
  /**
   * Class implementing an octree structure that keeps track of the level of
   * detail of the terrain meshes generated.
   */
  class LodTree {
    public:
      /**
       * Integer coordinates used to describe node and voxel block positions
       * within the level of detail tree.
       */
      typedef struct Coordinates {
        int x, y, z;
      } Coordinates;
      /**
       * A node within a level of detail tree. This node represents a single
       * octant at some level of detail in the tree, and, at levels of detail
       * other than 0, contains up to eight children. This node resides at a
       * specific voxel block position, which is represented by integer
       * coordinates. These coordinates align to the level of detail for this
       * node.
       */
      class Node {
        friend LodTree;
        public:
          /**
           * Enumeration of the different possible mesh status for a LOD node.
           * These values allow the terrain mesh generation thread to keep
           * track of which nodes have been marked or generated already.
           */
          enum class Status {
            /** The terrain mesh for this node has not been marked or
             * generated. */
            NOT_GENERATED,
            /** The terrain mesh for this node has already been marked for
             * generation, and is either waiting in the queue for generation or
             * presently being generated. */
            MARKED,
            /** The terrain mesh for this node has already been generated and
             * is ready for use. */
            GENERATED,
            /** The terrain mesh for this node was generated, but found to be
             * an empty mesh. Thus, this node's mesh is a null pointer, and
             * nothing needs to be drawn for this node. */
            EMPTY,
          };

          /**
           * Iterator for a LOD node that can iterate over the octree.
           */
          class Iterator {
            friend Node;
            private:
              const Node *m_current;

              /**
               * Constructs an iterator pointing to the given node. If current
               * is a null pointer, this signifies the parent of the root node
               * of the octree.
               *
               * \param current Pointer to the node that this iterator refers
               * to, which may be null.
               */
              Iterator(const Node *current) : m_current(current) {}
            public:
              /**
               * Overloaded pre-increment operator so that this iterator class
               * can be used as an iterator. This method moves the iterator to
               * the next node in the LOD octree. Node ordering is carried out
               * depth-first.
               */
              Iterator &operator++();

              /**
               * Overloaded dereference operator for accessing the node that
               * this iterator references.
               *
               * Note that it is possible that this iterator contains a null
               * pointer to a node if this iterator is past the octree. In
               * order to avoid dereferencing a null iterator, it is important
               * to check that this iterator does not equal the iterator
               * returned by Node::end().
               *
               * \return Reference to the LOD tree node that this iterator is
               * currently at. 
               */
              const Node &operator*() {
                return *m_current;
              }

              /**
               * Overloaded equality operator for comparing this iterator with
               * other iterators. Iterators are equal if they reference the
               * same LOD tree node.
               *
               * \param other The iterator with which to compare this iterator.
               * \return True if these iterators reference the same node, false
               * otherwise.
               */
              bool operator==(const Iterator &other) {
                return this->m_current == other.m_current;
              }

              /**
               * Overloaded inequality operator for comparing this iterator
               * with other iterators. This is implemented with the overloaded
               * equality operator.
               *
               * \param other The iterator with which to compare this iterator.
               * \return True if these iterators reference different nodes,
               * false otherwise.
               *
               * \sa operator==()
               */
              bool operator!=(const Iterator &other) {
                return !(*this == other);
              }
          };
        private:
          Node *m_parent;
          std::shared_ptr<Node> m_children[8];
          std::shared_ptr<TerrainMesh> m_mesh;
          Status m_status;
          Coordinates m_block;
          int m_lod;

          int m_childIndexContainingBlock(const Coordinates &block);
          void m_childIndexBlock(int index, Coordinates *block);
          std::shared_ptr<Node> m_createChild(int index);
        public:
          /**
           * Consturcts a LOD node at the given position and level of detail.
           */
          Node(const Coordinates &block, int lod, Node *parent);

          /**
           * Returns the current status of this node and the terrain mesh
           * object represented by this node.
           *
           * \return The status of the node as an enum.
           */
          Status status() const { return m_status; }
          /**
           * Sets the current status of the node as it pertains to the terrain
           * mesh object represented by this node.
           *
           * Node status changes as a node is created, a mesh is requested, and
           * the mesh is generated. The TerrainGenerator class uses the status
           * enum to avoid generating terrain meshes multiple times, or to
           * avoid generating terrains where there is no isosurface.
           *
           * \param status The status of the node to set.
           */
          void setStatus(Status status) { m_status = status; }

          /**
           * Returns the voxel block at which this node resides, in integer
           * coordinates. The node's position is measured at corner of the node
           * with the lowest coordinate value.
           *
           * \return Integer coordinates of the voxel block where this node is
           * located.
           */
          const Coordinates &block() const { return m_block; }

          /**
           * Returns the level of detail at this node. Level of detail is
           * highest at level 0, and greater values of the level of detail
           * indicate a lower level of detail. The level of detail determines
           * the size of the node, with higher levels of detail being smaller
           * nodes further down in the octree.
           *
           * \return The level of detail at this node.
           */
          int lod() const { return m_lod; }

          /**
           * Returns in world coordinates the position of the block that this
           * node is located at. Block positions are given with respect to the
           * lowest value corner of the block. These positions are computed
           * from the block coordinates, the TerrainMesh::BLOCK_SIZE, and the
           * TerrainMesh::VOXEL_DELTA values.
           *
           * \return Position in world coordinates of the block that this node
           * is located at.
           */
          glm::vec3 pos();

          /**
           * Returns the size of a side of this node in terms of world space
           * unit length. The nodes are cube volumes, so each side has the same
           * length.
           *
           * Since nodes double in size as the level of detail goes down, this
           * size depends on the level of detail of the node. This size is
           * calculated from the node level of detail, the
           * TerrainMesh::BLOCK_SIZE, and the TerrainMesh::VOXEL_DELTA values.
           *
           * \return Size of one side of this node in terms of world space
           * units.
           */
          float size();

          /**
           * Returns an iterator starting at this LOD tree node for iterating
           * over this node and all of its children.
           *
           * If this node is a null pointer, which signifies an empty octree,
           * then this method returns an iterator referencing a null pointer as
           * well.
           *
           * \return An iterator refering to this node.
           */
          Iterator begin() const {
            return Iterator(this);
          }

          /**
           * Returns an iterator representing the end of the subtree defined by
           * this node.
           *
           * The iterator returned by this method actually points to the parent
           * of this node, but since this node might be the root of the octree
           * it is important not to iterate past the iterator returned by this
           * method.  Iterating past this iterator is undefined behavior.
           */
          Iterator end() const {
            return Iterator(this->parent());
          }

          /**
           * \return A pointer to the parent node of this node.
           */
          Node *parent() const { return m_parent; }

          /**
           * Finds the sibling of this node considered the "next" sibling.
           * Siblings are ordered based on their child index within the parent
           * node. If there is no next sibling (i.e. this node is the last
           * sibling), then a null pointer is returned.
           *
           * \return Shared pointer to the next sibling node, if such a sibling
           * exists.
           */
          std::shared_ptr<Node> nextSibling() const;

          /**
           * This method accesses the child of this node at the given index.
           * Since it is not required for the octree to be complete, it is
           * possible that this method will return a null pointer indicating no
           * child nodes at the given index.
           *
           * \param index Index of the child node to access.
           * \return Shared pointer to the child of this node at the given
           * index.
           */
          std::shared_ptr<Node> child(int index) const {
            return m_children[index];
          }

          /**
           * Determine if the node with the given block and level of detail
           * would be contained within this node.
           *
           * \param block The coordinates of the block to check.
           * \param lod The level of detail.
           * \return True if the node with the given block coordinates and
           * level of detail wolud be contained within this node.
           */
          bool contains(const Coordinates &block, int lod) const;

          /**
           * Return the child node at the given block coordinates with the
           * given level of detail. If such a node does not already exist, then
           * it is created.
           *
           * \param block The coordinates of the block to look for.
           * \param lod The level of detail to look at.
           * \return Pointer to the node at the given block coordinates and
           * level of detail.
           */
          std::shared_ptr<Node> getChild(
              const Coordinates &block, int lod);

          /**
           * This method is used to check whether or not this node's voxel
           * block position is aligned to the lattice grid for this node's
           * level of detail. All nodes (other than the root node, which
           * straddles the origin) should be aligned in order for the LOD tree
           * to work correctly.
           *
           * \return True if this node is aligned to the lattice grid for this
           * node's level of detail, false otherwise.
           */
          bool isAligned() const;
      };
    private:
      std::shared_ptr<Node> m_root;

      void m_grow(const Coordinates &block, int lod);
      void m_alignBlockToLod(const Coordinates &block, int lod,
          Coordinates *alignedBlock);
    public:
      /**
       * Finds the coordinates of the voxel block that the point at the
       * given world space coordinates would reside in.
       *
       * \param pos The world position of the point in world space coordinates.
       * \param block Set to the coordinates of the voxel block that
       * contain the given point.
       */
      static void posToBlock(const glm::vec3 &pos, Coordinates *block);

      /**
       * Constructs a LOD tree which contains no terrain.
       */
      LodTree();

      /**
       * Returns the LOD node located at the given block for the given level of
       * detail. If that node does not exist, it and its parent nodes are
       * created when this method is called. If the given block coordinates do
       * not align to the given level of detail, then they are made to align to
       * the given level of detail; the returned node might not have the given
       * block coordinates.
       *
       * \param block The integer coordinates of the node to retrieve.
       * \param lod The level of detail at which the node resides, with the
       * default being the highest level of detail.
       */
      std::shared_ptr<Node> getNode(
          const Coordinates &block, int lod = 0);

      /**
       * Returns the node at the same level of detail as the given node at the
       * given offset. This offset is given in terms of nodes at this level of
       * detail, i.e. an offset of 2 would refer to a node 2 nodes over, not
       * two voxel blocks over. If the node at the offset does not exist, then
       * it and its parent nodes are created.
       *
       * \param node The originating node of which to find a relative node.
       * \param offset The offset given in terms of nodes at the same level of
       * detail adjacent to the given node node. 
       * \return Shared pointer to the node at the offset relative to this
       * node.
       */
      std::shared_ptr<Node> getRelativeNode(
          const Node &node, const Coordinates &offset);

      /**
       * Returns an iterator starting at the octree root for iterating over the
       * LOD tree. This method simply leverages the Node::begin() method on the
       * root node, which implements the desired behavior.
       *
       * \return Iterator at the begining of the LOD octree.
       *
       * \sa Node::begin()
       */
      Node::Iterator begin() const {
        assert(m_root);  // We always have a root node
        return m_root->begin();
      }
      /**
       * Returns an iterator signifying the end of the octree. This iterator
       * always contains a null pointer, since the root node does not have any
       * parent node. This method simply leverages the Node::end() method on
       * the root node, which implements the desired behavior.
       *
       * \return Iterator at the begining of the LOD octree.
       *
       * \sa Node::end()
       */
      Node::Iterator end() const {
        assert(m_root);  // We always have a root node
        return m_root->end();
      }
  };
} } }

#endif
