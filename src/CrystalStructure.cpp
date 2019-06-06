#include "CrystalStructure.h"

void CrystalStructure::Awake() {
   collider = (class Collider *)AddComponent(Type::Collider);
   collider->radius = 15.f;
   if (miniCrystal) {
      pillars = rand() % 4 + 2;
      color = glm::vec3(0.3f, 0.3f, 0.3f);
      transform.scale = glm::vec3(2.f, 10.f, 2.f);
      collider->cylinder = false;
   } else {
      light = (class Light *)AddComponent(Type::Light);
      light->dropOff = 1.f;
      light->intensity = 350.f;
      light->color = color * 0.1f;
      transform.scale = glm::vec3(10.f, 100.f, 10.f);
      collider->cylinder = true;
   }
   crystals = NULL;
   Generate();
}

void CrystalStructure::Start() {
   // terrain must already be initialized
   if (miniCrystal) {
      int x = rand() % MAP_WORLD_LENGTH_X - MAP_SIZE_X * CHUNK_GRID;
      int z = rand() % MAP_WORLD_LENGTH_Z - MAP_SIZE_Z * CHUNK_GRID;
      transform.position = glm::vec3(x, Terrain::GetHeight(x, z)+5.f, z);
   } 
}

void CrystalStructure::ChangeLight(glm::vec3 newColor, float intensity) {
   if (color == newColor) {
      return;
   }
   if (colorChanges >= 2) {
      return;
   }
   colorChanges++;
   color = newColor;
   if (!miniCrystal) {
      light->color = color;
      light->intensity = intensity;
      Terrain::RedrawAt(transform.position.x, transform.position.z, intensity);
   }
   Generate();
}

void CrystalStructure::Generate() {
   if (crystals != NULL) {
      for (int i = 0; i < pillars; i++) {
         crystals[i]->Destroy();
         free(crystals[i]);
      }
      free(crystals);
      crystals = NULL;
   }
   if (pillars == 1) {
      CreateCrystal(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
                    glm::vec3(1.f, 1.f, 1.f), 0);
   }
   if (pillars == 2) {
      CreateCrystal(glm::vec3(-3.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 10.f),
                    glm::vec3(1.f, 1.f, 1.f), 0);
      CreateCrystal(glm::vec3(3.f, 0.f, 0.f), glm::vec3(0.f, 90.f, -10.f),
                    glm::vec3(1.f, 1.f, 1.f), 1);
   }
   if (pillars == 3) {
      CreateCrystal(glm::vec3(-3.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 10.f),
                    glm::vec3(0.7f, 1.f, 1.f), 0);
      CreateCrystal(glm::vec3(3.f, 0.f, 0.f), glm::vec3(0.f, 30.f, -10.f),
                    glm::vec3(1.f, 0.9f, 0.8f), 1);
      CreateCrystal(glm::vec3(0.f, 0.f, 9.f), glm::vec3(-5.f, 50.f, 0.f),
                    glm::vec3(1.1f, 1.1f, 1.1f), 2);
   }
   if (pillars == 4) {
      CreateCrystal(glm::vec3(-3.f, 0.f, 2.f), glm::vec3(10.f, 30.f, -10.f),
                    glm::vec3(1.f, 1.f, 1.f), 0);
      CreateCrystal(glm::vec3(-3.f, 0.f, -3.f), glm::vec3(10.f, 0.f, 10.f),
                    glm::vec3(0.9f, 0.f, 0.f), 1);
      CreateCrystal(glm::vec3(3.f, 0.f, 3.f), glm::vec3(-10.f, 230.f, -10.f),
                    glm::vec3(1.1f, 1.1f, 1.1f), 2);
      CreateCrystal(glm::vec3(3.f, 0.f, -3.f), glm::vec3(-10.f, 50.f, 10.f),
                    glm::vec3(1.f, 1.f, 1.f), 3);
   }
   if (pillars == 5) {
      CreateCrystal(glm::vec3(-3.f, 0.f, 2.f), glm::vec3(0.f, 20.f, 10.f),
                    glm::vec3(0.7f, 0.6f, 0.7f), 0);
      CreateCrystal(glm::vec3(3.f, 0.f, 2.f), glm::vec3(0.f, 100.f, -10.f),
                    glm::vec3(0.7f, 0.7f, 0.7f), 1);
      CreateCrystal(glm::vec3(0.f, 0.f, 1.f), glm::vec3(5.f, 150.f, 0.f),
                    glm::vec3(1.2f, 1.2f, 1.2f), 2);
      CreateCrystal(glm::vec3(1.f, 0.f, 3.f), glm::vec3(0.f, 280.f, -10.f),
                    glm::vec3(0.4f, 0.5f, 0.4f), 3);
      CreateCrystal(glm::vec3(3.f, 0.f, 3.f), glm::vec3(0.f, 70.f, -10.f),
                    glm::vec3(0.5f, 0.8f, 0.6f), 4);
   }
}

void CrystalStructure::CreateCrystal(glm::vec3 position, glm::vec3 rotation,
                                     glm::vec3 scale, int index) {
   if (crystals == NULL) {
      crystals = (class Crystal **)malloc(sizeof(Crystal*) * pillars);
   }
   crystals[index] = new Crystal();
   crystals[index]->Awake();
   crystals[index]->transform.parent = &transform;
   crystals[index]->transform.position = position;
   crystals[index]->transform.setEulerAngles(rotation);
   crystals[index]->transform.scale = scale;
   crystals[index]->meshRenderer->mesh->SetPrimitiveSphere(5, 5, 1.f, color, true);
}
