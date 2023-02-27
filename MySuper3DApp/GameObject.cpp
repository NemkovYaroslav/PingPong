#include "GameObject.h"

GameObject::GameObject()
{
	position = std::make_shared<DirectX::SimpleMath::Vector4>();
}

void GameObject::Initialize()
{
	for (auto component : components)
	{
		component->Initialize();
	}
}

void GameObject::Draw()
{
	for (auto component : components)
	{
		component->Draw();
	}
}
