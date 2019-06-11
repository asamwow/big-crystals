#include "GameCamera.h"

void GameCamera::StartPan(Transform *start, Transform *end, float time,
                          float startTime, Transform *toMod) {
   PanIndex = startTime;
   PanStart = start;
   PanEnd = end;
   PanTime = time;
   ModifyTransform = toMod;
}

void GameCamera::InitialPan() {
   StartPan(GroundUpTransform, ForwardTransform, 2.f, 0.f, &transform);
}

void GameCamera::FlipPan(char forward) {
  if (forward) {
      StartPan(BackwardTransform, ForwardTransform, 0.5f, PanTime - PanIndex,
               &transform);
   }
  if (!forward) {
      StartPan(ForwardTransform, BackwardTransform, 0.5f, PanTime - PanIndex,
               &transform);
   }
}

void GameCamera::FarPan(char far, char forward) {
   Transform *nearTransform;
   Transform *farTransform;
   Transform *modTransform;
   if (forward) {
      nearTransform = ForwardNearTransform;
      farTransform = ForwardFarTransform;
      modTransform = ForwardTransform;
   } else {
      nearTransform = BackwardNearTransform;
      farTransform = BackwardFarTransform;
      modTransform = BackwardTransform;
   }
   if (far && PanStart != nearTransform) {
      StartPan(nearTransform, farTransform, 3.f, 0.f,
               modTransform);
   }
   if (!far && PanStart != farTransform) {
      StartPan(&transform, nearTransform, 3.f, 0.f,
               modTransform);
   }
}

void GameCamera::ResetDistance() {
   ForwardTransform->Copy(ForwardNearTransform);
   BackwardTransform->Copy(BackwardNearTransform);
}

void GameCamera::ResetDistance(char forward) {
  if (!forward) {
    BackwardTransform->Copy(BackwardNearTransform);
  } else {
    ForwardTransform->Copy(ForwardNearTransform);
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
         ModifyTransform->slerp(PanStart, PanEnd, PanIndex / PanTime);
         transform.slerp(PanStart, PanEnd, PanIndex / PanTime);
         PanIndex += Scene::deltaTime;
         if (PanIndex >= PanTime) {
            PanIndex = PanTime;
            ModifyTransform->slerp(PanStart, PanEnd, 1.f);
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
