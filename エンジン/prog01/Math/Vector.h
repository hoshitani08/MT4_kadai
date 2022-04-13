#pragma once
#include <string>
#include <cstdlib>
#include <vector>
#include <complex>

#include <DirectXMath.h>

class Vector3 : public DirectX::XMFLOAT3
{
public:
	Vector3();
	Vector3(float x, float y, float z);

	float length() const;
	Vector3& normalize();
	float dot(const Vector3& v) const;
	Vector3 cross(const Vector3& v) const;

	Vector3 operator+() const;
	Vector3 operator-() const;

	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);