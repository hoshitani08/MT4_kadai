#include "MyGame.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	std::unique_ptr<BaseGame> game = std::make_unique<MyGame>();

	game->Run();

	return 0;
}