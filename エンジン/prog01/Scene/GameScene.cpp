#include "GameScene.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "ContactableObject.h"

#include "SceneManager.h"
#include "FbxFactory.h"
#include "ObjFactory.h"

#include "DirectXCommon.h"
#include "DebugText.h"
#include "Audio.h"
#include "Input.h"
#include "Ease.h"

using namespace DirectX;

GameScene::~GameScene()
{
	Finalize();
}

void GameScene::Initialize()
{
	collisionManager = CollisionManager::GetInstance();

	// �J��������
	camera = std::make_unique<Camera>(WinApp::window_width, WinApp::window_height);

	// 3D�I�u�W�F�N�g�ɃJ�������Z�b�g
	Object3d::SetCamera(camera.get());
	// FBX�I�u�W�F�N�g�ɃJ�������Z�b�g
	FbxObject3d::SetCamera(camera.get());

	// �w�i�X�v���C�g����
	sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ 100.0f,100.0f });
	sprite->SetPosition({ 100.0f,100.0f });

	// �p�[�e�B�N���}�l�[�W������
	particleMan = ParticleManager::Create(DirectXCommon::GetInstance()->GetDevice(), camera.get());

	//���C�g����
	light = LightGroup::Create();
	//�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light.get());
	FbxObject3d::SetLight(light.get());
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, false);
	light->SetDirLightActive(2, false);
	light->SetPointLightActive(0, false);
	light->SetPointLightActive(1, false);
	light->SetPointLightActive(2, false);
	light->SetCircleShadowActive(0, true);

	light->SetDirLightDir(0, { 0,0,1,0 });

	// 3D�I�u�W�F�N�g����
	object3d1 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	object3d2 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	object3d3 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));

	object3d1->SetPosition({ -45, 10, 0 });
	object3d2->SetPosition({ -45, 0, 0 });
	object3d3->SetPosition({ -45, -10, 0 });

	for (int i = 0; i < sab1.size(); i++)
	{
		sab1[i] = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
		sab2[i] = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
		sab3[i] = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));

		sab1[i]->SetPosition({ 0, 100, 0 });
		sab2[i]->SetPosition({ 0, 100, 0 });
		sab3[i]->SetPosition({ 0, 100, 0 });
	}

	// FBX�I�u�W�F�N�g����

	// �J���������_���Z�b�g
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-50 });
}

void GameScene::Finalize()
{
}

void GameScene::Update()
{
	Input* input = Input::GetInstance();
	light->Update();
	camera->Update();
	particleMan->Update();

	if (input->TriggerKey(DIK_SPACE) && !easeFlag)
	{
		easeFlag = true;
	}

	if (easeFlag)
	{
		float timeRate = 0.0f;
		int countNum = 60;
		timeRate = timer / countNum;
		timer++;

		object3d1->SetPosition(Ease::easeIn({ -45, 10, 0 }, { 45, 10, 0 }, timeRate));
		object3d2->SetPosition(Ease::easeOut({ -45, 0, 0 }, { 45, 0, 0 }, timeRate));
		object3d3->SetPosition(Ease::easeInOut({ -45, -10, 0 }, { 45, -10, 0 }, timeRate));

		if (timer >= 6 * count && count < 10)
		{
			sab1[count - 1]->SetPosition(object3d1->GetPosition());
			sab2[count - 1]->SetPosition(object3d2->GetPosition());
			sab3[count - 1]->SetPosition(object3d3->GetPosition());
			count++;
		}

		if (timer > countNum)
		{
			timer = 0;
			easeFlag = false;
		}
	}

	for (int i = 0; i < sab1.size(); i++)
	{
		sab1[i]->Update();
		sab2[i]->Update();
		sab3[i]->Update();
	}
	object3d1->Update();
	object3d2->Update();
	object3d3->Update();
	// �S�Ă̏Փ˂��`�F�b�N
	collisionManager->CheckAllCollisions();
}

void GameScene::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();
#pragma region �w�i�X�v���C�g�`��
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �w�i�X�v���C�g�`��
	
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion �w�i�X�v���C�g�`��
#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�N�W�F�N�g�̕`��
	Object3d::PreDraw(cmdList);
	for (int i = 0; i < sab1.size(); i++)
	{
		sab1[i]->Draw();
		sab2[i]->Draw();
		sab3[i]->Draw();
	}
	object3d1->Draw();
	object3d2->Draw();
	object3d3->Draw();
	Object3d::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��
#pragma region 3D�I�u�W�F�N�g(FBX)�`��

#pragma endregion 3D�I�u�W�F�N�g(FBX)�`��
#pragma region �p�[�e�B�N��
	// �p�[�e�B�N���̕`��
	particleMan->Draw(cmdList);
#pragma endregion �p�[�e�B�N��
#pragma region �O�i�X�v���C�g�`��
	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(cmdList);
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion �O�i�X�v���C�g�`��
}