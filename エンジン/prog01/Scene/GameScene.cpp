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

	// FBX�I�u�W�F�N�g����
	object3d = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));
	object3d2 = Object3d::Create(ObjFactory::GetInstance()->GetModel("sphere"));

	object3d->SetPosition({ 10,0,0 });
	object3d2->SetPosition({ -10,0,0 });

	// �J���������_���Z�b�g
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-150 });
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

	// ��������
	if (!onGround)
	{
		//��C��R�̔��萔
		const float fallVYMinX = -5.0f;
		// �ړ�
		XMFLOAT3 position = object3d->GetPosition();
		position.x += fallV.m128_f32[0];
		position.y += gravity;

		// �ړ�2
		XMFLOAT3 position2 = object3d2->GetPosition();
		position2.x += fallV2.m128_f32[0];
		position2.y += gravity;

		if (position.y > 0 || position2.y > 0)
		{
			if (fallV.m128_f32[0] != 0.0f || fallV2.m128_f32[0] != 0.0f)
			{
				fallV.m128_f32[0] = max(fallV.m128_f32[0] * damp, fallVYMinX);
				fallV2.m128_f32[0] = max(fallV2.m128_f32[0] * damp, fallVYMinX);
			}
			position.y = 0;    //�{�[���͉�ʂ̊O�ɊO��Ȃ�
			position2.y = 0;    //�{�[���͉�ʂ̊O�ɊO��Ȃ�
		}

		object3d->SetPosition(position);
		object3d2->SetPosition(position2);

		Sphere p1 = { position.x, position.y, position.z, 1 };
		p1.radius = 1.15f;
		Sphere p2 = { position2.x, position2.y, position2.z, 1 };
		p2.radius = 1.15f;

		if (Collision::CheckSphere2Sphere(p1, p2))
		{
			fallV.m128_f32[0] = fallV.m128_f32[0] * 2.0f;
			fallV2.m128_f32[0] = fallV2.m128_f32[0] * 2.0f;
		}
	}
	else if (input->TriggerKey(DIK_SPACE))
	{
		onGround = false;
		const float jumpVYFistX = 3.0f; //�E��������
		fallV = { -jumpVYFistX, 0, 0, 0 };
		fallV2 = { jumpVYFistX, 0, 0, 0 };
	}

	object3d->Update();
	object3d2->Update();
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
	object3d->Draw();
	object3d2->Draw();
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