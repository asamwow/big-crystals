#include "Transform.h"

Transform::Transform() {
   position = glm::vec3(0.0f, 0.0f, 0.0f);
   // there must be a more efficient way to initialize rotation
   rotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.f, 1.f, 0.f));
   scale = glm::vec3(1.0f, 1.0f, 1.0f);
   parent = NULL;
}

Transform::Transform(Transform &transform) {
   position = glm::vec3(transform.position.x, transform.position.y,
                        transform.position.z);
   // there must be a more efficient way to initialize rotation
   rotation = glm::quat(transform.rotation.w, transform.rotation.x,
                        transform.rotation.y, transform.rotation.z);
   scale = glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z);
   parent = NULL;
}

Transform::Transform(glm::vec3 position, glm::vec3 scale, glm::quat rotation)
    : position(position), scale(scale), rotation(rotation) {
   parent = NULL;
}

glm::vec3 Transform::getGlobalPosition() {
   if (parent == NULL) {
      return position;
   }
   return position + parent->getGlobalPosition();
}

glm::vec3 Transform::getGlobalScale() {
   if (parent == NULL) {
      return scale;
   }
   return scale * parent->getGlobalScale();
}

glm::quat Transform::getGlobalRotation() {
   if (parent == NULL) {
      return rotation;
   }
   return rotation * parent->getGlobalRotation();
}

glm::vec3 Transform::getEulerAngles() { return eulerAngles(rotation); }

void Transform::setEulerAngles(glm::vec3 eulerAngles) {
   rotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.f, 1.f, 0.f));
   rotation =
       glm::rotate(rotation, glm::radians(eulerAngles.x), glm::vec3(1, 0, 0));
   rotation =
       glm::rotate(rotation, glm::radians(eulerAngles.z), glm::vec3(0, 0, 1));
   rotation =
       glm::rotate(rotation, glm::radians(eulerAngles.y), glm::vec3(0, 1, 0));
}

void Transform::setEulerAnglesRadians(glm::vec3 eulerAngles) {
   rotation = glm::angleAxis(0.0f, glm::vec3(0.f, 1.f, 0.f));
   rotation = glm::rotate(rotation, eulerAngles.x, glm::vec3(1, 0, 0));
   rotation = glm::rotate(rotation, eulerAngles.z, glm::vec3(0, 0, 1));
   rotation = glm::rotate(rotation, eulerAngles.y, glm::vec3(0, 1, 0));
}

glm::vec3 Transform::getGlobalEulerAngles() {
   glm::vec3 eulerAngles = glm::eulerAngles(getGlobalRotation());
   eulerAngles.x = glm::degrees(eulerAngles.x);
   eulerAngles.y = glm::degrees(eulerAngles.x);
   eulerAngles.z = glm::degrees(eulerAngles.x);
}

void Transform::slerp(Transform *startNode, Transform *endNode, float index) {
   if (index < 0) {
      index = 0;
   }
   if (index > 1) {
      index = 1;
   }
   index = index * index * index * (index * (index * 6 - 15) + 10);
   position = glm::mix(startNode->position, endNode->position, index);
   scale = glm::mix(startNode->scale, endNode->scale, index);
   rotation = glm::mix(startNode->rotation, endNode->rotation, index);
}

void Transform::SetGlobalMatrix(std::shared_ptr<MatrixStack> matrixStack) {
   matrixStack->pushMatrix();
   matrixStack->loadIdentity();
   matrixStack->translate(getGlobalPosition());
   matrixStack->rotate(getGlobalRotation());
   matrixStack->pushMatrix();
   matrixStack->scale(getGlobalScale());
}

glm::vec3 Transform::forward() { return rotation * glm::vec3(0.f, 0.f, 1.f); }

glm::vec3 Transform::right() { return rotation * glm::vec3(1.f, 0.f, 0.f); }

glm::vec3 Transform::up() { return rotation * glm::vec3(0.f, 1.f, 0.f); }

void Transform::log() {
   printf("Transform:\n");
   printf("Pos:%f, %f, %f\n", position.x, position.y, position.z);
   printf("Rot:%f, %f, %f, %f\n", rotation.w, rotation.x, rotation.y,
          rotation.z);
   printf("Scl:%f, %f, %f\n", scale.x, scale.y, scale.z);
}

void Transform::logGlobal() {
   printf("Transform:\n");
   glm::vec3 globalPosition = getGlobalPosition();
   glm::quat globalRotation = getGlobalRotation();
   glm::vec3 globalScale = getGlobalScale();
   printf("Pos:%f, %f, %f\n", globalPosition.x, globalPosition.y,
          globalPosition.z);
   printf("Rot:%f, %f, %f, %f\n", globalRotation.w, globalRotation.x,
          globalRotation.y, globalRotation.z);
   printf("Scl:%f, %f, %f\n", globalScale.x, globalScale.y, globalScale.z);
}
