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

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <mc/algorithms/common/square.h>
#include <mc/algorithms/marchingSquares/common.h>

#include <mc/algorithms/marchingSquares/marchingSquares.h>

#include "marching_squares_tables.c"
#include "marching_squares_line_tables.c"

void mcMarchingSquares_contourFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int x_res, unsigned int y_res,
    const mcVec2 *min, const mcVec2 *max,
    mcContour *contour)
{
  float delta_x = fabs(max->x - min->x) / (float)(x_res - 1);
  float delta_y = fabs(max->y - min->y) / (float)(y_res - 1);
  /* TODO: Make a cache of sample values */
  /* Loop over the sample lattice */
  for (int y = 0; y < y_res - 1; ++y) {
    for (int x = 0; x < x_res - 1; ++x) {
      /* Determine the configuration of this square */
      int square = 0;
      for (int sampleIndex = 0; sampleIndex < 4; ++sampleIndex) {
        int pos[2];
        mcSquare_sampleRelativePosition(sampleIndex, pos);
        /* TODO: Retrieve the sample value from a buffer */
        float sample = sf(min->x + (float)(x + pos[0]) * delta_x,
                          min->y + (float)(y + pos[1]) * delta_y,
                          0.0f,
                          args);
        square |= (sample >= 0.0f ? 0 : 1) << sampleIndex;
      }
      fprintf(stderr, "square: 0x%01x\n", square);
      /* Generate vertices for this square configuration */
      mcMarchingSquares_EdgeIntersectionList *intersectionList =
        &mcMarchingSquares_edgeIntersectionTable[square];
      int intersectionIndex = 0;
      int vertexIndices[4];
      for (int edge = 0; edge < 4; ++edge) {
        vertexIndices[edge] = -1;
        int edgeIntersection = intersectionList->edges[intersectionIndex];
        if (edge == edgeIntersection) {
          /* TODO: Look for the vertex index of this edge in our buffers */
          /* Determine the sample indices on this edge */
          int sampleIndices[2];
          mcSquare_edgeSampleIndices(edge, sampleIndices);
          /* Compute the lattice positions and samples on this edge */
          mcVec3 latticePos[2];
          float values[2];
          for (int i = 0; i < 2; ++i) {
            int rel[2], abs[2];
            mcSquare_sampleRelativePosition(sampleIndices[i], rel);
            abs[0] = x + rel[0];
            abs[1] = y + rel[1];
            latticePos[i].x = (float)(abs[0]) * delta_x;
            latticePos[i].y = (float)(abs[1]) * delta_y;
            latticePos[i].z = 0.0f;
            /* TODO: Retrieve this sample value from a buffer */
            values[i] = sf(
                min->x + latticePos[i].x,
                min->y + latticePos[i].y,
                0.0f,
                args);
          }
          /* Interpolate between the sample values at each vertex */
          float weight = fabs(values[0] / (values[0] - values[1]));
          /* The corresponding edge vertex must lie on the edge between the
           * lattice points, so we interpolate between these points. */
          mcVertex vertex;
          vertex.pos = mcVec3_lerp(&latticePos[0], &latticePos[1], weight);
          /* TODO: Compute the curve normal */
          /* Add this vertex to the contour */
          vertexIndices[edge] = mcContour_addVertex(contour, &vertex);
          fprintf(stderr, "added mesh vertex: %d\n",
              vertexIndices[edge]);
          intersectionIndex += 1;
        }
      }
      /* Look in the line table for the lines corresponding to this
       * square configuration */
      for (int i = 0; i < MC_MARCHING_SQUARES_MAX_NUM_LINES; ++i) {
        mcLine line;
        const mcLine *l = &mcMarchingSquares_lineTable[square].lines[i];
        if (l->a == -1)
          break;  /* No more lines */
        line.a = vertexIndices[l->a];
        line.b = vertexIndices[l->b];
        assert(line.a != -1);
        assert(line.b != -1);
        mcContour_addLine(contour, &line);
      }
    }
  }
}
