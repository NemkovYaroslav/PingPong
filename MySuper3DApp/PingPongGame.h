#pragma once
#include "Game.h"
#include "GameObject.h"
#include "RenderComponent.h"

class PingPongGame : public Game
{
private:

	PingPongGame(LPCWSTR name, int screenWidth, int screenHeight);

	void Update() override;

public:

	std::shared_ptr<GameObject> leftPlayer;
	std::shared_ptr<GameObject> rightPlayer;
	std::shared_ptr<GameObject> ball;

	std::shared_ptr<GameObject> collisionBall;
	std::shared_ptr<GameObject> collisionRightRacket;
	std::shared_ptr<GameObject> collisionLeftRacket;
	std::shared_ptr<GameObject> collisionTopWall;
	std::shared_ptr<GameObject> collisionBottomWall;

	DirectX::BoundingBox* BBTopWall    = {};
	DirectX::BoundingBox* BBBottomWall = {};
	DirectX::BoundingBox* BBLeftWall   = {};
	DirectX::BoundingBox* BBRightWall  = {};

	DirectX::BoundingBox* BBLeftRacket = {};
	DirectX::BoundingBox* BBRightRacket = {};
	DirectX::BoundingSphere* BBBall = {};

	float ballSpeed = 0.3f;
	float racketSpeed = 1.2f;
	DirectX::SimpleMath::Vector4 direction = { 1, 0, 0, 0 };

	bool RRCanMoveUp    = true;
	bool RRCanMoveDown  = true;
	bool LRCanMoveUp    = true;
	bool LRCanMoveDown  = true;

	bool isGameEnded = false;

	int pointPlayer = 0;
	int pointEnemy  = 0;

	static void CreateInstance(LPCWSTR name, int screenWidth, int screenHeight);

	void Run() override;
	void ConfigureGameObjects();
};
