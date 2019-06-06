#ifndef COMPONENTFACTORY_INCLUDED
#define COMPONENTFACTORY_INCLUDED

#include "Camera.h"
#include "Collider.h"
#include "Component.h"
#include "KeyboardInput.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Renderer.h"
#include "Material.h"
#include "MouseInput.h"
#include "Rigidbody.h"
#include "Scene.h"
#include "Transform.h"
#include "Types.h"
#include "Light.h"

// used to make components
class ComponentFactory {
 public:
   static Component *Make(Type type, GameObject *gameObject) {
      Component *component = NULL;
      switch (type) {
      case Type::MeshRenderer: {
         class MeshRenderer *meshRenderer = new class MeshRenderer();
         meshRenderer->gameObject = gameObject;
         meshRenderer->transform = &(gameObject->transform);
         meshRenderer->material = Material::defaultMaterial;
         Scene::currentScene->renderers.push_back(meshRenderer);
         component = meshRenderer;
         return component;
      }
      case Type::Mesh: {
         class Mesh *mesh = new class Mesh();
         mesh->gameObject = gameObject;
         mesh->transform = &(gameObject->transform);
         component = mesh;
         return component;
         break;
      }
      case Type::Collider: {
         class Collider *collider = new class Collider();
         collider->gameObject = gameObject;
         collider->transform = &(gameObject->transform);
         Scene::currentScene->colliders.push_back(collider);
         component = collider;
         return component;
         break;
      }
      case Type::Rigidbody: {
         class Rigidbody *rigidbody = new class Rigidbody();
         rigidbody->gameObject = gameObject;
         rigidbody->transform = &(gameObject->transform);
         Scene::currentScene->physicsObjects.push_back(rigidbody);
         component = rigidbody;
         return component;
         break;
      }
      case Type::Camera: {
         class Camera *camera = new class Camera();
         camera->gameObject = gameObject;
         camera->transform = &(gameObject->transform);
         component = camera;
         return component;
         break;
      }
      case Type::KeyboardInput: {
         class KeyboardInput *keyboardInput = new class KeyboardInput();
         keyboardInput->gameObject = gameObject;
         keyboardInput->transform = &(gameObject->transform);
         component = keyboardInput;
         return component;
         break;
      }
      case Type::MouseInput: {
         class MouseInput *mouseInput = new class MouseInput();
         mouseInput->gameObject = gameObject;
         mouseInput->transform = &(gameObject->transform);
         component = mouseInput;
         return component;
         break;
      }
      case Type::Light: {
         class Light *light = new class Light();
         light->intensity = 10.f;
         light->color = glm::vec3(0.f, 0.f, 1.f);
         light->gameObject = gameObject;
         light->transform = &(gameObject->transform);
         Scene::currentScene->lights.push_back(light);
         component = light;
         return component;
         break;
      }
      }
   }
};

#endif
