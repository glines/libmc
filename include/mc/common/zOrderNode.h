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

#include <stdlib.h>

#define DECLARE_Z_ORDER_NODE(PREFIX, DIMENSION) \
typedef struct PREFIX ## Node PREFIX ## Node; \
struct PREFIX ## Node { \
  struct PREFIX ## Node *children[1 << DIMENSION]; \
  struct PREFIX ## Node *parent; \
  int level; \
}; \
\
void PREFIX ## Node_init(PREFIX ## Node *self); \
\
typedef struct { \
  PREFIX ## Node *current; \
} PREFIX ## NodeIterator; \
\
void PREFIX ## NodeIterator_next(PREFIX ## NodeIterator *self);

#define DEFINE_Z_ORDER_NODE(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_nextSibling(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION) \

#define DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  void PREFIX ## Node_init(PREFIX ## Node *self) { \
    self->level = -1; \
    self->parent = NULL; \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      self->children[i] = NULL; \
    } \
  }

#define DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  PREFIX ## NodeIterator PREFIX ## Node_begin(PREFIX ## Node *self) { \
    PREFIX ## NodeIterator i; \
    i.current = self; \
    return i; \
  }

#define DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  PREFIX ## NodeIterator PREFIX ## Node_end(PREFIX ## Node *self) { \
    PREFIX ## NodeIterator i; \
    i.current = self->parent; \
    return i; \
  }

#define DEFINE_Z_ORDER_NODE_nextSibling(PREFIX, DIMENSION) \
  PREFIX ## Node *PREFIX ## Node_nextSibling(PREFIX ## Node *self) { \
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

#define DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION) \
  void PREFIX ## NodeIterator_next( \
      PREFIX ## NodeIterator self) \
  { \
    /* Look for our first child */ \
    for (int i = 0; i < (1 << DIMENSION); ++i) { \
      PREFIX ## Node *child = self->current->children[i]; \
      if (child) { \
        m_current = child; \
        return; \
      } \
    } \
    /* Check for a parent */ \
    PREFIX ## Node *parent = self->current->parent; \
    if (!parent) { \
      /* We're done */ \
      /* NOTE: This only happens with a tree consisting of a single node */ \
      self->current = NULL; \
      return; \
    } \
    /* Look for our next sibling */ \
    PREFIX ## Node *sibling = PREFIX ## Node_nextSibling(self->current); \
    if (sibling) { \
      self->current = sibling; \
      return; \
    } \
    /* Recursively look for siblings of our parent */ \
    do { \
      PREFIX ## Node *uncle = parent->nextSibling(); \
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
