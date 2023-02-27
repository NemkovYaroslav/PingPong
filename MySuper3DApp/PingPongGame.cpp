#include "PingPongGame.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <random>

PingPongGame::PingPongGame(LPCWSTR name, int screenWidth, int screenHeight) : Game(name, screenWidth, screenHeight)
{
	leftPlayer = std::make_shared<GameObject>();
	rightPlayer = std::make_shared<GameObject>();
	ball = std::make_shared<GameObject>();
	
	collisionBall = std::make_shared<GameObject>();
	collisionRightRacket = std::make_shared<GameObject>();
	collisionLeftRacket = std::make_shared<GameObject>();
	collisionTopWall = std::make_shared<GameObject>();
	collisionBottomWall = std::make_shared<GameObject>();
}

void PingPongGame::CreateInstance(LPCWSTR name, int screenWidth, int screenHeight)
{
	if (!instance) { instance = new PingPongGame(name, screenWidth, screenHeight); }
}

void PingPongGame::Run()
{
	ConfigureGameObjects();
	Game::Run();
}

void PingPongGame::ConfigureGameObjects()
{
// LEFT WALL
	// COLLISION
	BBLeftWall = new DirectX::BoundingBox();
	BBLeftWall->Center  = { -1.5f, 0.0f, 0.0f };
	BBLeftWall->Extents = { 0.5f, 1.0f, 0.0f };

// RIGHT WALL
	// COLLISION
	BBRightWall = new DirectX::BoundingBox();
	BBRightWall->Center  = { 1.5f, 0.0f, 0.0f };
	BBRightWall->Extents = { 0.5f, 1.0f, 0.0f };

// BALL
	RenderComponent* ballMesh = new RenderComponent(ball->position);
	DirectX::SimpleMath::Vector4 center = { 0.0f, 0.0f, 0.0f, 0.0f };
	float radius = 0.04f;
	int sides = 16;
	double stepAngle = (2 * M_PI) / sides;
	double curAngle = 0.0;
	double nextAngle = 0.0;
	DirectX::SimpleMath::Vector4 temp;
	int j = 0;
	for (int i = 0; i < sides; i++)
	{
		curAngle  = stepAngle * i;
		nextAngle = stepAngle * (i + 1);
		temp = { (float)(cos(curAngle) * radius), (float)(sin(curAngle) * radius), 0.0f, 1.0f};
		ballMesh->points.push_back(DirectX::XMFLOAT4((float)(center + temp).x, (float)(center + temp).y, 0.0f, 1.0f));
		ballMesh->points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		ballMesh->indeces.push_back(j++);
		temp = { (float)(cos(nextAngle) * radius), (float)(sin(nextAngle) * radius), 0.0f, 1.0f };
		ballMesh->points.push_back(DirectX::XMFLOAT4((float)(center + temp).x, (float)(center + temp).y, 0.0f, 1.0f));
		ballMesh->points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		ballMesh->indeces.push_back(j++);
		ballMesh->points.push_back(DirectX::XMFLOAT4(center.x, center.y, 0.0f, 1.0f));
		ballMesh->points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		ballMesh->indeces.push_back(j++);
	}
	ball->components.push_back(ballMesh);
	PingPongGame::GetInstance()->gameObjects.push_back(ball.get());

	// COLLISION
	BBBall = new DirectX::BoundingSphere();
	BBBall->Center = { 0.0f, 0.0f, 0.0f };
	BBBall->Radius = radius;
	RenderComponent* RCBall = new RenderComponent(collisionBall->position);
	RCBall->points =
	{
		DirectX::XMFLOAT4(    0.0f,    radius, 0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(    0.0f,  - radius, 0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(   radius,    0.0f,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( - radius,    0.0f,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	RCBall->indeces = { 0, 1, 2, 3 };
	RCBall->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	collisionBall->components.push_back(RCBall);
	PingPongGame::GetInstance()->gameObjects.push_back(collisionBall.get());
	// COLLISION


// LEFT PLAYER RACKER
	RenderComponent* leftPlayerRacket = new RenderComponent(leftPlayer->position);
	leftPlayerRacket->points =
	{
		DirectX::XMFLOAT4( 0.025f,  0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f,  0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4( 0.025f, -0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f, -0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	leftPlayerRacket->indeces = { 0, 1, 2, 2, 3, 1 };
	leftPlayer->components.push_back(leftPlayerRacket);
	PingPongGame::GetInstance()->gameObjects.push_back(leftPlayer.get());
	*leftPlayer->position += { -1.0f + 0.15f, 0.0f, 0.0f, 0.0f };
	
	// COLLISION
	BBLeftRacket = new DirectX::BoundingBox();
	BBLeftRacket->Center = { leftPlayer->position->x, 0.0f, 0.0f };
	BBLeftRacket->Extents = { 0.025f + BBBall->Radius, 0.15f + BBBall->Radius, 0.0f };
	RenderComponent* RCLeftRacket = new RenderComponent(collisionLeftRacket->position);
	RCLeftRacket->points =
	{
		DirectX::XMFLOAT4(BBLeftRacket->Center.x - BBLeftRacket->Extents.x,   BBLeftRacket->Extents.y,  0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBLeftRacket->Center.x + BBLeftRacket->Extents.x,   BBLeftRacket->Extents.y,  0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBLeftRacket->Center.x - BBLeftRacket->Extents.x, - BBLeftRacket->Extents.y, 0.0f, 1.0f),   DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBLeftRacket->Center.x + BBLeftRacket->Extents.x, - BBLeftRacket->Extents.y, 0.0f, 1.0f),   DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	RCLeftRacket->indeces = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 3, 1, 2 };
	collisionLeftRacket->components.push_back(RCLeftRacket);
	RCLeftRacket->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	PingPongGame::GetInstance()->gameObjects.push_back(collisionLeftRacket.get());
	// COLLISION

// RIGHT PLAYER RACKER
	RenderComponent* rightPlayerRacket = new RenderComponent(rightPlayer->position);
	rightPlayerRacket->points =
	{
		DirectX::XMFLOAT4( 0.025f,  0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f,  0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4( 0.025f, -0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f, -0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	rightPlayerRacket->indeces = { 0, 1, 2, 2, 3, 1 };
	rightPlayer->components.push_back(rightPlayerRacket);
	PingPongGame::GetInstance()->gameObjects.push_back(rightPlayer.get());
	*rightPlayer->position += { 1.0f - 0.15f, 0.0f, 0.0f, 0.0f };

	// COLLISION
	BBRightRacket = new DirectX::BoundingBox();
	BBRightRacket->Center = { rightPlayer->position->x, 0.0f, 0.0f };
	BBRightRacket->Extents = { 0.025f + BBBall->Radius, 0.15f + BBBall->Radius, 0.0f };
	RenderComponent* RCRightRacket = new RenderComponent(collisionRightRacket->position);
	RCRightRacket->points =
	{
		DirectX::XMFLOAT4( BBRightRacket->Center.x - BBRightRacket->Extents.x,   BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( BBRightRacket->Center.x + BBRightRacket->Extents.x,   BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( BBRightRacket->Center.x - BBRightRacket->Extents.x, - BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( BBRightRacket->Center.x + BBRightRacket->Extents.x, - BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	RCRightRacket->indeces = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 3, 1, 2 };
	RCRightRacket->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	collisionRightRacket->components.push_back(RCRightRacket);
	PingPongGame::GetInstance()->gameObjects.push_back(collisionRightRacket.get());
	// COLLISION

// TOP WALL
	// COLLISION
	BBTopWall = new DirectX::BoundingBox();
	BBTopWall->Center  = { 0.0f, 1.5f, 0.0f };
	BBTopWall->Extents = { 1.0f + BBBall->Radius, 0.5f + BBBall->Radius, 0.0f };
	RenderComponent* RCTopWall = new RenderComponent();
	RCTopWall->points =
	{
		DirectX::XMFLOAT4(BBTopWall->Center.x - BBTopWall->Extents.x,   BBTopWall->Center.y + BBTopWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBTopWall->Center.x + BBTopWall->Extents.x,   BBTopWall->Center.y + BBTopWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBTopWall->Center.x - BBTopWall->Extents.x,   BBTopWall->Center.y - BBTopWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBTopWall->Center.x + BBTopWall->Extents.x,   BBTopWall->Center.y - BBTopWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	//RCTopWall->indeces = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 3, 1, 2 };
	RCTopWall->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	collisionTopWall->components.push_back(RCTopWall);
	PingPongGame::GetInstance()->gameObjects.push_back(collisionTopWall.get());

// BOTTOM WALL
	// COLLISION
	BBBottomWall = new DirectX::BoundingBox();
	BBBottomWall->Center = { 0.0f, -1.5f, 0.0f };
	BBBottomWall->Extents = { 1.0f + BBBall->Radius, 0.5f + BBBall->Radius, 0.0f };
	RenderComponent* RCBottomWall = new RenderComponent();
	RCBottomWall->points =
	{
		DirectX::XMFLOAT4(BBBottomWall->Center.x - BBBottomWall->Extents.x,   BBBottomWall->Center.y + BBBottomWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBBottomWall->Center.x + BBBottomWall->Extents.x,   BBBottomWall->Center.y + BBBottomWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBBottomWall->Center.x - BBBottomWall->Extents.x,   BBBottomWall->Center.y - BBBottomWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBBottomWall->Center.x + BBBottomWall->Extents.x,   BBBottomWall->Center.y - BBBottomWall->Extents.y,  0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	//RCBottomWall->indeces = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 3, 1, 2 };
	RCBottomWall->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	collisionBottomWall->components.push_back(RCBottomWall);
	PingPongGame::GetInstance()->gameObjects.push_back(collisionBottomWall.get());

// MIDDLE WALL
}

void PingPongGame::Update()
{
	Game::Update();

	// COUNT
	if (BBBall->Center.x < -1.0f)
	{
		ballSpeed = 0.0f;
		ball->position->x = -1.0f;
		collisionBall->position->x = -1.0f;
		BBBall->Center.x = -1.0f;
		pointEnemy++;
		std::cout << "Second player WON! Count: " << pointPlayer << "/" << pointEnemy << std::endl;
		isGameEnded = true;
	}
	if (BBBall->Center.x > 1.0f)
	{
		ballSpeed = 0.0f;
		ball->position->x = 1.0f;
		collisionBall->position->x = 1.0f;
		BBBall->Center.x = 1.0f;
		pointPlayer++;
		std::cout << "First player WON! Count: " << pointPlayer << "/" << pointEnemy << std::endl;
		isGameEnded = true;
	}

	// BALL START MOVEMENT
	if (!isGameEnded)
	{
		*ball->position += DirectX::XMVectorScale(direction, ballSpeed * deltaTime);
		*collisionBall->position += DirectX::XMVectorScale(direction, ballSpeed * deltaTime);
		BBBall->Center.x += direction.x * ballSpeed * deltaTime;
		BBBall->Center.y += direction.y * ballSpeed * deltaTime;
	}
	else
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<float> distr(-1, 1);
		*ball->position          = { 0.0f, 0.0f, 0.0f, 0.0f };
		*collisionBall->position = { 0.0f, 0.0f, 0.0f, 0.0f };
		BBBall->Center           = { 0.0f, 0.0f, 0.0f };
		direction                = { distr(eng), distr(eng), 0, 0};
		direction.Normalize();
		ballSpeed                = 0.3f;
		isGameEnded = false;
	}

	//Stop move for rackets
	float dist = 0.0f;
	if (BBTopWall->Intersects({ BBRightRacket->Center.x, BBRightRacket->Center.y, BBRightRacket->Center.z }, DirectX::SimpleMath::Vector3::Up, dist))
	{
		if (dist <= BBRightRacket->Extents.y - BBBall->Radius * 2) { RRCanMoveUp = false; }
		else { RRCanMoveUp = true; }
	}
	if (BBBottomWall->Intersects({ BBRightRacket->Center.x, BBRightRacket->Center.y, BBRightRacket->Center.z }, DirectX::SimpleMath::Vector3::Down, dist))
	{
		if (dist <= BBRightRacket->Extents.y - BBBall->Radius * 2) { RRCanMoveDown = false; }
		else { RRCanMoveDown = true; }
	}

	if (BBTopWall->Intersects({ BBLeftRacket->Center.x, BBLeftRacket->Center.y, BBLeftRacket->Center.z }, DirectX::SimpleMath::Vector3::Up, dist))
	{
		if (dist <= BBLeftRacket->Extents.y - BBBall->Radius * 2) { LRCanMoveUp = false; }
		else { LRCanMoveUp = true; }
	}
	if (BBBottomWall->Intersects({ BBLeftRacket->Center.x, BBLeftRacket->Center.y, BBLeftRacket->Center.z }, DirectX::SimpleMath::Vector3::Down, dist))
	{
		if (dist <= BBLeftRacket->Extents.y - BBBall->Radius * 2) { LRCanMoveDown = false; }
		else { LRCanMoveDown = true; }
	}
	//Stop move for rackets


	// ÑÒÎËÊÍÎÂÅÍÈÅ
	float distance1;
	if (BBRightRacket->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance1))
	{
		if (BBBall->Center.x + direction.x * ballSpeed * deltaTime > BBRightRacket->Center.x - BBRightRacket->Extents.x - 0.01f)
		{
			*ball->position          = { BBBall->Center.x + direction.x * distance1, BBBall->Center.y + direction.y * distance1, 0.0f };
			*collisionBall->position = { BBBall->Center.x + direction.x * distance1, BBBall->Center.y + direction.y * distance1, 0.0f };
			BBBall->Center           = { BBBall->Center.x + direction.x * distance1, BBBall->Center.y + direction.y * distance1, 0.0f };
			// REFLECT
			DirectX::SimpleMath::Vector3 vector0 = {
				BBBall->Center.x - (BBRightRacket->Center.x + BBRightRacket->Extents.x * 2),
				BBBall->Center.y - BBRightRacket->Center.y,
				0.0f
			};
			vector0.Normalize();
			DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect({ direction.x, direction.y, direction.z }, vector0);
			direction.x = vector.x;
			direction.y = vector.y;
			direction.z = vector.z;
			
			ballSpeed += 0.3f;
		}
	}
	float distance2;
	if (BBLeftRacket->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance2))
	{
		if (BBBall->Center.x + direction.x * ballSpeed * deltaTime < BBLeftRacket->Center.x + BBLeftRacket->Extents.x + 0.01f)
		{
			*ball->position          = { BBBall->Center.x + direction.x * distance2, BBBall->Center.y + direction.y * distance2, 0.0f };
			*collisionBall->position = { BBBall->Center.x + direction.x * distance2, BBBall->Center.y + direction.y * distance2, 0.0f };
			BBBall->Center           = { BBBall->Center.x + direction.x * distance2, BBBall->Center.y + direction.y * distance2, 0.0f };
			// REFLECT
			DirectX::SimpleMath::Vector3 vector0 = {
				BBBall->Center.x - (BBLeftRacket->Center.x - BBLeftRacket->Extents.x * 2),
				BBBall->Center.y -  BBLeftRacket->Center.y,
				0.0f
			};
			vector0.Normalize();		
			DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect({ direction.x, direction.y, direction.z }, vector0);
			direction.x = vector.x;
			direction.y = vector.y;
			direction.z = vector.z;
			
			ballSpeed += 0.3f;
		}
	}

	float distance3;
	if (BBTopWall->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance3))
	{
		if (BBBall->Center.y + direction.y * ballSpeed * deltaTime > BBTopWall->Center.y - BBTopWall->Extents.y - 0.01f)
		{
			*ball->position          = { BBBall->Center.x + direction.x * distance3, BBBall->Center.y + direction.y * distance3, 0.0f };
			*collisionBall->position = { BBBall->Center.x + direction.x * distance3, BBBall->Center.y + direction.y * distance3, 0.0f };
			BBBall->Center           = { BBBall->Center.x + direction.x * distance3, BBBall->Center.y + direction.y * distance3, 0.0f };
			// REFLECT
			DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect({ direction.x, direction.y, direction.z }, { 0.0f, -1.0f, 0.0f });
			direction.x = vector.x;
			direction.y = vector.y;
			direction.z = vector.z;
		}
	}
	float distance4;
	if (BBBottomWall->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance4))
	{
		if (BBBall->Center.y + direction.y * ballSpeed * deltaTime < BBBottomWall->Center.y + BBBottomWall->Extents.y + 0.01f)
		{
			*ball->position          = { BBBall->Center.x + direction.x * distance4, BBBall->Center.y + direction.y * distance4, 0.0f };
			*collisionBall->position = { BBBall->Center.x + direction.x * distance4, BBBall->Center.y + direction.y * distance4, 0.0f };
			BBBall->Center           = { BBBall->Center.x + direction.x * distance4, BBBall->Center.y + direction.y * distance4, 0.0f };
			// REFLECT
			DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect( { direction.x, direction.y, direction.z }, { 0.0f, 1.0f, 0.0f });
			direction.x = vector.x;
			direction.y = vector.y;
			direction.z = vector.z;
		}
	}

//KEYBOARDS
	if (!isGameEnded)
	{
		if (inputDevice->IsKeyDown(Keys::W))
		{
			if (LRCanMoveUp)
			{
				*leftPlayer->position += {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				*collisionLeftRacket->position += {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBLeftRacket->Center.y += racketSpeed * deltaTime;
			}
		}
		if (inputDevice->IsKeyDown(Keys::S))
		{
			if (LRCanMoveDown)
			{
				*leftPlayer->position -= {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				*collisionLeftRacket->position -= {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBLeftRacket->Center.y -= racketSpeed * deltaTime;
			}
		}
		/*
		if (inputDevice->IsKeyDown(Keys::A))
		{
			*leftPlayer->position -= {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			*collisionLeftRacket->position -= {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			BBLeftRacket->Center.x -= racketSpeed * deltaTime;
		}
		if (inputDevice->IsKeyDown(Keys::D))
		{
			*leftPlayer->position += {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			*collisionLeftRacket->position += {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			BBLeftRacket->Center.x += racketSpeed * deltaTime;
		}
		*/

		if (inputDevice->IsKeyDown(Keys::Up))
		{
			if (RRCanMoveUp)
			{
				*rightPlayer->position += {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				*collisionRightRacket->position += {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBRightRacket->Center.y += racketSpeed * deltaTime;
			}
		}
		if (inputDevice->IsKeyDown(Keys::Down))
		{
			if (RRCanMoveDown)
			{
				*rightPlayer->position -= {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				*collisionRightRacket->position -= {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBRightRacket->Center.y -= racketSpeed * deltaTime;
			}
		}
		/*
		if (inputDevice->IsKeyDown(Keys::Left))
		{
			*rightPlayer->position -= {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			*collisionRightRacket->position -= {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			BBRightRacket->Center.x -= racketSpeed * deltaTime;
		}
		if (inputDevice->IsKeyDown(Keys::Right))
		{
			*rightPlayer->position += {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			*collisionRightRacket->position += {racketSpeed* deltaTime, 0.0f, 0.0f, 0.0f};
			BBRightRacket->Center.x += racketSpeed * deltaTime;
		}
		*/
	}
//KEYBOARDS
}
