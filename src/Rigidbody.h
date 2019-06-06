#ifndef RIGIDBODY_INCLUDED
#define RIGIDBODY_INCLUDED

#include <glm/gtc/matrix_transform.hpp>

#include "Component.h"
#include "Collider.h"

#define vector3 glm::vec3

/// non-static objects that interact with physics engine
class Rigidbody : public Component {
 public:
   Type getType();

   vector3 velocity;
   float radius;
   char colliding = true;

   virtual void PhysicsStep(float deltaTime);
};

#endif
