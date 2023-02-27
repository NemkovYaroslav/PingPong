#include "Game.h"
#include "PingPongGame.h"

int main()
{
	PingPongGame::CreateInstance(L" Pong ", 1200, 1200);
	PingPongGame::GetInstance()->Run();
}
