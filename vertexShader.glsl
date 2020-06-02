# version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normalVector;

out vec2 UV;
out vec3 Normal;
out vec3 FragPos;

// Values that stay constant for the whole mesh.
uniform mat4 M; // model matrix
uniform mat4 V; // view matrix
uniform mat4 P; // projection matrix


void main(){
  gl_Position = P * V * M * vec4(vertexPosition_modelspace, 1.0);

  // UV of the vertex. No special space for this one.
  UV = vertexUV;

  // push normal to fragment
  Normal = mat3(transpose(inverse(M))) * normalVector;

  // calculate fragment position
  FragPos = vec3(M * vec4(vertexPosition_modelspace, 1.0));
}