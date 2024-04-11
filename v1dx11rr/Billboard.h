#ifndef _BillBoard_
#define _BillBoard_

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>

struct vector2 {
	float u, v;
};

class BillboardRR {
public:
	struct VertexComponent
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 UV;
	};

	struct VertexCollide
	{
		D3DXVECTOR3 pos;
	};

	

	ID3D11VertexShader* VertexShaderVS;
	ID3D11PixelShader* solidColorPS;

	ID3D11InputLayout* inputLay;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* colorMap;
	ID3D11ShaderResourceView* normalMap;
	ID3D11SamplerState* colorMapSampler;

	ID3D11Buffer* viewCB;
	ID3D11Buffer* projCB;
	ID3D11Buffer* worldCB;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	float posx, posz;
	float escalx, escaly;

	UINT* indices;
	VertexComponent* vertices;
	VertexCollide* vertcol;
	ID3D11Resource* heightMap;
	BYTE** alturaData;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	D3DXVECTOR3 frontal;

public:
	BillboardRR(WCHAR* billb, WCHAR* normal, ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, float escala)
	{
		//copiamos el device y el device context a la clase terreno
		d3dContext = D3DContext;
		d3dDevice = D3DDevice;
		//este es el ancho y el alto del terreno en su escala
		
		float escal = escala;
		frontal = D3DXVECTOR3(0, 0, 1);
		//aqui cargamos las texturas de alturas y el cesped
		CargaParametros(billb,normal, escal);
	}

	~BillboardRR()
	{
		//libera recursos
		delete vertcol;
		UnloadContent();
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

	bool CargaParametros(WCHAR* billb, WCHAR* normal, float escala)
	{
		HRESULT d3dResult;
		ID3DBlob* vsBuffer = 0;

		//cargamos el shaders de vertices que esta contenido en el Shader.fx, note
		//que VS_Main es el nombre del vertex shader en el shader, vsBuffer contendra
		//al puntero del mismo
		bool compileResult = CompileD3DShader(L"Billboard.hlsl", "VS_Main", "vs_4_0", &vsBuffer);
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
		D3D11_INPUT_ELEMENT_DESC billLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		unsigned int elementos = ARRAYSIZE(billLayout);

		d3dResult = d3dDevice->CreateInputLayout(billLayout, elementos,
			vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLay);

		vsBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		ID3DBlob* psBuffer = 0;
		// de los vertices pasamos al pixel shader, note que el nombre del shader es el mismo
		//ahora buscamos al pixel shader llamado PS_Main
		compileResult = CompileD3DShader(L"Billboard.hlsl", "PS_Main", "ps_4_0", &psBuffer);

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


		vertices = new VertexComponent[4];

		// Se calculan los vertices 'x' y 'z'. 'Y' se saca del mapa de normales
		vertices[0].pos.x = 0;
		vertices[0].pos.y = 0;
		vertices[0].pos.z = -1 * escala;
		vertices[0].UV.x = 0;
		vertices[0].UV.y = 1;

		vertices[1].pos.x = 0;
		vertices[1].pos.y = 2 * escala;
		vertices[1].pos.z = -1 * escala;
		vertices[1].UV.x = 0;
		vertices[1].UV.y = 0;

		vertices[2].pos.x = 0;
		vertices[2].pos.y = 2 * escala;
		vertices[2].pos.z = 1 * escala;
		vertices[2].UV.x = 1;
		vertices[2].UV.y = 0;

		vertices[3].pos.x = 0;
		vertices[3].pos.y = 0;
		vertices[3].pos.z = 1 * escala;
		vertices[3].UV.x = 1;
		vertices[3].UV.y = 1;

		//proceso de guardar el buffer de vertices para su uso en el render
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(VertexComponent) * 4;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;

		d3dResult = d3dDevice->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);

		if (FAILED(d3dResult))
		{
			MessageBox(0, L"Error", L"Error al crear vertex buffer", MB_OK);
			return false;
		}
		//ya una vez pasados los vertices al buffer borramos el arreglo donde los habiamos creado inicialmente
		delete vertices;

		//creamos los indices para hacer el terreno
		estableceIndices();
		//crea los accesos de las texturas para los shaders 
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, billb, 0, 0, &colorMap, 0);
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, normal, 0, 0, &normalMap, 0);
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


		return true;
	}




	bool UnloadContent()
	{
		if (colorMapSampler)
			colorMapSampler->Release();
		if (colorMap)
			colorMap->Release();
		if (normalMap)
			normalMap->Release();
		if (VertexShaderVS)
			VertexShaderVS->Release();
		if (solidColorPS)
			solidColorPS->Release();
		if (inputLay)
			inputLay->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (viewCB)
			viewCB->Release();
		if (projCB)
			projCB->Release();
		if (worldCB)
			worldCB->Release();


		colorMapSampler = 0;
		colorMap = 0;
		normalMap = 0;
		VertexShaderVS = 0;
		solidColorPS = 0;
		inputLay = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		viewCB = 0;
		projCB = 0;
		worldCB = 0;
	}


	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXVECTOR3 poscam, float xx, float zz, float posy, float escala, vector2* uv1, vector2* uv2, vector2* uv3, vector2* uv4, int frame)
	{
		posx = xx;
		posz = zz;
		vertices = new VertexComponent[4];

		// Se calculan los vertices 'x' y 'z'. 'Y' se saca del mapa de normales
		vertices[0].pos.x = 0;
		vertices[0].pos.y = 0;
		vertices[0].pos.z = -1 * escala;
		vertices[0].UV.x = uv1[frame].u;
		vertices[0].UV.y = uv1[frame].v;

		vertices[1].pos.x = 0;
		vertices[1].pos.y = 2 * escala;
		vertices[1].pos.z = -1 * escala;
		vertices[1].UV.x = uv2[frame].u;
		vertices[1].UV.y = uv2[frame].v;

		vertices[2].pos.x = 0;
		vertices[2].pos.y = 2 * escala;
		vertices[2].pos.z = 1 * escala;
		vertices[2].UV.x = uv3[frame].u;
		vertices[2].UV.y = uv3[frame].v;

		vertices[3].pos.x = 0;
		vertices[3].pos.y = 0;
		vertices[3].pos.z = 1 * escala;
		vertices[3].UV.x = uv4[frame].u;
		vertices[3].UV.y = uv4[frame].v;

		//proceso de guardar el buffer de vertices para su uso en el render
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(VertexComponent) * 4;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;

		d3dDevice->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);


		//ya una vez pasados los vertices al buffer borramos el arreglo donde los habiamos creado inicialmente
		delete vertices;


		//paso de datos, es decir cuanto es el ancho de la estructura
		unsigned int stride = sizeof(VertexComponent);
		unsigned int offset = 0;

		//define la estructura del vertice a traves de layout
		d3dContext->IASetInputLayout(inputLay);

		//define con que buffer trabajara
		d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		//define con buffer de indices trabajara
		d3dContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//define la forma de conexion de los vertices
		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Establece el vertex y pixel shader que utilizara
		d3dContext->VSSetShader(VertexShaderVS, 0, 0);
		d3dContext->PSSetShader(solidColorPS, 0, 0);
		//pasa lo sbuffers al shader
		d3dContext->PSSetShaderResources(0, 1, &colorMap);
		d3dContext->PSSetShaderResources(1, 1, &normalMap);
		d3dContext->PSSetSamplers(0, 1, &colorMapSampler);

		//mueve la camara
		float difz = poscam.z - posz;
		float difx = poscam.x - posx;
		float dist = sqrt(difz*difz + difx * difx);
		float angle = acos(difx / dist);// * (180.0 / D3DX_PI);

		D3DXMATRIX rotationMat;
		D3DXMatrixIdentity(&rotationMat);
		rotationMat._11 = rotationMat._33 = difx / dist;
		rotationMat._13 = difz / dist;
		rotationMat._31 = -rotationMat._13;

		D3DXMATRIX translationMat;
		D3DXMatrixTranslation(&translationMat, posx, posy, posz);

		D3DXMATRIX worldMat = rotationMat * translationMat;
		D3DXMatrixTranspose(&worldMat, &worldMat);
		//actualiza los buffers del shader
		d3dContext->UpdateSubresource(worldCB, 0, 0, &worldMat, 0, 0);
		d3dContext->UpdateSubresource(viewCB, 0, 0, &vista, 0, 0);
		d3dContext->UpdateSubresource(projCB, 0, 0, &proyeccion, 0, 0);
		//le pasa al shader los buffers
		d3dContext->VSSetConstantBuffers(0, 1, &worldCB);
		d3dContext->VSSetConstantBuffers(1, 1, &viewCB);
		d3dContext->VSSetConstantBuffers(2, 1, &projCB);
		//cantidad de trabajos

		d3dContext->DrawIndexed(6, 0, 0);


	}

	void estableceIndices()
	{
		HRESULT d3dResult;

		indices = new UINT[6];

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;


		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_DEFAULT;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(INT) * 6;
		indexDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = indices;

		d3dResult = d3dDevice->CreateBuffer(&indexDesc, &resourceData, &indexBuffer);

		if (FAILED(d3dResult))
		{
			return;
		}
		delete indices;
	}

};

#endif