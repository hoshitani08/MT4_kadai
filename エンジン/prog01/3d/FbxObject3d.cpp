#include "FbxObject3d.h"
#include "ShaderManager.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

// 静的メンバ変数の実体
ID3D12Device* FbxObject3d::device = nullptr;
Camera* FbxObject3d::camera = nullptr;
LightGroup* FbxObject3d::light = nullptr;

void FbxObject3d::StaticInitialize(ID3D12Device* device, Camera* camera)
{
	// nullptrチェック
	assert(device);

	FbxObject3d::device = device;
	FbxObject3d::camera = camera;
}

void FbxObject3d::StaticFinalize()
{
	
}

std::unique_ptr<FbxObject3d> FbxObject3d::Create(FbxModel* model, std::wstring HLSLfName, bool isAnimation)
{
	// 3Dオブジェクトのインスタンスを生成
	FbxObject3d* fbxObject3d = new FbxObject3d();
	if (fbxObject3d == nullptr)
	{
		return nullptr;
	}

	fbxObject3d->fName = HLSLfName;

	// 初期化
	if (!fbxObject3d->Initialize())
	{
		assert(0);
		return nullptr;
	}

	if (model)
	{
		fbxObject3d->SetModel(model);
	}

	if (isAnimation)
	{
		fbxObject3d->LoadAnimation();
	}

	return std::unique_ptr<FbxObject3d>(fbxObject3d);
}

FbxObject3d::~FbxObject3d()
{
	constBuffTransform.Reset();
	constBuffSkin.Reset();
}

bool FbxObject3d::Initialize()
{
	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);

	// 定数バッファ(スキン)の生成
	result = device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin)
	);

	//1フレーム分の時間を60FPSで設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	// 定数バッファ(スキン)へデータ転送
	ConstBufferDataSkin* constMatSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMatSkin);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMatSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	return true;
}

void FbxObject3d::Update()
{
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	// ビュープロジェクション行列
	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	// モデルのメッシュトランスフォーム
	const XMMATRIX& modelTransform = model->GetModelTransform();
	// カメラ座標
	const XMFLOAT3& cameraPos = camera->GetEye();

	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->viewproj = matViewProjection;
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBuffTransform->Unmap(0, nullptr);
	}

	//ボーン配列
	std::vector<FbxModel::Bone>& bones = model->GetBones();

	//アニメーション
	if (isPlay)
	{
		//1フレーム進める
		currentTime += frameTime;

		//最後まで再生したら先頭に戻す
		if (currentTime > endTime)
		{
			currentTime = startTime;
		}
	}

	// 定数バッファ(スキン)へデータ転送
	ConstBufferDataSkin* constMatSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMatSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		//今の姿勢行列
		XMMATRIX matCurrentPose;
		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
		//合成してスキニング行列に
		constMatSkin->bones[i] = model->GetModelTransform() * bones[i].invInitialPose * matCurrentPose * XMMatrixInverse(nullptr, model->GetModelTransform());
	}
	constBuffSkin->Unmap(0, nullptr);
}

void FbxObject3d::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// モデルの割り当てがなければ描画しない
	if (model == nullptr)
	{
		return;
	}

	// パイプラインステートの設定
	cmdList->SetPipelineState(ShaderManager::GetInstance()->GetPipelineState(fName));
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(ShaderManager::GetInstance()->GetRootSignature(fName));
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());

	//ライトの描画
	light->Draw(cmdList, 4);

	// モデル描画
	model->Draw(cmdList);
}

void FbxObject3d::LoadAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();
	//アニメーションカウント
	int sceneCount = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < sceneCount; i++)
	{
		//仮データ
		Animation tempData;
		//i番のアニメーション取得
		tempData.animstack = fbxScene->GetSrcObject<FbxAnimStack>(i);
		//アニメーションの名前を取得
		const char* animstackname = tempData.animstack->GetName();
		//アニメーションの時間情報
		tempData.takeinfo = fbxScene->GetTakeInfo(animstackname);
		//仮データを実データに入れる
		animationData.push_back(tempData);
	}
}

void FbxObject3d::PlayAnimation(int animationNumber)
{
	FbxScene* fbxScene = model->GetFbxScene();
	//アニメーションの変更
	fbxScene->SetCurrentAnimationStack(animationData[animationNumber].animstack);
	//開始時間取得
	startTime = animationData[animationNumber].takeinfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	endTime = animationData[animationNumber].takeinfo->mLocalTimeSpan.GetStop();
	//開始時間に合わせる
	currentTime = startTime;
	//再生中状態にする
	isPlay = true;
}