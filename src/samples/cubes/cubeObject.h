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
