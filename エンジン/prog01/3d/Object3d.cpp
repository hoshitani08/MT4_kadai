#include "Object3d.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "ShaderManager.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// 静的メンバ変数の実体
ID3D12Device* Object3d::device = nullptr;
ID3D12GraphicsCommandList* Object3d::cmdList = nullptr;
Camera* Object3d::camera = nullptr;
LightGroup* Object3d::light = nullptr;

void Object3d::StaticInitialize(ID3D12Device* device, Camera* camera)
{
	// nullptrチェック
	assert(device);

	Object3d::device = device;
	Object3d::camera = camera;

	Model::StaticInitialize(device);
}

void Object3d::StaticFinalize()
{

}

void Object3d::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Object3d::cmdList == nullptr);

	// コマンドリストをセット
	Object3d::cmdList = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(ShaderManager::GetInstance()->GetPipelineState(L"Object"));
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(ShaderManager::GetInstance()->GetRootSignature(L"Object"));
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3d::PostDraw()
{
	// コマンドリストを解除
	Object3d::cmdList = nullptr;
}

std::unique_ptr<Object3d> Object3d::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Object3d* object3d = new Object3d();
	if (object3d == nullptr)
	{
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize())
	{
		assert(0);
		return nullptr;
	}

	if (model)
	{
		object3d->SetModel(model);
	}

	return std::unique_ptr<Object3d>(object3d);
}

Object3d::~Object3d()
{
	if (collider)
	{
		//コリジョンマネージャから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
	constBuffB0.Reset();
}

bool Object3d::Initialize()
{
	// nullptrチェック
	assert(device);
	//クラス名の文字列を取得
	name = typeid(*this).name();

	HRESULT result;

	// 定数バッファの生成
	result = device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);

	return true;
}

void Object3d::Update()
{
	HRESULT result;

	//行列の更新
	UpdateWorldMatrix();

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// 定数バッファへデータ転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->viewproj = matViewProjection;
	constMap0->world = matWorld;
	constMap0->cameraPos = cameraPos;
	constBuffB0->Unmap(0, nullptr);

	// 当たり判定更新
	if (collider)
	{
		collider->Update();
	}
}

void Object3d::Draw()
{
	// nullptrチェック
	assert(device);
	assert(Object3d::cmdList);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	//ライトの描画
	light->Draw(cmdList, 3);
	//モデル描画
	model->Draw(cmdList);
}

void Object3d::UpdateWorldMatrix()
{
	assert(camera);

	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	if (isBillboard && camera)
	{
		const XMMATRIX& matBillboard = camera->GetBillboardMatrix();

		matWorld = XMMatrixIdentity();
		matWorld *= matScale; // ワールド行列にスケーリングを反映
		matWorld *= matRot; // ワールド行列に回転を反映
		matWorld *= matBillboard;
		matWorld *= matTrans; // ワールド行列に平行移動を反映
	}
	else
	{
		matWorld = XMMatrixIdentity(); // 変形をリセット
		matWorld *= matScale; // ワールド行列にスケーリングを反映
		matWorld *= matRot; // ワールド行列に回転を反映
		matWorld *= matTrans; // ワールド行列に平行移動を反映
	}

	// 親オブジェクトがあれば
	if (parent != nullptr)
	{
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}
}

void Object3d::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	// コリジョンマネージャに追加
	CollisionManager::GetInstance()->AddCollider(collider);
	//行列の更新
	UpdateWorldMatrix();
	//コライダーを更新しておく
	collider->Update();
}

XMFLOAT3 Object3d::GetWorldPosition()
{
	XMFLOAT3 worldpos;

	worldpos.x = matWorld.r[3].m128_f32[0];
	worldpos.y = matWorld.r[3].m128_f32[1];
	worldpos.z = matWorld.r[3].m128_f32[2];

	return worldpos;
}