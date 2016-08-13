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

#ifndef MC_SAMPLES_CASCADING_TREE_H_
#define MC_SAMPLES_CASCADING_TREE_H_

#include <GL/glew.h>
#include <memory>

#include "../common/sceneObject.h"
#include "../common/quadtree.h"

namespace mc { namespace samples {
  class Camera;
  class ShaderProgram;
  namespace cascading {
    class Tree;
    class TreeNode : public QuadtreeNode<TreeNode> {
      friend Tree;
      private:
        float m_points[4];

        bool m_containsPoint(const QuadtreeCoordinates &point);
        bool m_hasPoint(const QuadtreeCoordinates &point);
        void m_setPoint(const QuadtreeCoordinates &point, float value);
      public:
        TreeNode(TreeNode *parent, int index);

        glm::vec3 worldSpacePos() const;
        float size() const;

        float point(int i) const { return m_points[i]; }
    };

    class Tree :
      public SceneObject,
      Quadtree<TreeNode>
    {
      public:
        typedef struct {
          glm::vec3 p, v;
        } Ray;

        typedef struct {
          glm::vec3 p, n;
        } Plane;

      private:
        typedef struct {
          float pos[3];
          float color[3];
        } WireframeVertex;
        typedef struct {
          float pos[3];
          float tex[3];
        } BillboardVertex;

        std::shared_ptr<Camera> m_camera;
        GLuint m_squareVertexBuffer, m_squareIndexBuffer;
        GLuint m_billboardVertices, m_billboardIndices;

        /**
         * Follow the given ray in world space and intersect it with this
         * quadtree. The quadtree coordinates closes to which this ray intersects
         * the quadtree are returned in result.
         */
        bool m_intersectRay(const Ray &ray, QuadtreeCoordinates *result);

        void m_setPoint(const QuadtreeCoordinates &pos, float value);

        void m_generateSquare();
        void m_generateBillboard();

        void m_drawNodes(
            const glm::mat4 &modelView,
            const glm::mat4 &projection) const;
        void m_drawNodePoint(
            const QuadtreeCoordinates &point,
            float value,
            std::shared_ptr<ShaderProgram> shader,
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;
        void m_drawNodePoints(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;

      public:
        Tree(std::shared_ptr<Camera> camera);

        bool handleEvent(const SDL_Event &event);

        void draw(const glm::mat4 &modelWorld,
            const glm::mat4 &worldView, const glm::mat4 &projection,
            float alpha, bool debug);
    };
  }
} }

#endif
