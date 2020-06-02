#include "shader.h"

// Include glm for 3D mathematics
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

// control variable
glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // position
float horizontalAngle = 0; // horizontal angle: 0
float verticalAngle = glm::pi<float>() / 2.0f; // vertical angle: 90
float initialFoV = 60.0f; // Initial Field of View
float speed = 2.5f; // 3 units / second
float mouseSpeed = 0.001f;

// ship control
glm::vec3 ship_position = glm::vec3(0.0f, 0.0f, 0.0f); // position
float upSpeed = 1.0f;
float rightSpeed = 1.0f;
float ship_speed = 0.01f;
float upDir = 0.0f;
float rightDir = 0.0f;

glm::mat4 shipPosition(GLFWwindow *window){
  glm::mat4 pos(1.0f);
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    upDir += upSpeed;
    upDir = upDir < 360.0f ? upDir : upDir - 360.0f * (float)((int)upDir % 360);
  }
  else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    upDir -= upSpeed;
    upDir = upDir > 0.0f ? upDir : upDir + 360.0f * (float)((-1 * (int)upDir) % 360);
  }
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    rightDir += rightSpeed;
    rightDir = rightDir < 360.0f ? rightDir : rightDir - 360.0f * (float)((int)rightDir % 360);
  }
  else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    rightDir -= rightSpeed;
    rightDir = rightDir > 0.0f ? rightDir : rightDir + 360.0f * (float)((-1 * (int)rightDir) % 360);
  }
  pos = glm::rotate(glm::mat4(1.0f), glm::radians(upDir), glm::vec3(0.0f, 0.0f, 1.0f)) * pos;
  pos = glm::rotate(glm::mat4(1.0f), glm::radians(rightDir), glm::vec3(0.0f, 1.0f, 0.0f)) * pos;

  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
    glm::vec3 direction(
      glm::cos(glm::radians(upDir)) * glm::cos(glm::radians(rightDir)),
      glm::cos(glm::radians(upDir)) * glm::sin(glm::radians(rightDir)),
      glm::sin(glm::radians(upDir))
    );
    ship_position += direction * ship_speed;
  }
  return glm::translate(glm::mat4(1.0f), ship_position) * pos;
}

float deltaTime = 0.0f; // count time interval between each framce
float lasTime = glfwGetTime(); // set time interval each frame

glm::mat4 proj_matrix(1.0f);
glm::mat4 view_matrix(1.0f);

void computeMatricesFromInputs(GLFWwindow *window){
  // Get time interval
  float currentTime = glfwGetTime();
  deltaTime = currentTime - lasTime;
  lasTime = currentTime;

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction(
    glm::cos(verticalAngle) * glm::sin(horizontalAngle),
    glm::sin(verticalAngle),
    glm::cos(verticalAngle) * glm::cos(horizontalAngle)
  );
  // printf("Direction: %f %f %f\n", direction.x, direction.y, direction.z);

  // right vector
  glm::vec3 right = glm::vec3(
    glm::sin(horizontalAngle - glm::pi<float>() / 2.0f),
    0,
    glm::cos(horizontalAngle - glm::pi<float>() / 2.0f)
  );

  // up vector : perpendicular to both direction and right
  glm::vec3 up = glm::cross(right, direction);

  // Get new position
  // Move forward
  if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
    position += direction * deltaTime * speed;
  }
  // Move backward
  if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
    position -= direction * deltaTime * speed;
  }
  // Strafe right
  if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
    position += right * deltaTime * speed;
  }
  // Strafe left
  if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
    position -= right * deltaTime * speed;
  }
  // zoom in 
  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
    initialFoV -= deltaTime * speed * 5;
  }
  // zoom out
  if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
    initialFoV += deltaTime * speed * 5;
  }

  // get view matrix
  view_matrix = glm::lookAt(
    position,
    position + direction,
    up
  );

  // get projection matrix
  int width, height;
  glfwGetWindowSize(window, &width, &height); // Get current size of window
  proj_matrix = glm::perspective(glm::radians(initialFoV), (float)width / (float)height, 0.1f, 500.0f);
}

glm::mat4& getViewMatrix(void){
  return view_matrix;
}

glm::mat4& getProjMatrix(void){
  return proj_matrix;
}

glm::vec3 getCamPos(void){
  return position;
}

glm::vec3 getDirect(void){
  glm::vec3 direction(
    glm::cos(verticalAngle) * glm::sin(horizontalAngle),
    glm::sin(verticalAngle),
    glm::cos(verticalAngle) * glm::cos(horizontalAngle)
  );
  return direction;
}

glm::vec3 getUp(void){
  glm::vec3 right = glm::vec3(
    glm::sin(horizontalAngle - glm::pi<float>() / 2.0f),
    0,
    glm::cos(horizontalAngle - glm::pi<float>() / 2.0f)
  );

  glm::vec3 up = glm::cross(right, getDirect());
  return up;
}

void direction_update(GLFWwindow* window, double xpos, double ypos){
  // Get current size of window
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  // compute new orientation
  horizontalAngle += mouseSpeed * deltaTime * float((double)width / 2.0f - xpos);
  verticalAngle += mouseSpeed * deltaTime * float((double)height / 2.0f - ypos);

  // printf("Horizon: %lf Vertical: %lf\n", horizontalAngle, verticalAngle);

  // reset mouse position for next frame
  glfwSetCursorPos(window, (double)width / 2.0f, (float)height / 2.0f);
}
