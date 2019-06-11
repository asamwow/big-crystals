#ifndef GAMECAMERA_INCLUDED
#define GAMECAMERA_INCLUDED

#include "GameObject.h"
#include "Component.h"
#include "Camera.h"
#include "KeyboardInput.h"
#include "Light.h"

class GameCamera : public GameObject {
 public:

   class Camera *camera;
   class KeyboardInput *keyboardInput;
   int mode;

   Transform *PanStart = NULL;
   Transform *PanEnd = NULL;
   float PanIndex;
   float PanTime;

   Transform *ForwardTransform;
   Transform *ForwardFarTransform;
   Transform *ForwardNearTransform;
   Transform *BackwardTransform;
   Transform *BackwardFarTransform;
   Transform *BackwardNearTransform;
   Transform *GroundUpTransform;

   Transform *ModifyTransform;

   void StartPan(Transform *start, Transform *end, float time, float startTime, Transform *toMod);
   void FlipPan(char forward);
   void FarPan(char far, char forward);
   void InitialPan();
   void ResetDistance();
   void ResetDistance(char forward);
   
   void Awake();
   void Update();
};

#endif
