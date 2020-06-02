#include "shader.h"
#include <vector>
#include <string.h>
#include <iostream>

// object loader v2: allow load f with 4 values
// object loader v3: allow load f with missing uv coordinate

GLuint loadOBJ(const char *obj_file_path){
  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<glm::vec3> temp_vertices;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;
  std::vector<objectModel> final_data;

  FILE * file = fopen(obj_file_path, "r");
  if( file == NULL ){
    printf("Impossible to open the file !\n");
    return false;
  }

  while(1){
    char lineHeader[500];

    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if(res == EOF) break;
    else{ // parse lineHeader
      if(strcmp(lineHeader, "v") == 0){
        glm::vec3 vertex;
        fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
        temp_vertices.push_back(vertex);
      }
      else if(strcmp(lineHeader, "vt") == 0){
        glm::vec2 uv;
        fscanf(file, "%f %f\n", &uv.x, &uv.y);
        temp_uvs.push_back(uv);
      }
      else if(strcmp(lineHeader, "vn") == 0){
        glm::vec3 normal;
        fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
        temp_normals.push_back(normal);
      }
      else if(strcmp(lineHeader, "f") == 0){
        // check face does not have uv
        int miss = 0;
        int pos = ftell(file);
        unsigned int test;
        fscanf(file, "%d", &test);
        char c1 = fgetc(file);
        char c2 = fgetc(file);
        if(c1 == c2) miss = 1;
        else miss = 0;
        fseek(file, pos, SEEK_SET);

        unsigned int vertexIndex, uvIndex, normalIndex, count = 0;
        std::vector<unsigned int> temp;
        char end;
        while(1){
          int matches;
          if(miss == 0){
            matches = fscanf(file, "%d/%d/%d",  &vertexIndex, &uvIndex, &normalIndex);
          }
          else{
            matches = fscanf(file, "%d//%d",  &vertexIndex, &normalIndex);
            uvIndex = 1;
            matches++;
          }
          if(count == 0 || count == 2){
            temp.push_back(vertexIndex);
            temp.push_back(uvIndex);
            temp.push_back(normalIndex);
          }
          count++;
          if (matches != 3){
            printf("File can't be read by our simple parser : ( Try exporting with other options\n");
            fclose(file);
            return false;
          }
          vertexIndices.push_back(vertexIndex);
          uvIndices.push_back(uvIndex);
          normalIndices.push_back(normalIndex);
          
          // break signal
          while((end = fgetc(file)) == ' '){};
          if(end == '\r' || end == '\n') break;
          else fseek(file, -1, SEEK_CUR);
        }
        // add 4th vertex to buffer
        if(count > 3){
          vertexIndices.push_back(temp[0]);
          vertexIndices.push_back(temp[3]);
          uvIndices.push_back(temp[1]);
          uvIndices.push_back(temp[4]);
          normalIndices.push_back(temp[2]);
          normalIndices.push_back(temp[5]);
        }
      }
    }
  }

  // For each vertex of each triangle
  for(unsigned int i = 0; i < vertexIndices.size(); i++){
    // get index
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int uvIndex = uvIndices[i];
    unsigned int normalIndex = normalIndices[i];

    // assign corresponding value
    glm::vec3 vertex = temp_vertices[vertexIndex - 1];
    glm::vec2 uv = temp_uvs[uvIndex - 1];
    glm::vec3 normal = temp_normals[normalIndex - 1];
    
    // push to stack
    objectModel res(vertex, uv, normal);
    final_data.push_back(res);
  }

  // push data to vbo
  GLuint bufferID;
  glGenBuffers(1, &bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  glBufferData(GL_ARRAY_BUFFER, final_data.size() * sizeof(objectModel), &final_data[0], GL_STATIC_DRAW);
  // close file
  fclose(file);
  
  return bufferID;
}