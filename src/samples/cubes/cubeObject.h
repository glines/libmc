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

#ifndef MC_SAMPLES_CUBES_CUBE_OBJECT_H_
#define MC_SAMPLES_CUBES_CUBE_OBJECT_H_

#include <GL/glew.h>
#include <mcxx/isosurfaceBuilder.h>
#include <mcxx/mesh.h>
#include <mcxx/scalarField.h>
#include <memory>

#include "../common/sceneObject.h"

namespace mc { namespace samples {
  class ShaderProgram;
  namespace cubes {
    class CubeObject : public SceneObject {
      private:
        IsosurfaceBuilder m_builder;
        Mesh m_mesh;
        unsigned int m_cube;
        GLuint m_vertexBuffer;
        unsigned int m_numPoints;

        void m_generateDebugPoints();

        static std::shared_ptr<ShaderProgram> m_pointShader();

        typedef struct Vertex {
          float pos[3];
          float color[3];
        } Vertex;

        class CubeScalarField : public ScalarField {
          private:
            unsigned int m_cube;
          public:
            CubeScalarField(unsigned int cube);

            float operator()(float x, float y, float z);
        };
      public:
        /**
         * A scene object that represents a single voxel cube. This is used to
         * visualize individual voxels for debugging purposes.
         */
        CubeObject(
            unsigned int cube,
            const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
            const glm::quat &orientation = glm::quat()
            );

        /**
         * Draws the cube voxel, its vertices, edges, and the triangles that make
         * up its mesh.
         */
        void draw(const glm::mat4 &modelWorld,
            const glm::mat4 &worldView, const glm::mat4 &projection,
            float alpha, bool debug) const;

        /**
         * Changes the cube being represented by this cube object. Calling this
         * method will change the underlying isosurface.
         */
        void setCube(unsigned int cube);
    };
  }
} }

#endif
