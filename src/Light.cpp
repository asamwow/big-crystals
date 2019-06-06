#include "Light.h"

Type Light::getType() { return Type::Light; }

float Light::getRange() {
   // TODO 
   return intensity / dropOff;
}
