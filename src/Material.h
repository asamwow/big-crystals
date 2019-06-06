#ifndef MATERIAL_INCLUDED
#define MATERIAL_INCLUDED

#include "Component.h"
#include "Program.h"

/// contains shader settings
class Material : public Component {
 public:
   Type getType();
   static Material *defaultMaterial;
   static Material *crystalMaterial;
   
   Program *shader;

   float emission;
};

#endif
