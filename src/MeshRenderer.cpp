#include "MeshRenderer.h"

Type MeshRenderer::getType() { return Type::MeshRenderer; }

void MeshRenderer::Cull() {
   if (!active) {
      return;
   }
   Scene::currentScene->CullRenderer((void *)(this));
   active = false;
}

void MeshRenderer::Activate() {
   if (active) {
      return;
   }
   Scene::currentScene->ActivateRenderer((void *)(this));
   active = true;
}
