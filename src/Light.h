#ifndef LIGHT_INCLUDED
#define LIGHT_INCLUDED

#include "Component.h"

class Light : public Component {
 public:
   Type getType();

   glm::vec3 color;

   // intensity of light right next to it
   float intensity;
   // how quickly it drops off per unit
   float dropOff;
   
   float getRange();
};
   
#endif
