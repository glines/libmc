#include <assert.h>
#include <math.h>
#include <stdio.h>  /* XXX */
#include <stdlib.h>

#include <mc/isosurfaceBuilder.h>
#include <mc/mesh.h>

#include "simple_table.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * This file implements the simple marching cubes algorithm as described by
 * FIXME.
 */

void mcSimple_isosurfaceFromLattice(mcScalarLattice sl) {
}

void mcSimple_isosurfaceFromField(mcScalarField sf, mcMesh *mesh) {
  mcMesh_grow(mesh);  /* XXX */
  /* TODO: Pass the grid resolution as a parameter */
  int res = 10;
  float delta = 1.0f / (float)res;
  /* TODO: Iterate over the voxel grid to gather samples */
  float *samples = (float*)malloc(sizeof(float) * res * res * res);
  float d = 1.0f / (float)res;
  for (int z = 0; z < res; ++z) {
    for (int y = 0; y < res; ++y) {
      for (int x = 0; x < res; ++x) {
        int i = x + y * res + z * res * res;
        /* Add to the grid of samples */
        samples[i] = sf((float)x * d, (float)y * d, (float)z * d);
      }
    }
  }
  /* TODO: Iterate over the lattice structure */
  unsigned int *cubes = (unsigned int*)malloc(
      sizeof(unsigned int) * (res - 1) * (res - 1) * (res - 1));
  for (int z = 0; z < res - 1; ++z) {
    for (int y = 0; y < res - 1; ++y) {
      for (int x = 0; x < res - 1; ++x) {
        int i = x + y * res + z * res * res;
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          /* Determine this vertex's position in the cube */
          unsigned int vertexPosition;
          /* FIXME: Replace this switch with call to
           *        mcSimpleVertexRelativePosition() */
          switch (vertex) {
            case 0:  /* Origin */
              vertexPosition = i;
              break;
            case 1:  /* X-axis */
              vertexPosition = i + 1;
              break;
            case 2:  /* XY-axis */
              vertexPosition = i + res + 1;
              break;
            case 3:  /* Y-axis */
              vertexPosition = i + res;
              break;
            case 4:  /* Z-axis */
              vertexPosition = i + res * res;
              break;
            case 5:  /* XZ-axis */
              vertexPosition = i + res * res + 1;
              break;
            case 6:  /* XYZ-axis */
              vertexPosition = i + res * res + res + 1;
              break;
            case 7:  /* YZ-axis */
              vertexPosition = i + res * res + res;
              break;
            default:
              assert(0);
          }
          /* Add the bit this vertex contributes to the cube */
          cube |= (samples[vertexPosition] >= 0.0f ? 1 : 0) << vertex;
          fprintf(stderr, "sample: %g\n", samples[vertexPosition]);  /* XXX */
        }
        fprintf(stderr, "voxel cube: 0x%02x\n", cube);  /* XXX */
        /* Look in the edge table for the edges that intersect the
         * isosurface */
        fprintf(stderr, "edges: (%d, %d, %d, ...)\n",
            mcSimpleEdgeTable[cube].edges[0],
            mcSimpleEdgeTable[cube].edges[1],
            mcSimpleEdgeTable[cube].edges[2]);  /* XXX */
        unsigned int numEdgeIntersections = 0;
        for (unsigned int j = 0;
            j < MC_SIMPLE_MAX_EDGES && mcSimpleEdgeTable[cube].edges[j] != -1;
            ++j)
        {
          unsigned int vertices[2];
          float values[2];
          int edge;
          mcVec3 latticePos[2];
          numEdgeIntersections += 1;
          edge = mcSimpleEdgeTable[cube].edges[j];
          mcSimpleEdgeVertices(edge, vertices);
          fprintf(stderr, "edge: %d, vertices: %d, %d\n",
              edge,
              vertices[0], 
              vertices[1]
              );
          /* Determine the value of each edge vertex */
          for (unsigned int k = 0; k < 2; ++k) {
            unsigned int pos[3];
            unsigned int index;
            mcSimpleVertexRelativePosition(vertices[k], pos);
            index = i + pos[0] + res * pos[1] + res * res * pos[2];
            values[k] = samples[index];
            latticePos[k].x = x + pos[0] * delta;
            latticePos[k].y = y + pos[1] * delta;
            latticePos[k].z = z + pos[2] * delta;
          }
          fprintf(stderr, "values: %g, %g\n",
              values[0],
              values[1]
              );
          /* Interpolate between vertex values */
          float weight = fabs(values[1] / (values[0] - values[1]));
          fprintf(stderr, "weight: %g\n", weight);
          /* The corresponding edge vertex must lie on the edge between the
           * lattice points, so we interpolate between these points. */
          mcVertex vertex;
          vertex.pos = mcVec3_lerp(&latticePos[0], &latticePos[1], weight);
          mcMesh_addVertex(mesh, &vertex);
        }
        /* Look in the triangulation table for the triangles corresponding to
         * this cube configuration. The number of edge intersections determines
         * the number of resulting triangles. */
        for (unsigned int j = 0; j < max(numEdgeIntersections - 2, 0); ++j) {
          mcSimpleTriangle triangle =
            mcSimpleTriangulationTable[cube].triangles[j];
          fprintf(stderr, "triangle: %d, %d, %d\n",
              triangle.edges[0],
              triangle.edges[1],
              triangle.edges[2]);
        }
      }
    }
  }
}
