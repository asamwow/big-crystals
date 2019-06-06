#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include <stdlib.h>
#include <vector>

//#include "Camera.h"

class Scene {
 public:
   static Scene *currentScene;
   static class Camera *mainCamera;
   static class Transform *mainPlayer;
   static float deltaTime;
   static char gameState;

   char lightingEnabled = 1;

   std::vector<void *> sceneObjects;
   std::vector<void *> renderers;
   std::vector<void *> culledRenderers;
   std::vector<void *> physicsObjects;
   std::vector<void *> lights;
   std::vector<void *> colliders;

   void Awake(); // runners take your marks
   void Start(); // Go!
   void Update();
   void PhysicsUpdate(float deltaTime);
   void RemoveRenderer(void *renderer);
   void CullRenderer(void *renderer);
   void ActivateRenderer(void *renderer);
   void RemoveGameObject(void *gameObject);
   void RemoveRigidbody(void *rigidbody);
   void RemoveCollider(void *collider);
   void RemoveLight(void *light);
};

#endif
