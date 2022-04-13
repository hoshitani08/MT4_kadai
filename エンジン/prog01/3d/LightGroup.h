#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"

class LightGroup
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // 定数
	// 平行光源の数
	static const int DirLightNum = 3;
	// 点光源の数
	static const int PointLightNum = 3;
	// スポットライトの数
	static const int SpotLightNum = 3;
	// 丸影の数
	static const int CircleShadowNum = 1;

public: // サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		//環境光の色
		XMFLOAT3 ambientColor;
		float pad1;
		//平行光源用
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
		// 点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
		// スポットライト用
		SpotLight::ConstBufferData spotLights[SpotLightNum];
		// 丸影用
		CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
	};

public: // 静的メンバ関数
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//インスタンス生成
	static std::unique_ptr<LightGroup> Create();

private: // 静的メンバ変数
	//デバイス
	static ID3D12Device* device;

public: // メンバ関数

	~LightGroup();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);
	//定数バッファ転送
	void TransferConstBuffer();
	// 標準のライト設定
	void DefaultLightSetting();
	//環境光のライト色をセット
	void SetAmbientColor(const XMFLOAT3& color);
	//平行光源の有効フラグをセット
	void SetDirLightActive(int index, bool active);
	//平行光源のライト方向をセット
	void SetDirLightDir(int index, const XMVECTOR& lightdir);
	//平行光源のライト色をセット
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);
	//点光源の有効フラグをセット
	void SetPointLightActive(int index, bool active);
	//点光源のライト座標をセット
	void SetPointLightPos(int index, const XMFLOAT3& lightpos);
	//点光源のライト色をセット
	void SetPointLightColor(int index, const XMFLOAT3& lightcolor);
	//点光源のライト距離減衰係数をセット
	void SetPointLightAtten(int index, const XMFLOAT3& lightAtten);
	// スポットライトの有効フラグをセット
	void SetSpotLightActive(int index, bool active);
	// スポットライトのライト方向をセット
	void SetSpotLightDir(int index, const XMVECTOR& lightdir);
	// スポットライトのライト座標をセット
	void SetSpotLightPos(int index, const XMFLOAT3& lightpos);
	// スポットライトのライト色をセット
	void SetSpotLightColor(int index, const XMFLOAT3& lightcolor);
	// スポットライトのライト距離減衰係数をセット
	void SetSpotLightAtten(int index, const XMFLOAT3& lightAtten);
	// スポットライトのライト減衰角度をセット
	void SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle);
	// 丸影の有効フラグをセット
	void SetCircleShadowActive(int index, bool active);
	// 丸影のキャスター座標をセット
	void SetCircleShadowCasterPos(int index, const XMFLOAT3& casterPos);
	// 丸影の方向をセット
	void SetCircleShadowDir(int index, const XMVECTOR& lightdir);
	// 丸影のキャスターとライトの距離をセット
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	// 丸影の距離減衰係数をセット
	void SetCircleShadowAtten(int index, const XMFLOAT3& lightAtten);
	// 丸影の減衰角度をセット
	void SetCircleShadowFactorAngle(int index, const XMFLOAT2& lightFactorAngle);

private: // メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//環境光の色
	XMFLOAT3 ambientColor = { 1,1,1 };
	//平行光源の配列
	DirectionalLight dirLights[DirLightNum];
	//点光源の配列
	PointLight pointLights[PointLightNum];
	// スポットライトの配列
	SpotLight spotLights[SpotLightNum];
	// 丸影の配列
	CircleShadow circleShadows[CircleShadowNum];
	//ダーティフラグ
	bool dirty = false;
};