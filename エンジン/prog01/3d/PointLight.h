#pragma once

#include <DirectXMath.h>

class PointLight
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		unsigned int active;
	};

public: // メンバ関数
	//ライト座標をセット
	inline void SetLightPos(const XMFLOAT3& lightpos) { this->lightpos = lightpos; }
	//ライト座標を取得
	inline const XMFLOAT3& GetLightPos() { return lightpos; }
	//ライト色をセット
	inline void SetLightColor(const XMFLOAT3& lightcolor) { this->lightcolor = lightcolor; }
	//ライト色を取得
	inline const XMFLOAT3& GetLightColor() { return lightcolor; }
	//ライト距離減衰係数をセット
	inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten = lightAtten; }
	//ライト距離減衰係数を取得
	inline const XMFLOAT3& GetLightAtten() { return lightAtten; }
	//有効フラグをセット
	inline void SetActive(bool active) { this->active = active; }
	//有効チェック
	inline bool IsActive() { return active; }

private: // メンバ変数
	//ライト座標（ワールド座標系）
	XMFLOAT3 lightpos = { 0,0,0 };
	//ライト色
	XMFLOAT3 lightcolor = { 1,1,1 };
	//ライト距離減衰係数
	XMFLOAT3 lightAtten = { 1.0f, 1.0f, 1.0f };
	//有効フラグ
	bool active = false;
};