#include <glew.h>
#include <iostream>
#include <SDL3/SDL.h>
#include "Core/Debug.h"
#include "Scene/Scene0.h"
#include <MMath.h>
#include <QMath.h>
#include "Physics/TransformComponent.h"
#include "GPU/MaterialComponent.h"
#include "Assets/XMLAssetManager.h"
#include "GPU/ShaderComponent.h"
#include "GPU/MeshComponent.h"
#include "Physics/ShapeComponent.h"

bool Scene0::OnCreate()
{
	assetManager = std::make_shared<XMLAssetManager>();

	// Make sure these names match the stuff in your xml file:
	std::vector<std::string> names{ 
		"ActorGameBoard", 
		"ActorChecker1", 
		"ActorChecker2", 
		"ActorSkull", 
		"ActorCube", 
		"ActorMario"
	};

	// Did everything load ok from the xml file?
	bool everythingLoaded = true;
	for (const std::string& name : names) {
		auto iterator = assetManager->xmlAssets.find(name);
		// Is a name missing or has a typo?
		if (iterator == assetManager->xmlAssets.end()) {
			// Strings are handy to use the "+" symbol to join them up
			Debug::Error("Actor not found in Scene0.xml: " + name, __FILE__, __LINE__);
			everythingLoaded = false;
			continue; // skip to the next iteration
		}
		actors[name] = std::dynamic_pointer_cast<Actor>(iterator->second);
	}

	// Check our actors have the components they need
	for (const auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		if (actor == nullptr ||
			actor->GetComponent<ShaderComponent>()   == nullptr ||
			actor->GetComponent<MaterialComponent>() == nullptr ||
			actor->GetComponent<MeshComponent>()     == nullptr ||
			actor->GetComponent<ShapeComponent>()    == nullptr) 
		{
			Debug::Error("Actor is missing a shader, material, mesh or shape: " + pair.first, __FILE__, __LINE__);
			everythingLoaded = false;
		}
	}

	// Finally load the camera and light actors
	auto cameraIterator = assetManager->xmlAssets.find("Camera1");
	camera = std::dynamic_pointer_cast<CameraActor>(cameraIterator->second);

	auto lightIterator = assetManager->xmlAssets.find("Light1");
	light = std::dynamic_pointer_cast<LightActor>(lightIterator->second);
	
	// joystick setup
	SDL_InitSubSystem(SDL_INIT_GAMEPAD);
	int count;
	SDL_JoystickID* gamepads = SDL_GetGamepads(&count);
	if (gamepads) {
		if (count > 0) {
			gamepad = SDL_OpenGamepad(gamepads[0]);
			Debug::Info("Gamepad found on startup.", __FILE__, __LINE__);
		}
		SDL_free(gamepads);
	}
	
	return everythingLoaded;
}

void Scene0::OnDestroy()
{
	actors.clear();
	camera = nullptr;
	light = nullptr;
	assetManager = nullptr;
	
	if (gamepad) {
		SDL_CloseGamepad(gamepad); 
		gamepad = nullptr;
	}
	SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}


void Scene0::HandleEvents(const SDL_Event& sdlEvent)
{
	camera->SetQuat(sdlEvent);
	Ref<TransformComponent> cameraTransform = camera->GetComponent <TransformComponent>();
	const float deltaAngle = 2.0f;

	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:

		// Render meshes and or collision shapes
		 if (sdlEvent.key.scancode == SDL_SCANCODE_M) {
			renderMeshes = !renderMeshes;
		}
		else if (sdlEvent.key.scancode == SDL_SCANCODE_C) {
			renderCollisionShapes = !renderCollisionShapes;
		}
		// TODO for YOU
		// Would be nice to have keys to move the camera based on where you are looking
	
		break;
		// this is so I can plug in a controller while the program is runnning
	
	// All this mouse clicking stuff below is for Assignment 2: 
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
			Vec3 mouseCoords(static_cast<float>(sdlEvent.button.x), static_cast<float>(sdlEvent.button.y), 0.0f);
			// TODO for Assignment 2: 
			// Get a ray pointing into the world, We have the x, y pixel coordinates
			// Need to convert this into world space to build our ray
			int w, h;
			SDL_GetWindowSize(SDL_GetWindowFromID(sdlEvent.button.windowID), &w, &h);
			// NDC
			float x = (2.0f * mouseCoords.x) / static_cast<float>(w) - 1.0f;
			float y = 1.0f - (2.0f * mouseCoords.y) / static_cast<float>(h); 

			// take the inv proj to get to view space
			Matrix4 invProj = MMath::inverse(camera->GetProjectionMatrix());
			Vec4 rayView = invProj * Vec4(x, y, -1.0f, 1.0f); 
			rayView /= rayView.w; // Normalize the perspective W
			// the rest is self-explanatory by the names of the vars
			Matrix4 camWorldMatrix = MMath::inverse(camera->GetViewMatrix());

			Vec4 worldPoint = camWorldMatrix * rayView;
			Vec3 worldRayStart = camera->GetComponent<TransformComponent>()->pos;
			Vec3 worldRayDir = VMath::normalize(Vec3(worldPoint.x, worldPoint.y, worldPoint.z) - worldRayStart);
			
		
			for (int i = 0; i < 100; ++i) {
				std::string sphereName = "raySphere" + std::to_string(i);
				auto it = actors.find(sphereName);
				if (it != actors.end()) {
					actors.erase(it); 
				}
			}

			
			float stepSize = 1.0f; // Distance between each sphere 
			// make one sphere and one shader and one shape component to use for all the spheres
			Ref<GEOMETRY::Sphere> sphere = std::make_shared<GEOMETRY::Sphere>(0.1f, Vec3(0.0f, 0.0f, 0.0f));
			Ref<ShapeComponent> shapeComponent = std::make_shared<ShapeComponent>(nullptr, *sphere);
			Ref<ShaderComponent> shaderComponent = actors.at("ActorMario")->GetComponent<ShaderComponent>();
			for (int i = 0; i < 30; ++i) {
				float distance = i * stepSize;
				Vec3 spherePos = worldRayStart + worldRayDir * distance;

				Ref<Actor> actor = std::make_shared<Actor>(nullptr);
 
				Ref<TransformComponent> transformComponent = 
				   std::make_shared<TransformComponent>(nullptr, spherePos, 
					  Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f)));
				 actor->AddComponent<TransformComponent>(transformComponent);
				 actor->AddComponent<ShapeComponent>(shapeComponent);
				 actor->AddComponent<ShaderComponent>(shaderComponent);
				
				actors.emplace("raySphere" + std::to_string(i), actor);
			}
			
			for (auto it = actors.begin(); it != actors.end(); ++it) {
				Ref<Actor> actor = it->second;
				Ref<TransformComponent> transformComponent = actor->GetComponent <TransformComponent>();
				Ref<ShapeComponent> shapeComponent = actor->GetComponent <ShapeComponent>();
				// TODO for Assignment 2: 
				// Transform the ray into the local space of the object and check if a collision occured
			}
		}
		break;
	case SDL_EVENT_GAMEPAD_ADDED:
		if (!gamepad) {
			gamepad = SDL_OpenGamepad(sdlEvent.gdevice.which);
			Debug::Info("Gamepad connected.", __FILE__, __LINE__);
			int count;
			SDL_JoystickID* gamepads = SDL_GetGamepads(&count);
			if (gamepads) {
				if (count > 0) {
					gamepad = SDL_OpenGamepad(gamepads[0]);
					Debug::Info("Gamepad found on Event.", __FILE__, __LINE__);
				}
				SDL_free(gamepads); 
			}
		}
		break;
		// this detects when one is removed during run time
	case SDL_EVENT_GAMEPAD_REMOVED:
		if (gamepad && sdlEvent.gdevice.which == SDL_GetGamepadID(gamepad)) {
			SDL_CloseGamepad(gamepad);
			gamepad = nullptr;
		}
		break;
		// basic button input for controller.
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		switch (sdlEvent.gbutton.button) {
	case SDL_GAMEPAD_BUTTON_DPAD_UP:
			//drawInWireMode = !drawInWireMode;
			break;
	default:
			break;
		}
		break;
	default:
		break;
	}
	
}

void Scene0::Update(const float deltaTime)
{
	// Camera Controls
camera->CameraMovement(deltaTime, gamepad);
}

void Scene0::Render() const
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatricesID());
	glBindBuffer(GL_UNIFORM_BUFFER, light->GetLightID());
	glBindTexture(GL_TEXTURE_2D, 0);
	for (const auto [name, actor] : actors) {
		glUseProgram(actor->GetComponent<ShaderComponent>()->GetProgram());
		glUniformMatrix4fv(actor->GetComponent<ShaderComponent>()->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
		if (actor->GetComponent<MaterialComponent>()) {
			glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
		} else {
			glBindTexture(GL_TEXTURE_2D, 0); 
		}
		if (renderMeshes && actor->GetComponent<MeshComponent>()) {
			actor->GetComponent<MeshComponent>()->Render();
		}
		if (renderCollisionShapes && actor->GetComponent<ShapeComponent>()) {
			glUniform4f(actor->GetComponent<ShaderComponent>()->GetUniformID("Debugcolour"), 0.0f, 1.0f, 0.0f, 1.0f);
			actor->GetComponent<ShapeComponent>()->Render();
			glUniform4f(actor->GetComponent<ShaderComponent>()->GetUniformID("Debugcolour"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
	}
}