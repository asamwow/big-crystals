#include "Camera.h"

Type Camera::getType() { return Type::Camera; }

void Camera::SetViewMatrix(std::shared_ptr<MatrixStack> Projection,
                           std::shared_ptr<MatrixStack> View) {
   Projection->pushMatrix();
   Projection->perspective(0.45f, aspect, nearClippingPlane, farClippingPlane);

   View->pushMatrix();
   View->loadIdentity();
   View->rotate(transform->rotation);
   View->translate(-transform->getGlobalPosition());
   View->pushMatrix();
}

float accessMatrix(int a, int b, int c, int d, char minus, glm::mat4 matrix) {
   if (!minus) {
      return matrix[b-1][a-1] + matrix[d-1][c-1];
   }
   return matrix[b-1][a-1] - matrix[d-1][c-1];
}

/*TODO fill in */
void Camera::ExtractVFPlanes(glm::mat4 P, glm::mat4 V) {

   /* composite matrix */
   glm::mat4 comp = P*V;

   Left.x = accessMatrix(4, 1, 1, 1, 0, comp);
   Left.y = accessMatrix(4, 2, 1, 2, 0, comp);
   Left.z = accessMatrix(4, 3, 1, 3, 0, comp);
   Left.w = accessMatrix(4, 4, 1, 4, 0, comp);
   planes[0] = Left;
  
   Right.x = accessMatrix(4, 1, 1, 1, 1, comp);
   Right.y = accessMatrix(4, 2, 1, 2, 1, comp);
   Right.z = accessMatrix(4, 3, 1, 3, 1, comp);
   Right.w = accessMatrix(4, 4, 1, 4, 1, comp);
   planes[1] = Right;

   Bottom.x = accessMatrix(4, 1, 2, 1, 0, comp);
   Bottom.y = accessMatrix(4, 2, 2, 2, 0, comp);
   Bottom.z = accessMatrix(4, 3, 2, 3, 0, comp);
   Bottom.w = accessMatrix(4, 4, 2, 4, 0, comp);
   planes[2] = Bottom;
  
   Top.x = accessMatrix(4, 1, 2, 1, 1, comp);
   Top.y = accessMatrix(4, 2, 2, 2, 1, comp);
   Top.z = accessMatrix(4, 3, 2, 3, 1, comp);
   Top.w = accessMatrix(4, 4, 2, 4, 1, comp);
   planes[3] = Top;

   Near.x = accessMatrix(4, 1, 3, 1, 0, comp);
   Near.y = accessMatrix(4, 2, 3, 2, 0, comp);
   Near.z = accessMatrix(4, 3, 3, 3, 0, comp);
   Near.w = accessMatrix(4, 4, 3, 4, 0, comp);
   planes[4] = Near;

   Far.x = accessMatrix(4, 1, 3, 1, 1, comp);
   Far.y = accessMatrix(4, 2, 3, 2, 1, comp);
   Far.z = accessMatrix(4, 3, 3, 3, 1, comp);
   Far.w = accessMatrix(4, 4, 3, 4, 1, comp);
   planes[5] = Far;

   for (int i = 0; i < 6; i++) {
      float size = glm::length(glm::vec3(planes[i].x, planes[i].y, planes[i].z));
      planes[i].x = planes[i].x/size;
      planes[i].y = planes[i].y/size;
      planes[i].z = planes[i].z/size;
      planes[i].w = planes[i].w/size;
   }
}

float Camera::DistToPlane(float A, float B, float C, float D, glm::vec3 point) {
   return A*point.x + B*point.y + C*point.z + D;
}

int Camera::ViewFrustCull(glm::vec3 center, float radius) {
   float dist;
   for (int i=0; i < 6; i++) {
      dist = DistToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, center);
      //test against each plane
      if (dist < -radius) {
         return 1;
      }
   }
   return 0; 
}
