#ifndef TERRAIN_INCLUDED
#define TERRAIN_INCLUDED

#include <vector>

// max height possible
#define MAX_AMPLITUDE 150

// extent in chunks of world
#define MAP_SIZE_X 10
#define MAP_SIZE_Z 50
#define MAP_LENGTH_X ((2 * MAP_SIZE_X) + 1)
#define MAP_LENGTH_Z ((2 * MAP_SIZE_Z) + 1)
#define MAP_VERT_X (MAP_LENGTH_X * FAR_DETAIL+1)
#define MAP_VERT_Z (MAP_LENGTH_Z * FAR_DETAIL+1)
#define MAP_GRID_X MAP_LENGTH_X * FAR_DETAIL
#define MAP_GRID_Z MAP_LENGTH_Z * FAR_DETAIL
#define MAP_VERT_COUNT MAP_VERT_X * MAP_VERT_Z * 3
#define MAP_GRID_COUNT MAP_GRID_X * MAP_GRID_Z * 6
// CHUNK_GRID_LENGTH / (float)FAR_DETAIL;
#define MAP_UNIT 42.333f

// length in quads of one side of a chunk of the far mesh
#define FAR_DETAIL 3
#define FAR_SIZE_X MAP_SIZE_X * FAR_DETAIL
#define FAR_SIZE_Z MAP_SIZE_Z * FAR_DETAIL
// at this distance, delete chunks from memory, not view distance
#define DESTROY_DISTANCE 15
// can help remove gaps in terrain
#define FAR_MESH_HEIGHT_ADJUST 0

// side length of a chunk
#define CHUNK_VERT 128
// in terms of squares, you have one less
#define CHUNK_GRID 127
// set to 1/CHUNK_GRID/LENGTH
#define CHUNKS_PER_METER 0.0078740
// size of world in player units
#define MAP_WORLD_LENGTH_X (MAP_LENGTH_X * CHUNK_GRID+1)
#define MAP_WORLD_LENGTH_Z (MAP_LENGTH_Z * CHUNK_GRID+1)

// color
// #define CEILING_R 0.03f
// #define CEILING_G 0.015f
// #define CEILING_B 0.1f
#define CEILING_R 0.04f
#define CEILING_G 0.02f
#define CEILING_B 0.13f
#define GROUND_R 0.04f
#define GROUND_G 0.02f
#define GROUND_B 0.13f

// how much light can affect the base color of terrain 
#define LIGHTING_MAX 0.1f

// will regenerate the entire world if = 1, will take a while
// will generate only nearby terrain if = 2 without saving
// load saved height map if = 0
#define GENERATE_HEIGHT_MAP 0

#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Chunk.h"
#include "Scene.h"
#include "Camera.h"
#include "HeightMap.h"

class Terrain : public GameObject {
 public:
   static Terrain *mainTerrain;
   
   int viewDistance;
   
   std::vector<class Chunk*> chunks;
   char **chunkMap;
   
   class HeightMap *heightMap;
   class HeightMap *ceilingMap;

   class Chunk* ceiling = NULL;
   class Chunk* farTerrain = NULL;

   char GetChunkMap(int x, int z);
   char SetChunkMap(int x, int z, char chunk);
   
   static float GetHeight(float x, float z);
   static float GetCeiling(float x, float z);
   static unsigned char GenerateHeight(float x, float z);
   static unsigned char GenerateCeiling(float x, float z);
   static void GenerateFarTerrain(char isCeiling);

   // use to update baked lighting
   static void RedrawAt(float x, float z, float radius);

   void Generate(int xStride, int yStride);
   class Chunk *CreateChunk(int xStride, int yStride);
   class Chunk *CreateFarTerrain(char isCeiling);
   void DestroyChunk(int xStride, int yStride);
   void CullChunk(int xStride, int yStride, char cullType);
   void ActivateChunk(int xStride, int yStride);

   void Awake();
   void Update();

private:
   bool drawing = false;
   int prevCameraX = -1000;
   int prevCameraZ = -1000;
};

#endif
