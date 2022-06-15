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
	sprite1 = Sprite::Create(2, { 0.0f,0.0f });
	sprite1->SetSize({ 100.0f,100.0f });
	sprite1->SetPosition({ 1280.0f / 2 - 60, 720.0f / 2 - 60 });

	sprite2 = Sprite::Create(3, { 0.0f,0.0f });
	sprite2->SetSize({ 1.0f,200.0f });
	sprite2->SetPosition({ 450.0f,250.0f });

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

	//アニメーション

	// カメラ注視点をセット
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-500 });
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

	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_A) | input->PushKey(DIK_D))
	{
		XMFLOAT2 pos = sprite2->GetPosition();

		if (input->PushKey(DIK_W))
		{
			pos.y -= 2.0f;
		}
		else if (input->PushKey(DIK_S))
		{
			pos.y += 2.0f;
		}

		if (input->PushKey(DIK_A))
		{
			pos.x -= 2.0f;
		}
		else if (input->PushKey(DIK_D))
		{
			pos.x += 2.0f;
		}
		sprite2->SetPosition(pos);
	}

	XMFLOAT2 spos = sprite1->GetPosition();
	XMFLOAT2 bpos = sprite2->GetPosition();

	/*float d = ((bpos.x + 0.5f) - (bpos.x + 0.5f)) * (spos.y - bpos.y);
	float e = ((bpos.y + 200) - bpos.y) * (spos.x - (bpos.x + 0.5f));

	float a = -(d - e);*/

	Sphere a;
	a.center = { spos.x + 50, spos.y + 50, 0 };
	a.radius = 50;
	
	Box b;
	b.center = { bpos.x + 0.5f, bpos.y + 100, 0 };
	b.scale = { 1,150,1 };

	if (Collision::CheckSphere2Box(a,b))
	{
		sprite1->SetColor({1,0,0,1});
	}
	else
	{
		sprite1->SetColor({ 1,1,1,1 });
	}

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
	sprite2->Draw();
	sprite1->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブクジェクトの描画
	Object3d::PreDraw(cmdList);
	
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