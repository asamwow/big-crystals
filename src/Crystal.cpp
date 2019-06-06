#include "Crystal.h"

void Crystal::Awake() {
   meshRenderer = (class MeshRenderer*)AddComponent(Type::MeshRenderer);
   meshRenderer->material = Material::crystalMaterial;
   meshRenderer->mesh = (class Mesh*)AddComponent(Type::Mesh);
}

void Crystal::Update() {
   if (Scene::mainCamera->ViewFrustCull(transform.getGlobalPosition(), 100.f)) {
      meshRenderer->Cull();
   } else {
      meshRenderer->Activate();
   }
}
