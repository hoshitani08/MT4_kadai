#include "GameScene.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "ContactableObject.h"

#include "SceneManager.h"
#include "FbxFactory.h"
#include "ObjFactory.h"

#include "DirectXCommon.h"
#include "DebugText.h"
#include "Audio.h"
#include "Input.h"
#include "Ease.h"

using namespace DirectX;

GameScene::~GameScene()
{
	Finalize();
}

void GameScene::Initialize()
{
	collisionManager = CollisionManager::GetInstance();

	// カメラ生成
	camera = std::make_unique<Camera>(WinApp::window_width, WinApp::window_height);

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera.get());
	// FBXオブジェクトにカメラをセット
	FbxObject3d::SetCamera(camera.get());

	// 背景スプライト生成
	sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ 100.0f,100.0f });
	sprite->SetPosition({ 100.0f,100.0f });

	// パーティクルマネージャ生成
	particleMan = ParticleManager::Create(DirectXCommon::GetInstance()->GetDevice(), camera.get());

	//ライト生成
	light = LightGroup::Create();
	//オブジェクトにライトをセット
	Object3d::SetLight(light.get());
	FbxObject3d::SetLight(light.get());
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, false);
	light->SetDirLightActive(2, false);
	light->SetPointLightActive(0, false);
	light->SetPointLightActive(1, false);
	light->SetPointLightActive(2, false);
	light->SetCircleShadowActive(0, true);

	light->SetDirLightDir(0, { 0,0,1,0 });

	// 3Dオブジェクト生成
	object3d1 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	object3d2 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	object3d3 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));

	object3d1->SetPosition({ -45, 10, 0 });
	object3d2->SetPosition({ -45, 0, 0 });
	object3d3->SetPosition({ -45, -10, 0 });

	for (int i = 0; i < sab1.size(); i++)
	{
		sab1[i] = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
		sab2[i] = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
		sab3[i] = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));

		sab1[i]->SetPosition({ 0, 100, 0 });
		sab2[i]->SetPosition({ 0, 100, 0 });
		sab3[i]->SetPosition({ 0, 100, 0 });
	}

	// FBXオブジェクト生成

	// カメラ注視点をセット
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-50 });
}

void GameScene::Finalize()
{
}

void GameScene::Update()
{
	Input* input = Input::GetInstance();
	light->Update();
	camera->Update();
	particleMan->Update();

	if (input->TriggerKey(DIK_SPACE) && !easeFlag)
	{
		easeFlag = true;
	}

	if (easeFlag)
	{
		float timeRate = 0.0f;
		int countNum = 60;
		timeRate = timer / countNum;
		timer++;

		object3d1->SetPosition(Ease::easeIn({ -45, 10, 0 }, { 45, 10, 0 }, timeRate));
		object3d2->SetPosition(Ease::easeOut({ -45, 0, 0 }, { 45, 0, 0 }, timeRate));
		object3d3->SetPosition(Ease::easeInOut({ -45, -10, 0 }, { 45, -10, 0 }, timeRate));

		if (timer >= 6 * count && count < 10)
		{
			sab1[count - 1]->SetPosition(object3d1->GetPosition());
			sab2[count - 1]->SetPosition(object3d2->GetPosition());
			sab3[count - 1]->SetPosition(object3d3->GetPosition());
			count++;
		}

		if (timer > countNum)
		{
			timer = 0;
			easeFlag = false;
		}
	}

	for (int i = 0; i < sab1.size(); i++)
	{
		sab1[i]->Update();
		sab2[i]->Update();
		sab3[i]->Update();
	}
	object3d1->Update();
	object3d2->Update();
	object3d3->Update();
	// 全ての衝突をチェック
	collisionManager->CheckAllCollisions();
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブクジェクトの描画
	Object3d::PreDraw(cmdList);
	for (int i = 0; i < sab1.size(); i++)
	{
		sab1[i]->Draw();
		sab2[i]->Draw();
		sab3[i]->Draw();
	}
	object3d1->Draw();
	object3d2->Draw();
	object3d3->Draw();
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画
#pragma region 3Dオブジェクト(FBX)描画

#pragma endregion 3Dオブジェクト(FBX)描画
#pragma region パーティクル
	// パーティクルの描画
	particleMan->Draw(cmdList);
#pragma endregion パーティクル
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(cmdList);
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}