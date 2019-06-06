#ifndef MESHRENDERER_INCLUDED
#define MESHRENDERER_INCLUDED

#include "Renderer.h"
#include "Mesh.h"

/// a gameobject with a material and a mesh can be rendered
class MeshRenderer : public Renderer {
 public:
   MeshRenderer() {}
   
   Type getType();

   char active = true;

   void Cull();
   void Activate();

   class Mesh* mesh;
};

#endif
