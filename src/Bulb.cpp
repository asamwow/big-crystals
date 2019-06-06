#include "Bulb.h"

class std::vector<Bulb*> Bulb::bulbs;

void Bulb::Awake() {
   meshRenderer = (class MeshRenderer *)AddComponent(Type::MeshRenderer);
   meshRenderer->material = Material::crystalMaterial;
   rigidbody = (class Rigidbody *)AddComponent(Type::Rigidbody);
   ResetVelocity();
   rigidbody->radius = 1.f;
   transform.scale = glm::vec3(1.f, 1.f, 1.2f);
   bulbs.push_back(this);
   if (bulbs.size() > BULBS_MAX) {
      // bulbs[1]->Destroy();
      // we need to do custom destroy to preserve mesh
      bulbs[1]->DestroyComponent(Type::Rigidbody);
      bulbs[1]->DestroyComponent(Type::MeshRenderer);
      Scene::currentScene->RemoveGameObject(bulbs[1]);
      bulbs.erase(bulbs.begin() + 1);
   }
}

void Bulb::Activate(char miniCrystal) {
   if (boostTime > 0 || childDepth > 2) {
      boostTime = 1.f;
      return;
   }
   int spawns;
   if (miniCrystal) {
      boostTime = 1.f;
      spawns = 3;
   } else {
      boostTime = 2.f;
      spawns = 5;
   }
   childDepth++;
   for (int i = 0; i < spawns; i++) {
      class Bulb *bulb = new class Bulb();
      bulb->transform.position = transform.position;
      if (miniCrystal) {
         bulb->transform.rotation =
             transform.rotation * glm::angleAxis(glm::radians(-3.f + 3.f * i),
                                                 glm::vec3(0.f, 1.f, 0.f));
      } else {
         bulb->transform.rotation =
             transform.rotation * glm::angleAxis(glm::radians(-6.f + 3.f * i),
                                                 glm::vec3(0.f, 1.f, 0.f));
      }
      bulb->forward = forward;
      bulb->Awake();
      bulb->meshRenderer->mesh = meshRenderer->mesh;
      bulb->boostTime = boostTime;
      bulb->childDepth = childDepth;
      bulb->color = color;
   }
}

void Bulb::Flip() {
   if (forward) {
      forward = false;
   } else {
      forward = true;
   }
   ResetVelocity();
}

void Bulb::ResetVelocity() {
   if (forward) {
      rigidbody->velocity.z = 200.f;
   } else {
      rigidbody->velocity.z = -200.f;
   }
}

void Bulb::Update() {
   if (Scene::mainCamera->ViewFrustCull(transform.getGlobalPosition(), 1.f)) {
      meshRenderer->Cull();
   } else {
      meshRenderer->Activate();
   }
   if (transform.position.x < -MAP_SIZE_X * CHUNK_GRID) {
      transform.position.x = -MAP_SIZE_X * CHUNK_GRID;
      Flip();
   }
   if (transform.position.x > (MAP_SIZE_X + 1) * CHUNK_GRID) {
      transform.position.x = (MAP_SIZE_X + 1) * CHUNK_GRID;
      Flip();
   }
   if (transform.position.z < -MAP_SIZE_Z * CHUNK_GRID) {
      transform.position.z = -MAP_SIZE_Z * CHUNK_GRID;
      Flip();
   }
   if (transform.position.z > (MAP_SIZE_Z + 1) * CHUNK_GRID) {
      transform.position.z = (MAP_SIZE_Z + 1) * CHUNK_GRID;
      Flip();
   }
   if (transform.position.y <
       Terrain::GetHeight(transform.position.x, transform.position.z)) {
      if (!burried) {
         burried = true;
         Flip();
      }
   } else {
      burried = false;
   }
   if (transform.position.y > MAX_AMPLITUDE * 1.7f) {
      if (!burriedCeiling) {
         burriedCeiling = true;
         Flip();
      }
   } else {
      burriedCeiling = false;
   }
   boostTime -= Scene::deltaTime;
   if (boostTime > 0) {
      if (forward) {
         rigidbody->velocity.z = 200.f + boostTime * 150.f;
      } else {
         rigidbody->velocity.z = -200.f - boostTime * 150.f;
      }
      transform.scale.z = 1.2f + boostTime * 0.8f;
   } else {
      ResetVelocity();
      transform.scale.z = 1.2f;
   }
}

void Bulb::Regenerate(glm::vec3 newColor) {
   meshRenderer->mesh = (class Mesh *)AddComponent(Type::Mesh);
   meshRenderer->mesh->SetPrimitiveSphere(20, 20, 1.f, color, false);
}

void Bulb::OnCollision(class Collider *other) {
   CrystalStructure *crystal = (CrystalStructure *)(other->gameObject);
   if (crystal->miniCrystal) {
      crystal->ChangeLight(color, 0.f);
   } else {
      if (color != crystal->color) {
         color = color + crystal->color;
         crystal->ChangeLight(color, 500.f);
         Regenerate(color);
      }
   }
   Activate(crystal->miniCrystal);
}
