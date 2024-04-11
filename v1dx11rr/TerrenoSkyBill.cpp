#include <windows.h>
#include <windowsx.h>

#include "DXRR.h"
#include "GamePadRR.h"

#include <dinput.h>
#include <xinput.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define SCREEN_X 1920
#define SCREEN_Y 1080

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DXRR *dxrr;
GamePadRR *gamePad;
tagPOINT initialPoint;
tagPOINT actualPoint;
LPDIRECTINPUT8 m_pDirectInput = NULL;
LPDIRECTINPUTDEVICE8 m_pKeyboardDevice = NULL;
LPDIRECTINPUTDEVICE8 m_pMouseDevice = NULL;

void createMouseDevice(HWND hWnd) {
    m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, 0);

    m_pMouseDevice->SetDataFormat(&c_dfDIMouse);

    m_pMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_pMouseDevice->Acquire();

}

void createKeyboardDevice(HWND hWnd) {
    m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, 0);

    m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

    m_pKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_pKeyboardDevice->Acquire();

}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"DXRR_E1";
    wc.cbSize = sizeof(WNDCLASSEX);


    RegisterClassEx(&wc);

    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = (unsigned long)SCREEN_X;
    dmScreenSettings.dmPelsHeight = (unsigned long)SCREEN_Y;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;

    //RECT wr = {0, 0, SCREEN_X, SCREEN_Y};
    //AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    hWnd = CreateWindowEx(WS_EX_APPWINDOW,
                          L"DXRR_E1",
                          L"PLANTILLA PROYECTO",
                          WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX,
        posY,
        SCREEN_X,
        SCREEN_Y,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);
	dxrr = new DXRR(hWnd, 800, 600);
	dxrr->vel=0;
    gamePad = new GamePadRR(1);

    ClientToScreen(hWnd, &initialPoint);
    actualPoint.x = initialPoint.x + SCREEN_X / 2;
    actualPoint.y = initialPoint.y + SCREEN_Y / 2;

	SetTimer(hWnd, 100, 33, NULL);
    MSG msg;
    ::DirectInput8Create(
        hInstance, DIRECTINPUT_VERSION,
        IID_IDirectInput8, (void**)&m_pDirectInput, 0);
    createMouseDevice(hWnd);
    createKeyboardDevice(hWnd);
    while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }

        dxrr->Render();
    }

    if (m_pMouseDevice) {
        m_pMouseDevice->Unacquire();
        m_pMouseDevice->Release();
    }
    m_pMouseDevice = NULL;

    if (m_pKeyboardDevice) {
        m_pKeyboardDevice->Unacquire();
        m_pKeyboardDevice->Release();
    }
    m_pKeyboardDevice = NULL;

    if (m_pDirectInput)
        m_pDirectInput->Release();
    m_pDirectInput = NULL;

    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    float xPos = 0;
    float yPos = 0;

    switch(message)
    {
        case WM_DESTROY:
            {
				KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            } break;

		case WM_TIMER:
			{

			} break;
        
        case WM_MOUSEMOVE: {

            SetCursorPos(actualPoint.x, actualPoint.y);
            dxrr->frameBillboard++;
            if (dxrr->frameBillboard == 32)
                dxrr->frameBillboard = 0;

            dxrr->izqder = 0;
            dxrr->arriaba = 0;
            dxrr->vel = 0;

            char keyboardData[256];
            m_pKeyboardDevice->GetDeviceState(sizeof(keyboardData), (void*)&keyboardData);

            if (keyboardData[DIK_S] & 0x80) {
                dxrr->vel = -5.f;
            }


            if (keyboardData[DIK_W] & 0x80) {
                dxrr->vel = 5.f;
            }

            if (keyboardData[DIK_B] & 0x80) {
                dxrr->breakpoint = true;
            }

            if (keyboardData[DIK_ESCAPE] & 0x80) {
                KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            }

            DIMOUSESTATE mouseData;
            m_pMouseDevice->GetDeviceState(sizeof(mouseData), (void*)&mouseData);

            // Mouse move
            dxrr->izqder = (mouseData.lX / 1000.0f);
            dxrr->arriaba = -(mouseData.lY / 1000.0f);

            if (gamePad->IsConnected())
            {


                float grados = (float)gamePad->GetState().Gamepad.sThumbRX / 32767.0;

                if (grados > 0.19 || grados < -0.19) dxrr->izqder = grados / 15;

                grados = (float)gamePad->GetState().Gamepad.sThumbRY / 32767.0;

                if (grados > 0.19 || grados < -0.19)dxrr->arriaba = grados / 15;


                float velocidad = (float)gamePad->GetState().Gamepad.sThumbLY / 32767.0;
                if (velocidad > 0.19 || velocidad < -0.19) {
                    if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                        velocidad *= 14.5;
                    else if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) velocidad /= 3;
                    else velocidad *= 2.5;
                    if (velocidad > 0.19) dxrr->vel = velocidad;
                    else if (velocidad < -0.19) dxrr->vel = velocidad;
                }

            }

        }break;

    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}



