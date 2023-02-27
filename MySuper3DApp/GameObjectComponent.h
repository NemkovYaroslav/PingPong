#pragma once

class GameObjectComponent
{
public:

	virtual ~GameObjectComponent() = default;

	virtual void Initialize() = 0;
	virtual void Draw() = 0;
};