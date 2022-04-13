#include "FBX.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
// 法線マップ
Texture2D<float4> g_normalMap : register(t1);
// メタリックスムースマップ。rにメタリック、aにスムース
Texture2D<float4> g_metallicSmoothMap : register(t2);

SamplerState smp : register(s0);      //0番スロットに設定されたサンプラー

static const float PI = 3.1415926f;

float3 GetNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
	float3 binSpaceNormal = g_normalMap.SampleLevel(smp, uv, 0.0f).xyz;
	binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

	float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

	return newNormal;
}

// ベックマン分布を計算する
float Beckmann(float m, float t)
{
	float t2 = t * t;
	float t4 = t * t * t * t;
	float m2 = m * m;
	float D = 1.0f / (4.0f * m2 * t4);
	D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
	return D;
}

// フレネルを計算。Schlick近似を使用
float SpcFresnel(float f0, float u)
{
	// from Schlick
	return f0 + (1 - f0) * pow(1 - u, 5);
}

/// <summary>
/// Cook-Torranceモデルの鏡面反射を計算
/// </summary>
/// <param name="L">光源に向かうベクトル</param>
/// <param name="V">視点に向かうベクトル</param>
/// <param name="N">法線ベクトル</param>
/// <param name="metallic">金属度</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float metallic)
{
	float microfacet = 0.76f;

	// 金属度を垂直入射の時のフレネル反射率として扱う
	// 金属度が高いほどフレネル反射は大きくなる
	float f0 = metallic;

	// ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
	float3 H = normalize(L + V);

	// 各種ベクトルがどれくらい似ているかを内積を利用して求める
	float NdotH = saturate(dot(N, H));
	float VdotH = saturate(dot(V, H));
	float NdotL = saturate(dot(N, L));
	float NdotV = saturate(dot(N, V));

	// D項をベックマン分布を用いて計算する
	float D = Beckmann(microfacet, NdotH);

	// F項をSchlick近似を用いて計算する
	float F = SpcFresnel(f0, VdotH);

	// G項を求める
	float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

	// m項を求める
	float m = PI * NdotV * NdotH;

	// ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
	return max(F * D * G / m, 0.0);
}

/// <summary>
/// フレネル反射を考慮した拡散反射を計算
/// </summary>
/// <remark>
/// この関数はフレネル反射を考慮した拡散反射率を計算します
/// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
/// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
/// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります
///
/// </remark>
/// <param name="N">法線</param>
/// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
/// <param name="V">視線に向かうベクトル。</param>
/// <param name="roughness">粗さ。0～1の範囲。</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
	// ディズニーベースのフレネル反射による拡散反射を真面目に実装する。
	// 光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
	float3 H = normalize(L + V);

	// 粗さは0.5で固定。
	float roughness = 0.5f;

	float energyBias = lerp(0.0f, 0.5f, roughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

	// 光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める
	float dotLH = saturate(dot(L, H));

	// 光源に向かうベクトルとハーフベクトル、
	// 光が平行に入射したときの拡散反射量を求めている
	float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

	// 法線と光源に向かうベクトルwを利用して拡散反射率を求める
	float dotNL = saturate(dot(N, L));
	float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));

	// 法線と視点に向かうベクトルを利用して拡散反射率を求める
	float dotNV = saturate(dot(N, V));
	float FV = (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

	// 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
	// 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
	return (FL * FV * energyFactor);
}

float4 main(VSOutput input) : SV_TARGET
{
	// 法線を計算
	float3 normal = GetNormal(input.normal, input.tangent, input.biNormal, input.uv);

	// 各種マップをサンプリングする
	// アルベドカラー（拡散反射光）
	float4 albedoColor = tex.Sample(smp, input.uv);

	// スペキュラカラーはアルベドカラーと同じにする
	float3 specColor = albedoColor;

	// 金属度
	float metallic = g_metallicSmoothMap.Sample(smp, input.uv).r;

	// 滑らかさ
	float smooth = g_metallicSmoothMap.Sample(smp, input.uv).a;

	// 視線に向かって伸びるベクトルを計算する
	float3 toEye = normalize(cameraPos - input.worldpos);

	float3 lig = 0;
	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (!dirLights[i].active)
		{
			continue;
		}

		// フレネル反射を考慮した拡散反射を計算
		float diffuseFromFresnel = CalcDiffuseFromFresnel(
			normal, dirLights[i].lightv, toEye);

		// 正規化Lambert拡散反射を求める
		float NdotL = saturate(dot(normal, dirLights[i].lightv));
		float3 lambertDiffuse = dirLights[i].lightcolor * NdotL / PI;

		// 最終的な拡散反射光を計算する
		float3 diffuse = albedoColor * diffuseFromFresnel * lambertDiffuse;

		// Cook-Torranceモデルの鏡面反射率を計算する
		float3 spec = CookTorranceSpecular(
			dirLights[i].lightv, toEye, normal, smooth)
			* dirLights[i].lightcolor;

		// 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
		// スペキュラカラーの強さを鏡面反射率として扱う
		spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);

		// 滑らかさが高ければ、拡散反射は弱くなる
		lig += diffuse * (1.0f - smooth) + spec;
	}
	float3 a = { 0.4f, 0.4f, 0.4f };

	// 環境光による底上げ
	lig += a * albedoColor;

	float4 finalColor = 1.0f;
	finalColor.xyz = lig;
	return finalColor;
}