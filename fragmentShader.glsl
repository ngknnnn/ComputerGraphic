# version 330 core
// color pass from vertex shader
// in vec3 fragmentColor;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D mySampler;
uniform float ambientStrength;
uniform float specularStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){
  // for simple light color is white
  vec3 lightColor = vec3(0.9, 0.9, 0.8);
  // object color
  vec3 objectColor = texture(mySampler, UV).rgb;

  // ambient reflection
  vec3 ambient = ambientStrength * lightColor;

  // diffuse reflection
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // specular reflection
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
  vec3 specular = specularStrength * spec * lightColor;

  color = (ambient + diffuse + specular) * objectColor;
}