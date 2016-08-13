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

#define DECLARE_Z_ORDER_NODE(PREFIX, DIMENSION) \
typedef struct PREFIX ## Node { \
  PREFIX ## Node *children[1 << DIMENSION]; \
  PREFIX ## Node *parent; \
  int level; \
} PREFIX ## Node; \
\
void PREFIX ## Node_init(PREFIX ## Node *self); \
\
typedef struct PREFIX ## NodeIterator { \
  PREFIX ## Node *current; \
}; PREFIX ## NodeIterator; \
\
void PREFIX ## NodeIterator_next(PREFIX ## NodeIterator *self);


#define DEFINE_Z_ORDER_NODE(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_begin(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_end(PREFIX, DIMENSION) \
  DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION)

#define DEFINE_Z_ORDER_NODE_init(PREFIX, DIMENSION) \
  void PREFIX ## Node_init(PREFIX ## Node *self) { \
    level = -1; \
    parent = NULL; \
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

#define DEFINE_Z_ORDER_NODE_ITERATOR_next(PREFIX, DIMENSION) \
  void PREFIX ## NodeIterator

#endif
