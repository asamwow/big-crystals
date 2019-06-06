#ifndef BULB_INCLUDED
#define BULB_INCLUDED

#include "GameObject.h"
#include "Component.h"
#include "Rigidbody.h"
#include "MeshRenderer.h"
#include "Terrain.h"
#include "CrystalStructure.h"

#define BULBS_MAX 500

class Bulb : public GameObject {
 public:

   static std::vector<Bulb*> bulbs;

   class MeshRenderer *meshRenderer;
   class Rigidbody *rigidbody;

   char forward = true;
   char burried = false;
   char burriedCeiling = false;
   float boostTime = 0.f;
   char childDepth = 0;
   glm::vec3 color;

   virtual void Flip();
   void Activate(char miniCrystal);
   void ResetVelocity();
   void Regenerate(glm::vec3 newColor);
   
   void Awake();
   void Update();
   void OnCollision(class Collider *other);
};

#endif
