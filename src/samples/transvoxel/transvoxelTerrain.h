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

#ifndef MC_SAMPLES_TRANSVOXEL_TRANSVOXEL_TERRAIN_H_
#define MC_SAMPLES_TRANSVOXEL_TRANSVOXEL_TERRAIN_H_

#include <GL/glew.h>
#include <memory>

#include "../common/sceneObject.h"
#include "transvoxelTree.h"

namespace mc { namespace samples {
  class PerspectiveCamera;
  namespace transvoxel {
    class Frustum;
    /**
     * A voxel terrain representing the given scalar field function. The terrain
     * mesh is comprised of many small meshes stitched together with the
     * Transvoxel algorithm.
     */
    class TransvoxelTerrain : public SceneObject {
      private:
        typedef struct WireframeVertex {
          float pos[3];
          float color[3];
        } WireframeVertex;

        TransvoxelTree m_tree;
        std::shared_ptr<PerspectiveCamera> m_camera;
        GLuint m_cubeWireframeVertices, m_cubeWireframeIndices;
        GLuint m_frustumWireframeVertices, m_frustumWireframeIndices;
        float m_aspect;
        int m_minimumLod;

        void m_updateVisibility();
        void m_ensureRootContainsFrustum(const Frustum &frustum);
        bool m_checkRootContainsFrustum(const Frustum &frustum);
        bool m_updateNodeVisibility(
            const Frustum &frustum,
            std::shared_ptr<TransvoxelNode> node);

        void m_generateCubeWireframe();
        void m_generateFrustumWireframe();
        void m_updateFrustumWireframe();

        void m_drawOctree(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection);
        void m_drawViewFrustum(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection);
      public:
        /**
         * \param camera The camera whose viewing frustum determines which
         * terrain geometry to load and at what level of detail.
         * \param aspect The initial aspect ratio of the camera.
         * \param minimumLod The lowest level of detail to generate, with higher
         * numbers indicating a lower level of detail.
         */
        TransvoxelTerrain(
            std::shared_ptr<PerspectiveCamera> camera,
            float aspect,
            int minimumLod = 8);

        /**
         * The TransvoxelTerrain class implements handleEvent() so that it can
         * check when the window aspect ratio changes. This aspect ratio affects
         * the view frustum volume and thus determines which meshes are generated
         * by the TransvoxelTerrain object.
         */
        bool handleEvent(const SDL_Event &event);

        void tick(float dt);

        void draw(const glm::mat4 &modelWorld,
            const glm::mat4 &worldView, const glm::mat4 &projection,
            float alpha, bool debug);
    };
  }
} }

#endif
