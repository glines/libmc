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

#include <GL/glew.h>
#include <SDL.h>
#include <assert.h>
#include <math.h>
#include <mc.h>
#include <stdio.h>
#include <stdlib.h>

struct demo {
  SDL_Window *window;
  SDL_GLContext glContext;
  int window_width, window_height;
  GLuint wireframeShader;
  GLuint vertPosition;
  GLuint vertexBuffer, indexBuffer;
  unsigned int numIndices;
} demo;

float torus(float x, float y, float z) {
  float R = 0.75f;
  float r = 0.25f;

  float a = (R - sqrt(x * x + z * z));

  return a * a + y * y - r * r;
}

void init_sdl() {
  /* Initialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Failed to initialize SDL: %s\n",
        SDL_GetError());
    exit(EXIT_FAILURE);
  }
  demo.window_width = 640;
  demo.window_height = 480;
  demo.window = SDL_CreateWindow(
      "256 Voxel Cubes",  /* title */
      SDL_WINDOWPOS_UNDEFINED,  /* x */
      SDL_WINDOWPOS_UNDEFINED,  /* y */
      demo.window_width, demo.window_height,  /* w, h */
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE  /* flags */
      );
  if (demo.window == NULL) {
    fprintf(stderr, "Failed to create SDL window: %s\n",
        SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void init_gl() {
  /* Create an OpenGL context for our window */
  demo.glContext = SDL_GL_CreateContext(demo.window);
  if (demo.glContext == NULL) {
    fprintf(stderr, "Failed to initialize OpenGL context: %s\n",
        SDL_GetError());
    exit(EXIT_FAILURE);
  }

  /* Initialize GL entry points */
  GLenum error = glewInit();
  if (error != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n",
        glewGetErrorString(error));
    exit(EXIT_FAILURE);
  }

  /* Configure the GL */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glLineWidth(1.0);
  glViewport(0, 0, demo.window_width, demo.window_height);
}

GLuint compile_shader(const char *file, GLuint type) {
    FILE *f;
    int length;
    char *source;
    GLint status;

    GLuint shader = glCreateShader(type);

    f = fopen(file, "r");
    if (f == NULL) {
      fprintf(stderr, "Could not open shader file: %s\n", file);
      exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    rewind(f);
    source = (char*)malloc(length+1);
    length = fread(source, 1, length, f);
    source[length] = '\0';
    glShaderSource(shader, 1, (const char **)&source, &length);
    free(source);

    glCompileShader(shader);
    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &status);
    if (status != GL_TRUE) {
      char *log;
      int logLength;
      glGetShaderiv(
          shader,
          GL_INFO_LOG_LENGTH,
          &logLength);
      log = (char*)malloc(logLength);
      glGetShaderInfoLog(
          shader,
          logLength,
          NULL,
          log);
      fprintf(stderr, "Error compiling shader '%s': %s\n",
          file, log);
      free(log);
      // FIXME: Maybe call something other than exit() here
      exit(EXIT_FAILURE);
    }

    return shader;
}

void link_shader_program(GLuint program) {
  GLint status;

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    char *log;
    int logLength;
    glGetProgramiv(
        program,
        GL_INFO_LOG_LENGTH,
        &logLength);
    log = (char *)malloc(logLength);
    glGetProgramInfoLog(
        program,
        logLength,
        NULL,
        log);
    fprintf(stderr, "Error linking shader program: %s\n", log);
    free(log);
    // FIXME: Maybe call something other than exit() here
    exit(EXIT_FAILURE);
  }
}

void init_shaders() {
  GLuint vertexShader, fragmentShader;
  const char *vert = "./assets/shaders/glsl/wireframe.vert";
  const char *frag = "./assets/shaders/glsl/wireframe.frag";

  /* Create a shader program */
  demo.wireframeShader = glCreateProgram();

  /* Compile the vertex shader */
  vertexShader = compile_shader(vert, GL_VERTEX_SHADER);
  glAttachShader(demo.wireframeShader, vertexShader);
  /* Compile the fragment shader */
  fragmentShader = compile_shader(frag, GL_FRAGMENT_SHADER);
  glAttachShader(demo.wireframeShader, fragmentShader);
  /* Link the shaders into the shader program */
  link_shader_program(demo.wireframeShader);

  /* Store the vertex attribute locations */
  demo.vertPosition =
    glGetAttribLocation(demo.wireframeShader, "vertPosition");
}

typedef struct Vertex {
  float pos[3];
} Vertex;

void init_mesh() {
  const mcMesh *mesh;
  Vertex *vertices;
  unsigned int *indices;

  /* Generate the isosurface mesh using libmc */
  mcIsosurfaceBuilder ib;
  mesh = mcIsosurfaceBuilder_isosurfaceFromField(
      &ib,  /* self */
      torus,  /* sf */
      MC_SIMPLE_MARCHING_CUBES  /* algorithm */
      );
  /* Send the mesh to the GL */
  vertices = (Vertex*)malloc(sizeof(Vertex) * mesh->numVertices);
  for (unsigned int i = 0; i < mesh->numVertices; ++i) {
    vertices[i].pos[0] = mesh->vertices[i].pos.x;
    vertices[i].pos[1] = mesh->vertices[i].pos.y;
    vertices[i].pos[2] = mesh->vertices[i].pos.z;
  }
  glGenBuffers(1, &demo.vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, demo.vertexBuffer);
  glBufferData(
      GL_ARRAY_BUFFER,  /* target */
      sizeof(Vertex) * mesh->numVertices,  /* size */
      vertices,  /* data */
      GL_STATIC_DRAW  /* usage */
      );
  free(vertices);
  indices = (unsigned int*)malloc(mesh->numFaces * 3);
  for (unsigned int i = 0; i < mesh->numFaces; ++i) {
    assert(mesh->faces[i].numIndices == 3);
    for (unsigned int j = 0; j < 3; ++j) {
      indices[i * 3 + j] = mesh->faces[i].indices[j];
    }
  }
  glGenBuffers(1, &demo.indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, demo.indexBuffer);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,  /* target */
      sizeof(unsigned int) * mesh->numFaces * 3, /* size */
      indices,  /* data */
      GL_STATIC_DRAW  /* usage */
      );
  free(indices);
  demo.numIndices = mesh->numFaces * 3;
  /* FIXME: Free the mesh */
}

void draw_mesh() {
  glUseProgram(demo.wireframeShader);

  // Prepare the vertex attributes
  glBindBuffer(GL_ARRAY_BUFFER, demo.vertexBuffer);
  assert(demo.vertPosition != -1);
  glEnableVertexAttribArray(demo.vertPosition);
  glVertexAttribPointer(
      demo.vertPosition,  /* index */
      3,  /* size */
      GL_FLOAT,  /* type */
      0,  /* normalized */
      sizeof(Vertex),  /* stride */
      &(((Vertex*)0)->pos[0])  /* pointer */
      );

  // Draw the mesh as triangles
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, demo.indexBuffer);
  glDrawElements(
      GL_TRIANGLES,  /* mode */
      demo.numIndices,  /* count */
      GL_UNSIGNED_INT,  /* type */
      0  /* indices */
      );
}

void main_loop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw the torus mesh
  float aspect = (float)demo.window_width / (float)demo.window_height;
  draw_mesh();

  SDL_GL_SwapWindow(demo.window);

  // Check for SDL events (user input, etc.)
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            demo.window_width = event.window.data1;
            demo.window_height = event.window.data2;
            glViewport(0, 0, demo.window_width, demo.window_height);
            break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        break;
      case SDL_MOUSEMOTION:
        break;
      case SDL_QUIT:
        exit(EXIT_SUCCESS);
    }
  }
}

int main(int argc, char **argv) {
  init_sdl();
  init_gl();
  init_shaders();
  init_mesh();

  while (1) {
    main_loop();
  }
}
