#ifndef _skydome
#define _skydome

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>

struct SkyComponent
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 UV;
};

struct MatrixType
{
	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;
	D3DXVECTOR4 valores;
};

class SkyDome
{
public:
	float gamepad;
private:
	ID3D11VertexShader* solidColorVS;
	ID3D11PixelShader* solidColorPS;

	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* textura;
	ID3D11SamplerState* texSampler;

	ID3D11Buffer* matrixBufferCB;
	MatrixType* matrices;

	SkyComponent* vertices;
	int slices, stacks, cantIndex;
	short* indices;
	float radio;
	ID3D11Device** d3dDevice;
	ID3D11DeviceContext** d3dContext;

public:
	SkyDome(int slices, int stacks, float radio, ID3D11Device** d3dDevice,
		ID3D11DeviceContext** d3dContext, WCHAR* diffuseTex)
	{
		this->slices = slices;
		this->stacks = stacks;
		this->radio = radio;
		indices = NULL;
		vertices = NULL;
		this->d3dDevice = d3dDevice;
		this->d3dContext = d3dContext;
		LoadContent(diffuseTex);
	}

	~SkyDome()
	{
		UnloadContent();
	}


	bool CompileD3DShader(WCHAR* filePath, char* entry, char* shaderModel, ID3DBlob** buffer)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* errorBuffer = 0;
		HRESULT result;

		result = D3DX11CompileFromFile(filePath, 0, 0, entry, shaderModel, shaderFlags,
			0, 0, buffer, &errorBuffer, 0);
		if (FAILED(result))
		{
			if (errorBuffer != 0)
			{
				OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
				errorBuffer->Release();
			}
			return false;
		}

		if (errorBuffer != 0)
			errorBuffer->Release();

		return true;
	}

	bool LoadContent(WCHAR* diffuseTex)
	{
		HRESULT d3dResult;

		ID3DBlob* vsBuffer = 0;

		bool compileResult = CompileD3DShader(L"Skydome.fx", "VS_Main", "vs_4_0", &vsBuffer);

		if (compileResult == false)
		{
			return false;
		}


		d3dResult = (*d3dDevice)->CreateVertexShader(vsBuffer->GetBufferPointer(),
			vsBuffer->GetBufferSize(), 0, &solidColorVS);

		if (FAILED(d3dResult))
		{

			if (vsBuffer)
				vsBuffer->Release();

			return false;
		}

		D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

		d3dResult = (*d3dDevice)->CreateInputLayout(solidColorLayout, totalLayoutElements,
			vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);

		vsBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		ID3DBlob* psBuffer = 0;

		compileResult = CompileD3DShader(L"Skydome.fx", "PS_Main", "ps_4_0", &psBuffer);

		if (compileResult == false)
		{
			return false;
		}

		d3dResult = (*d3dDevice)->CreatePixelShader(psBuffer->GetBufferPointer(),
			psBuffer->GetBufferSize(), 0, &solidColorPS);

		psBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		int cantVert = slices * stacks;
		vertices = new SkyComponent[cantVert];
		float dx = slices - 1.0f;
		float dy = stacks - 1.0f;
		float maxY = 0;

		for (int x = 0; x < slices; x++)
		{
			for (int y = 0; y < stacks; y++)
			{
				float u = (float)(((double)((dx - x)*0.5f) / dx) *
					sin(D3DX_PI * y * 2.0f / dx)) + 0.5f;
				float v = (float)(((double)((dy - x)*0.5f) / dy) *
					cos(D3DX_PI * y * 2.0 / dy)) + 0.5f;

				int indiceArreglo = x * slices + y;
				vertices[indiceArreglo].pos = D3DXVECTOR3((float)(radio*cos(((double)x / dx)* D3DX_PI / 2) *
					cos(2.0 * D3DX_PI * (double)y / dx)), (float)(radio*sin(((double)x / dx) * D3DX_PI / 2)),
					(float)(radio*cos(((double)x / dy) * D3DX_PI / 2)*sin(2.0f * D3DX_PI * (double)y / dy)));
				vertices[indiceArreglo].UV = D3DXVECTOR2(u, v);
			}
		}
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(SkyComponent) * cantVert;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;

		d3dResult = (*d3dDevice)->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);

		if (FAILED(d3dResult))
		{
			MessageBox(0, L"Error", L"Error al crear vertex buffer", MB_OK);
			return false;
		}
		delete vertices;

		creaIndices();

		d3dResult = D3DX11CreateShaderResourceViewFromFile((*d3dDevice), diffuseTex, 0, 0, &textura, 0);

		if (FAILED(d3dResult))
		{
			return false;
		}

		D3D11_SAMPLER_DESC colorMapDesc;
		ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
		colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		colorMapDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		colorMapDesc.MaxAnisotropy = 8;
		colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

		d3dResult = (*d3dDevice)->CreateSamplerState(&colorMapDesc, &texSampler);

		if (FAILED(d3dResult))
		{
			return false;
		}


		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(MatrixType);
		constDesc.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = (*d3dDevice)->CreateBuffer(&constDesc, 0, &matrixBufferCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		matrices = new MatrixType;

		return true;
	}

	bool UnloadContent()
	{
		if (texSampler)
			texSampler->Release();
		if (textura)
			textura->Release();
		if (solidColorVS)
			solidColorVS->Release();
		if (solidColorPS)
			solidColorPS->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();
		if (matrixBufferCB)
			matrixBufferCB->Release();

		texSampler = 0;
		textura = 0;
		solidColorVS = 0;
		solidColorPS = 0;
		inputLayout = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		matrixBufferCB = 0;

		return true;
	}

	void Update(D3DXMATRIX view, D3DXMATRIX projection)
	{
		matrices->viewMatrix = view;
		matrices->projMatrix = projection;
	}

	void Render(D3DXVECTOR3 trans)
	{
		if (d3dContext == 0)
			return;

		unsigned int stride = sizeof(SkyComponent);
		unsigned int offset = 0;

		(*d3dContext)->IASetInputLayout(inputLayout);
		(*d3dContext)->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		(*d3dContext)->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
		(*d3dContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		(*d3dContext)->VSSetShader(solidColorVS, 0, 0);
		(*d3dContext)->PSSetShader(solidColorPS, 0, 0);
		(*d3dContext)->PSSetShaderResources(0, 1, &textura);
		(*d3dContext)->PSSetSamplers(0, 1, &texSampler);

		D3DXMATRIX worldMat;
		D3DXMatrixTranslation(&worldMat, trans.x, trans.y - 50.0f, trans.z);
		D3DXMatrixTranspose(&worldMat, &worldMat);
		matrices->worldMatrix = worldMat;

		(*d3dContext)->UpdateSubresource(matrixBufferCB, 0, 0, matrices, sizeof(MatrixType), 0);
		(*d3dContext)->VSSetConstantBuffers(0, 1, &matrixBufferCB);

		(*d3dContext)->DrawIndexed(cantIndex, 0, 0);
	}

private:
	void creaIndices()
	{
		cantIndex = (slices - 1)*(stacks - 1) * 6;
		indices = new short[cantIndex];
		int counter = 0;
		for (int i = 0; i < slices - 1; i++)
		{
			for (int j = 0; j < stacks - 1; j++)
			{
				int lowerLeft = j + i * stacks;
				int lowerRight = (j + 1) + i * stacks;
				int topLeft = j + (i + 1)*stacks;
				int topRight = (j + 1) + (i + 1)*stacks;

				indices[counter++] = lowerLeft;
				indices[counter++] = lowerRight;
				indices[counter++] = topLeft;

				indices[counter++] = lowerRight;
				indices[counter++] = topRight;
				indices[counter++] = topLeft;
			}
		}
		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_DEFAULT;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(short) * cantIndex;
		indexDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = indices;

		HRESULT d3dResult = (*d3dDevice)->CreateBuffer(&indexDesc, &resourceData, &indexBuffer);

		if (FAILED(d3dResult))
		{
			return;
		}
		delete indices;
	}
};
#endif