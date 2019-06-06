#ifndef MOUSEINPUT_INCLUDED
#define MOUSEINPUT_INCLUDED

#define SENSITIVITY 0.02f

//#include <GLFW/glfw3.h>

#include "Component.h"
#include "Camera.h"

/// a gameobject with a material and a mesh can be rendered
class MouseInput : public Component {
 public:
   Type getType();

   int cursorTravelX;
   int cursorTravelY;

   double lastCursorX;
   double lastCursorY;

   void cursorCallback(double xpos, double ypos);
};

#endif



