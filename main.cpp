// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include Glew. Always include it before gl.h and glfw3.h, since its a bit magic
#include <GL/glew.h>

// Include GLFW = handle the window and the keyboard 2 option: GLFW/freeglut
#include <GLFW/glfw3.h>

// Include glm for 3D mathematics
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include "common/shader.h"

GLFWwindow* window; // main window

// rotation speed of object
float moon_speed = 2.0f;
float earth_speed = 0.1f;

struct model_buffer{
  GLuint id;
  GLuint texture_id;
  int size;
  glm::mat4 MVP;
};

model_buffer setObject(const std::string image, const std::string o_file, const std::string o_RGB){
  // This will identify our vertex buffer
  model_buffer o_model;
  // load texture
  o_model.texture_id = loadBMP_custom(image.c_str(), o_RGB.c_str());
  // create vbo
  o_model.id = loadOBJ(o_file.c_str());
  // get size of VBO
  glBindBuffer(GL_ARRAY_BUFFER, o_model.id);
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &o_model.size);
  // set MVP
  o_model.MVP = glm::mat4(1.0f);
  // return model
  return o_model;
}

void drawObject(GLuint program_id, model_buffer o_model){
  GLuint MatrixID = glGetUniformLocation(program_id, "M");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(o_model.MVP));
  MatrixID = glGetUniformLocation(program_id, "V");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(getViewMatrix()));
  MatrixID = glGetUniformLocation(program_id, "P");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(getProjMatrix()));
  // load texture
  glBindTexture(GL_TEXTURE_2D, o_model.texture_id);
  glActiveTexture(GL_TEXTURE + o_model.texture_id);

  glBindBuffer(GL_ARRAY_BUFFER, o_model.id);
  // 1st attribute buffer: vertices
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void *)0);
  // 2nd attr: uv coordinate
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void *)12);
  // 3rd attr: normal coordinate
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void *)20);
  // Accept fragment if it closer to the camera than the former one
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
  // Use our shader
  glUseProgram(program_id);
  // Draw the triangle
  glDrawArrays(GL_TRIANGLES, 0, o_model.size / 12); // Starting from vertex 0; 3 vertices total -> 1 triangle
  // disable stream
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

int main(){
  glewExperimental = true; // Load extension even if it is not in driver

  // Initialize GLFW
  if(!glfwInit()){
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  // open openGL window
  glfwWindowHint(GLFW_SAMPLES, 4); //  4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // we want openGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // we dont want the old openGL

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(900, 858, "openGL_Learn", NULL, NULL);
  if(window == NULL){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n" );
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window); // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // register callback for mouse
  glfwSetCursorPosCallback(window, direction_update);


  // vertex array object
  GLuint vao_array_id;
  glGenVertexArrays(1, &vao_array_id);
  glBindVertexArray(vao_array_id);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // build shader program
  GLuint program_id = loadShaders("vertexShader.glsl", "fragmentShader.glsl");

  // set clear buffer
  glClearColor(0.53, 0.8, 0.92, 1.0);

  // animation rotation degree
  float moon_degree = 0.0f;
  float earth_degree = 0.0f;

  // setup model
  // sun
  model_buffer sun = setObject(std::string("image/sun.bmp"), std::string("blender_model/sphere-1.obj"), std::string("021"));

  // earth
  model_buffer earth = setObject(std::string("image/earth.bmp"), std::string("blender_model/sphere-1.obj"), std::string("120"));

  // moon
  model_buffer moon = setObject(std::string("image/moon.bmp"), std::string("blender_model/sphere-1.obj"), std::string("021"));

  // star ship
  model_buffer ship = setObject(std::string("image/silver-metallic.bmp"), std::string("blender_model/star-trek.obj"), std::string("021"));

  do { // buffer loop
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // compute new earth rotate position
    earth_degree = earth_degree + earth_speed;
    earth_degree = earth_degree < 360.0f ? earth_degree : (earth_degree - 360.0f * (float)((int)earth_degree % 360));

    // compute new moon rotate position
    moon_degree = moon_degree + moon_speed;
    moon_degree = moon_degree < 360.0f ? moon_degree : (moon_degree - 360.0f * (float)((int)moon_degree % 360));

    // compute new ship position
    glm::mat4 ship_matrix = shipPosition(window);

    // compute new eye position
    computeMatricesFromInputs(window);

    // lighting setup
    GLuint ambientStrength = glGetUniformLocation(program_id, "ambientStrength");
    glUniform1f(ambientStrength, 0.1f);
    GLuint specularStrength = glGetUniformLocation(program_id, "specularStrength");
    glUniform1f(specularStrength, 0.9f);
    GLuint lightPos = glGetUniformLocation(program_id, "lightPos");
    glUniform3f(lightPos, 0.0f, 0.0f, 0.0f);
    GLuint viewPos = glGetUniformLocation(program_id, "viewPos");
    glUniform3f(viewPos, getCamPos().x, getCamPos().y, getCamPos().z);

    // compute world transform
    sun.MVP = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 6.0f, 6.0f)); // scale up

    earth.MVP = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)); // scale up
    earth.MVP = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)) * earth.MVP; // move out of origin
    earth.MVP = glm::rotate(glm::mat4(1.0f), glm::radians(earth_degree), glm::vec3(0.0f, 1.0f, 0.0f)) * earth.MVP;

    moon.MVP = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f)); // scale down
    moon.MVP = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f)) * moon.MVP; // move out of origin
    moon.MVP = glm::rotate(glm::mat4(1.0f), glm::radians(moon_degree), glm::vec3(-1.0f, 2.0f, 0.0f)) * moon.MVP;
    // move to earth position
    moon.MVP = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)) * moon.MVP;
    moon.MVP = glm::rotate(glm::mat4(1.0f), glm::radians(earth_degree), glm::vec3(0.0f, 1.0f, 0.0f)) * moon.MVP;

    ship.MVP = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f,0.5f));
    ship.MVP = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)) * ship.MVP;
    ship.MVP = ship_matrix * ship.MVP;

    // draw object
    glUniform1f(ambientStrength, 1.0f);
    drawObject(program_id, sun);

    glUniform1f(ambientStrength, 0.1f);
    drawObject(program_id, earth);
    drawObject(program_id, moon);
    drawObject(program_id, ship);

    // swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  return 0;
}
