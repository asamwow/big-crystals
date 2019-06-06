#ifndef COMPONENT_INCLUDED
#define COMPONENT_INCLUDED

#include "GameObject.h"
#include "Transform.h"
#include "Types.h"

class Component {
 public:
   /// linked list
   Component *next;

   Transform *transform;
   void* gameObject;
   
   virtual Type getType() = 0;
   static Component *Create();
};

#endif
