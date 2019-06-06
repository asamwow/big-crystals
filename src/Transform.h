#ifndef TRANSFORM_INCLUDED
#define TRANSFORM_INCLUDED

#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MatrixStack.h"

class Transform {
 public:
   Transform(glm::vec3 position, glm::vec3 scale, glm::quat rotation);
   Transform();
   Transform(Transform &transform);

   // local values
   glm::vec3 position;
   glm::vec3 scale;
   glm::quat rotation;
   glm::vec3 getEulerAngles();
   void setEulerAngles(glm::vec3 eulerAngles);
   void setEulerAnglesRadians(glm::vec3 eulerAngles);

   // inherits transformation
   Transform *parent;

   // global values
   glm::vec3 getGlobalPosition();
   glm::vec3 getGlobalEulerAngles();
   glm::quat getGlobalRotation();
   glm::vec3 getGlobalScale();
   glm::vec3 forward();
   glm::vec3 right();
   glm::vec3 up();

   void slerp(Transform *startNode, Transform *endNode, float index);

   void SetGlobalMatrix(std::shared_ptr<MatrixStack> matirxStack);

   void log();
   void logGlobal();
};
#endif
