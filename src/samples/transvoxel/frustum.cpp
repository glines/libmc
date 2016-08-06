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

#include <cstdio>

extern "C" {
#include <mc/algorithms/common/cube.h>
}

#include "../common/perspectiveCamera.h"
#include "../common/debug.h"
#include "frustum.h"
#include "transvoxelTree.h"

namespace mc { namespace samples { namespace transvoxel {
  Frustum::Frustum(const PerspectiveCamera &camera, float aspect)
    : m_worldView(camera.worldView()),
    m_projection(camera.projection(aspect)),
    m_focalLength(camera.focalLength()),
    m_aspect(aspect),
    m_fovy(camera.fovy())
  {
    m_wvp = m_projection * m_worldView;
    m_pvw = glm::inverse(m_wvp);
    m_viewWorld = glm::inverse(m_worldView);
  }

  glm::vec3 Frustum::vertexPosition(int index) const {
    unsigned int pos[3];
    mcCube_sampleRelativePosition(index, pos);
    /* Find this vertex in the canonical view volume and apply the inverse of
     * the perspective transform */
    glm::vec3 canonicalVertex;
    canonicalVertex.x = pos[0] ? 1.0 : -1.0;
    canonicalVertex.y = pos[1] ? 1.0 : -1.0;
    canonicalVertex.z = pos[2] ? 1.0 : -1.0;
    /* FIXME: Compute the near and far planes? */
    float n = 0.1f, f = 1000000.0f;
    float depth = pos[2] ? f : n;
    glm::vec3 vertex = glm::vec3(m_pvw * glm::vec4(canonicalVertex * depth, depth));
    return vertex;
  }

  bool Frustum::testVisibility(const TransvoxelNode &node) const {
    /* Calculate the center point of the node */
    glm::vec3 nodeCenter = node.worldSpaceCenter();
    float n = 0.1f, f = 1000000.0f;
    /* Iterate over all frustum planes */
    bool result = true;
    for (int plane = 0; plane < 6; ++plane) {
      /* Calculate the normal for this plane */
      glm::vec3 planeNormal;
#define CALCULATE_PLANE_NORMAL(x, y, z) \
      do { \
        float t = 0.5f * ((z) + 1.0f); \
        float depth = (1.0f - t) * n + t * f; \
        planeNormal = glm::normalize(glm::vec3( \
            m_viewWorld * glm::vec4(glm::vec3((x), (y), (z)), 0.0f))); \
        fprintf(stderr, \
            "x, y, z: (%g, %g, %g) \n" \
            "t: %g,  depth: %g\n" \
            "planeNormal: (%g, %g, %g)", \
            (x), (y), (z), \
            t, depth, \
            planeNormal.r, \
            planeNormal.g, \
            planeNormal.b \
            ); \
      } while (0);
      /* Calculate the center point for this plane */
      glm::vec3 planeCenter;
      /* FIXME: Compute the near and far planes? */
#define CALCULATE_PLANE_CENTER(x, y, z) \
      do { \
        float a = -(2.0f * n * f) / (f - n); \
        float b = (z) - (f + n) / (f - n); \
        float depth = a / b; \
        planeCenter = glm::vec3( \
            m_pvw * glm::vec4(glm::vec3((x), (y), (z)) * depth, depth)); \
      } while (0);
      switch ((FrustumPlane)plane) {
        case FrustumPlane::NEAR:
          CALCULATE_PLANE_NORMAL(0.0f, 0.0f, -1.0f);
          CALCULATE_PLANE_CENTER(0.0f, 0.0f, -1.0f);
          break;
        case FrustumPlane::RIGHT:
          CALCULATE_PLANE_NORMAL(-m_focalLength, 0.0f, -1.0f);
          CALCULATE_PLANE_CENTER(1.0f, 0.0f, 0.99f);
          break;
        case FrustumPlane::TOP:
          CALCULATE_PLANE_NORMAL(0.0f, -m_focalLength, -m_fovy);
          CALCULATE_PLANE_CENTER(0.0f, 1.0f, 0.99f);
          break;
        case FrustumPlane::BOTTOM:
          CALCULATE_PLANE_NORMAL(0.0f, m_focalLength, -m_fovy);
          CALCULATE_PLANE_CENTER(0.0f, -1.0f, 0.99f);
          break;
        case FrustumPlane::LEFT:
          CALCULATE_PLANE_NORMAL(m_focalLength, 0.0f, -1.0f);
          CALCULATE_PLANE_CENTER(-1.0f, 0.0f, 0.99f);
          break;
        case FrustumPlane::FAR:
          CALCULATE_PLANE_NORMAL(0.0f, 0.0f, 1.0f);
          CALCULATE_PLANE_CENTER(0.0f, 0.0f, 1.0f);
          break;
        default:
          assert(false);
      }
      Debug::drawLine(
          planeCenter,
          planeCenter + planeNormal * 10.0f,
          glm::vec3(0.0f, 1.0f, 0.0f));
      /* Calculate the effective radius of the node's cube volume */
      glm::vec3 R(node.size(), 0.0f, 0.0f);
      glm::vec3 S(0.0f, node.size(), 0.0f);
      glm::vec3 T(0.0f, 0.0f, node.size());
      /* FIXME:
      float effectiveRadius =
        fabs((glm::dot(R, planeNormal)
                  + glm::dot(S, planeNormal)
                  + glm::dot(T, planeNormal)));
                  */
      float effectiveRadius = sqrt((3.0f / 4.0f) * node.size() * node.size());
      /* Check which side of the frustum plane our cube lies */
      float test = glm::dot(
          (planeCenter - effectiveRadius * planeNormal) - nodeCenter,
          planeNormal);
      /*
      Debug::drawLine(
          nodeCenter,
          planeCenter - effectiveRadius * planeNormal,
          test > 0.0f ? glm::vec3(1.0f, 0.0f, 1.0f)
                        : glm::vec3(0.0f, 1.0f, 1.0f));
                        */
      if (test > 0.0f) {
        /* FIXME: We should really return here, but for debugging purposes we
         * don't */
        result = false;
        fprintf(stderr,
            "failed the frustum culling test:\n"
            "  planeCenter: (%g, %g, %g)\n"
            "  planeNormal: (%g, %g, %g)\n"
            "  nodeCenter: (%g, %g, %g)\n"
            "  node size: %g\n"
            "  plane: %d\n"
            "  effectiveRadius: %g\n",
            planeCenter.x, planeCenter.y, planeCenter.z,
            planeNormal.x, planeNormal.y, planeNormal.z,
            nodeCenter.x, nodeCenter.y, nodeCenter.z,
            node.size(),
            plane,
            effectiveRadius);
      }
    }
    Debug::drawLine(
        nodeCenter,
        nodeCenter + glm::vec3(0.0f, 0.0f, 1.0f),
        result ? glm::vec3(0.0f, 1.0f, 0.0f)
                 : glm::vec3(1.0f, 0.0f, 0.0f));
    return result;
  }

  float Frustum::projectedSize(const TransvoxelNode &node) const {
    glm::vec3 a = glm::vec3(m_worldView * glm::vec4(node.worldSpacePos(), 1.0f))
      + glm::vec3(0.5f * node.size(), 0.0f, 0.0f);
    glm::vec3 b = glm::vec3(m_worldView * glm::vec4(node.worldSpacePos(), 1.0f))
      - glm::vec3(0.5f * node.size(), 0.0f, 0.0f);
    glm::vec4 a_proj = m_projection * glm::vec4(a, 1.0f);
    glm::vec4 b_proj = m_projection * glm::vec4(b, 1.0f);
    glm::vec2 a_norm, b_norm;
    a_norm.x = a_proj.x / a_proj.w;
    a_norm.y = a_proj.y / a_proj.w;
    b_norm.x = b_proj.x / b_proj.w;
    b_norm.y = b_proj.y / b_proj.w;
    /* FIXME: This length does not account for the aspect ratio */
    return glm::length(b_norm - a_norm);
  }
} } }
