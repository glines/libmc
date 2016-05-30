#ifndef MC_SAMPLES_COMMON_SHADER_PROGRAM_H_
#define MC_SAMPLES_COMMON_SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <string>

namespace mc { namespace samples {
  /**
   * Class representing a compiled and linked GL shader program, suitable for
   * rendering GL graphics.
   */
  class ShaderProgram {
    private:
      GLuint m_shaderProgram;
      GLuint m_modelViewLocation, m_projectionLocation, m_timeLocation;
      GLuint m_vertPositionLocation, m_vertNormalLocation, m_vertColorLocation,
             m_vertTexCoordLocation, m_vertVelocityLocation;
      GLuint m_vertStartTimeLocation;
      GLuint m_texture0;

      static GLuint m_compileShader(const std::string &file, GLenum type);
      static void m_linkShaderProgram(GLuint shaderProgram);

    protected:
      /**
       * A virtual member function to allow derived classes the opportunity to
       * register attribute and/or uniform locations immediately after the shader
       * program has been linked.
       */
      // FIXME: This virtual call cannot be made in the constructor... I should
      //        just remove this.
      virtual void initLocations();

    public:
      ShaderProgram(const std::string &vert, const std::string &frag);
      virtual ~ShaderProgram();

      GLuint modelViewLocation() const { return m_modelViewLocation; }
      GLuint projectionLocation() const { return m_projectionLocation; }
      GLuint timeLocation() const { return m_timeLocation; }
      GLuint vertPositionLocation() const { return m_vertPositionLocation; }
      GLuint vertNormalLocation() const { return m_vertNormalLocation; }
      GLuint vertColorLocation() const { return m_vertColorLocation; }
      GLuint vertTexCoordLocation() const { return m_vertTexCoordLocation; }
      GLuint vertVelocityLocation() const { return m_vertVelocityLocation; }
      GLuint vertStartTimeLocation() const { return m_vertStartTimeLocation; }
      GLuint texture0() const { return m_texture0; }

      void use() const;
  };
} }

#endif
