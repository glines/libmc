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

#ifndef MC_COMMON_Z_ORDER_NODE_H_
#define MC_COMMON_Z_ORDER_NODE_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MC_DECLARE_Z_ORDER_NODE(PREFIX, DIMENSION) \
typedef struct { \
  int coord[DIMENSION]; \
} mc ## PREFIX ## NodeCoordinates; \
typedef struct mc ## PREFIX ## Node mc ## PREFIX ## Node; \
struct mc ## PREFIX ## Node { \
  mc ## PREFIX ## Node *children[1 << DIMENSION]; \
  mc ## PREFIX ## Node *parent; \
  mc ## PREFIX ## NodeCoordinates pos; \
  int level; \
  float value; \
}; \
\
typedef struct { \
  mc ## PREFIX ## Node *current; \
} mc ## PREFIX ## NodeIterator; \
\
void mc ## PREFIX ## Node_init(mc ## PREFIX ## Node *self); \
void mc ## PREFIX ## Node_initRoot(mc ## PREFIX ## Node *root); \
void mc ## PREFIX ## Node_destroy(mc ## PREFIX ## Node *self); \
mc ## PREFIX ## NodeIterator mc ## PREFIX ## Node_begin( \
    mc ## PREFIX ## Node *self); \
mc ## PREFIX ## NodeIterator mc ## PREFIX ## Node_end( \
    mc ## PREFIX ## Node *self); \
mc ## PREFIX ## Node *mc ## PREFIX ## Node_getNode( \
    mc ## PREFIX ## Node *self, \
    const mc ## PREFIX ## NodeCoordinates *pos, \
    int level); \
mc ## PREFIX ## Node *mc ## PREFIX ## Node_getRoot( \
    mc ## PREFIX ## Node *self); \
int mc ## PREFIX ## Node_contains( \
    const mc ## PREFIX ## Node *self, \
    const mc ## PREFIX ## NodeCoordinates *pos, \
    int level); \
void mc ## PREFIX ## Node_growRoot( \
    mc ## PREFIX ## Node *root, \
    const mc ## PREFIX ## NodeCoordinates *pos, \
    int level); \
int mc ## PREFIX ## Node_childIndexContainingPos( \
    const mc ## PREFIX ## Node *self, \
    const mc ## PREFIX ## NodeCoordinates *pos); \
void mc ## PREFIX ## Node_childIndexToPos( \
    const mc ## PREFIX ## Node *self, \
    int index, \
    mc ## PREFIX ## NodeCoordinates *pos); \
mc ## PREFIX ## Node *mc ## PREFIX ## Node_getDescendant( \
    mc ## PREFIX ## Node *self, \
    const mc ## PREFIX ## NodeCoordinates *pos, \
    int level); \
mc ## PREFIX ## Node *mc ## PREFIX ## Node_getChild( \
    mc ## PREFIX ## Node *self, \
    int index); \
mc ## PREFIX ## Node *mc ## PREFIX ## Node_createChild( \
    mc ## PREFIX ## Node *self, \
    int index); \
\
void mc ## PREFIX ## NodeIterator_next(mc ## PREFIX ## NodeIterator *self);

#define MC_DEFINE_Z_ORDER_NODE(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_alignPosToLevel(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_initRoot(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_destroy(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_nextSibling(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_getNode(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_getRoot(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_contains(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_growRoot(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_childIndexContainingPos(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_childIndexToPos(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_getDescendant(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_getChild(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_createChild(PREFIX, DIMENSION) \
  MC_DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION)

#define MC_DEFINE_Z_ORDER_alignPosToLevel(PREFIX, DIMENSION) \
  void mc ## PREFIX ## _alignPosToLevel( \
      const mc ## PREFIX ## NodeCoordinates *pos, \
      int level, \
      mc ## PREFIX ## NodeCoordinates *alignedPos) \
  { \
    assert(level >= 0); \
    assert(level < sizeof(int) * 8 - 1); \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      alignedPos->coord[i] = (pos->coord[i] >> level) << level; \
    } \
  }

#define MC_DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  void mc ## PREFIX ## Node_init(mc ## PREFIX ## Node *self) { \
    self->level = -1; \
    self->parent = NULL; \
    for (int i = 0; i < DIMENSION; ++i) { \
      self->pos.coord[i] = 0; \
    } \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      self->children[i] = NULL; \
    } \
  }

#define MC_DEFINE_Z_ORDER_NODE_initRoot(PREFIX, DIMENSION) \
  void mc ## PREFIX ## Node_initRoot(mc ## PREFIX ## Node *root) { \
    /* The root node starts at level 1 and straddles the origin */ \
    root->level = 1; \
    root->parent = NULL; \
    for (int i = 0; i < DIMENSION; ++i) { \
      root->pos.coord[i] = -1; \
    } \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      root->children[i] = NULL; \
    } \
  }

#define MC_DEFINE_Z_ORDER_NODE_destroy(PREFIX, DIMENSION) \
  void mc ## PREFIX ## Node_destroy(mc ## PREFIX ## Node *self) { \
    /* Recursively destroy all children nodes */ \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      if (self->children[i]) { \
        mc ## PREFIX ## Node_destroy(self->children[i]); \
      } \
    } \
  }

#define MC_DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  mc ## PREFIX ## NodeIterator mc ## PREFIX ## Node_begin( \
      mc ## PREFIX ## Node *self) \
  { \
    mc ## PREFIX ## NodeIterator i; \
    i.current = self; \
    return i; \
  }

#define MC_DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  mc ## PREFIX ## NodeIterator PREFIX ## Node_end( \
      mc ## PREFIX ## Node *self) \
  { \
    mc ## PREFIX ## NodeIterator i; \
    i.current = self->parent; \
    return i; \
  }

#define MC_DEFINE_Z_ORDER_NODE_nextSibling(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_nextSibling( \
      mc ## PREFIX ## Node *self) \
  { \
    if (!self->parent) \
      return NULL; /* The root node has no siblings */ \
    /* Determine this node's index in its parent's children array */ \
    /* NOTE: We might want to simply store this index in the future */ \
    int i; \
    for (i = 0; i < (1 << DIMENSION); ++i) { \
      if (self->parent->children[i] == self) { \
        break; \
      } \
    } \
    assert(i < (1 << DIMENSION)); \
    /* Look for our next sibling */ \
    for (i = i + 1; i < (1 << DIMENSION); ++i) { \
      if (self->parent->children[i]) { \
        return self->parent->children[i]; \
      } \
    } \
    /* This node is the last sibling; it has no next sibling */ \
    return NULL; \
  }

#define MC_DEFINE_Z_ORDER_NODE_getNode(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_getNode( \
      mc ## PREFIX ## Node *self, \
      const mc ## PREFIX ## NodeCoordinates *pos, \
      int level) \
  { \
    /* This function requires the root node */ \
    mc ## PREFIX ## Node *root = mc ## PREFIX ## Node_getRoot(self); \
    /* Make sure the position is aligned to the coordinate lattice for the
     * given level */ \
    mc ## PREFIX ## NodeCoordinates alignedPos; \
    mc ## PREFIX ## _alignPosToLevel(pos, level, &alignedPos); \
    fprintf(stderr, /* XXX */ \
        "level: %d\n" \
        "pos: (0x%08x, 0x%08x)\n" \
        "alignedPos: (0x%08x, 0x%08x)\n", \
        level, \
        pos->coord[0], \
        pos->coord[1], \
        alignedPos.coord[0], \
        alignedPos.coord[1]); \
    /* Make sure the root node contains the node */ \
    if (!mc ## PREFIX ## Node_contains(root, &alignedPos, level)) { \
      /* We grow the root to contain the node */ \
      mc ## PREFIX ## Node_growRoot(root, &alignedPos, level); \
    } \
    if (level == root->level) { \
      /* The node we are retrieving happens to be the root node */ \
      assert(memcmp(&alignedPos, &root->pos, sizeof(alignedPos)) == 0); \
      return root; \
    } \
    /* Look for the node in the descendants of the root (creating the node if
     * necessary) */ \
    mc ## PREFIX ## Node *node = \
      mc ## PREFIX ## Node_getDescendant(root, &alignedPos, level); \
    assert(node); \
    return node; \
  }

#define MC_DEFINE_Z_ORDER_NODE_getRoot(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_getRoot( \
      mc ## PREFIX ## Node *self) \
  { \
    if (!self->parent) { \
      return self; \
    } \
    return mc ## PREFIX ## Node_getRoot(self->parent); \
  }

#define MC_DEFINE_Z_ORDER_NODE_contains(PREFIX, DIMENSION) \
  int mc ## PREFIX ## Node_contains( \
      const mc ## PREFIX ## Node *self, \
      const mc ## PREFIX ## NodeCoordinates *pos, \
      int level) \
  { \
    /* Make sure this position/level combination is aligned with the
     * coordinate lattice */ \
    for (int i = 0; i < DIMENSION; ++i) { \
      assert((pos->coord[i] & ((1 << level) - 1)) == 0); \
    } \
    if (level > self->level) { \
      /* The target is larger than this node */ \
      fprintf(stderr, "the target is larger than this node\n"); \
      return 0; \
    } else if (level == self->level) { \
      /* The target is the same size as this node */ \
      if (memcmp(&self->pos, pos, sizeof(*pos)) != 0) { \
        fprintf(stderr, "the target is the same size as this node\n"); \
        return 0; \
      } \
    } else { \
      /* The target is smaller than this node and might be contained within
       * this node */ \
      fprintf(stderr, "the target is smaller than this node\n"); \
      for (int i = 0; i < DIMENSION; ++i) { \
        if (pos->coord[i] < self->pos.coord[i]) \
          return 0; \
        if (pos->coord[i] >= self->pos.coord[i] + (1 << self->level)) \
          return 0; \
      } \
    } \
    fprintf(stderr, "the target is contained in this node\n"); \
    return 1; \
  }

#define MC_DEFINE_Z_ORDER_NODE_growRoot(PREFIX, DIMENSION) \
  void mc ## PREFIX ## Node_growRoot( \
      mc ## PREFIX ## Node *root, \
      const mc ## PREFIX ## NodeCoordinates *pos, \
      int level) \
  { \
    while (!mc ## PREFIX ## Node_contains(root, pos, level)) { \
      /* Don't get carried away */ \
      assert(root->level < sizeof(int) * 8 - 1); \
      /* We double the size of the root node along each dimension. Since the
       * root node divides space at the origin, no child of the root can occupy
       * more than one orthant (n-dimensional quadrant). Thus, growing the root
       * node is as simple as adding the root node's children to even larger
       * children and increasing the level of the root node. */ \
      root->level += 1; \
      for (int i = 0; i < DIMENSION; ++i) { \
        assert(root->pos.coord[i] == -(1 << (root->level - 2))); \
        root->pos.coord[i] = -(1 << (root->level - 1)); \
      } \
      for (int i = 0; i < (1 << DIMENSION); ++i) { \
        mc ## PREFIX ## Node *oldChild = root->children[i]; \
        if (!oldChild) \
          continue; \
        mc ## PREFIX ## Node *newChild = (mc ## PREFIX ## Node *)malloc( \
            sizeof(mc ## PREFIX ## Node)); \
        mc ## PREFIX ## Node_init(newChild); \
        newChild->level = root->level - 1; \
        newChild->parent = root; \
        int oldChildNewIndex = mc ## PREFIX ## Node_childIndexContainingPos( \
            newChild, \
            &oldChild->pos); \
        root->children[i] = newChild; \
        newChild->children[oldChildNewIndex] = oldChild; \
      } \
    } \
  }

#define MC_DEFINE_Z_ORDER_NODE_childIndexContainingPos(PREFIX, DIMENSION) \
  int mc ## PREFIX ## Node_childIndexContainingPos( \
      const mc ## PREFIX ## Node *self, \
      const mc ## PREFIX ## NodeCoordinates *pos) \
  { \
    int index = 0; \
    mc ## PREFIX ## NodeCoordinates adjPos; \
    if (self->parent == NULL) { \
      /* The root node needs special treatment because it straddles the
       * origin */ \
      for (int i = 0; i < DIMENSION; ++i) { \
        adjPos.coord[i] = pos->coord[i] - self->pos.coord[i]; \
      } \
    } else { \
      memcpy(&adjPos, pos, sizeof(*pos)); \
    } \
    for (int i = 0; i < DIMENSION; ++i) { \
      index |= ((adjPos.coord[i] >> (self->level - 1)) & 1) << i; \
    } \
    return index; \
  }

#define MC_DEFINE_Z_ORDER_NODE_childIndexToPos(PREFIX, DIMENSION) \
void mc ## PREFIX ## Node_childIndexToPos( \
    const mc ## PREFIX ## Node *self, \
    int index, \
    mc ## PREFIX ## NodeCoordinates *pos) \
{ \
  for (int i = 0; i < DIMENSION; ++i) { \
    pos->coord[i] = self->pos.coord[i] \
        + (index & (1 << i) ? 1 << (self->level - 1) : 0); \
  } \
}

#define MC_DEFINE_Z_ORDER_NODE_getDescendant(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_getDescendant( \
      mc ## PREFIX ## Node *self, \
      const mc ## PREFIX ## NodeCoordinates *pos, \
      int level) \
  { \
    assert(mc ## PREFIX ## Node_contains(self, pos, level)); \
    assert(level < self->level); \
    /* Determine which of our children this descendant must belong to */ \
    int index = mc ## PREFIX ## Node_childIndexContainingPos(self, pos); \
    mc ## PREFIX ## Node *child = \
        mc ## PREFIX ## Node_getChild(self, index); \
    if (child->level == level) { \
      assert(memcmp(&child->pos, pos, sizeof(*pos)) == 0); \
      /* The base case, where our child is the node we are looking for */ \
      return child; \
    } \
    /* Recursive call to find the node */ \
    return mc ## PREFIX ## Node_getDescendant(child, pos, level); \
  }

#define MC_DEFINE_Z_ORDER_NODE_getChild(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_getChild( \
      mc ## PREFIX ## Node *self, \
      int index) \
  { \
    assert(self->level != 0); \
    mc ## PREFIX ## Node *child = self->children[index]; \
    if (!child) { \
      /* Create children as necessary */ \
      child = mc ## PREFIX ## Node_createChild(self, index); \
    } \
    return child; \
  }

#define MC_DEFINE_Z_ORDER_NODE_createChild(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_createChild( \
      mc ## PREFIX ## Node *self, \
      int index) \
  { \
    assert(!self->children[index]);  /* Don't overwrite any children */ \
    assert(self->level > 0);  /* Level 0 cannot have children */ \
    self->children[index] = (mc ## PREFIX ## Node *)malloc( \
        sizeof(mc ## PREFIX ## Node)); \
    mc ## PREFIX ## Node_init(self->children[index]); \
    self->children[index]->parent = self; \
    self->children[index]->level = self->level - 1; \
    mc ## PREFIX ## Node_childIndexToPos( \
        self, \
        index, \
        &self->children[index]->pos); \
    return self->children[index]; \
  }

#define MC_DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION) \
  void mc ## PREFIX ## NodeIterator_next( \
      mc ## PREFIX ## NodeIterator *self) \
  { \
    /* Look for our first child */ \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      mc ## PREFIX ## Node *child = self->current->children[i]; \
      if (child) { \
        self->current = child; \
        return; \
      } \
    } \
    /* Check for a parent */ \
    mc ## PREFIX ## Node *parent = self->current->parent; \
    if (!parent) { \
      /* We're done */ \
      /* NOTE: This only happens with a tree consisting of a single node */ \
      self->current = NULL; \
      return; \
    } \
    /* Look for our next sibling */ \
    mc ## PREFIX ## Node *sibling = \
        mc ## PREFIX ## Node_nextSibling(self->current); \
    if (sibling) { \
      self->current = sibling; \
      return; \
    } \
    /* Recursively look for siblings of our parent */ \
    do { \
      mc ## PREFIX ## Node *uncle = \
          mc ## PREFIX ## Node_nextSibling(parent); \
      if (uncle) { \
        self->current = uncle; \
        return; \
      } \
      parent = parent->parent; \
    } while (parent); \
    /* We recursed past the root node, so we're done */ \
    self->current = NULL; \
  }

#endif
