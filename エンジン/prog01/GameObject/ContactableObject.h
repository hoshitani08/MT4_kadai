#pragma once

#include "Object3d.h"

class ContactableObject : public Object3d
{
public:
	// オブジェクト生成
	static ContactableObject* Create(Model* model = nullptr);
	// 初期化
	bool Initialize(Model* model);
};