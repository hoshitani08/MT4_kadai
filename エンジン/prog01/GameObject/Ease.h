#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

class Ease
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public:
	static const float easeIn(const float& start, const float& end, const float t);
	static const XMFLOAT2 easeIn(const XMFLOAT2& start, const XMFLOAT2& end, const float t);
	static const XMFLOAT3 easeIn(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
	static const XMFLOAT4 easeIn(const XMFLOAT4& start, const XMFLOAT4& end, const float t);

	static const float easeOut(const float& start, const float& end, const float t);
	static const XMFLOAT2 easeOut(const XMFLOAT2& start, const XMFLOAT2& end, const float t);
	static const XMFLOAT3 easeOut(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
	static const XMFLOAT4 easeOut(const XMFLOAT4& start, const XMFLOAT4& end, const float t);

	static const float easeInOut(const float& start, const float& end, const float t);
	static const XMFLOAT2 easeInOut(const XMFLOAT2& start, const XMFLOAT2& end, const float t);
	static const XMFLOAT3 easeInOut(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
	static const XMFLOAT4 easeInOut(const XMFLOAT4& start, const XMFLOAT4& end, const float t);

	static const float lerp(const float& start, const float& end, const float t);
	static const XMFLOAT2 lerp(const XMFLOAT2& start, const XMFLOAT2& end, const float t);
	static const XMFLOAT3 lerp(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
	static const XMFLOAT4 lerp(const XMFLOAT4& start, const XMFLOAT4& end, const float t);
};