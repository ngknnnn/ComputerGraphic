// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include Glew. Always include it before gl.h and glfw3.h, since its a bit magic
#include <GL/glew.h>

// Include GLFW = handle the window and the keyboard 2 option: GLFW/freeglut
#include <GLFW/glfw3.h>

// Include glm for 3D mathematics
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
using namespace glm;

struct objectModel{
  glm::vec3 vertex;
  glm::vec2 uv;
  glm::vec3 normal;
  objectModel(glm::vec3 _v, glm::vec2 _u, glm::vec3 _n):vertex(_v), uv(_u), normal(_n){}
};

GLuint loadShaders(const char *vertex_file_path, const char *fragment_file_path);
GLuint loadBMP_custom(const char *image_path_file, const char *o_RGB);

void computeMatricesFromInputs(GLFWwindow *window);
glm::mat4 shipPosition(GLFWwindow *window);
glm::mat4& getViewMatrix(void);
glm::mat4& getProjMatrix(void);
glm::vec3 getCamPos(void);
glm::vec3 getDirect(void);
glm::vec3 getUp(void);

void direction_update(GLFWwindow* window, double xpos, double ypos);
GLuint loadOBJ(const char *obj_file_path);