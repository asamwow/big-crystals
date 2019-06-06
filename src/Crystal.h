#ifndef CRYSTAL_INCLUDED
#define CRYSTAL_INCLUDED

#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "Bulb.h"

class Crystal : public GameObject {
 public:

   class MeshRenderer *meshRenderer;

   void Awake();
   void Update();
};

#endif
