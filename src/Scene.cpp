#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "Player.h"

Scene *Scene::currentScene;
class Camera *Scene::mainCamera;
class Transform *Scene::mainPlayer;
float Scene::deltaTime;
char Scene::gameState;

void Scene::Awake() {
   // awake game object functions
   for (int i = 0; i < sceneObjects.size(); i++) {
      ((class GameObject *)sceneObjects[i])->Awake();
   }
}
void Scene::Start() {
   // start game object functions
   for (int i = 0; i < sceneObjects.size(); i++) {
      ((class GameObject *)sceneObjects[i])->Start();
   }
}
void Scene::Update() {
   // start game object functions
   for (int i = sceneObjects.size()-1; i >= 0; i--) {
      ((class GameObject *)sceneObjects[i])->Update();
   }
}
void Scene::PhysicsUpdate(float deltaTime) {
   // start game object functions
   for (int i = 0; i < physicsObjects.size(); i++) {
      ((class Rigidbody *)physicsObjects[i])->PhysicsStep(deltaTime);
   }
}

void Scene::RemoveRenderer(void *renderer) {
   for (int i = 0; i < culledRenderers.size(); i++) {
      if (culledRenderers[i] == renderer) {
         culledRenderers.erase(culledRenderers.begin() + i);
         return;
      }
   }
   for (int i = 0; i < renderers.size(); i++) {
      if (renderers[i] == renderer) {
         renderers.erase(renderers.begin() + i);
         return;
      }
   }
}

void Scene::RemoveGameObject(void *gameObject) {
   for (int i = sceneObjects.size()-1; i >= 0; i--) {
      if (sceneObjects[i] == gameObject) {
         sceneObjects.erase(sceneObjects.begin() + i);
         return;
      }
   }
}

void Scene::RemoveRigidbody(void *rigidbody) {
   for (int i = physicsObjects.size()-1; i >= 0; i--) {
      if (physicsObjects[i] == rigidbody) {
         physicsObjects.erase(physicsObjects.begin() + i);
         return;
      }
   }
}

void Scene::RemoveLight(void *light) {
   for (int i = lights.size()-1; i >= 0; i--) {
      if (lights[i] == light) {
         lights.erase(lights.begin() + i);
         return;
      }
   }
}

void Scene::RemoveCollider(void *collider) {
   for (int i = colliders.size()-1; i >= 0; i--) {
      if (colliders[i] == collider) {
         colliders.erase(colliders.begin() + i);
         return;
      }
   }
}

void Scene::CullRenderer(void *renderer) {
   for (int i = 0; i < renderers.size(); i++) {
      if (renderers[i] == renderer) {
         culledRenderers.push_back(renderer);
         renderers.erase(renderers.begin() + i);
         return;
      }
   }
}

void Scene::ActivateRenderer(void *renderer) {
   for (int i = 0; i < culledRenderers.size(); i++) {
      if (culledRenderers[i] == renderer) {
         renderers.push_back(renderer);
         culledRenderers.erase(culledRenderers.begin() + i);
         return;
      }
   }
}
