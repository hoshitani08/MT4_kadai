#pragma once

#include "BaseScene.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Model.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "CollisionPrimitive.h"
#include "Camera.h"
#include "LightGroup.h"
#include "FbxObject3d.h"

#include <DirectXMath.h>
#include <vector>
#include <memory>

class CollisionManager;
class Player;
class ContactableObject;

class GameScene : public BaseScene
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize() override;
	//終了
	void Finalize() override;
	// 毎フレーム処理
	void Update() override;
	// 描画
	void Draw() override;

private: // メンバ変数
	//カメラ
	std::unique_ptr<Camera> camera;
	//スプライト
	std::unique_ptr<Sprite> sprite;
	//パーティクル
	std::unique_ptr<ParticleManager> particleMan;
	//オブジェクト
	std::unique_ptr<FbxObject3d> fbxObject3d;

	//ライト
	std::unique_ptr<LightGroup> light;
	//光線方向初期値
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowPos[3] = { 1,2,0 };
	float circleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float circleShadowFactorAngle[2] = { 0.0f, 0.5f };

	float fighterPos[3] = { 1, 0.0f, 0 };
	//当たり判定
	CollisionManager* collisionManager = nullptr;
};