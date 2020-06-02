# include "shader.h"

GLuint loadBMP_custom(const char *image_path_file, const char *o_RGB){
  // Data read from the header of the BMP file
  unsigned char header[54]; // Each BMP file begins by a 54-bytes header
  unsigned int dataPos; // Position in the file where the actual data begins
  unsigned int width, height;
  unsigned int imageSize; // = width*height*3
  // Actual RGB data
  unsigned char * data;

  // open the file
  FILE *file = fopen(image_path_file, "rb");
  if(!file){
    printf("Image could not be opened\n");
    return 0;
  }

  // read head file
  if(fread(header, 1, 54, file) != 54){ // If not 54 bytes read 
    printf("Not a correct BMP file\n");
    fclose(file);
    return 0;
  }
  if(header[0] != 'B' || header[1] != 'M'){ // Wrong begin head
    printf("Not a correct BMP file\n");
    fclose(file);
    return 0;
  }
  // read ints from the byte
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos == 0)   dataPos = 54; // The BMP header is done that way

  // print out information
  // printf("Load image %s:\ndataPos: %d\nimageSize: %d\nwidth: %d\nheight: %d\n", image_path_file, dataPos, imageSize, width, height);

  // Cretate a buffer
  data = new unsigned char[imageSize];

  // read the actual data from the file into the buffer
  fseek(file, dataPos - 1, SEEK_SET); // jump to data position
  for(int i = 0; i < width * height; i++){ // convert struct BRG to RGB
      fread(&data[i * 3 + (int)(o_RGB[0] - '0')], 1, 1, file);
      fread(&data[i * 3 + (int)(o_RGB[1] - '0')], 1, 1, file);
      fread(&data[i * 3 + (int)(o_RGB[2] - '0')], 1, 1, file);
  }

  // Everything is in memory now, the file can be closed
  fclose(file);

  // create one openGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to openGL
  // #define FORMAT

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // Generate mipmaps, by the way.
  glGenerateMipmap(GL_TEXTURE_2D);

  // delete buffer data
  delete[] data;

  return textureID;
}