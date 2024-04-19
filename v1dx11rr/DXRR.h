#ifndef _dxrr
#define _dxrr
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include "TerrenoRR.h"
#include "Camara.h"
#include "SkyDome.h"
#include "Billboard.h"
#include "ModeloRR.h"
#include "XACT3Util.h"

class DXRR{	

private:
	int ancho;
	int alto;
public:	
	HINSTANCE hInstance;
	HWND hWnd;

	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferTarget;

	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilDisabledState;

	ID3D11BlendState *alphaBlendState, *commonBlendState;

	int frameBillboard;
	int frameBillboardRain;
	int skydomeAnim;
	int skyIndice;

	TerrenoRR *terreno;
	SkyDome* skydome[15];
	BillboardRR *billboard;
	BillboardRR* billboardRain;
	BillboardRR* arbol;
	BillboardRR* arbolFeoBill;
	BillboardRR* arbustoBill;
	BillboardRR* libros;

	Camara *camara;
	ModeloRR* model;
	ModeloRR* house;
	ModeloRR* jeep;
	ModeloRR* house2;
	ModeloRR* oldCar;
	ModeloRR* oldCar2;
	ModeloRR* tanque;
	ModeloRR* tanque2;
	ModeloRR* caja;
	ModeloRR* fogata;
	ModeloRR* rock;
	ModeloRR* tree;
	ModeloRR* tronco;
	ModeloRR* silla;
	ModeloRR* casaAban;
	ModeloRR* arbolFeo;


	
	float izqder;
	float arriaba;
	float vel;
	bool breakpoint;
	vector2 uv1[32];
	vector2 uv2[32];
	vector2 uv3[32];
	vector2 uv4[32];

	vector2 uv01[4];
	vector2 uv02[4];
	vector2 uv03[4];
	vector2 uv04[4];

	XACTINDEX cueIndex;
	CXACT3Util m_XACT3;
	
    DXRR(HWND hWnd, int Ancho, int Alto)
	{
		breakpoint = false;
		frameBillboard = 0;
		frameBillboardRain = 0;
		skydomeAnim = 0;
		skyIndice = 0;
		ancho = Ancho;
		alto = Alto;
		driverType = D3D_DRIVER_TYPE_NULL;
		featureLevel = D3D_FEATURE_LEVEL_11_0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
		IniciaD3D(hWnd);
		izqder = 0;
		arriaba = 0;
		billCargaFuego();
		camara = new Camara(D3DXVECTOR3(0,80,6), D3DXVECTOR3(0,80,0), D3DXVECTOR3(0,1,0), Ancho, Alto);
		terreno = new TerrenoRR(300, 300, d3dDevice, d3dContext);

		
		skydome[0] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/dia0.png");
		skydome[1] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/dia1.png");
		skydome[2] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/dia2.png");
		skydome[3] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/dia3.png");
		skydome[4] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/dia4.png");
		skydome[5] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/tarde0.png");
		skydome[6] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/tarde1.png");
		skydome[7] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/tarde2.png");
		skydome[8] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/tarde3.png");
		skydome[9] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/tarde4.png");
		skydome[10] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/noche0.png");
		skydome[11] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/noche1.png");
		skydome[12] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/noche2.png");
		skydome[13] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/noche3.png");
		skydome[14] = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"Modelos/Sky/noche4.png");


		billboard = new BillboardRR(L"Assets/Billboards/fuego-anim.png", L"Assets/Billboards/fuego-anim-normal.png", d3dDevice, d3dContext, 5);
		billboardRain = new BillboardRR(L"Modelos/rain2.png", L"Modelos/rain2.png", d3dDevice, d3dContext, 10);
		model = new ModeloRR(d3dDevice, d3dContext, "Assets/Cofre/Cofre.obj", L"Assets/Cofre/Cofre-color.png", L"Assets/Cofre/Cofre-spec.png", 0, 0);
		house = new ModeloRR(d3dDevice, d3dContext, "Modelos/house/house.obj", L"Modelos/house/unknown_Base_Color.png", L"Modelos/house/unknown_Roughness.png", -90, 60);
		jeep = new ModeloRR(d3dDevice, d3dContext, "Modelos/jeep/jeep_phat_ref.obj", L"Modelos/jeep/jeeptext.png", L"Modelos/jeep/jeeptexspec.png", -80, 20);
		house2 = new ModeloRR(d3dDevice, d3dContext, "Modelos/house2/house2.obj", L"Modelos/house2/FbxTemp_0001.png", L"Modelos/house2/FbxTemp_0002.png", -40, 30);
		oldCar = new ModeloRR(d3dDevice, d3dContext, "Modelos/oldcar/oldcar.obj", L"Modelos/oldcar/car_d.png", L"Modelos/oldcar/car_m.png", 100, -110);
		oldCar2 = new ModeloRR(d3dDevice, d3dContext, "Modelos/oldcar2/oldcar2.obj", L"Modelos/oldcar2/Material _95_Base_Blue.png", L"Modelos/oldcar2/Material _95_Metallic.png", -70, 140);
		tanque = new ModeloRR(d3dDevice, d3dContext, "Modelos/tanque/tanque.obj", L"Modelos/tanque/om_reddruma01_01_bm.png", L"Modelos/tanque/om_reddruma01_01_mm.png", -85, 30);
		tanque2 = new ModeloRR(d3dDevice, d3dContext, "Modelos/tanque/tanque.obj", L"Modelos/tanque/om_reddruma01_01_bm.png", L"Modelos/tanque/om_reddruma01_01_mm.png", -80, 35);
		caja = new ModeloRR(d3dDevice, d3dContext, "Modelos/caja/caja.obj", L"Modelos/caja/Safe_albedo.png", L"Modelos/caja/Safe_roughness.png", -35, 15);		
		fogata = new ModeloRR(d3dDevice, d3dContext, "Modelos/fogata/campfire.obj", L"Modelos/fogata/campfire.png", L"Modelos/fogata/campfire.png", -65, 0);
		rock = new ModeloRR(d3dDevice, d3dContext, "Modelos/rock/Rock.obj", L"Modelos/rock/ModelTexture.jpg", L"Modelos/rock/ModelTexture.jpg", 5, 5);
		tree = new ModeloRR(d3dDevice, d3dContext, "Modelos/arbol/arbol.obj", L"Modelos/arbol/treeTex.png", L"Modelos/arbol/treeTex.png", -8, -8);
		silla = new ModeloRR(d3dDevice, d3dContext, "Modelos/silla/silla.obj", L"Modelos/silla/chair_d.png", L"Modelos/silla/chair_s.png", -55, 80);
		casaAban = new ModeloRR(d3dDevice, d3dContext, "Modelos/casaAban/casaAban.obj", L"Modelos/casaAban/casaAbanTex.png", L"Modelos/casaAban/casaAbanTex.png", 70,  130);
		arbolFeo = new ModeloRR(d3dDevice, d3dContext, "Modelos/arbolFeo/arbolFeo.obj", L"Modelos/arbolFeo/ArbolFeoTex.png", L"Modelos/arbolFeo/ArbolFeoTex.png", -130, -100);


		arbol = new BillboardRR(L"Modelos/arbolBill/imagen (3).png", L"Modelos/arbolBill/imagen (4).png", d3dDevice, d3dContext, 10);
		arbolFeoBill = new BillboardRR(L"Modelos/arbolFeoBill.png", L"Modelos/arbolFeoBill.png", d3dDevice, d3dContext, 10);
		arbustoBill = new BillboardRR(L"Modelos/arbustoBill.png", L"Modelos/arbustoBill.png", d3dDevice, d3dContext, 10);
		libros = new BillboardRR(L"Modelos/librosBill.png", L"Modelos/librosBill.png", d3dDevice, d3dContext, 10);


		//\Modelos\house jeep_phat_ref om_reddruma01_01_mm
		

		
	}

	~DXRR()
	{
		LiberaD3D();
		m_XACT3.Terminate();
	}
	
	bool IniciaD3D(HWND hWnd)
	{
		this->hInstance = hInstance;
		this->hWnd = hWnd;

		//obtiene el ancho y alto de la ventana donde se dibuja
		RECT dimensions;
		GetClientRect(hWnd, &dimensions);
		unsigned int width = dimensions.right - dimensions.left;
		unsigned int heigth = dimensions.bottom - dimensions.top;

		//Las formas en como la pc puede ejecutar el DX11, la mas rapida es D3D_DRIVER_TYPE_HARDWARE pero solo se puede usar cuando lo soporte el hardware
		//otra opcion es D3D_DRIVER_TYPE_WARP que emula el DX11 en los equipos que no lo soportan
		//la opcion menos recomendada es D3D_DRIVER_TYPE_SOFTWARE, es la mas lenta y solo es util cuando se libera una version de DX que no sea soportada por hardware
		D3D_DRIVER_TYPE driverTypes[]=
		{
			D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
		};
		unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

		//La version de DX que utilizara, en este caso el DX11
		D3D_FEATURE_LEVEL featureLevels[]=
		{
			D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
		};
		unsigned int totalFeaturesLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = heigth;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		HRESULT result;
		unsigned int driver = 0, creationFlags = 0;
		for(driver = 0; driver<totalDriverTypes; driver++)
		{
			result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
				creationFlags, featureLevels, totalFeaturesLevels, 
				D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
				&d3dDevice, &featureLevel, &d3dContext);

			if(SUCCEEDED(result))
			{
				driverType = driverTypes[driver];
				break;
			}
		}

		if(FAILED(result))
		{

			//Error al crear el Direct3D device
			return false;
		}
		
		ID3D11Texture2D* backBufferTexture;
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
		if(FAILED(result))
		{
			//"Error al crear el swapChainBuffer
			return false;
		}

		result = d3dDevice->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget);
		if(backBufferTexture)
			backBufferTexture->Release();

		if(FAILED(result))
		{
			//Error al crear el renderTargetView
			return false;
		}


		D3D11_VIEWPORT viewport;
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)heigth;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		d3dContext->RSSetViewports(1, &viewport);

		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width;
		depthTexDesc.Height = heigth;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		
		result = d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &depthTexture);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear la DepthTexture", MB_OK);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		result = d3dDevice->CreateDepthStencilView(depthTexture, &descDSV, &depthStencilView);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el depth stencil target view", MB_OK);
			return false;
		}

		d3dContext->OMSetRenderTargets(1, &backBufferTarget, depthStencilView);

		return true;			
		
	}

	void LiberaD3D(void)
	{
		if(depthTexture)
			depthTexture->Release();
		if(depthStencilView)
			depthStencilView->Release();
		if(backBufferTarget)
			backBufferTarget->Release();
		if(swapChain)
			swapChain->Release();
		if(d3dContext)
			d3dContext->Release();
		if(d3dDevice)
			d3dDevice->Release();

		depthTexture = 0;
		depthStencilView = 0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
	}
	
	void Render(void)
	{
		float sphere[3] = { 0,0,0 };
		float prevPos[3] = { camara->posCam.x, camara->posCam.z, camara->posCam.z };
		static float angle = 0.0f;
		angle += 0.005;
		if (angle >= 360) angle = 0.0f;
		bool collide = false;
		if( d3dContext == 0 )
			return;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView( backBufferTarget, clearColor );
		d3dContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		camara->posCam.y = terreno->Superficie(camara->posCam.x, camara->posCam.z) + 5 ;
		camara->UpdateCam(vel, arriaba, izqder);
		skydome[skyIndice]->Update(camara->vista, camara->proyeccion);

		float camPosXZ[2] = { camara->posCam.x, camara->posCam.z };

		TurnOffDepth();
		skydome[skyIndice]->Render(camara->posCam);
		TurnOnDepth();
		terreno->Draw(camara->vista, camara->proyeccion);
		//TurnOnAlphaBlending();
		billboard->Draw(camara->vista, camara->proyeccion, camara->posCam,
			-65, 0, 5, 5, true ,uv1, uv2, uv3, uv4, frameBillboard);

		//billboardRain->Draw(camara->vista, camara->proyeccion, camara->posCam,
		//	-65, 0, 5, 10, true, uv01, uv02, uv03, uv04, frameBillboardRain);

		//BILLBOARDS DE ARBOLES ---------
		{

			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-80, -80, terreno->Superficie(-60, 0), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-114, -120, terreno->Superficie(-114, -120), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				150, 80, terreno->Superficie(150, 80), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-150, 72, terreno->Superficie(-150, 72), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				100, -50, terreno->Superficie(100, -50), 10, false);


			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				20, -50, terreno->Superficie(20, -50), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				70, -90, terreno->Superficie(70, -90), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				110, -40, terreno->Superficie(110, -40), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				113, -30, terreno->Superficie(113, -30), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				130, -115, terreno->Superficie(130, -115), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				35, -130, terreno->Superficie(35, -130), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				145, -140, terreno->Superficie(145, -140), 10, false);


			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				70, 40, terreno->Superficie(70, 40), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				20, 45, terreno->Superficie(20, 45), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				90, 90, terreno->Superficie(90, 90), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				20, 100, terreno->Superficie(20, 100), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				140, 80, terreno->Superficie(140, 80), 10, false);

			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-40, -60, terreno->Superficie(-40, -60), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-20, -110, terreno->Superficie(-20, -110), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-30, -140, terreno->Superficie(-30, -140), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-90, -130, terreno->Superficie(-90, -130), 10, false);
			arbol->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-120, -70, terreno->Superficie(-120, -70), 10, false);


			arbolFeoBill->Draw(camara->vista, camara->proyeccion, camara->posCam,
				120, -130, terreno->Superficie(120, -130) -3, 10, false);
			arbolFeoBill->Draw(camara->vista, camara->proyeccion, camara->posCam,
				40, -110, terreno->Superficie(40, -110) -3, 10, false);
			arbolFeoBill->Draw(camara->vista, camara->proyeccion, camara->posCam,
				120, 70, terreno->Superficie(120, 70)-3, 10, false);
			arbolFeoBill->Draw(camara->vista, camara->proyeccion, camara->posCam,
				140, 110, terreno->Superficie(140, 110)-3, 10, false);
			arbolFeoBill->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-140, -50, terreno->Superficie(-140, -50), 10, false);
			arbolFeoBill->Draw(camara->vista, camara->proyeccion, camara->posCam,
				-145, 90, terreno->Superficie(-145, 90)-3, 10, false);

		}

		//BILLBOARD DE LIBROS ---------
		{
			libros->Draw(camara->vista, camara->proyeccion, camara->posCam,
				145, -100, terreno->Superficie(145, -100) - 3, 10, false);
			libros->Draw(camara->vista, camara->proyeccion, camara->posCam,
				110, -130, terreno->Superficie(110, -130) - 3, 10, false);
			libros->Draw(camara->vista, camara->proyeccion, camara->posCam,
				130, -130, terreno->Superficie(110, -130) - 3, 10, false);
		}


		//TurnOffAlphaBlending();
		model->Draw(camara->vista, camara->proyeccion, terreno->Superficie(100, 20), camara->posCam, 10.0f, 0, 'A', 1);
		house->Draw(camara->vista, camara->proyeccion, terreno->Superficie(-70, 40), camara->posCam, 10.0f, 45, 'Y', 1);
		jeep->Draw(camara->vista, camara->proyeccion, terreno->Superficie(60, 30), camara->posCam, 10.0f, 0, 'A', 1);
		house2->Draw(camara->vista, camara->proyeccion, terreno->Superficie(-40, 40), camara->posCam, 30.0f, 180, 'Y', 0.8);
		oldCar->Draw(camara->vista, camara->proyeccion, terreno->Superficie(100, -110), camara->posCam, 30.0f, 0, 'A', 1);
		oldCar2->Draw(camara->vista, camara->proyeccion, terreno->Superficie(-70, 120), camara->posCam, 30.0f, 0, 'A', 1);
		tanque->Draw(camara->vista, camara->proyeccion, 10.8, camara->posCam, 30.0f, 80, 'Z', 1);
		tanque2->Draw(camara->vista, camara->proyeccion, terreno->Superficie(-80, 25), camara->posCam, 30.0f, 0, 'A', 1);
		caja->Draw(camara->vista, camara->proyeccion, terreno->Superficie(-80, 25), camara->posCam, 30.0f, 80, 'Y', 1);
		fogata->Draw(camara->vista, camara->proyeccion, 10.5, camara->posCam, 30.0f, 0, 'A', 1.5);
		rock->Draw(camara->vista, camara->proyeccion, terreno->Superficie(12, 15), camara->posCam, 30.0f, 0, 'A', 1);
		casaAban->Draw(camara->vista, camara->proyeccion, terreno->Superficie(casaAban->getPosX(), casaAban->getPosZ()), camara->posCam, 30.0f, 0, 'A', 1);
		

		arbolFeo->setPosX(-130);
		arbolFeo->setPosZ(-100);
		arbolFeo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(arbolFeo->getPosX(), arbolFeo->getPosZ()), camara->posCam, 30.0f, 0, 'A', 1);
		
		arbolFeo->setPosX(80);
		arbolFeo->setPosZ(-130);
		arbolFeo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(arbolFeo->getPosX(), arbolFeo->getPosZ()) -2, camara->posCam, 30.0f, 0, 'A', 1);
		
		arbolFeo->setPosX(140);
		arbolFeo->setPosZ(-20);
		arbolFeo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(arbolFeo->getPosX(), arbolFeo->getPosZ()), camara->posCam, 30.0f, 0, 'A', 1);
		
		arbolFeo->setPosX(120);
		arbolFeo->setPosZ(90);
		arbolFeo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(arbolFeo->getPosX(), arbolFeo->getPosZ()), camara->posCam, 30.0f, 0, 'A', 1);
		
		arbolFeo->setPosX(-130);
		arbolFeo->setPosZ(70);
		arbolFeo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(arbolFeo->getPosX(), arbolFeo->getPosZ()), camara->posCam, 30.0f, 0, 'A', 1);
		
		arbolFeo->setPosX(-120);
		arbolFeo->setPosZ(120);
		arbolFeo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(arbolFeo->getPosX(), arbolFeo->getPosZ()), camara->posCam, 30.0f, 0, 'A', 1);


		
		tree->setPosX(-8);
		tree->setPosZ(-8);
		tree->Draw(camara->vista, camara->proyeccion, terreno->Superficie(-8,-8), camara->posCam, 30.0f, 0, 'A', 1);
		tree->setPosX(0);
		tree->setPosZ(-130);
		tree->Draw(camara->vista, camara->proyeccion, terreno->Superficie(0,-130), camara->posCam, 30.0f, 0, 'A', 1);



		//-70-130
		silla->Draw(camara->vista, camara->proyeccion, terreno->Superficie(12, 15), camara->posCam, 30.0f, 0, 'A', 1);




		swapChain->Present( 1, 0 );
	}

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

	//Activa el alpha blend para dibujar con transparencias
	void TurnOnAlphaBlending()
	{
		float blendFactor[4];
		blendFactor[0] = 0.0f;
		blendFactor[1] = 0.0f;
		blendFactor[2] = 0.0f;
		blendFactor[3] = 0.0f;
		HRESULT result;

		D3D11_BLEND_DESC descABSD;
		ZeroMemory(&descABSD, sizeof(D3D11_BLEND_DESC));
		descABSD.RenderTarget[0].BlendEnable = TRUE;
		descABSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descABSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descABSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		result = d3dDevice->CreateBlendState(&descABSD, &alphaBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(alphaBlendState, blendFactor, 0xffffffff);
	}

	//Regresa al blend normal(solido)
	void TurnOffAlphaBlending()
	{
		D3D11_BLEND_DESC descCBSD;
		ZeroMemory(&descCBSD, sizeof(D3D11_BLEND_DESC));
		descCBSD.RenderTarget[0].BlendEnable = FALSE;
		descCBSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descCBSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descCBSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		HRESULT result;

		result = d3dDevice->CreateBlendState(&descCBSD, &commonBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(commonBlendState, NULL, 0xffffffff);
	}

	void TurnOnDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDSD;
		ZeroMemory(&descDSD, sizeof(descDSD));
		descDSD.DepthEnable = true;
		descDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDSD.StencilEnable=true;
		descDSD.StencilReadMask = 0xFF;
		descDSD.StencilWriteMask = 0xFF;
		descDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDSD, &depthStencilState);
		
		d3dContext->OMSetDepthStencilState(depthStencilState, 1);
	}

	void TurnOffDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDDSD;
		ZeroMemory(&descDDSD, sizeof(descDDSD));
		descDDSD.DepthEnable = false;
		descDDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDDSD.StencilEnable=true;
		descDDSD.StencilReadMask = 0xFF;
		descDDSD.StencilWriteMask = 0xFF;
		descDDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDDSD, &depthStencilDisabledState);
		d3dContext->OMSetDepthStencilState(depthStencilDisabledState, 1);
	}

	void billCargaFuego()
	{
		uv1[0].u = .125;
		uv2[0].u = .125;
		uv3[0].u = 0;
		uv4[0].u = 0;

		uv1[0].v = .25;
		uv2[0].v = 0;
		uv3[0].v = 0;
		uv4[0].v = .25;


		for (int j = 0; j < 8; j++) {
			uv1[j].u = uv1[0].u + (j * .125);
			uv2[j].u = uv2[0].u + (j * .125);
			uv3[j].u = uv3[0].u + (j * .125);
			uv4[j].u = uv4[0].u + (j * .125);

			uv1[j].v = .25;
			uv2[j].v = 0;
			uv3[j].v = 0;
			uv4[j].v = .25;
		}
		for (int j = 0; j < 8; j++) {
			uv1[j + 8].u = uv1[0].u + (j * .125);
			uv2[j + 8].u = uv2[0].u + (j * .125);
			uv3[j + 8].u = uv3[0].u + (j * .125);
			uv4[j + 8].u = uv4[0].u + (j * .125);

			uv1[j + 8].v = .5;
			uv2[j + 8].v = .25;
			uv3[j + 8].v = .25;
			uv4[j + 8].v = .5;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 16].u = uv1[0].u + (j * .125);
			uv2[j + 16].u = uv2[0].u + (j * .125);
			uv3[j + 16].u = uv3[0].u + (j * .125);
			uv4[j + 16].u = uv4[0].u + (j * .125);

			uv1[j + 16].v = .75;
			uv2[j + 16].v = .5;
			uv3[j + 16].v = .5;
			uv4[j + 16].v = .75;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 24].u = uv1[0].u + (j * .125);
			uv2[j + 24].u = uv2[0].u + (j * .125);
			uv3[j + 24].u = uv3[0].u + (j * .125);
			uv4[j + 24].u = uv4[0].u + (j * .125);

			uv1[j + 24].v = 1;
			uv2[j + 24].v = .75;
			uv3[j + 24].v = .75;
			uv4[j + 24].v = 1;
		}
	}

	void billCargaLluvia()
	{
		uv01[0].u = 0;		  
		uv01[0].v = 0;
		uv01[1].u = 0;
		uv01[1].v = .25;
		uv01[2].u = 0;
		uv01[2].v = .50;
		uv01[3].u = 0;
		uv01[3].v = .75;

		uv02[0].u = 0;
		uv02[0].v = 0;		   
		uv02[1].u = 0;
		uv02[1].v = .25;		   
		uv02[2].u = 0;
		uv02[2].v = .50;		 
		uv02[3].u = 0;
		uv02[3].v = .75;

		uv03[0].u = 0;
		uv03[0].v = 0;
		uv03[1].u = 0;
		uv03[1].v = .25;
		uv03[2].u = 0;
		uv03[2].v = .50;
		uv03[3].u = 0;
		uv03[3].v = .75;

		uv04[0].u = 0;
		uv04[0].v = 0;
		uv04[1].u = 0;
		uv04[1].v = .25;
		uv04[2].u = 0;
		uv04[2].v = .50;
		uv04[3].u = 0;
		uv04[3].v = .75;


	}
		

};


#endif