// Cabeceras Basicas para la creacion de una ventana de windows
#include <windows.h>
#include <windowsx.h>
#include "Terrain.h"
 
 #define SCREEN_WIDTH  900
 #define SCREEN_HEIGHT 700

TerrainClass *DirXRR;
 
 LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



 int WINAPI WinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow)
 {
     HWND hWnd;
     WNDCLASSEX wc;
	 DirXRR = new TerrainClass(512,512);

     ZeroMemory(&wc, sizeof(WNDCLASSEX));

     wc.cbSize = sizeof(WNDCLASSEX);
     wc.style = CS_HREDRAW | CS_VREDRAW;
     wc.lpfnWndProc = WindowProc;
     wc.hInstance = hInstance;
     wc.hCursor = LoadCursor(NULL, IDC_ARROW);    
     wc.lpszClassName = L"WindowClass";

     RegisterClassEx(&wc);

     hWnd = CreateWindowEx(NULL,
                           L"WindowClass",
                           L"Programa para LMAD Ejercicio 4",
                           WS_OVERLAPPEDWINDOW,    
                           0, 0,    
                           SCREEN_WIDTH, SCREEN_HEIGHT,    
                           NULL,
                           NULL,
                           hInstance,
                           NULL);

     ShowWindow(hWnd, nCmdShow);

     // inicializamos el D3D, es la libreria para trazos en 3D
	 DirXRR->Initialize(hInstance, hWnd);
	 DirXRR->LoadContent(L"grass.jpg", L"GrandCanyon.png", 10.0f);

     // lazo principal de manejo de eventos de windows.

	 MSG msg = {0};

	 while(msg.message != WM_QUIT)
     {
         if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
         {
             TranslateMessage(&msg);
             DispatchMessage(&msg);
         }
		 else
		 {
			 DirXRR->Update(0.0f);
			 DirXRR->Render();
		 }
     }

     // cuando ya vayamos a salir debemos liberar los recursos liberados
	 //para el directx, si hay tarjeta grafica se debe de liberar tambien.
	 DirXRR->ShutDown();

     return msg.wParam;
 }


 //en este procedimiento tipo callback manejaremos los eventos de windows
 //mouse teclado, joystick, etc.
 LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
 {
	 PAINTSTRUCT paintStruct;
	 HDC  hDC;
     switch(message)
     {
	 case WM_PAINT:
		 {
			 hDC = BeginPaint(hWnd, &paintStruct);
			 EndPaint(hWnd, &paintStruct);
		 }
		 break;
         case WM_DESTROY:
             {
                 PostQuitMessage(0);
             } 
			 break;
		 default:
			 return DefWindowProc (hWnd, message, wParam, lParam);
     }
 }

