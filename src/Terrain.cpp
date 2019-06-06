#include "Terrain.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

class Terrain *Terrain::mainTerrain;

// class HeightMap *heightMap;

void Terrain::Awake() {
   Terrain::mainTerrain = this;
   // set game to l for loading
   Scene::gameState = 'l';
   // initialize chunk buffer map to e for empty
   chunkMap = (char **)malloc(sizeof(char *) * MAP_LENGTH_X);
   for (int x = -MAP_SIZE_X; x <= MAP_SIZE_X; x++) {
      chunkMap[x + MAP_SIZE_X] = (char *)malloc(sizeof(char) * MAP_LENGTH_Z);
      for (int z = -MAP_SIZE_Z; z <= MAP_SIZE_Z; z++) {
         chunkMap[x + MAP_SIZE_X][z + MAP_SIZE_Z] = 'e';
      }
   }

   heightMap = new HeightMap();
   ceilingMap = new HeightMap();

   if (GENERATE_HEIGHT_MAP == 1) {
      int maxDim = glm::max(MAP_WORLD_LENGTH_X, MAP_WORLD_LENGTH_Z);
      int maxDimVert = glm::max(MAP_VERT_X, MAP_VERT_Z);
      heightMap->NewMap(maxDim, maxDim);
      ceilingMap->NewMap(maxDimVert, maxDimVert);
      printf("Generating Ceiling\n");
      for (int x = 0; x < MAP_VERT_X; x++) {
         for (int z = 0; z < MAP_VERT_Z; z++) {
            ceilingMap->SetPixel(x, z, (unsigned char)GenerateCeiling(x, z));
         }
      }

      for (int x = 0; x < MAP_WORLD_LENGTH_X; x++) {
         if (x % 10 == 0) {
            printf("Generating Height Map: %f\%\n",
                   ((float)x + 1) / MAP_WORLD_LENGTH_X * 100.f);
         }
         for (int z = 0; z < MAP_WORLD_LENGTH_Z; z++) {
            heightMap->SetPixel(x, z, (unsigned char)GenerateHeight(x, z));
         }
      }
      printf("Saving Textures\n");
      ceilingMap->SaveFile("../resources/ceilingHeightMap.png");
      heightMap->SaveFile("../resources/terrainHeightMap.png");
   } else if (GENERATE_HEIGHT_MAP == 0) {
      ceilingMap->LoadFile("../resources/ceilingHeightMap.png");
      heightMap->LoadFile("../resources/terrainHeightMap.png");
   }
   printf("Creating Ceiling\n");
   GenerateFarTerrain(true);
   printf("done\n");
}

float Terrain::GetHeight(float x, float z) {
   return (Terrain::mainTerrain->heightMap->GetPixel(
               x + MAP_SIZE_X * CHUNK_GRID, z + MAP_SIZE_Z * CHUNK_GRID) /
           255.f) *
          MAX_AMPLITUDE;
}

float Terrain::GetCeiling(float x, float z) {
   return (Terrain::mainTerrain->ceilingMap->GetPixel(x, z) / 255.f) *
          MAX_AMPLITUDE;
}

void Terrain::GenerateFarTerrain(char isCeiling) {
   if (isCeiling) {
      if (Terrain::mainTerrain->ceiling != NULL) {
         Terrain::mainTerrain->ceiling->Destroy();
         delete Terrain::mainTerrain->ceiling;
      }
      Terrain::mainTerrain->ceiling =
          Terrain::mainTerrain->CreateFarTerrain(true);
   } else {
      if (Terrain::mainTerrain->farTerrain != NULL) {
         Terrain::mainTerrain->farTerrain->Destroy();
         delete Terrain::mainTerrain->farTerrain;
      }
      Terrain::mainTerrain->farTerrain =
          Terrain::mainTerrain->CreateFarTerrain(false);
   }
}

void Terrain::RedrawAt(float x, float z, float radius) {
   int chunkRadius = glm::ceil(radius / CHUNK_GRID);
   int chunkX = x / CHUNK_GRID;
   int chunkZ = z / CHUNK_GRID;
   for (int x = -chunkRadius; x <= chunkRadius; x++) {
      for (int z = -chunkRadius; z <= chunkRadius; z++) {
         Terrain::mainTerrain->DestroyChunk(chunkX + x, chunkZ + z);
      }
   }
   GenerateFarTerrain(false);
   GenerateFarTerrain(true);
}

unsigned char caveWallsCurve(float x, float z, float thresholdX,
                             float farThresholdX, float thresholdZ,
                             float farThresholdZ) {
   float sum = 0.f;
   if (x < thresholdX) {
      sum += (thresholdX - x) / thresholdX;
   }
   if (x > farThresholdX) {
      sum += (x - farThresholdX + 1) / thresholdX;
   }
   if (z < thresholdZ) {
      sum += (thresholdZ - z) / thresholdZ;
   }
   if (z > farThresholdZ) {
      sum += (z - farThresholdZ + 1) / thresholdZ;
   }
   // smooth it
   sum = sum * sum;
   if (sum > 254) {
      return 254;
   }
   return sum * 254;
}

unsigned char Terrain::GenerateHeight(float x, float z) {
   int noise =
       glm::floor(254.f * stb_perlin_ridge_noise3(x * .0005f, 0.f, z * .0005f,
                                                  2.0f, 0.6f, 0.9f, 10));
   int walls =
       caveWallsCurve(x, z, CHUNK_GRID * 2, MAP_WORLD_LENGTH_X - CHUNK_GRID * 2,
                      CHUNK_GRID * 2, MAP_WORLD_LENGTH_Z - CHUNK_GRID * 2);
   if (walls + noise > 254) {
      return 254;
   }
   return walls + noise;
}

unsigned char Terrain::GenerateCeiling(float x, float z) {
   int noise =
       glm::floor(254.f * stb_perlin_ridge_noise3(x * .05f, 0.f, z * .05f, 2.0f,
                                                  0.3f, 0.9f, 10));
   int walls = caveWallsCurve(x, z, FAR_DETAIL * 2, MAP_VERT_X - FAR_DETAIL * 2,
                              FAR_DETAIL * 2, MAP_VERT_Z - FAR_DETAIL * 2);
   if (walls + noise > 254) {
      return 254;
   }
   return walls + noise;
}

Chunk *Terrain::CreateChunk(int xStride, int yStride) {
   class Chunk *chunk = new Chunk();
   float **heights = (float **)malloc(sizeof(float *) * CHUNK_VERT);
   for (int x = 0; x < CHUNK_VERT; x++) {
      *(heights + x) = (float *)malloc(sizeof(float) * CHUNK_VERT);
      for (int z = 0; z < CHUNK_VERT; z++) {
         heights[x][z] = Terrain::GetHeight(x + CHUNK_GRID * xStride,
                                            z + CHUNK_GRID * yStride);
      }
   }
   chunk->vertsX = CHUNK_VERT;
   chunk->vertsZ = CHUNK_VERT;
   chunk->x = xStride;
   chunk->y = yStride;
   chunk->transform.position =
       glm::vec3(CHUNK_GRID * xStride, 0.f, CHUNK_GRID * yStride);
   chunk->Generate(heights, false, false);
   for (int x = 0; x < CHUNK_VERT; x++) {
      free(*(heights + x));
   }
   free(heights);
   chunk->transform.parent = &transform;
   return chunk;
}

Chunk *Terrain::CreateFarTerrain(char isCeiling) {
   class Chunk *chunk = new Chunk();
   float **heights = (float **)malloc(sizeof(float *) * MAP_VERT_X);
   for (int x = 0; x < MAP_VERT_X; x++) {
      *(heights + x) = (float *)malloc(sizeof(float) * MAP_VERT_Z);
   }
   for (int x = 0; x < MAP_LENGTH_X; x++) {
      for (int z = 0; z < MAP_LENGTH_Z; z++) {
         for (int quadX = 0; quadX <= FAR_DETAIL; quadX++) {
            for (int quadZ = 0; quadZ <= FAR_DETAIL; quadZ++) {
               if (isCeiling) {
                  heights[x * FAR_DETAIL + quadX][z * FAR_DETAIL + quadZ] =
                      MAX_AMPLITUDE -
                      Terrain::GetCeiling(x * FAR_DETAIL + quadX,
                                          z * FAR_DETAIL + quadZ);
               } else {
                  if (chunkMap[x][z] == 'f' || chunkMap[x][z] == 'e') {
                     heights[x * FAR_DETAIL + quadX][z * FAR_DETAIL + quadZ] =
                         Terrain::GetHeight(
                             (x * FAR_DETAIL + quadX) * MAP_UNIT -
                                 MAP_SIZE_X * CHUNK_GRID,
                             (z * FAR_DETAIL + quadZ) * MAP_UNIT -
                                 MAP_SIZE_Z * CHUNK_GRID);
                  } else {
                     heights[x * FAR_DETAIL + quadX][z * FAR_DETAIL + quadZ] =
                         0.f;
                  }
               }
            }
         }
      }
   }
   chunk->vertsX = MAP_VERT_X;
   chunk->vertsZ = MAP_VERT_Z;
   chunk->x = 0;
   chunk->y = 0;
   chunk->Generate(heights, isCeiling, true);
   for (int x = 0; x < MAP_VERT_X; x++) {
      free(*(heights + x));
   }
   free(heights);
   chunk->transform.parent = &transform;
   chunk->transform.scale = glm::vec3(MAP_UNIT, 1.f, MAP_UNIT);
   chunk->transform.position =
       glm::vec3(-MAP_SIZE_X * CHUNK_GRID, FAR_MESH_HEIGHT_ADJUST,
                 -MAP_SIZE_Z * CHUNK_GRID);
   if (isCeiling) {
      chunk->transform.position.y += MAX_AMPLITUDE - 1;
   }
   return chunk;
}

void Terrain::Generate(int xStride, int yStride) {
   // if chunk already exists, overwrite it
   for (int i = 0; i < chunks.size(); i++) {
      if (chunks[i]->x == xStride && chunks[i]->y == yStride) {
         printf("duplicate found\n");
         exit(1);
         chunks[i] = CreateChunk(xStride, yStride);
         SetChunkMap(xStride, yStride, 'd');
         return;
      }
   }
   chunks.push_back(CreateChunk(xStride, yStride));
   SetChunkMap(xStride, yStride, 'd');
}

void Terrain::DestroyChunk(int xStride, int yStride) {
   for (int i = chunks.size() - 1; i >= 0; i--) {
      if (chunks[i]->x == xStride && chunks[i]->y == yStride) {
         chunks[i]->Destroy();
         delete chunks[i];
         chunks.erase(chunks.begin() + i);
         SetChunkMap(xStride, yStride, 'e');
         return;
      }
   }
}

char Terrain::GetChunkMap(int x, int z) {
   return chunkMap[x + MAP_SIZE_X][z + MAP_SIZE_Z];
}

char Terrain::SetChunkMap(int x, int z, char chunkChar) {
   chunkMap[x + MAP_SIZE_X][z + MAP_SIZE_Z] = chunkChar;
}

void Terrain::CullChunk(int xStride, int yStride, char cullType) {
   for (int i = 0; i < chunks.size(); i++) {
      if (chunks[i]->x == xStride && chunks[i]->y == yStride) {
         chunks[i]->meshRenderer->Cull();
         // set to c for culled
         SetChunkMap(xStride, yStride, cullType);
         return;
      }
   }
}

void Terrain::ActivateChunk(int xStride, int yStride) {
   for (int i = 0; i < chunks.size(); i++) {
      if (chunks[i]->x == xStride && chunks[i]->y == yStride) {
         chunks[i]->meshRenderer->Activate();
         // set it to d for drawn
         SetChunkMap(xStride, yStride, 'd');
         return;
      }
   }
}

void Terrain::Update() {
   glm::vec3 cameraPosition = Scene::mainCamera->transform->getGlobalPosition();
   int cameraX = glm::floor(cameraPosition.x * CHUNKS_PER_METER);
   int cameraZ = glm::floor(cameraPosition.z * CHUNKS_PER_METER);

   for (int x = -MAP_SIZE_X; x <= MAP_SIZE_X; x++) {
      for (int z = -MAP_SIZE_Z; z <= MAP_SIZE_Z; z++) {
         int distance = glm::abs(cameraX - x) + glm::abs(cameraZ - z);
         char chunkChar = GetChunkMap(x, z);
         if (distance < viewDistance) {
            if (chunkChar == 'e') {
               drawing = true;
               Generate(x, z);
               return;
            }
            if (Scene::mainCamera->ViewFrustCull(
                    glm::vec3(CHUNK_GRID * x, 0.f, CHUNK_GRID * z),
                    MAX_AMPLITUDE * 1.4142f)) {
               if (chunkChar != 'c' && chunkChar != 'e') {
                  CullChunk(x, z, 'c');
               }
            } else {
               if (chunkChar != 'd' && chunkChar != 'e') {
                  ActivateChunk(x, z);
               }
            }
         } else {
            if (chunkChar != 'f' && chunkChar != 'e') {
               CullChunk(x, z, 'f');
               drawing = true;
            }
            if (distance > DESTROY_DISTANCE) {
               if (chunkChar != 'e') {
                  DestroyChunk(x, z);
               }
            }
         }
      }
   }
   if (drawing) {
      if (Scene::gameState == 'l') {
         Scene::gameState = 'p';
      }
      GenerateFarTerrain(false);
      drawing = false;
   }
}
