#pragma once

#include "BaseScene.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Model.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "CollisionPrimitive.h"
#include "Camera.h"
#include "LightGroup.h"
#include "FbxObject3d.h"

#include <DirectXMath.h>
#include <vector>
#include <memory>

class CollisionManager;
class Player;
class ContactableObject;

class GameScene : public BaseScene
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;

public: // �����o�֐�
	// �f�X�g���N�^
	~GameScene();
	// ������
	void Initialize() override;
	//�I��
	void Finalize() override;
	// ���t���[������
	void Update() override;
	// �`��
	void Draw() override;

private: // �����o�ϐ�
	//�J����
	std::unique_ptr<Camera> camera;
	//�X�v���C�g
	std::unique_ptr<Sprite> sprite;
	//�p�[�e�B�N��
	std::unique_ptr<ParticleManager> particleMan;
	//�I�u�W�F�N�g
	std::unique_ptr<Object3d> fbxObject3d;
	std::unique_ptr<Object3d> fbxObject3d2;

	//���C�g
	std::unique_ptr<LightGroup> light;
	//�������������l
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowPos[3] = { 1,2,0 };
	float circleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float circleShadowFactorAngle[2] = { 0.0f, 0.5f };

	float fighterPos[3] = { 1, 0.0f, 0 };
	//�����蔻��
	CollisionManager* collisionManager = nullptr;

	bool flag = false;

	float y;             //�I�u�W�F�N�g��y���W
	float mass = 10.1;   //�I�u�W�F�N�g�̎���
	float k = 0.3;       //�΂˂̋����i�΂˒萔�j
	float damp = 0.97;   //���C�ɂ�錸����
	float velY = 0.0;    //y�����̃X�s�[�h
	float accel = 0;     //�����x
	float force = 0;     //�΂˂̔����� 
	float originY = 20;       //��_
};