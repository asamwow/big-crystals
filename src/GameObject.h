#ifndef GAMEOBJECT_INCLUDED
#define GAMEOBJECT_INCLUDED

#include <cstdio>
#include <stdlib.h>
//#include <type_traits>

#include "Component.h"
#include "Transform.h"
#include "Types.h"
#include "Scene.h"

class Component;

class GameObject {
 public:
   GameObject(Transform transform);
   GameObject();
   GameObject(Scene *scene, Transform transform);
   GameObject(Scene *scene);

   Transform transform;
   Component *components;

   Component *GetComponent(Type type);
   Component *AddComponent(Type type);
   Component *AddComponent(class Component *newComponent);
   Component *RemoveComponent(Type type);
   void DestroyComponent(Type tyep);
   void DestroyComponent(class Component *component);
   void Destroy();

   // since game objects use it the most
   static float deltaTime;

   virtual void Awake(); // runners take your marks
   virtual void Start(); // Go!
   virtual void Update();
   virtual void OnCollision(class Collider *other);
};

#endif
