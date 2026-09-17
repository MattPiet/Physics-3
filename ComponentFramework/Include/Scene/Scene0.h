#ifndef SCENE0_H
#define SCENE0_H

#include "Scene.h"
#include <unordered_map>
#include "GPU/LightActor.h"
#include "ECS/CameraActor.h"
#include "Assets/XMLAssetManager.h"


struct Scene0 : public Scene
{
	bool renderMeshes = true;
	bool renderCollisionShapes = true;
	explicit Scene0() {};
	virtual ~Scene0() {};

	Ref<XMLAssetManager> assetManager;
	// Keep track of Actors from XML asset manager
	std::unordered_map< std::string, Ref<Actor>> actors;
	// We only have one camera and light, so they don't need to be in the map
	Ref<CameraActor> camera;
	Ref<LightActor> light;
	SDL_Gamepad* gamepad = nullptr;

	bool OnCreate() override;
	void OnDestroy() override;
	void Update(const float deltaTime) override;
	void Render() const override;
	void HandleEvents(const SDL_Event& sdlEvent) override;

	// TODO for Assignment 2:
	// We'll use a ray to click on our geometry objects 
	// GEOMETRY::RayIntersectionInfo rayInfo;
};
#endif
