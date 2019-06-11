#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include "Bulb.h"
#include "Camera.h"
#include "Collider.h"
#include "Component.h"
#include "GameCamera.h"
#include "GameObject.h"
#include "KeyboardInput.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"
#include "Terrain.h"

#define RUN_SPEED 100.f

class Player : public Bulb {
 public:
   class KeyboardInput *keyboardInput;
   class GameCamera *gameCamera;

   char far = false;
  char startedFlipForward;

  char flipQueued = false;
   float delay = 0.f;
  
   float closestCrystalDistance = 9999999.f;
   
   void Flip();

   void Awake();
   void Update();
  void OnCollision(class Collider *other);
};

#endif
