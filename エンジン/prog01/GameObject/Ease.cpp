#include "Ease.h"

using namespace DirectX;

const float Ease::easeIn(const float& start, const float& end, const float t)
{
	return start * (1.0f - t * t) + end * (t * t);
}

const XMFLOAT2 Ease::easeIn(const XMFLOAT2& start, const XMFLOAT2& end, const float t)
{
	float x, y, z = 0;

	x = start.x * (1.0f - t * t) + end.x * (t * t);
	y = start.y * (1.0f - t * t) + end.y * (t * t);

	return XMFLOAT2({ x, y });
}

const XMFLOAT3 Ease::easeIn(const XMFLOAT3& start, const XMFLOAT3& end, const float t)
{
	float x, y, z = 0;

	x = start.x * (1.0f - t * t) + end.x * (t * t);
	y = start.y * (1.0f - t * t) + end.y * (t * t);
	z = start.z * (1.0f - t * t) + end.z * (t * t);

	return XMFLOAT3({ x, y, z });
}

const XMFLOAT4 Ease::easeIn(const XMFLOAT4& start, const XMFLOAT4& end, const float t)
{
	float x, y, z, w = 0;

	x = start.x * (1.0f - t * t) + end.x * (t * t);
	y = start.y * (1.0f - t * t) + end.y * (t * t);
	z = start.z * (1.0f - t * t) + end.z * (t * t);
	w = start.w * (1.0f - t * t) + end.w * (t * t);

	return XMFLOAT4({x, y, z, w});
}

const float Ease::easeOut(const float& start, const float& end, const float t)
{
	return start * (1.0f - (t * (2 - t))) + end * (t * (2 - t));
}

const XMFLOAT2 Ease::easeOut(const XMFLOAT2& start, const XMFLOAT2& end, const float t)
{
	float x, y = 0;

	x = start.x * (1.0f - (t * (2 - t))) + end.x * (t * (2 - t));
	y = start.y * (1.0f - (t * (2 - t))) + end.y * (t * (2 - t));

	return XMFLOAT2({ x, y });
}

const XMFLOAT3 Ease::easeOut(const XMFLOAT3& start, const XMFLOAT3& end, const float t)
{
	float x, y, z = 0;

	x = start.x * (1.0f - (t * (2 - t))) + end.x * (t * (2 - t));
	y = start.y * (1.0f - (t * (2 - t))) + end.y * (t * (2 - t));
	z = start.z * (1.0f - (t * (2 - t))) + end.z * (t * (2 - t));

	return XMFLOAT3({ x, y, z});
}

const XMFLOAT4 Ease::easeOut(const XMFLOAT4& start, const XMFLOAT4& end, const float t)
{
	float x, y, z, w = 0;

	x = start.x * (1.0f - (t * (2 - t))) + end.x * (t * (2 - t));
	y = start.y * (1.0f - (t * (2 - t))) + end.y * (t * (2 - t));
	z = start.z * (1.0f - (t * (2 - t))) + end.z * (t * (2 - t));
	w = start.w * (1.0f - (t * (2 - t))) + end.w * (t * (2 - t));

	return XMFLOAT4({ x, y, z, w});
}

const float Ease::easeInOut(const float& start, const float& end, const float t)
{
	return start * (1.0f - (t * t) * (3 - (2 * t))) + end * (t * t) * (3 - (2 * t));
}

const XMFLOAT2 Ease::easeInOut(const XMFLOAT2& start, const XMFLOAT2& end, const float t)
{
	float x, y = 0;

	x = start.x * (1.0f - (t * t) * (3 - (2 * t))) + end.x * (t * t) * (3 - (2 * t));
	y = start.y * (1.0f - (t * t) * (3 - (2 * t))) + end.y * (t * t) * (3 - (2 * t));

	return XMFLOAT2({ x, y });
}

const XMFLOAT3 Ease::easeInOut(const XMFLOAT3& start, const XMFLOAT3& end, const float t)
{
	float x, y, z = 0;

	x = start.x * (1.0f - (t * t) * (3 - (2 * t))) + end.x * (t * t) * (3 - (2 * t));
	y = start.y * (1.0f - (t * t) * (3 - (2 * t))) + end.y * (t * t) * (3 - (2 * t));
	z = start.z * (1.0f - (t * t) * (3 - (2 * t))) + end.z * (t * t) * (3 - (2 * t));

	return XMFLOAT3({ x, y, z });
}

const XMFLOAT4 Ease::easeInOut(const XMFLOAT4& start, const XMFLOAT4& end, const float t)
{
	float x, y, z, w = 0;

	x = start.x * (1.0f - (t * t) * (3 - (2 * t))) + end.x * (t * t) * (3 - (2 * t));
	y = start.y * (1.0f - (t * t) * (3 - (2 * t))) + end.y * (t * t) * (3 - (2 * t));
	z = start.z * (1.0f - (t * t) * (3 - (2 * t))) + end.z * (t * t) * (3 - (2 * t));
	w = start.w * (1.0f - (t * t) * (3 - (2 * t))) + end.w * (t * t) * (3 - (2 * t));

	return XMFLOAT4({ x, y, z, w });
}

const float Ease::lerp(const float& start, const float& end, const float t)
{
	return start * (1.0f - t) + end * t;
}

const XMFLOAT2 Ease::lerp(const XMFLOAT2& start, const XMFLOAT2& end, const float t)
{
	float x, y = 0;

	x = start.x * (1.0f - t) + end.x * t;
	y = start.y * (1.0f - t) + end.y * t;

	return XMFLOAT2({ x, y });
}

const XMFLOAT3 Ease::lerp(const XMFLOAT3& start, const XMFLOAT3& end, const float t)
{
	float x, y, z = 0;

	x = start.x * (1.0f - t) + end.x * t;
	y = start.y * (1.0f - t) + end.y * t;
	z = start.z * (1.0f - t) + end.z * t;

	return XMFLOAT3({ x, y, z });
}

const XMFLOAT4 Ease::lerp(const XMFLOAT4& start, const XMFLOAT4& end, const float t)
{
	float x, y, z, w = 0;

	x = start.x * (1.0f - t) + end.x * t;
	y = start.y * (1.0f - t) + end.y * t;
	z = start.z * (1.0f - t) + end.z * t;
	w = start.w * (1.0f - t) + end.w * t;

	return XMFLOAT4({ x, y, z, w });
}