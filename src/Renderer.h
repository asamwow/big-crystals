#ifndef RENDERER_INCLUDED
#define RENDERER_INCLUDED

#include "Component.h"
#include "Material.h"

/// a gameobject with a material and a mesh can be rendered
class Renderer : public Component {
 public:
   Type getType();
   class Material* material;
   void SetLighted();
};

#endif
