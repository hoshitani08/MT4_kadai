#pragma once
#include "Sprite.h"
#include "DebugText.h"

class SpriteFactory final
{
private:
	SpriteFactory() = default;
	~SpriteFactory() = default;

public:
	SpriteFactory(const SpriteFactory& spriteFactory) = delete;
	SpriteFactory& operator=(const SpriteFactory& spriteFactory) = delete;

	// èâä˙âª
	void Initialize();

	static SpriteFactory* GetInstance();
};