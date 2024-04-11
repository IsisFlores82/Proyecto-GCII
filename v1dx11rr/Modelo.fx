Texture2D colorMap : register(t0);
Texture2D specMap : register(t1);

SamplerState colorSampler : register(s0);

cbuffer cbChangerEveryFrame : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbNeverChanges : register(b1)
{
	matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
	matrix projMatrix;
};

cbuffer cbChangesOccasionally : register(b3)
{
	float3 cameraPos;
};

cbuffer cbChangesOccasionally : register(b4)
{
	float specForce;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;	
	float3 normal : NORMAL0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;	
	float3 normal : TEXCOORD1;
	float3 campos : TEXCOORD2;
	float specForce : TEXCOORD3;
};

PS_Input VS_Main(VS_Input vertex)
{
	
	float4 worldPosition;

		PS_Input vsOut = (PS_Input)0;

		vsOut.pos = mul(vertex.pos, worldMatrix);
		vsOut.pos = mul(vsOut.pos, viewMatrix);
		vsOut.pos = mul(vsOut.pos, projMatrix);

		vsOut.tex0 = vertex.tex0;
		vsOut.normal = normalize(mul(vertex.normal, worldMatrix));

		worldPosition = mul(vertex.pos, worldMatrix);

		vsOut.campos = cameraPos.xyz - worldPosition.xyz;

		vsOut.campos = normalize(vsOut.campos);

		vsOut.specForce = specForce;

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float3 reflection;
	float4 specular;
	float4 specularMap;
	float4 finalSpec;

	textureColor = colorMap.Sample(colorSampler, pix.tex0);
	color = float4(0.2, 0.2, 0.2, 1);// ambient color

	specular = float4(0.0, 0.0, 0.0, 1.0); //specular color
	specularMap = specMap.Sample(colorSampler, pix.tex0);

	lightDir = -(float3(0.5f, -1.0f, 0.0f)); // lightDirection

	lightIntensity = saturate(dot(pix.normal, lightDir));

	if (lightIntensity > 0) {
		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		color += (float4(1.0f, 1.f, 1.f, 1.0f)/*diffuse color*/ * lightIntensity);

		// Saturate the ambient and diffuse color.
		color = saturate(color);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		reflection = normalize(2 * lightIntensity * pix.normal - lightDir);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, pix.campos)), pix.specForce);
		finalSpec = specular * specularMap;
	}

	color = color * textureColor;

	color = saturate(color + finalSpec);

	return color;
}