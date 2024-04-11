#ifndef _modelo
#define _modelo

//#define _XM_NO_INTRINSICS_

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>
#include <conio.h>
#include <vector>
#include <iostream>
#include "loadModel.h"

using namespace std;

class ModeloRR {
private:
	struct VertexComponent
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 UV;		
		D3DXVECTOR3 normal;		
	};

	struct VertexCollide
	{
		D3DXVECTOR3 pos;
	};

	struct vector3 {
		float x, y, z;
	};

	struct vector2 {
		float u, v;
	};

	ID3D11VertexShader* VertexShaderVS;
	ID3D11PixelShader* solidColorPS;

	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* colorMap;
	ID3D11ShaderResourceView* specMap;
	ID3D11SamplerState* colorMapSampler;

	ID3D11Buffer* viewCB;
	ID3D11Buffer* projCB;
	ID3D11Buffer* worldCB;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	ID3D11Buffer* cameraPosCB;
	XMFLOAT3 camPos;
	ID3D11Buffer* specForceCB;
	float specForce;

	int ancho, alto;
	int anchoTexTerr, altoTexTerr;
	float anchof, altof;
	float deltax, deltay;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;

	CObjParser m_ObjParser;
	
	float posX;
	float posZ;

public:
	ModeloRR(ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, char* ModelPath, WCHAR* colorTexturePath, WCHAR* specularTexturePath, float _posX, float _posZ)
	{
		//copiamos el device y el device context a la clase terreno
		d3dContext = D3DContext;
		d3dDevice = D3DDevice;	

		posX = _posX;
		posZ = _posZ;

		//aqui cargamos las texturas de alturas y el cesped
		CargaParametros(ModelPath, colorTexturePath, specularTexturePath);//L"Assets/Tent-Tower/tent_diffuse.jpg"
	}

	~ModeloRR()
	{
		//libera recursos
		
		UnloadContent();
	}

	float getPosX() {
		return this->posX;
	}
	
	float getPosZ() {
		return this->posZ;
	}

	bool CompileD3DShader(WCHAR* filePath, char* entry, char* shaderModel, ID3DBlob** buffer)
	{
		//forma de compilar el shader
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

	bool CargaParametros(char* ModelPath, WCHAR* diffuseTex, WCHAR* specularTex)
	{
		HRESULT d3dResult;
		
		ID3DBlob* vsBuffer = 0;

		//cargamos el shaders de vertices que esta contenido en el Shader.fx, note
		//que VS_Main es el nombre del vertex shader en el shader, vsBuffer contendra
		//al puntero del mismo
		bool compileResult = CompileD3DShader(L"Modelo.fx", "VS_Main", "vs_4_0", &vsBuffer);
		//en caso de no poder cargarse ahi muere la cosa
		if (compileResult == false)
		{
			return false;
		}

		//aloja al shader en la memeoria del gpu o el cpu
		d3dResult = d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(),
			vsBuffer->GetBufferSize(), 0, &VertexShaderVS);
		//en caso de falla sale
		if (FAILED(d3dResult))
		{

			if (vsBuffer)
				vsBuffer->Release();

			return false;
		}

		//lo nuevo, antes creabamos una estructura con la definicion del vertice, ahora creamos un mapa o layout
		//de como queremos construir al vertice, esto es la definicion
		D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },			
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

		d3dResult = d3dDevice->CreateInputLayout(solidColorLayout, totalLayoutElements,
			vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);

		vsBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		ID3DBlob* psBuffer = 0;
		// de los vertices pasamos al pixel shader, note que el nombre del shader es el mismo
		//ahora buscamos al pixel shader llamado PS_Main
		compileResult = CompileD3DShader(L"Modelo.fx", "PS_Main", "ps_4_0", &psBuffer);

		if (compileResult == false)
		{
			return false;
		}
		//ya compilado y sin error lo ponemos en la memoria
		d3dResult = d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(),
			psBuffer->GetBufferSize(), 0, &solidColorPS);

		psBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		//aqui va la carga del modelo con el metodo creadoModelPath
		m_ObjParser.LoadFile(ModelPath); //"Assets/Tent-Tower/Tienda-Top.obj"
		//proceso de guardar el buffer de vertices para su uso en el render
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(VertexObj) * m_ObjParser.m_nVertexCount;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = m_ObjParser.m_pVertex;

		d3dResult = d3dDevice->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);

		if (FAILED(d3dResult))
		{
			MessageBox(0, L"Error", L"Error al crear vertex buffer", MB_OK);
			return false;
		}
	
		
		//crea los accesos de las texturas para los shaders 
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, diffuseTex, 0, 0, &colorMap, 0);
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, specularTex, 0, 0, &specMap, 0);
		
		if (FAILED(d3dResult))
		{
			return false;
		}

		

		//aqui creamos el sampler
		D3D11_SAMPLER_DESC colorMapDesc;
		ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
		colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
		//con la estructura de descripion creamos el sampler
		d3dResult = d3dDevice->CreateSamplerState(&colorMapDesc, &colorMapSampler);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//creamos los buffers para el shader para poder pasarle las matrices
		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(D3DXMATRIX);
		constDesc.Usage = D3D11_USAGE_DEFAULT;
		//de vista
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &viewCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//de proyeccion
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &projCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//de mundo
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &worldCB);

		if (FAILED(d3dResult))
		{
			return false;
		}

		constDesc.ByteWidth = sizeof(XMFLOAT4);
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &cameraPosCB);

		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &specForceCB);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//posicion de la camara
		D3DXVECTOR3 eye = D3DXVECTOR3(0.0f, 100.0f, 200.0f);
		//a donde ve
		D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//crea la matriz de vista
		D3DXMatrixLookAtLH(&viewMatrix, &eye, &target, &up);
		//la de proyeccion
		D3DXMatrixPerspectiveFovLH(&projMatrix, D3DX_PI / 4.0, ancho / alto, 0.01f, 1000.0f);
		//las transpone para acelerar la multiplicacion
		D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
		D3DXMatrixTranspose(&projMatrix, &projMatrix);

		return true;
	}

	bool UnloadContent()
	{
		if (colorMapSampler)
			colorMapSampler->Release();
		if (colorMap)
			colorMap->Release();
		if (specMap)
			specMap->Release();
		if (VertexShaderVS)
			VertexShaderVS->Release();
		if (solidColorPS)
			solidColorPS->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (viewCB)
			viewCB->Release();
		if (projCB)
			projCB->Release();
		if (worldCB)
			worldCB->Release();

		if (cameraPosCB)
			cameraPosCB->Release();
		if (specForceCB)
			specForceCB->Release();
		

		colorMapSampler = 0;
		colorMap = 0;
		specMap = 0;
		VertexShaderVS = 0;
		solidColorPS = 0;
		inputLayout = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		viewCB = 0;
		projCB = 0;
		worldCB = 0;
		cameraPosCB = 0;
		specForceCB = 0;
	}

	void Update(float dt)
	{

	}

	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, float ypos, D3DXVECTOR3 posCam, float specForce, float rot, char angle, float scale)
	{
		static float rotation = 0.0f;
		rotation += 0.01;

		//paso de datos, es decir cuanto es el ancho de la estructura
		unsigned int stride = sizeof(VertexObj);
		unsigned int offset = 0;

		camPos.x = posCam.x;
		camPos.y = posCam.y;
		camPos.z = posCam.z;

		//define la estructura del vertice a traves de layout
		d3dContext->IASetInputLayout(inputLayout);

		//define con que buffer trabajara
		d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

		//define la forma de conexion de los vertices
		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Establece el vertex y pixel shader que utilizara
		d3dContext->VSSetShader(VertexShaderVS, 0, 0);
		d3dContext->PSSetShader(solidColorPS, 0, 0);
		//pasa lo sbuffers al shader
		d3dContext->PSSetShaderResources(0, 1, &colorMap);	
		d3dContext->PSSetShaderResources(1, 1, &specMap);

		d3dContext->PSSetSamplers(0, 1, &colorMapSampler);

		//mueve la camara
		D3DXMATRIX rotationMat;
		D3DXMatrixRotationYawPitchRoll(&rotationMat, 0.0f, 0.0f, 0.0f);
		D3DXMATRIX translationMat;
		D3DXMatrixTranslation(&translationMat, posX, ypos, posZ);
		if(angle == 'X')
			D3DXMatrixRotationX(&rotationMat, rot);
		else if (angle == 'Y')
			D3DXMatrixRotationY(&rotationMat, rot);
		else if (angle == 'Z')
			D3DXMatrixRotationZ(&rotationMat, rot);
		viewMatrix *= rotationMat;

		D3DXMATRIX scaleMat;
		D3DXMatrixScaling(&scaleMat, scale,scale,scale);

		D3DXMATRIX worldMat = rotationMat * scaleMat * translationMat;
		D3DXMatrixTranspose(&worldMat, &worldMat);
		//actualiza los buffers del shader
		d3dContext->UpdateSubresource(worldCB, 0, 0, &worldMat, 0, 0);
		d3dContext->UpdateSubresource(viewCB, 0, 0, &vista, 0, 0);
		d3dContext->UpdateSubresource(projCB, 0, 0, &proyeccion, 0, 0);
		d3dContext->UpdateSubresource(cameraPosCB, 0, 0, &camPos, 0, 0);
		d3dContext->UpdateSubresource(specForceCB, 0, 0, &specForce, 0, 0);
		//le pasa al shader los buffers
		d3dContext->VSSetConstantBuffers(0, 1, &worldCB);
		d3dContext->VSSetConstantBuffers(1, 1, &viewCB);
		d3dContext->VSSetConstantBuffers(2, 1, &projCB);
		d3dContext->VSSetConstantBuffers(3, 1, &cameraPosCB);
		d3dContext->VSSetConstantBuffers(4, 1, &specForceCB);
		//cantidad de trabajos
		
		d3dContext->Draw(m_ObjParser.m_nVertexCount, 0);


	}
};
#endif