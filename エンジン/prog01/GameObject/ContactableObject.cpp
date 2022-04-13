#include "ContactableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

ContactableObject* ContactableObject::Create(Model* model)
{
	// オブジェクトのインスタンスを生成
	ContactableObject* instance = new ContactableObject();
	if (instance == nullptr)
	{
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(model))
	{
		delete instance;
		assert(0);
	}

	return instance;
}

bool ContactableObject::Initialize(Model* model)
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	SetModel(model);

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(model);
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;
}