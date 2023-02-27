#pragma once
#include "Game.h"
#include "GameObjectComponent.h"

class Game;

class ColliderComponent : public GameObjectComponent
{
protected:

	RenderComponent* RCCollider;

public:

	void Initialize();
	void Draw();
};