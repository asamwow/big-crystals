#include "HeightMap.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

void HeightMap::NewMap(int x, int y) {
   heights = (unsigned char **)malloc(sizeof(unsigned char *) * x);
   for (int i = 0; i < x; i++) {
      heights[i] = (unsigned char *)malloc(sizeof(unsigned char) * y);
      for (int j = 0; j < y; j++) {
         heights[i][j] = 0;
      }
   }
   sizeX = x;
   sizeY = y;
}

void HeightMap::SetPixel(int x, int y, unsigned char value) {
   heights[x][y] = value;
}

unsigned char HeightMap::GetPixel(int x, int y) {
   return heights[x][y];
}

void HeightMap::LoadFile(char *fileName) {
   int x, y, n;
   printf("loading texture\n");
   unsigned char *data = stbi_load(fileName, &x, &y, &n, 1);
   NewMap(x, y);
   for (int i = 0; i < x; i++) {
      for (int j = 0; j < y; j++) {
         heights[i][j] = data[i * x + j];
      }
   }
   stbi_image_free(data);
}

void HeightMap::SaveFile(char *fileName) {
   unsigned char *data = (unsigned char *)malloc(sizeof(char) * sizeX * sizeY);
   for (int i = 0; i < sizeX; i++) {
      for (int j = 0; j < sizeY; j++) {
         data[i * sizeX + j] = heights[i][j];
      }
   }
   stbi_write_png(fileName, sizeX, sizeY, 1, data, sizeX);
}
