#pragma once

#include <DirectXMath.h>

class Object3d;
class BaseCollider;

struct QueryHit // クエリーによる情報を得る為の構造体
{
	// 衝突相手のオブジェクト
	Object3d* object = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider = nullptr;
	// 衝突点
	DirectX::XMVECTOR inter;
	// 排斥ベクトル
	DirectX::XMVECTOR reject;
};

class QueryCallback // クエリーで交差を検出した時の動作を規定するクラス
{
public:
	QueryCallback() = default;
	virtual ~QueryCallback() = default;

	// 交差時コールバック
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};