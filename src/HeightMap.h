#ifndef HEIGHTMAP_INCLUDED
#define HEIGHTMAP_INCLUDED

#include <stdlib.h>

/// contains shader settings
class HeightMap {
 public:

   unsigned char** heights;
   int sizeX;
   int sizeY;

   void LoadFile(char* fileName);
   void SaveFile(char* fileName);
   void SetPixel(int x, int y, unsigned char value);
   unsigned char GetPixel(int x, int y);
   void NewMap(int x, int y);
};

#endif
