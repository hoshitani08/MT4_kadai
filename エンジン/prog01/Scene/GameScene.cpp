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
	sprite1 = Sprite::Create(2, { 0.0f,0.0f });
	sprite1->SetSize({ 100.0f,100.0f });
	sprite1->SetPosition({ 1280.0f / 2 - 60, 720.0f / 2 - 60 });

	sprite2 = Sprite::Create(3, { 0.0f,0.0f });
	sprite2->SetSize({ 1.0f,200.0f });
	sprite2->SetPosition({ 450.0f,250.0f });

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

	//�A�j���[�V����

	// �J���������_���Z�b�g
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,1,-500 });
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

	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_A) | input->PushKey(DIK_D))
	{
		XMFLOAT2 pos = sprite2->GetPosition();

		if (input->PushKey(DIK_W))
		{
			pos.y -= 2.0f;
		}
		else if (input->PushKey(DIK_S))
		{
			pos.y += 2.0f;
		}

		if (input->PushKey(DIK_A))
		{
			pos.x -= 2.0f;
		}
		else if (input->PushKey(DIK_D))
		{
			pos.x += 2.0f;
		}
		sprite2->SetPosition(pos);
	}

	XMFLOAT2 spos = sprite1->GetPosition();
	XMFLOAT2 bpos = sprite2->GetPosition();

	/*float d = ((bpos.x + 0.5f) - (bpos.x + 0.5f)) * (spos.y - bpos.y);
	float e = ((bpos.y + 200) - bpos.y) * (spos.x - (bpos.x + 0.5f));

	float a = -(d - e);*/

	Sphere a;
	a.center = { spos.x + 50, spos.y + 50, 0 };
	a.radius = 50;
	
	Box b;
	b.center = { bpos.x + 0.5f, bpos.y + 100, 0 };
	b.scale = { 1,150,1 };

	if (Collision::CheckSphere2Box(a,b))
	{
		sprite1->SetColor({1,0,0,1});
	}
	else
	{
		sprite1->SetColor({ 1,1,1,1 });
	}

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
	sprite2->Draw();
	sprite1->Draw();
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion �w�i�X�v���C�g�`��
#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�N�W�F�N�g�̕`��
	Object3d::PreDraw(cmdList);
	
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