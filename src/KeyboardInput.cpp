#include "KeyboardInput.h"

Type KeyboardInput::getType() { return Type::KeyboardInput; }

#define GLFW_PRESS 1

#define GLFW_RELEASE 0

void KeyboardInput::keyCallBack(int key, int action) {
   if (key == forwardKey && action == GLFW_PRESS) {
      forwardInput = 1;
   }
   if (key == backwardKey && action == GLFW_PRESS) {
      forwardInput = -1;
   }
   if (key == rightKey && action == GLFW_PRESS) {
      rightInput = 1;
   }
   if (key == leftKey && action == GLFW_PRESS) {
      rightInput = -1;
   }
   if (key == upKey && action == GLFW_PRESS) {
      upInput = 1;
   }
   if (key == downKey && action == GLFW_PRESS) {
      upInput = -1;
   }
   
   if (key == forwardKey && (action == GLFW_RELEASE)) {
      if (forwardInput == 1)
         forwardInput = 0;
   }
   if (key == backwardKey && (action == GLFW_RELEASE)) {
      if (forwardInput == -1)
         forwardInput = 0;
   }
   if (key == rightKey && (action == GLFW_RELEASE)) {
      if (rightInput == 1)
         rightInput = 0;
   }
   if (key == leftKey && (action == GLFW_RELEASE)) {
      if (rightInput == -1)
         rightInput = 0;
   }
   if (key == upKey && (action == GLFW_RELEASE)) {
      if (upInput == 1)
         upInput = 0;
   }
   if (key == downKey && (action == GLFW_RELEASE)) {
      if (upInput == -1)
         upInput = 0;
   }
}
