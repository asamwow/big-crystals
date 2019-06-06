#include "MouseInput.h"

Type MouseInput::getType() {
   return Type::MouseInput;
}

void MouseInput::cursorCallback(double xpos, double ypos) {
   double mouseMoveX = xpos - lastCursorX;
   double mouseMoveY = -(ypos - lastCursorY); // invert

   Scene::mainCamera->transform->rotation = glm::rotate(
      Scene::mainCamera->transform->rotation,
      glm::radians((float)mouseMoveX * SENSITIVITY), glm::vec3(0, 1, 0));

   glm::vec3 cameraRight = Scene::mainCamera->transform->right();
   cameraRight.x = -cameraRight.x;
   cameraRight.y = 0.f;

   Scene::mainCamera->transform->rotation = glm::rotate(
      Scene::mainCamera->transform->rotation,
      glm::radians((float)mouseMoveY * SENSITIVITY), cameraRight);

   lastCursorX = xpos;
   lastCursorY = ypos;
}
