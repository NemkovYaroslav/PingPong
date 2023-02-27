#pragma once
#include <vector>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "SimpleMath.h"
#include "SimpleMath.inl"

#include "GameObjectComponent.h"

class GameObject
{
public:

	std::shared_ptr<DirectX::SimpleMath::Vector4> position;
	std::vector<GameObjectComponent*> components;

	GameObject();
	~GameObject() = default;

	void Initialize();
	void Draw();
};
