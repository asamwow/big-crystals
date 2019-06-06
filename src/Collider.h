#ifndef COLLIDER_INCLUDED
#define COLLIDER_INCLUDED

#include <glm/gtc/quaternion.hpp>

#include "Component.h"

class Component;

class Collider : public Component {
 public:
   Type getType();

   float radius;
   char cylinder;
};

#endif
