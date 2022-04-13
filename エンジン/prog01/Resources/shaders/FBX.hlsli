//ボーンの最大数
static const int MAX_BONES = 32;

cbuffer cbuff0 : register(b0)
{
	matrix viewproj; //ビュープロジェクション行列
	matrix world; //ワールド行列
	float3 cameraPos; //カメラ座標（ワールド座標）
};

//ボーンのスキニング行列が入る
cbuffer skinning:register(b3)
{
	matrix matSkinning[MAX_BONES];
};

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos : POSITION;//位置
	float3 normal : NORMAL;//頂点法線
	float2 uv : TEXCOORD;//テクスチャー座標
	float3 tangent : TANGENT; //U軸
	float3 biNormal : BINORMAL; //V軸
	uint4 boneIndices : BONEINDICES;//ボーン番号
	float4 boneWeights : BONEWEIGHTS;//ボーンのスキンウェイト
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; //システム用頂点座標
	float3 worldpos : POS; // ワールド座標
	float3 normal :NORMAL; //法線
	float2 uv  :TEXCOORD; //uv値
	float3 tangent : TANGENT; //U軸
	float3 biNormal : BINORMAL; //V軸
};

//スキニング後の頂点・法線が入る
struct SkinOutput
{
	float4 pos;
	float3 normal;
	float3 tangent;
	float3 biNormal;
};

// 平行光源の数
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
	float3 lightv;    // ライトへの方向の単位ベクトル
	float3 lightcolor;    // ライトの色(RGB)
	uint active;
};

// 点光源の数
static const int POINTLIGHT_NUM = 3;

struct PointLight
{
	float3 lightpos;    // ライト座標
	float3 lightcolor;  // ライトの色(RGB)
	float3 lightatten;  //ライト距離減衰係数
	uint active;
};

// スポットライトの数
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
	float3 lightv;      // ライトの光線方向の逆ベクトル（単位ベクトル）
	float3 lightpos;    // ライト座標
	float3 lightcolor;  // ライトの色(RGB)
	float3 lightatten;  // ライト距離減衰係数
	float2 lightfactoranglecos; // ライト減衰角度のコサイン
	uint active;
};

// 丸影の数
static const int CIRCLESHADOW_NUM = 1;

struct CircleShadow
{
	float3 dir;          // 投影方向の逆ベクトル（単位ベクトル）
	float3 casterPos;    // キャスター座標
	float  distanceCasterLight; // キャスターとライトの距離
	float3 atten;        // 距離減衰係数
	float2 factorAngleCos; // 減衰角度のコサイン
	uint active;
};

cbuffer cbuff2 : register(b2)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}