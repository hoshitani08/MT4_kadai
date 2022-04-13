#include "SceneManager.h"

#include <cassert>

SceneManager::~SceneManager()
{
	if (scene)
	{
		Finalize();
	}
}

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Finalize()
{
	scene->Finalize();
	delete scene;
	scene = nullptr;
}

void SceneManager::Update()
{
	if (nextScene)
	{
		if (scene)
		{
			scene->Finalize();
			delete scene;
		}
		scene = nextScene;
		nextScene = nullptr;

		scene->Initialize();
	}
	scene->Update();
}

void SceneManager::Draw()
{
	scene->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory);
	assert(nextScene == nullptr);

	nextScene = sceneFactory->CreateScene(sceneName);
}