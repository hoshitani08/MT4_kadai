#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Model.h"
#include "Camera.h"
#include "LightGroup.h"
#include "CollisionInfo.h"

#include <memory>

class Object3d
{
protected: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		XMMATRIX viewproj;  //ビュープロジェクション行列
		XMMATRIX world;     //ワールド行列
		XMFLOAT3 cameraPos; //カメラ座標(ワールド座標)
	};

private: // 定数

public: // 静的メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device, Camera* camera = nullptr);
	// 静的破棄
	static void StaticFinalize();
	// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	// 描画後処理
	static void PostDraw();
	// 3Dオブジェクト生成
	static std::unique_ptr<Object3d> Create(Model* model = nullptr);
	// カメラのセット
	static void SetCamera(Camera* camera) { Object3d::camera = camera; }
	//ライトのセット
	static void SetLight(LightGroup* light) { Object3d::light = light; }

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// カメラ
	static Camera* camera;
	//ライト
	static LightGroup* light;

public: // メンバ関数
	// コンストラクタ
	Object3d() = default;
	// デストラクタ
	virtual ~Object3d();
	// 初期化
	virtual bool Initialize();
	// 毎フレーム処理
	virtual void Update();
	// 描画
	virtual void Draw();
	// 行列の更新
	void UpdateWorldMatrix();
	// 座標の取得
	const XMFLOAT3& GetPosition() { return position; }
	// 座標の設定
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	// X,Y,Z軸回りの取得
	const XMFLOAT3& GetRotation() { return rotation; }
	// X,Y,Z軸回りの設定
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	// スケールの取得
	const XMFLOAT3& GetScale() { return scale; }
	// スケールの設定
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	// モデルの設定
	void SetModel(Model* model) { this->model = model; };
	// ビルボードの設定
	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }
	// ワールド行列の取得
	const XMMATRIX& GetMatWorld() { return matWorld; }
	// コライダーのセット
	void SetCollider(BaseCollider* collider);
	// 衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) {}
	// ワールド座標を取得
	XMFLOAT3 GetWorldPosition();
	// モデルを取得
	inline Model* GetModel() { return model; }

protected: // メンバ変数
	ComPtr<ID3D12Resource> constBuffB0; // 定数バッファ
	// 色
	XMFLOAT4 color = { 1,1,1,1 };
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	// ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	// ローカルワールド変換行列
	XMMATRIX matWorld;
	// 親オブジェクト
	Object3d* parent = nullptr;
	// モデル
	Model* model = nullptr;
	// ビルボード
	bool isBillboard = false;
	//クラス名(デバック用)
	const char* name = nullptr;
	// コライダー
	BaseCollider* collider = nullptr;
};