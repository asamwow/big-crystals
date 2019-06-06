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
         // glm::vec3 cameraRight = Scene::mainCamera->transform->right();
         // cameraRight.y = 0.f;
         // cameraRight.x *= -1.f;
         // transform.position +=
         //     glm::normalize(cameraRight) * Scene::deltaTime * RUN_SPEED;
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
}

void Player::Flip() {
   Bulb::Flip();
   gameCamera->FlipPan(forward);
}
