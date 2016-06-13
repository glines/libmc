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

#define MC_CUBE_NUM_VERTICES 8
#define MC_CUBE_NUM_EDGES 12
#define MC_CUBE_NUM_FACES 6

/**
 * The numbering of the cube faces is based on the numbering of the faces on a
 * 6-sided die with 1 facing front and 3 facing top.
 *
 * Note that when viewing the 1 side of a cube from the front in this manner,
 * the left face of the cube is side 2. That is to say, left and right are
 * considered relative to the cube not relative to the viewer.
 */
typedef enum mcCubeFace {
  MC_CUBE_FACE_FRONT = 0,
  MC_CUBE_FACE_LEFT,
  MC_CUBE_FACE_TOP,
  MC_CUBE_FACE_BOTTOM,
  MC_CUBE_FACE_RIGHT,
  MC_CUBE_FACE_BACK,
} mcCubeFace;

/**
 * The numbering for this cube edge is taken from the original Lorensen paper
 * on marching cubes.
 *
 * Note that the ordering of faces in these names is done alphabetically to
 * avoid confusion that might result from arbitrary ordering.
 */
typedef enum mcCubeEdge {
  MC_CUBE_EDGE_BOTTOM_FRONT = 0,
  MC_CUBE_EDGE_FRONT_LEFT,
  MC_CUBE_EDGE_FRONT_TOP,
  MC_CUBE_EDGE_FRONT_RIGHT,
  MC_CUBE_EDGE_BACK_BOTTOM,
  MC_CUBE_EDGE_BACK_LEFT,
  MC_CUBE_EDGE_BACK_TOP,
  MC_CUBE_EDGE_BACK_RIGHT,
  MC_CUBE_EDGE_BOTTOM_RIGHT,
  MC_CUBE_EDGE_BOTTOM_LEFT,
  MC_CUBE_EDGE_RIGHT_TOP,
  MC_CUBE_EDGE_LEFT_TOP,
} mcCubeEdge;
