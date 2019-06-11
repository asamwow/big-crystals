#include "Chunk.h"

// plus one height for shadow
void Chunk::Generate(float **heights, char ceiling, char farMesh) {
   int vertCount = vertsX * vertsZ * 3;
   int gridCount = (vertsX - 1) * (vertsZ - 1) * 6;
   meshRenderer = (class MeshRenderer *)AddComponent(Type::MeshRenderer);
   mesh = (class Mesh *)AddComponent(Type::Mesh);
   meshRenderer->mesh = mesh;

   unsigned int *indices =
       (unsigned int *)malloc(gridCount * sizeof(unsigned int));
   float *vertices = (float *)malloc(vertCount * sizeof(float));
   float *colors = (float *)malloc(vertCount * sizeof(float));

   for (int x = 0; x < vertsX; x++) {
      for (int z = 0; z < vertsZ; z++) {

         // printf("%i, %i, %f\n", x, z, heights[x][z]);

         vertices[indexVertexArray(x, z) * 3 + 0] = x;
         vertices[indexVertexArray(x, z) * 3 + 1] = heights[x][z];
         vertices[indexVertexArray(x, z) * 3 + 2] = z;

         if (x == 0) {
            colors[indexVertexArray(x, z) * 3 + 0] = 0;
            colors[indexVertexArray(x, z) * 3 + 1] = 0;
            colors[indexVertexArray(x, z) * 3 + 2] = 0;
         }

         if (x == vertsX - 1 || z == vertsZ - 1) {
            continue;
         }

         colors[indexVertexArray(x + 1, z) * 3 + 0] = 0;
         colors[indexVertexArray(x + 1, z) * 3 + 1] = 0;
         colors[indexVertexArray(x + 1, z) * 3 + 2] = 0;

         glm::vec3 normal;
         if (farMesh) {
            normal = glm::normalize(glm::cross(
                glm::vec3(MAP_UNIT, heights[x][z] - heights[x + 1][z], 0),
                glm::vec3(0, heights[x][z] - heights[x][z + 1], MAP_UNIT)));
         } else {
            normal = glm::normalize(glm::cross(
                glm::vec3(1.f, heights[x][z] - heights[x + 1][z], 0),
                glm::vec3(0, heights[x][z] - heights[x][z + 1], 1.f)));
         }
         if (ceiling) {
            normal *= -1.f;
         }

         // baked lighting
         glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, 0.1f, -1));
         float lightIntensitey = 1.f;
         if (Scene::currentScene->lightingEnabled == 1) {
            for (int i = 0; i < Scene::currentScene->lights.size(); i++) {
               class Light *light =
                   (class Light *)Scene::currentScene->lights[i];
               glm::vec3 lightPosition = light->transform->getGlobalPosition();
               glm::vec3 vertexPosition;
               if (farMesh) {
                  vertexPosition = glm::vec3(
                      x * MAP_UNIT - FAR_DETAIL * MAP_SIZE_X * MAP_UNIT,
                      heights[x][z],
                      z * MAP_UNIT - FAR_DETAIL * MAP_SIZE_Z * MAP_UNIT);
               } else {
                  vertexPosition = transform.getGlobalPosition() +
                                   glm::vec3(x, heights[x][z], z);
               }
               float distance = glm::distance(lightPosition, vertexPosition);
               // printf("dist:%f\n", distance);
               if (glm::abs(distance) < light->intensity) {
                  lightIntensitey =
                      glm::pow((light->intensity - distance) / distance, 2) *
                      LIGHTING_MAX;
                  colors[Chunk::indexVertexArray(x + 1, z) * 3 + 0] +=
                      lightIntensitey * light->color.x;
                  colors[Chunk::indexVertexArray(x + 1, z) * 3 + 1] +=
                      lightIntensitey * light->color.y;
                  colors[Chunk::indexVertexArray(x + 1, z) * 3 + 2] +=
                      lightIntensitey * light->color.z;
               }
               lightDirection = glm::normalize(vertexPosition - lightPosition);
            }
         }
         float farMeshShadow = 0.f;
         if (!farMesh) {
            // to help blend between far and near terrain
            // take into account the normal of the far mesh this replaces
            glm::vec3 farMeshNormal = glm::normalize(glm::cross(
                glm::vec3(1.f, heights[0][0] - heights[CHUNK_VERT-1][0],
                          0),
                glm::vec3(0, heights[0][0] - heights[0][CHUNK_VERT-1],
                          1.f)));
            farMeshShadow =
                glm::max(glm::dot(farMeshNormal, lightDirection), 0.f) * 0.02f;
         }
         float shadow = glm::max(glm::dot(normal, lightDirection), 0.f);
         // printf("shadw %f\n", shadow);

         // default color
         if (ceiling) {
            colors[indexVertexArray(x + 1, z) * 3 + 0] +=
                glm::max(CEILING_R - shadow, 0.f);
            colors[indexVertexArray(x + 1, z) * 3 + 1] +=
                glm::max(CEILING_G - shadow, 0.f);
            colors[indexVertexArray(x + 1, z) * 3 + 2] +=
                glm::max(CEILING_B - shadow, 0.f);
         } else {
            colors[indexVertexArray(x + 1, z) * 3 + 0] +=
                glm::max(GROUND_R - shadow - farMeshShadow, 0.f);
            colors[indexVertexArray(x + 1, z) * 3 + 1] +=
                glm::max(GROUND_G - shadow - farMeshShadow, 0.f);
            colors[indexVertexArray(x + 1, z) * 3 + 2] +=
                glm::max(GROUND_B - shadow - farMeshShadow, 0.f);
         }

         // if (x == vertsX - 1 || z == vertsZ - 1)
         //    continue;
         if (heights[x][z] == 0.f && !ceiling && farMesh) {
            indices[indexIndiceArray(x, z) * 6 + 0] = 0;
            indices[indexIndiceArray(x, z) * 6 + 1] = 0;
            indices[indexIndiceArray(x, z) * 6 + 2] = 0;

            indices[indexIndiceArray(x, z) * 6 + 5] = 0;
            indices[indexIndiceArray(x, z) * 6 + 4] = 0;
            indices[indexIndiceArray(x, z) * 6 + 3] = 0;
            continue;
         }

         // triangles
         if (!ceiling) {
            indices[indexIndiceArray(x, z) * 6 + 0] = indexVertexArray(x, z);
            indices[indexIndiceArray(x, z) * 6 + 1] =
                indexVertexArray(x, z + 1);
            indices[indexIndiceArray(x, z) * 6 + 2] =
                indexVertexArray(x + 1, z);

            indices[indexIndiceArray(x, z) * 6 + 5] =
                indexVertexArray(x + 1, z);
            indices[indexIndiceArray(x, z) * 6 + 4] =
                indexVertexArray(x + 1, z + 1);
            indices[indexIndiceArray(x, z) * 6 + 3] =
                indexVertexArray(x, z + 1);
         } else {
            indices[indexIndiceArray(x, z) * 6 + 1] = indexVertexArray(x, z);
            indices[indexIndiceArray(x, z) * 6 + 0] =
                indexVertexArray(x, z + 1);
            indices[indexIndiceArray(x, z) * 6 + 2] =
                indexVertexArray(x + 1, z);

            indices[indexIndiceArray(x, z) * 6 + 5] =
                indexVertexArray(x + 1, z);
            indices[indexIndiceArray(x, z) * 6 + 3] =
                indexVertexArray(x + 1, z + 1);
            indices[indexIndiceArray(x, z) * 6 + 4] =
                indexVertexArray(x, z + 1);
         }
      }
   }
   meshRenderer->mesh->SetData(indices, gridCount, vertices, vertCount, colors,
                               vertCount);
}
