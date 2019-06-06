#ifndef MESH_INCLUDED
#define MESH_INCLUDED

#define PI 3.14159265358979323846

#include "../ext/tiny_obj_loader/tiny_obj_loader.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION

#include "Chunk.h"
#include "Component.h"
#include "GLSL.h"
#include "Program.h"

class Program;

class Mesh : public Component {
 public:
   Type getType();

   static Mesh *crystalMesh;

   void SetData(std::vector<unsigned int> indices, std::vector<float> vertices,
                std::vector<float> colors);
   void SetData(unsigned int *newIndices, int indiceCount, float *newVertices,
                int verticeCount, float *newColors, int colorsCount);

   void SetPrimitiveCube();
   void SetPrimitiveSphere(int stackCount, int sectorCount, float radius, glm::vec3 color, char shade);

   void loadMesh(const std::string &meshName);
   void init();
   void resize();
   void draw(const Program *prog) const;
   void Free();

 private:
   std::vector<unsigned int> indices;
   std::vector<float> vertices;
   std::vector<float> colors;

   unsigned int indicesID = 0;
   unsigned int verticesID = 0;
   unsigned int colorsID = 0;

   std::vector<float> textures;
   unsigned int texturesID = 0;
   unsigned int vaoID = 0;
};

#endif
