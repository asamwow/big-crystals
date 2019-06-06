#ifndef KEYBOARDINPUT_INCLUDED
#define KEYBOARDINPUT_INCLUDED

#define MOVE_SPEED 100.f

#include "Component.h"


/// a gameobject with a material and a mesh can be rendered
class KeyboardInput : public Component {
 public:
   Type getType();

   int forwardInput;
   int rightInput;
   int upInput;

   int forwardKey;
   int backwardKey;
   int rightKey;
   int leftKey;
   int upKey;
   int downKey;

   void keyCallBack(int key, int action);
};

#endif
