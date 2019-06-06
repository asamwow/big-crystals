#ifndef CRYSTALSTRUCTURE_INCLUDED
#define CRYSTALSTRUCTURE_INCLUDED

#include "Bulb.h"
#include "Collider.h"
#include "Component.h"
#include "Crystal.h"
#include "GameObject.h"
#include "MeshRenderer.h"

class CrystalStructure : public GameObject {
 public:
   class Collider *collider;
   class Light *light;

   class Crystal **crystals;
   glm::vec3 color;
   char activated = false;
   char pillars;
   char miniCrystal = false;
   char colorChanges = 0;

   void Generate();
   void ChangeLight(glm::vec3 color, float intensity);
   void CreateCrystal(glm::vec3 position, glm::vec3 rotation,
                               glm::vec3 scale, int index);

   void Awake();
   void Start();
};

#endif
