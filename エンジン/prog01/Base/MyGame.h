#pragma once
#include "BaseGame.h"

class MyGame : public BaseGame
{
public:
	// ‰Šú‰»
	void Initialize() override;
	//I—¹
	void Finalize() override;
	// –ˆƒtƒŒ[ƒ€ˆ—
	void Update() override;
	// •`‰æ
	void Draw() override;
};