#ifndef GAMECAMERA_INCLUDED
#define GAMECAMERA_INCLUDED

#include "GameObject.h"
#include "Component.h"
#include "Camera.h"
#include "KeyboardInput.h"

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
   Transform *BackwardTransform;
   Transform *GroundUpTransform;

   void StartPan(Transform *start, Transform *end, float time, float startTime);
   void FlipPan(char forward);
   void InitialPan();
   
   void Awake();
   void Update();
};

#endif
