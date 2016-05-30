#include "../../mc/simple.h"
/*
 * This program generates the edge and triangularization tables needed for
 * implementing a performant marching cubes algorithm. While these tables are
 * available pre-generated on the internet, generating these tables ourselves
 * serves as a stepping stone towards generating larger tables used by more
 * sophisticated variants of the marching cubes algorithm.
 *
 * The first table generated is the edge table, whose purpose is to determine
 * which edges are intersected for a given voxel configuration. The marching
 * cubes algorithm uses this information to interpolate between samples on the
 * relevent edges. This table is relatively easy to generate, since any edge
 * whose samples fall on different sides of the isosurface must be intersected
 * by the isosurface.
 *
 * The second table generated is the triangularization table. This table is
 * used to quickly find a triangle representation for the voxel that can be
 * used in the resulting surface mesh. This table is much more complicated to
 * generate, but it can build upon the edge table since triangle vertices must
 * lie on intersected edges.
 */
int main(int argc, char **argv) {
  /* TODO: Parse the arguments */

  /* TODO: Allocate memory for the edge table */
  /* TODO: Allocate memory for the triangulization table */

  /* TODO: Iterate through all voxel cube configurations */
  for (unsigned int i = 0; i <= 0xFF; ++i) {
    /* TODO: Iterate through all edges */
    for (unsigned int e = 0; e < 12; ++e) {
      /* TODO: Determine the two sample values */
      /* TODO: If the sample values disagree, add this edge to the list */
    }
    /* TODO: Generate the edge table. */

    /* TODO: Generate the triangulization table. */
  }
}
