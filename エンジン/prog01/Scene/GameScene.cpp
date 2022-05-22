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

	object3d->SetPosition({ 10,0,0 });
	object3d2->SetPosition({ -10,0,0 });

	// カメラ注視点をセット
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-150 });
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

	// 落下処理
	if (!onGround)
	{
		//空気抵抗の比例定数
		const float fallVYMinX = -5.0f;
		// 移動
		XMFLOAT3 position = object3d->GetPosition();
		position.x += fallV.m128_f32[0];
		position.y += gravity;

		// 移動2
		XMFLOAT3 position2 = object3d2->GetPosition();
		position2.x += fallV2.m128_f32[0];
		position2.y += gravity;

		if (position.y > 0 || position2.y > 0)
		{
			if (fallV.m128_f32[0] != 0.0f || fallV2.m128_f32[0] != 0.0f)
			{
				fallV.m128_f32[0] = max(fallV.m128_f32[0] * damp, fallVYMinX);
				fallV2.m128_f32[0] = max(fallV2.m128_f32[0] * damp, fallVYMinX);
			}
			position.y = 0;    //ボールは画面の外に外れない
			position2.y = 0;    //ボールは画面の外に外れない
		}

		object3d->SetPosition(position);
		object3d2->SetPosition(position2);

		Sphere p1 = { position.x, position.y, position.z, 1 };
		p1.radius = 1.15f;
		Sphere p2 = { position2.x, position2.y, position2.z, 1 };
		p2.radius = 1.15f;

		if (Collision::CheckSphere2Sphere(p1, p2))
		{
			fallV.m128_f32[0] = fallV.m128_f32[0] * 2.0f;
			fallV2.m128_f32[0] = fallV2.m128_f32[0] * 2.0f;
		}
	}
	else if (input->TriggerKey(DIK_SPACE))
	{
		onGround = false;
		const float jumpVYFistX = 3.0f; //右向き初速
		fallV = { -jumpVYFistX, 0, 0, 0 };
		fallV2 = { jumpVYFistX, 0, 0, 0 };
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