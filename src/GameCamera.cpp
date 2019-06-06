#include "GameCamera.h"

void GameCamera::StartPan(Transform *start, Transform *end, float time, float startTime) {
   PanIndex = startTime;
   PanStart = start;
   PanEnd = end;
   PanTime = time;
}

void GameCamera::InitialPan() {
   StartPan(GroundUpTransform, ForwardTransform, 2.f, 0.f);
}

void GameCamera::FlipPan(char forward) {
   if (forward && (PanStart == ForwardTransform || PanStart == GroundUpTransform)) {
      StartPan(BackwardTransform, ForwardTransform, 0.5f, PanTime - PanIndex);
   }
   if (!forward && (PanStart == BackwardTransform || PanStart == GroundUpTransform)) {
      StartPan(ForwardTransform, BackwardTransform, 0.5f, PanTime - PanIndex);
   } 
}

void GameCamera::Awake() {
   mode = 0;
   if (PanStart == NULL) {
      PanStart = &transform;
   }
   if (PanEnd == NULL) {
      PanEnd = &transform;
   }
}

void GameCamera::Update() {
   if (Scene::gameState != 'l') {
      // panning
      if (PanIndex < PanTime) {
         transform.slerp(PanStart, PanEnd, PanIndex/PanTime);
         PanIndex += Scene::deltaTime;
         if (PanIndex >= PanTime) {
            PanIndex = PanTime;
            transform.slerp(PanStart, PanEnd, 1.f);
         }
      }
   }

   // debug camera
   if (keyboardInput != NULL && mode == 0) {
      if (keyboardInput->forwardInput == 1) {
         glm::vec3 cameraForward = Scene::mainCamera->transform->forward();
         cameraForward.y = 0.f;
         cameraForward.x *= -1.f;
         transform.position -=
            glm::normalize(cameraForward) * Scene::deltaTime * MOVE_SPEED;
      }
      if (keyboardInput->forwardInput == -1) {
         glm::vec3 cameraForward = Scene::mainCamera->transform->forward();
         cameraForward.y = 0.f;
         cameraForward.x *= -1.f;
         transform.position +=
            glm::normalize(cameraForward) * Scene::deltaTime * MOVE_SPEED;
      }
      if (keyboardInput->rightInput == 1) {
         glm::vec3 cameraRight = Scene::mainCamera->transform->right();
         cameraRight.y = 0.f;
         cameraRight.x *= -1.f;
         transform.position -=
            glm::normalize(cameraRight) * Scene::deltaTime * MOVE_SPEED;
      }
      if (keyboardInput->rightInput == -1) {
         glm::vec3 cameraRight = Scene::mainCamera->transform->right();
         cameraRight.y = 0.f;
         cameraRight.x *= -1.f;
         transform.position +=
            glm::normalize(cameraRight) * Scene::deltaTime * MOVE_SPEED;
      }
      if (keyboardInput->upInput == 1) {
         transform.position.y += Scene::deltaTime * MOVE_SPEED;
      }
      if (keyboardInput->upInput == -1) {
         transform.position.y -= Scene::deltaTime * MOVE_SPEED;
      }
   }
}
