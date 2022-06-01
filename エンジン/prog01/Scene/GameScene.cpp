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

	// FBXオブジェクト生成
	object3d = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	object3d2 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	//アニメーション
	//fbxObject3d->PlayAnimation();

	// カメラ注視点をセット
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-60 });
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

	if (isFlag)
	{
		Length += 0.5f;
		if (Length >= 30)
		{
			Length = 30;
		}
		float radius = angle * 3.14f / 180.0f;

		XMFLOAT3 pos = object3d2->GetPosition();
		XMFLOAT3 center = object3d->GetPosition();

		pos.x = cos(radius) * Length;
		pos.y = sin(radius) * Length;

		pos.x += center.x;
		pos.y += center.y;

		object3d2->SetPosition(pos);

		angle += 2.5f;
	}

	if (input->TriggerKey(DIK_SPACE) && !isFlag)
	{
		if (input->PushKey(DIK_A))
		{
			angle += 180;
			isFlag = true;
		}
		else if (input->PushKey(DIK_D))
		{
			angle += 360;
			isFlag = true;
		}

		if (input->PushKey(DIK_W))
		{
			angle += 90;
			isFlag = true;
		}
		else if (input->PushKey(DIK_S))
		{
			angle += 270;
			isFlag = true;
		}
	}

	if (input->TriggerKey(DIK_R))
	{
		angle = 0.0f;
		Length = 0.0f;
		isFlag = false;
		object3d2->SetPosition({0,0,0});
	}

	object3d->Update();
	object3d2->Update();
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
	object3d->Draw();
	object3d2->Draw();
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