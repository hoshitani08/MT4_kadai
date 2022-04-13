#pragma once

#include <DirectXMath.h>

class CircleShadow
{
private: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR dir;
		XMFLOAT3 casterPos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad3;
		XMFLOAT2 factorAngleCos;
		unsigned int active;
		float pad4;
	};

public: // メンバ関数
	// 方向をセット
	inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }
	// ライト方向を取得
	inline const XMVECTOR& GetDir() { return dir; }
	// キャスター座標をセット
	inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos = casterPos; }
	// キャスター座標を取得
	inline const XMFLOAT3& GetCasterPos() { return casterPos; }
	// キャスターとライトの距離をセット
	inline void SetDistanceCasterLight(float distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }
	// キャスターとライトの距離を取得
	inline float GetDistanceCasterLight() { return distanceCasterLight; }
	// 距離減衰係数をセット
	inline void SetAtten(const XMFLOAT3& atten) { this->atten = atten; }
	// 距離減衰係数を取得
	inline const XMFLOAT3& GetAtten() { return atten; }
	// 減衰角度をセット
	inline void SetFactorAngle(const XMFLOAT2& factorAngle)
	{
		this->factorAngleCos.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}
	// 減衰角度を取得
	inline const XMFLOAT2& GetFactorAngleCos() { return factorAngleCos; }
	// 有効フラグをセット
	inline void SetActive(bool active) { this->active = active; }
	// 有効チェック
	inline bool IsActive() { return active; }

private: // メンバ変数
	// 方向（単位ベクトル）
	XMVECTOR dir = { 1,0,0,0 };
	// キャスターとライトの距離
	float distanceCasterLight = 100.0f;
	// キャスター座標（ワールド座標系）
	XMFLOAT3 casterPos = { 0,0,0 };
	// 距離減衰係数
	XMFLOAT3 atten = { 0.5f, 0.6f, 0.0f };
	// 減衰角度
	XMFLOAT2 factorAngleCos = { 0.2f, 0.5f };
	// 有効フラグ
	bool active = false;
};