#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//éüÇÃÉVÅ[ÉìÇê∂ê¨
	BaseScene* newScene = nullptr;
	
	if (sceneName == "TitleScene")
	{
		newScene = new TitleScene();
	}
	else if (sceneName == "GameScene")
	{
		newScene = new GameScene();
	}
	else if (sceneName == "ClearScene")
	{
		newScene = new ClearScene();
	}
	else if (sceneName == "GameOverScene")
	{
		newScene = new GameOverScene();
	}

	return newScene;
}
