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
// BALL
	RenderComponent* RCBall = new RenderComponent(ball->position);
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
		RCBall->points.push_back(DirectX::XMFLOAT4((float)(center + temp).x, (float)(center + temp).y, 0.0f, 1.0f));
		RCBall->points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		RCBall->indeces.push_back(j++);
		temp = { (float)(cos(nextAngle) * radius), (float)(sin(nextAngle) * radius), 0.0f, 1.0f };
		RCBall->points.push_back(DirectX::XMFLOAT4((float)(center + temp).x, (float)(center + temp).y, 0.0f, 1.0f));
		RCBall->points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		RCBall->indeces.push_back(j++);
		RCBall->points.push_back(DirectX::XMFLOAT4(center.x, center.y, 0.0f, 1.0f));
		RCBall->points.push_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		RCBall->indeces.push_back(j++);
	}
	ball->components.push_back(RCBall);
	// COLLISION
	BBBall = new DirectX::BoundingSphere();
	BBBall->Center = { ball->position->x, ball->position->y, 0.0f };
	BBBall->Radius = radius;
	RenderComponent* RCCBall = new RenderComponent(ball->position);
	RCCBall->points =
	{
		DirectX::XMFLOAT4(BBBall->Center.x,                    BBBall->Center.y + BBBall->Radius,   0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBBall->Center.x,                    BBBall->Center.y - BBBall->Radius,   0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBBall->Center.x + BBBall->Radius,   BBBall->Center.y,                    0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBBall->Center.x - BBBall->Radius,   BBBall->Center.y,                    0.0f,  1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	RCCBall->indeces = { 0, 1, 2, 3 };
	RCCBall->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	ball->components.push_back(RCCBall);
	PingPongGame::GetInstance()->gameObjects.push_back(ball.get());
	// COLLISION

	float stepFromCenter = 0.85f;
// LEFT PLAYER RACKER
	RenderComponent* RCLeftPlayer = new RenderComponent(leftPlayer->position);
	RCLeftPlayer->points =
	{
		DirectX::XMFLOAT4( 0.025f,  0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f,  0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4( 0.025f, -0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f, -0.15f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	RCLeftPlayer->indeces = { 0, 1, 2, 2, 3, 1 };
	leftPlayer->components.push_back(RCLeftPlayer);	
	// COLLISION
	BBLeftRacket = new DirectX::BoundingBox();
	BBLeftRacket->Center  = { leftPlayer->position->x - stepFromCenter, leftPlayer->position->y, 0.0f };
	BBLeftRacket->Extents = { 0.025f + BBBall->Radius,  0.15f + BBBall->Radius, 0.0f };
	RenderComponent* RCCLeftRacket = new RenderComponent(leftPlayer->position);
	RCCLeftRacket->points =
	{
		DirectX::XMFLOAT4(BBLeftRacket->Center.x - BBLeftRacket->Extents.x + stepFromCenter,  BBLeftRacket->Center.y + BBLeftRacket->Extents.y,  0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBLeftRacket->Center.x + BBLeftRacket->Extents.x + stepFromCenter,  BBLeftRacket->Center.y + BBLeftRacket->Extents.y,  0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBLeftRacket->Center.x - BBLeftRacket->Extents.x + stepFromCenter,  BBLeftRacket->Center.y - BBLeftRacket->Extents.y,  0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(BBLeftRacket->Center.x + BBLeftRacket->Extents.x + stepFromCenter,  BBLeftRacket->Center.y - BBLeftRacket->Extents.y,  0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	RCCLeftRacket->indeces = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 3, 1, 2 };	
	*leftPlayer->position += { -stepFromCenter, 0.0f, 0.0f, 0.0f };
	leftPlayer->components.push_back(RCCLeftRacket);
	RCCLeftRacket->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	PingPongGame::GetInstance()->gameObjects.push_back(leftPlayer.get());
	// COLLISION

// RIGHT PLAYER RACKER
	RenderComponent* RCRightPlayer = new RenderComponent(rightPlayer->position);
	RCRightPlayer->points =
	{
		DirectX::XMFLOAT4( 0.025f,  0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f,  0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4( 0.025f, -0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.025f, -0.15f, 0.0f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	RCRightPlayer->indeces = { 0, 1, 2, 2, 3, 1 };
	rightPlayer->components.push_back(RCRightPlayer);
	// COLLISION
	BBRightRacket = new DirectX::BoundingBox();
	BBRightRacket->Center = { rightPlayer->position->x + stepFromCenter,  rightPlayer->position->y, 0.0f };
	BBRightRacket->Extents = { 0.025f + BBBall->Radius, 0.15f + BBBall->Radius, 0.0f };
	RenderComponent* RCCRightRacket = new RenderComponent(rightPlayer->position);
	RCCRightRacket->points =
	{
		DirectX::XMFLOAT4( BBRightRacket->Center.x - BBRightRacket->Extents.x - stepFromCenter, BBLeftRacket->Center.y + BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( BBRightRacket->Center.x + BBRightRacket->Extents.x - stepFromCenter, BBLeftRacket->Center.y + BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( BBRightRacket->Center.x - BBRightRacket->Extents.x - stepFromCenter, BBLeftRacket->Center.y - BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f),
		DirectX::XMFLOAT4( BBRightRacket->Center.x + BBRightRacket->Extents.x - stepFromCenter, BBLeftRacket->Center.y - BBRightRacket->Extents.y,  0.0f,  1.0f ),  DirectX::XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f)
	};
	RCCRightRacket->indeces = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 3, 1, 2 };
	*rightPlayer->position += { stepFromCenter, 0.0f, 0.0f, 0.0f };
	rightPlayer->components.push_back(RCCRightRacket);
	RCCRightRacket->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	PingPongGame::GetInstance()->gameObjects.push_back(rightPlayer.get());
	// COLLISION

// TOP WALL
	// COLLISION
	BBTopWall = new DirectX::BoundingBox();
	BBTopWall->Center  = { 0.0f, 1.5f, 0.0f };
	BBTopWall->Extents = { 1.0f + BBBall->Radius, 0.5f + BBBall->Radius, 0.0f };

// BOTTOM WALL
	// COLLISION
	BBBottomWall = new DirectX::BoundingBox();
	BBBottomWall->Center = { 0.0f, -1.5f, 0.0f };
	BBBottomWall->Extents = { 1.0f + BBBall->Radius, 0.5f + BBBall->Radius, 0.0f };

}

void PingPongGame::Update()
{
	Game::Update();

	// COUNT
	if (BBBall->Center.x < -1.0f)
	{
		ballSpeed = 0.0f;
		ball->position->x = -1.0f;
		pointSecondPlayer++;
		std::cout << "Second player WON! Count: " << pointFirstPlayer << "/" << pointSecondPlayer << std::endl;
		isGameEnded = true;
	}
	if (BBBall->Center.x > 1.0f)
	{
		ballSpeed = 0.0f;
		ball->position->x = 1.0f;
		pointFirstPlayer++;
		std::cout << "First player WON! Count: " << pointFirstPlayer << "/" << pointSecondPlayer << std::endl;
		isGameEnded = true;
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

	if (!isGameEnded)
	{
		float distance1;
		if (BBRightRacket->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance1))
		{
			if (BBBall->Center.x + direction.x * ballSpeed * deltaTime > BBRightRacket->Center.x - BBRightRacket->Extents.x - 0.01f)
			{
				*ball->position = { BBBall->Center.x + direction.x * distance1, BBBall->Center.y + direction.y * distance1, 0.0f };
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
			else
			{
				*ball->position += DirectX::XMVectorScale(direction, ballSpeed * deltaTime);
				BBBall->Center = { ball->position->x, ball->position->y, 0.0f };
			}
		}
		else
		{
			float distance2;
			if (BBLeftRacket->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance2))
			{
				if (BBBall->Center.x + direction.x * ballSpeed * deltaTime < BBLeftRacket->Center.x + BBLeftRacket->Extents.x + 0.01f)
				{
					*ball->position = { BBBall->Center.x + direction.x * distance2, BBBall->Center.y + direction.y * distance2, 0.0f };
					// REFLECT
					DirectX::SimpleMath::Vector3 vector0 = {
						BBBall->Center.x - (BBLeftRacket->Center.x - BBLeftRacket->Extents.x * 2),
						BBBall->Center.y - BBLeftRacket->Center.y,
						0.0f
					};
					vector0.Normalize();
					DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect({ direction.x, direction.y, direction.z }, vector0);
					direction.x = vector.x;
					direction.y = vector.y;
					direction.z = vector.z;

					ballSpeed += 0.3f;
				}
				else
				{
					*ball->position += DirectX::XMVectorScale(direction, ballSpeed * deltaTime);
					BBBall->Center = { ball->position->x, ball->position->y, 0.0f };
				}
			}
			else
			{
				float distance3;
				if (BBTopWall->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance3))
				{
					if (BBBall->Center.y + direction.y * ballSpeed * deltaTime > BBTopWall->Center.y - BBTopWall->Extents.y - 0.01f)
					{
						*ball->position = { BBBall->Center.x + direction.x * distance3, BBBall->Center.y + direction.y * distance3, 0.0f };
						// REFLECT
						DirectX::SimpleMath::Vector3 vector0 = { 0.0f, -1.0f, 0.0f };
						vector0.Normalize();
						DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect({ direction.x, direction.y, direction.z }, vector0);
						direction.x = vector.x;
						direction.y = vector.y;
						direction.z = vector.z;

						//ballSpeed += 0.8f;
					}
					else
					{
						*ball->position += DirectX::XMVectorScale(direction, ballSpeed * deltaTime);
						BBBall->Center = { ball->position->x, ball->position->y, 0.0f };
					}
				}
				else
				{
					float distance4;
					if (BBBottomWall->Intersects({ BBBall->Center.x, BBBall->Center.y, BBBall->Center.z }, direction, distance4))
					{
						if (BBBall->Center.y + direction.y * ballSpeed * deltaTime < BBBottomWall->Center.y + BBBottomWall->Extents.y + 0.01f)
						{
							*ball->position = { BBBall->Center.x + direction.x * distance4, BBBall->Center.y + direction.y * distance4, 0.0f };
							// REFLECT
							DirectX::SimpleMath::Vector3 vector0 = { 0.0f, 1.0f, 0.0f };
							vector0.Normalize();
							DirectX::SimpleMath::Vector3 vector = DirectX::SimpleMath::Vector3::Reflect({ direction.x, direction.y, direction.z }, vector0);
							direction = { vector.x, vector.y, vector.z };

							//ballSpeed += 0.8f;
						}
						else
						{
							*ball->position += DirectX::XMVectorScale(direction, ballSpeed * deltaTime);
							BBBall->Center = { ball->position->x, ball->position->y, 0.0f };
						}
					}
				}
			}
		}
	}
	else
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<float> distr(-1, 1);
		*ball->position = { 0.0f, 0.0f, 0.0f, 0.0f };
		BBBall->Center = { 0.0f, 0.0f, 0.0f };
		direction = { distr(eng), distr(eng), 0, 0 };
		direction.Normalize();
		ballSpeed = 0.3f;
		isGameEnded = false;
	}

//KEYBOARDS
	if (!isGameEnded)
	{
		if (inputDevice->IsKeyDown(Keys::W))
		{
			if (LRCanMoveUp)
			{
				*leftPlayer->position += {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBLeftRacket->Center.y += racketSpeed * deltaTime;
			}
		}
		if (inputDevice->IsKeyDown(Keys::S))
		{
			if (LRCanMoveDown)
			{
				*leftPlayer->position -= {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBLeftRacket->Center.y -= racketSpeed * deltaTime;
			}
		}
		if (inputDevice->IsKeyDown(Keys::Up))
		{
			if (RRCanMoveUp)
			{
				*rightPlayer->position += {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBRightRacket->Center.y += racketSpeed * deltaTime;
			}
		}
		if (inputDevice->IsKeyDown(Keys::Down))
		{
			if (RRCanMoveDown)
			{
				*rightPlayer->position -= {0.0f, racketSpeed* deltaTime, 0.0f, 0.0f};
				BBRightRacket->Center.y -= racketSpeed * deltaTime;
			}
		}
	}
//KEYBOARDS
}
