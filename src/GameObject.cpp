#include "GameObject.h"

#include "ComponentFactory.h"

GameObject::GameObject(Scene *scene) : transform() {
   components = NULL;
   scene->sceneObjects.push_back(this);
}
GameObject::GameObject(Scene *scene, Transform transform)
    : transform(transform) {
   components = NULL;
   scene->sceneObjects.push_back(this);
}
GameObject::GameObject() : GameObject(Scene::currentScene) {}
GameObject::GameObject(Transform transform)
    : GameObject(Scene::currentScene, transform) {}

Component *GameObject::GetComponent(Type type) {
   Component *component = components;
   while (component != NULL) {
      if (component->getType() == type) {
         return component;
      }
      component = component->next;
   }
   return NULL;
}

Component *GameObject::AddComponent(Type type) {
   if (components == NULL) {
      components = ComponentFactory::Make(type, this);
      components->next = NULL;
      return components;
   }
   Component *component = components;
   while (component->next != NULL) {
      component = component->next;
   }
   component->next = ComponentFactory::Make(type, this);
   component->next->next = NULL;
   return component->next;
}

Component *GameObject::AddComponent(class Component *newComponent) {
   if (components == NULL) {
      components = newComponent;
      components->next = NULL;
      return components;
   }
   Component *component = components;
   while (component->next != NULL) {
      component = component->next;
   }
   component->next = newComponent;
   component->next->next = NULL;
   return component->next;
}

Component *GameObject::RemoveComponent(Type type) {
   if (components == NULL) {
      return NULL;
   }
   Component *component = components;
   if (components->getType() == type) {
      components = component->next;
      return component;
   }
   while (component->next != NULL) {
      if (component->next->getType() == type) {
         Component *removedComponent = component->next;
         component->next = removedComponent->next;
         return removedComponent;
      }
      component = component->next;
   }
   return NULL;
}

void GameObject::Destroy() {
   Scene::currentScene->RemoveGameObject(this);
   if (components == NULL) {
      return;
   }
   Component *component = components;
   Component *nextComponent = component->next;
   while (nextComponent != NULL) {
      Component *newComponent = nextComponent;
      nextComponent = newComponent->next;
      DestroyComponent(component);
      component = newComponent;
   }
   DestroyComponent(component);
   components = NULL;
}

void GameObject::DestroyComponent(Type type) {
   DestroyComponent(GetComponent(type));
}

void GameObject::DestroyComponent(class Component *component) {
   if (component->getType() == Type::MeshRenderer) {
      ((class MeshRenderer *)component)->mesh = NULL;
      Scene::currentScene->RemoveRenderer((void *)component);
   } else if (component->getType() == Type::Rigidbody) {
      Scene::currentScene->RemoveRigidbody((void *)component);
   } else if (component->getType() == Type::Light) {
      Scene::currentScene->RemoveLight((void *)component);
   } else if (component->getType() == Type::Collider) {
      Scene::currentScene->RemoveCollider((void *)component);
   } else if (component->getType() == Type::Mesh) {
     ((class Mesh*)component)->Free();
   }
   delete component;
}

void GameObject::Awake() {}
void GameObject::Start() {}
void GameObject::Update() {}
void GameObject::OnCollision(class Collider *other) {}
