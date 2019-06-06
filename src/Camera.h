#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED

#include "Component.h"
#include "MatrixStack.h"

class Camera : public Component {
 public:
   Type getType();

   float fov;
   float aspect;
   float nearClippingPlane;
   float farClippingPlane;

   glm::vec4 Left, Right, Bottom, Top, Near, Far;
   glm::vec4 planes[6];
   
   void SetViewMatrix(std::shared_ptr<MatrixStack> Projection,
                      std::shared_ptr<MatrixStack> View);

   void ExtractVFPlanes(glm::mat4 P, glm::mat4 V);
   float DistToPlane(float A, float B, float C, float D, glm::vec3 point);
   int ViewFrustCull(glm::vec3 center, float radius);
};
   
#endif
