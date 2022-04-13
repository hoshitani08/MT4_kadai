#include "MyGame.h"
#include "MapChip.h"
#include "SceneFactory.h"
#include "SpriteFactory.h"
#include "FbxFactory.h"
#include "ObjFactory.h"

void MyGame::Initialize()
{
	//基底クラスの初期化
	BaseGame::Initialize();

	//ゲームで必要なマップデータのロード
	//MapChip::GetInstance()->CsvLoad();
	//ゲームで必要な画像のロード
	SpriteFactory::GetInstance()->Initialize();
	//ゲームで必要なモデル(.fbx)のロード
	FbxFactory::GetInstance()->Initialize();
	//ゲームで必要なモデル(.obj)のロード
	ObjFactory::GetInstance()->Initialize();
	//ゲームで必要なサウンドのロード
	Audio::GetInstance()->LoadWave(0, "Resources/Alarm01.wav");

	// シーンの初期化
	//シーンファクトリを生成し、マネージャーにセット
	sceneFactory = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory);
	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("GameScene");
}

void MyGame::Finalize()
{
	// シーンの終了
	SceneManager::GetInstance()->Finalize();
	//各Factoryの解放
	FbxFactory::GetInstance()->Finalize();
	ObjFactory::GetInstance()->Finalize();
	//基底クラスの解放
	BaseGame::Finalize();
}

void MyGame::Update()
{
	//基底クラスの更新
	BaseGame::Update();
}

void MyGame::Draw()
{
	BaseGame::Draw();
}