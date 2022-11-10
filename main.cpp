#include "Game/arkanoid/ArkanoidController.hpp"

int main(const int argc, const char* args[])
{
	GameBase::Ptr game(new Arkanoid());

	game->start();

	return 0;
}