/*
 * Program 3 base code - includes modifications to shape and initGeom in
 * preparation to load multi shape objects CPE 471 Cal Poly Z. Wood + S. Sueda +
 * I. Dunn
 */

#include <chrono>
#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <irrKlang.h>

#include "Bulb.h"
#include "Camera.h"
#include "Chunk.h"
#include "GLSL.h"
#include "GameCamera.h"
#include "GameObject.h"
#include "KeyboardInput.h"
#include "MatrixStack.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "MouseInput.h"
#include "Player.h"
#include "Program.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "Scene.h"
#include "Terrain.h"
#include "Types.h"
#include "WindowManager.h"

#define TINYOBJLOADER_IMPLEMENTATION

#define SCR_HEIGHT 1020
#define SCR_WIDTH 1280
#define BLOOM_AMOUNT 8
#define EXPOSURE 0.5f
#define INITIAL_VIEWDISTANCE 7

using namespace std;
using namespace glm;
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class Application : public EventCallbacks {

 public:
   WindowManager *windowManager = nullptr;

   Program *faceShader;
   Program *lightedFaceShader;
   Program *blurShader;
   Program *combineShader;
   Program *depthShader;

   unsigned int hdrFBO;
   unsigned int colorBuffers[3];
   unsigned int attachments[3];
   unsigned int rboDepth;
   unsigned int depthMap;
   unsigned int depthMapFBO;
   unsigned int pingpongFBO[2];
   unsigned int pingpongColorbuffers[2];

   ISoundEngine *SoundEngine;

   time_t currentTime;
   Scene scene;

   class GameCamera *gameCamera;
   class Player *player;
   class GameObject *light;
  
   class MouseInput *mouseInput = NULL;
   class KeyboardInput *keyboardInput = NULL;

   void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                    int mods) {
      glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

      // debug inputs
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
         glfwSetWindowShouldClose(window, GL_TRUE);
      }
      if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      // debug camera
      if (key == GLFW_KEY_C && action == GLFW_PRESS) {
         if (gameCamera->GetComponent(Type::KeyboardInput) == NULL) {
            player->keyboardInput = NULL;
            gameCamera->transform.parent = NULL;
            gameCamera->transform.position = player->transform.position;
            gameCamera->keyboardInput =
                (class KeyboardInput *)player->RemoveComponent(
                    Type::KeyboardInput);
            gameCamera->AddComponent(gameCamera->keyboardInput);
            mouseInput =
              (class MouseInput *)gameCamera->AddComponent(
                                                           Type::MouseInput);
         }
      }
      if (key == GLFW_KEY_L && action == GLFW_PRESS) {
         Scene::mainCamera->transform->log();
         exit(0);
      }

      // keyboard component
      if (keyboardInput != NULL) {
         keyboardInput->keyCallBack(key, action);
      }
   }

   void mouseCallback(GLFWwindow *window, int button, int action, int mods) {}
   void scrollCallback(GLFWwindow *window, double deltaX, double deltaY) {}

   void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
      if (mouseInput != NULL) {
         mouseInput->cursorCallback(xpos, ypos);
      }
   }

   void resizeCallback(GLFWwindow *window, int width, int height) {
      // glViewport(0, 0, width, height);
   }

   void init(const std::string &resourceDirectory) {
      GLSL::checkVersion();

      glClearColor(0.f, 0.f, 0.f, 1.f);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);

      glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR,
                       GLFW_CURSOR_DISABLED);

      faceShader = new Program();
      faceShader->setShaderNames(resourceDirectory + "/simple_vert.glsl",
                                 resourceDirectory + "/simple_frag.glsl");
      faceShader->init();
      faceShader->addUniform("P");
      faceShader->addUniform("V");
      faceShader->addUniform("M");
      faceShader->addUniform("lightV");
      faceShader->addAttribute("vertPos");
      faceShader->addAttribute("color");
      faceShader->bind();
      faceShader->setIntUniform("shadows", 0);
      faceShader->unbind();
      Material::defaultMaterial = new class Material();
      Material::defaultMaterial->shader = faceShader;
      Material::defaultMaterial->emission = 0.f;

      lightedFaceShader = new Program();
      lightedFaceShader->setShaderNames(
          resourceDirectory + "/lighted_vert.glsl",
          resourceDirectory + "/lighted_frag.glsl");
      lightedFaceShader->init();
      lightedFaceShader->addUniform("P");
      lightedFaceShader->addUniform("V");
      lightedFaceShader->addUniform("M");
      lightedFaceShader->addUniform("emission");
      lightedFaceShader->addAttribute("vertPos");
      lightedFaceShader->addAttribute("color");
      lightedFaceShader->addAttribute("depth");
      Material::crystalMaterial = new class Material();
      Material::crystalMaterial->shader = lightedFaceShader;
      Material::crystalMaterial->emission = 10.f;

      combineShader = new Program();
      combineShader->setShaderNames(resourceDirectory + "/combine_vertex.glsl",
                                    resourceDirectory +
                                        "/combine_fragment.glsl");
      combineShader->init();
      combineShader->addUniform("scene");
      combineShader->addUniform("bloomBlur");
      combineShader->addUniform("bloom");
      combineShader->addUniform("exposure");
      combineShader->bind();
      combineShader->setIntUniform("scene", 0);
      combineShader->setIntUniform("bloomBlur", 1);
      combineShader->unbind();

      blurShader = new Program();
      blurShader->setShaderNames(resourceDirectory + "/blur_vertex.glsl",
                                 resourceDirectory + "/blur_fragment.glsl");
      blurShader->init();
      blurShader->addUniform("image");
      blurShader->addUniform("horizontal");
      blurShader->addAttribute("aPos");
      blurShader->addAttribute("aTexCoords");
      blurShader->bind();
      blurShader->setIntUniform("image", 0);
      blurShader->unbind();

      depthShader = new Program();
      depthShader->setShaderNames(resourceDirectory + "/simple_depth_vert.glsl",
                                  resourceDirectory + "/simple_depth_frag.glsl");
      depthShader->init();
      depthShader->addUniform("P");
      depthShader->addUniform("V");
      depthShader->addUniform("M");
      depthShader->addAttribute("vertPos");
      depthShader->addAttribute("color");
      depthShader->addAttribute("depth");

      // depth
      glGenFramebuffers(1, &depthMapFBO);  

      glGenTextures(1, &depthMap);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT,
                   0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);


      // hdr color buffer
      glGenFramebuffers(1, &hdrFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
      glGenTextures(3, colorBuffers);
      glBindTexture(GL_TEXTURE_2D, colorBuffers[2]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0,
                   GL_RGB, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                             GL_TEXTURE_2D, colorBuffers[2], 0);
      for (unsigned int i = 0; i < 2; i++) {
         glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0,
                      GL_RGB, GL_FLOAT, NULL);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                GL_TEXTURE_2D, colorBuffers[i], 0);
      }
      attachments[0] = GL_COLOR_ATTACHMENT0;
      attachments[1] = GL_COLOR_ATTACHMENT1;
      attachments[2] = GL_COLOR_ATTACHMENT2;
      glDrawBuffers(3, attachments);
      
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
         std::cout << "Framebuffer not complete!" << std::endl;
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // ping-pong-framebuffer for blurring
      glGenFramebuffers(2, pingpongFBO);
      glGenTextures(2, pingpongColorbuffers);
      for (unsigned int i = 0; i < 2; i++) {
         glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
         glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0,
                      GL_RGB, GL_FLOAT, NULL);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

         if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
             GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
      }
   }

   void render(float deltaTime) {
      int width, height;
      double mousex, mousey;
      glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

      glfwGetCursorPos(windowManager->getHandle(), &mousex, &mousey);

      glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                             depthMap, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);

      float aspect = width / (float)height;
      gameCamera->camera->aspect = aspect;
      auto Projection = make_shared<MatrixStack>();
      auto View = make_shared<MatrixStack>();
      auto Model = make_shared<MatrixStack>();
      gameCamera->camera->SetViewMatrix(Projection, View);
      glm::mat4 lightView = glm::lookAt(light->transform.getGlobalPosition(), 
                                        Scene::mainCamera->transform->getGlobalPosition(), 
                                        glm::vec3( 0.0f, 1.0f,  0.0f));
      faceShader->bind();
      glUniformMatrix4fv(faceShader->getUniform("P"), 1, GL_FALSE,
                         value_ptr(Projection->topMatrix()));
      glUniformMatrix4fv(faceShader->getUniform("V"), 1, GL_FALSE,
                         value_ptr(View->topMatrix()));
      glUniformMatrix4fv(faceShader->getUniform("lightV"), 1, GL_FALSE,
                         value_ptr(lightView));
      faceShader->unbind();
      lightedFaceShader->bind();
      glUniformMatrix4fv(lightedFaceShader->getUniform("P"), 1, GL_FALSE,
                         value_ptr(Projection->topMatrix()));
      glUniformMatrix4fv(lightedFaceShader->getUniform("V"), 1, GL_FALSE,
                         value_ptr(View->topMatrix()));
      lightedFaceShader->unbind();

      glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
      // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      Scene::mainCamera->ExtractVFPlanes(Projection->topMatrix(),
                                         View->topMatrix());
      
      // lightView = View->topMatrix();
      depthShader->bind();
      glUniformMatrix4fv(depthShader->getUniform("P"), 1, GL_FALSE,
                         value_ptr(Projection->topMatrix()));
      glUniformMatrix4fv(depthShader->getUniform("V"), 1, GL_FALSE,
                         value_ptr(lightView));

      for (int i = 0; i < scene.renderers.size(); i++) {
         class MeshRenderer *renderer =
             (class MeshRenderer *)scene.renderers[i];
         Transform *rendererTransform = renderer->transform;
         if (renderer->material->shader != lightedFaceShader) {
           continue;
         }
         rendererTransform->SetGlobalMatrix(Model);
         glUniformMatrix4fv(depthShader->getUniform("M"), 1, GL_FALSE,
                            value_ptr(Model->topMatrix()));
         renderer->mesh->draw(depthShader);
         Model->popMatrix();
         Model->popMatrix();
      }
      depthShader->unbind();

      // return;
      glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, colorBuffers[2]);

      glUniformMatrix4fv(lightedFaceShader->getUniform("V"), 1, GL_FALSE,
                         value_ptr(View->topMatrix()));

      for (int i = 0; i < scene.renderers.size(); i++) {
         class MeshRenderer *renderer =
             (class MeshRenderer *)scene.renderers[i];
         Transform *rendererTransform = renderer->transform;
         rendererTransform->SetGlobalMatrix(Model);
         Program *shader = renderer->material->shader;
         shader->bind();
         glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE,
                            value_ptr(Model->topMatrix()));
         if (shader == lightedFaceShader) {
            shader->setFloatUniform("emission", renderer->material->emission);
         }
         renderer->mesh->draw(shader);
         Model->popMatrix();
         Model->popMatrix();
         shader->unbind();
      }
      glDisable(GL_DEPTH_TEST);

      // // clean up
      Projection->popMatrix();
      View->popMatrix();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // blur bloom
      blurShader->bind();
      bool horizontal = true, first_iteration = true;
      for (unsigned int i = 0; i < BLOOM_AMOUNT; i++) {
         glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
         blurShader->setIntUniform("horizontal", horizontal);
         glBindTexture(GL_TEXTURE_2D, first_iteration
                                          ? colorBuffers[1]
                                          : pingpongColorbuffers[!horizontal]);
         renderQuad();
         horizontal = !horizontal;
         if (first_iteration)
            first_iteration = false;
      }
      blurShader->unbind();

      // combine bloom and scene
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClear(GL_COLOR_BUFFER_BIT);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
      glActiveTexture(GL_TEXTURE0);
      combineShader->bind();
      combineShader->setIntUniform("bloom", 1);
      combineShader->setFloatUniform("exposure", EXPOSURE);
      renderQuad();
      combineShader->unbind();
   }

   unsigned int quadVAO = 0;
   unsigned int quadVBO;
   void renderQuad() {
      if (quadVAO == 0) {
         float quadVertices[] = {
             // positions        // texture Coords
             -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
         };
         // setup plane VAO
         glGenVertexArrays(1, &quadVAO);
         glGenBuffers(1, &quadVBO);
         glBindVertexArray(quadVAO);
         glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
         glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                      GL_STATIC_DRAW);
         glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                               (void *)0);
         glEnableVertexAttribArray(1);
         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                               (void *)(3 * sizeof(float)));
      }
      glBindVertexArray(quadVAO);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
   }
};

int main(int argc, char *argv[]) {
   std::string resourceDir = "../resources";

   if (argc >= 2) {
      resourceDir = argv[1];
   }

   Application *application = new Application();
   WindowManager *windowManager = new WindowManager();

   // scene
   application->scene = Scene();
   Scene::currentScene = &application->scene;
   Scene::gameState = 'p';
   Scene::makeSoundEffect = false;

   // camera
   application->gameCamera = (class GameCamera *)malloc(sizeof(GameCamera));
   application->gameCamera = new GameCamera();
   application->gameCamera->camera =
       (class Camera *)application->gameCamera->AddComponent(Type::Camera);
   Scene::mainCamera = application->gameCamera->camera;
   application->gameCamera->ForwardNearTransform = new Transform(
       glm::vec3(-0.340025, 8.573741, -29.776596), glm::vec3(1.f, 1.f, 1.f),
       glm::quat(0.021025, 0.003097, 0.997585, 0.066484));
   application->gameCamera->BackwardFarTransform = new Transform(
       glm::vec3(-20.316292, 16.553699, 140.759277), glm::vec3(1.f, 1.f, 1.f),
       glm::quat(-0.999828, -0.011186, -0.016011, 0.000445));
   application->gameCamera->ForwardTransform = new Transform(*application->gameCamera->ForwardNearTransform);
   application->gameCamera->BackwardNearTransform = new Transform(
       glm::vec3(-0.340025, 8.573741, 29.776596), glm::vec3(1.f, 1.f, 1.f),
       glm::quat(-0.998704, -0.052789, -0.004661, 0.001826));
   application->gameCamera->BackwardTransform = new Transform(*application->gameCamera->BackwardNearTransform);
   application->gameCamera->ForwardFarTransform = new Transform(
       glm::vec3(10.512974, 12.103701, -123.862465), glm::vec3(1.f, 1.f, 1.f),
       glm::quat(0.032620, -0.000033, 0.999496, 0.003819));
   application->gameCamera->GroundUpTransform = new Transform(
       glm::vec3(-1.332427f, -0.791858f, -3.887569f), glm::vec3(1.f, 1.f, 1.f),
       glm::quat(0.043826f, -0.003553f, 0.990929f, -0.127502f));
   application->gameCamera->InitialPan();
   application->light = new GameObject();
   application->light->transform.position = glm::vec3(100, 2000, 100);
   application->light->transform.parent = &(application->gameCamera->transform);

   // input
   // application->mouseInput =
       // (class MouseInput *)application->gameCamera->AddComponent(
           // Type::MouseInput);

   // window
   windowManager->init(SCR_WIDTH, SCR_HEIGHT);
   windowManager->setEventCallbacks(application);
   application->windowManager = windowManager;

   application->init(resourceDir);

   // Crystal *crystal = NULL;
   // for (;;) {
   //    printf("%i, %i, %i, %i, %i, %i\n",
   //           Scene::currentScene->culledRenderers.size(),
   //           Scene::currentScene->lights.size(),
   //           Scene::currentScene->renderers.size(),
   //           Scene::currentScene->renderers.size(),
   //           Scene::currentScene->sceneObjects.size(),
   //           Scene::currentScene->colliders.size());
   //    if (crystal == NULL) {
   //       printf("Creating Big Crystals\n");
   //       crystal = new Crystal();
   //       crystal->Awake();
   //       application->faceShader->bind();
   //       crystal->meshRenderer->mesh->draw(application->faceShader);
   //       application->faceShader->unbind();
   //    } else {
   //       crystal->Destroy();
   //       delete crystal;
   //       crystal = NULL;
   //    }
   // }

   printf("Creating Big Crystals\n");
   CrystalStructure *crystal1 = new CrystalStructure();
   crystal1->miniCrystal = false;
   crystal1->transform.position = glm::vec3(-50, 100, 2000);
   crystal1->transform.setEulerAngles(glm::vec3(1.f, 200.f, 10.f));
   crystal1->pillars = 1;
   crystal1->color = glm::vec3(0.3f, 0.01f, 0.01f);

   crystal1->pillars = 1;
   CrystalStructure *crystal2 = new CrystalStructure();
   crystal2->miniCrystal = false;
   crystal2->transform.position = glm::vec3(100, 90, -1500);
   crystal2->transform.setEulerAngles(glm::vec3(-6.f, 20.f, -3.f));
   crystal2->pillars = 1;
   crystal2->color = glm::vec3(0.01f, 0.3f, 0.01f);

   CrystalStructure *crystal3 = new CrystalStructure();
   crystal3->miniCrystal = false;
   crystal3->transform.position = glm::vec3(0, 50, 5000);
   crystal3->transform.setEulerAngles(glm::vec3(5.f, 20.f, -8.f));
   crystal3->pillars = 1;
   crystal3->color = glm::vec3(0.5f, 0.5f, 0.01f);

   printf("Creating Terrain\n");
   Terrain *terrain = new Terrain();
   terrain->viewDistance = INITIAL_VIEWDISTANCE;
   terrain->transform.position.y = 0;

   printf("Creating Player\n");
   application->player = new Player();
   application->player->gameCamera = application->gameCamera;
   application->player->transform.position = glm::vec3(0.f, 150.f, -6000.f);
   Scene::mainPlayer = &(application->player->transform);

   application->gameCamera->transform.parent =
       &(application->player->transform);
   application->keyboardInput =
       (class KeyboardInput *)application->player->AddComponent(
           Type::KeyboardInput);
   application->player->keyboardInput = application->keyboardInput;

   printf("Creating Mini Crystal\n");
   for (int i = 0; i < 100; i++) {
      CrystalStructure *miniCrystal = new CrystalStructure();
      miniCrystal->miniCrystal = true;
   }

   printf("Awaking GameObjects\n");
   application->scene.Awake();
   printf("Awoke GameObjects\n");

   application->gameCamera->camera->fov = 90.0f;
   application->gameCamera->camera->nearClippingPlane = 0.01f;
   application->gameCamera->camera->farClippingPlane = 10000.0f;
   application->gameCamera->keyboardInput = NULL;

   if (application->keyboardInput != NULL) {
      application->keyboardInput->forwardKey = GLFW_KEY_W;
      application->keyboardInput->backwardKey = GLFW_KEY_S;
      application->keyboardInput->rightKey = GLFW_KEY_D;
      application->keyboardInput->leftKey = GLFW_KEY_A;
      application->keyboardInput->upKey = GLFW_KEY_U;
      application->keyboardInput->downKey = GLFW_KEY_D;
   }

   printf("Starting GameObjects\n");
   application->scene.Start();

   application->SoundEngine = createIrrKlangDevice();
   ISound* music = application->SoundEngine->play2D("../resources/music.mp3",
		true, false, true, ESM_AUTO_DETECT, true);
   
   srand(time(NULL));
   auto lastTime = std::chrono::high_resolution_clock::now();

   int printSpacer = 0;

   while (!glfwWindowShouldClose(windowManager->getHandle())) {

      // printf("%i, %i, %i, %i, %i, %i\n",
      //        Scene::currentScene->culledRenderers.size(),
      //        Scene::currentScene->lights.size(),
      //        Scene::currentScene->renderers.size(),
      //        Scene::currentScene->culledRenderers.size(),
      //        Scene::currentScene->sceneObjects.size(),
      //        Scene::currentScene->colliders.size());

      auto nextLastTime = std::chrono::high_resolution_clock::now();
      Scene::deltaTime =
          std::chrono::duration_cast<std::chrono::microseconds>(
              std::chrono::high_resolution_clock::now() - lastTime)
              .count();
      Scene::deltaTime = Scene::deltaTime * 0.000001f;

      printSpacer++;
      if (printSpacer % 20 == 0) {
         if (Scene::gameState == 'l') {
            printf("Generating Terrain\n");
         } else {
            printf("%f FPS\n", 1.f / Scene::deltaTime);
         }
      }

      application->scene.Update();

      if (Scene::gameState != 'l') {
         application->render(Scene::deltaTime);
         application->scene.PhysicsUpdate(Scene::deltaTime);
      }

      if (Scene::makeSoundEffect) {
        ISound *weome = application->SoundEngine->play2D("../resources/weome.wav",
                                         false, false, true, ESM_AUTO_DETECT, true);
        if (Scene::makeSoundEffect == 2) {
          weome->setVolume(0.05f);
        } else {
          weome->setVolume(0.15f);
        }
        Scene::makeSoundEffect = false;
      }

      glfwSwapBuffers(windowManager->getHandle());
      glfwPollEvents();

      lastTime = nextLastTime;
   }

   windowManager->shutdown();
   return 0;
}
