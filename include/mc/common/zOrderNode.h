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

#define DECLARE_Z_ORDER_NODE(PREFIX, DIMENSION) \
typedef struct { \
  int coord[DIMENSION]; \
} mc ## PREFIX ## NodeCoordinates; \
typedef struct mc ## PREFIX ## Node mc ## PREFIX ## Node; \
struct mc ## PREFIX ## Node { \
  mc ## PREFIX ## Node *children[1 << DIMENSION]; \
  mc ## PREFIX ## Node *parent; \
  int level; \
}; \
\
typedef struct { \
  mc ## PREFIX ## Node *current; \
} mc ## PREFIX ## NodeIterator; \
\
void mc ## PREFIX ## Node_init(mc ## PREFIX ## Node *self); \
void mc ## PREFIX ## Node_destroy(mc ## PREFIX ## Node *self); \
mc ## PREFIX ## NodeIterator mc ## PREFIX ## Node_begin( \
    mc ## PREFIX ## Node *self); \
mc ## PREFIX ## NodeIterator mc ## PREFIX ## Node_end( \
    mc ## PREFIX ## Node *self); \
mc ## PREFIX ## Node *mc ## PREFIX ## Node_getNode( \
    mc ## PREFIX ## Node *self, \
    const mc ## PREFIX ## NodeCoordinates *pos, \
    int level); \
\
void mc ## PREFIX ## NodeIterator_next(mc ## PREFIX ## NodeIterator *self);

#define DEFINE_Z_ORDER_NODE(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_alignPosToLevel(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_destroy(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_nextSibling(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_getNode(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION)

#define DEFINE_Z_ORDER_alignPosToLevel(PREFIX, DIMENSION) \
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

#define DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  void mc ## PREFIX ## Node_init(mc ## PREFIX ## Node *self) { \
    self->level = -1; \
    self->parent = NULL; \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      self->children[i] = NULL; \
    } \
  }

#define DEFINE_Z_ORDER_NODE_destroy(PREFIX, DIMENSION) \
  void mc ## PREFIX ## Node_destroy(mc ## PREFIX ## Node *self) { \
    /* Recursively destroy all children nodes */ \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      if (self->children[i]) { \
        mc ## PREFIX ## Node_destroy(self->children[i]); \
      } \
    } \
  }

#define DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  mc ## PREFIX ## NodeIterator mc ## PREFIX ## Node_begin( \
      mc ## PREFIX ## Node *self) \
  { \
    mc ## PREFIX ## NodeIterator i; \
    i.current = self; \
    return i; \
  }

#define DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  mc ## PREFIX ## NodeIterator PREFIX ## Node_end( \
      mc ## PREFIX ## Node *self) \
  { \
    mc ## PREFIX ## NodeIterator i; \
    i.current = self->parent; \
    return i; \
  }

#define DEFINE_Z_ORDER_NODE_nextSibling(PREFIX, DIMENSION) \
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

#define DEFINE_Z_ORDER_NODE_getNode(PREFIX, DIMENSION) \
  mc ## PREFIX ## Node *mc ## PREFIX ## Node_getNode( \
      mc ## PREFIX ## Node *self, \
      const mc ## PREFIX ## NodeCoordinates *pos, \
      int level) \
  { \
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
    /* FIXME: In the old quadtree design, the root node straddles the origin */ \
  }

#define DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION) \
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
