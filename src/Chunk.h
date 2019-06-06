#ifndef CHUNK_INCLUDED
#define CHUNK_INCLUDED

#include "Component.h"
#include "CrystalStructure.h"
#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Terrain.h"

class Chunk : public GameObject {
 public:
   int x;
   int y;

   int vertsX;
   int vertsZ;

   class MeshRenderer *meshRenderer;
   class Mesh *mesh;

   void Generate(float **heights, char ceiling, char farMesh);

   int indexVertexArray(int x, int y) { return (x * vertsZ + y); }
   int indexIndiceArray(int x, int y) { return (x * (vertsZ-1) + y); }
};

#endif
