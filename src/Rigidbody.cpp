#include "Rigidbody.h"

Type Rigidbody::getType() { return Type::Rigidbody; }

void Rigidbody::PhysicsStep(float deltaTime) {
   transform->position += transform->rotation * velocity * deltaTime;
   for (int i = 0; i < Scene::currentScene->colliders.size(); i++) {
      class Collider *collider =
          (class Collider *)Scene::currentScene->colliders[i];
      bool collision = false;
      if (collider->cylinder) {
         glm::vec3 colliderPos = collider->transform->getGlobalPosition();
         glm::vec3 rbodyPos = transform->getGlobalPosition();
         colliderPos.y = 0.f;
         rbodyPos.y = 0.f;
         if (glm::distance(colliderPos, rbodyPos) < collider->radius + radius) {
            collision = true;
         }
      } else {
         if (glm::distance(collider->transform->getGlobalPosition(),
                           transform->getGlobalPosition()) <
             collider->radius + radius) {
            collision = true;
         }
      }
      if (collision) {
         if (!colliding) {
            ((class GameObject *)gameObject)->OnCollision(collider);
            colliding = true;
         }
      } else {
         colliding = false;
      }
   }
}
