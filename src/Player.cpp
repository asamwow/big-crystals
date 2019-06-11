#include "Player.h"

void Player::Awake() {
   Bulb::Awake();
   meshRenderer->mesh = (class Mesh *)AddComponent(Type::Mesh);
   meshRenderer->mesh->SetPrimitiveSphere(20, 20, 1.f,
                                          glm::vec3(0.6f, 0.6f, 0.6f), false);
   color = glm::vec3(0.f, 0.f, 1.f);
}

void Player::Update() {
   Bulb::Update();
   childDepth = 0;
   delay += Scene::deltaTime;
   if (delay > 1.f) {
     if (flipQueued) {
       gameCamera->ResetDistance(forward);
       if (startedFlipForward != forward) {
         gameCamera->FlipPan(forward);
       }
       flipQueued = false;
     }
   }
   glm::vec3 direction = glm::vec3(0, 0, 0);
   if (keyboardInput != NULL) {
      if (keyboardInput->forwardInput == -1) {
         direction.x = 15.f;
      }
      if (keyboardInput->forwardInput == 1 || burried) {
         direction.x = -15.f;
      }
      if (keyboardInput->rightInput == 1) {
         direction.y = -30.f;
      }
      if (keyboardInput->rightInput == -1) {
         direction.y = 30.f;
      }
      if (!forward) {
         direction.x = -direction.x;
      }
   }
   transform.setEulerAngles(direction);
   if (transform.position.y > MAX_AMPLITUDE * 1.2f) {
      transform.position.y = MAX_AMPLITUDE * 1.2f;
   }
   for (int i = 0; i < Scene::currentScene->lights.size(); i++) {
      class Light *light = (class Light *)Scene::currentScene->lights[i];
      float distance = glm::distance(light->transform->getGlobalPosition(),
                                     transform.getGlobalPosition());
      if (distance < closestCrystalDistance ||  far == i + 1) {
         closestCrystalDistance = distance;
      } else {
         continue;
      }
      if (distance < 1000.f) {
         if (far == 0) {
            far = i+1;
            gameCamera->FarPan(far, forward);
         }
      } else {
         if (far > 0 ) {
            gameCamera->FarPan(false, forward);
         }
         far = false;
      }
   }
}

void Player::Flip() {
  if (delay > 1.f) {
   startedFlipForward = forward;
  }
   Bulb::Flip();
   flipQueued = true;
   delay = 0.f;
}

void Player::OnCollision(class Collider *other) {
  Bulb::OnCollision(other);
  CrystalStructure *crystal = (CrystalStructure *)(other->gameObject);
  if (crystal->miniCrystal) {
    Scene::makeSoundEffect = 2;
  } else {
    Scene::makeSoundEffect = 1;
  }
}
