#include "Game.h"

Game* Game::instance = nullptr;

Game::Game(LPCWSTR name, int clientWidth, int clientHeight)
{
	this->name = name;
	this->clientWidth = clientWidth;
	this->clientHeight = clientHeight;

	totalTime = 0;
	deltaTime = 0;
	frameCount = 0;
	startTime = std::make_shared<std::chrono::time_point<std::chrono::steady_clock>>();
	prevTime = std::make_shared<std::chrono::time_point<std::chrono::steady_clock>>();

	viewport = std::make_shared<D3D11_VIEWPORT>();
	swapDesc = std::make_shared<DXGI_SWAP_CHAIN_DESC>();
}

Game* Game::CreateInstance(LPCWSTR name, int screenWidth, int screenHeight)
{
	if (!instance)
	{
		instance = new Game(name, screenWidth, screenHeight);
	}
	return instance;
}
Game* Game::GetInstance()
{
	return instance;
}

void Game::PrepareResources()
{
	display = std::make_shared<DisplayWin32>(name, clientWidth, clientHeight, WndProc);
	inputDevice = std::make_shared<InputDevice>();

	viewport->TopLeftX = 0;
	viewport->TopLeftY = 0;
	viewport->Width = static_cast<float>(display->GetClientWidth());
	viewport->Height = static_cast<float>(display->GetClientHeight());
	viewport->MinDepth = 0;
	viewport->MaxDepth = 1.0f;

	swapDesc->BufferDesc.Width = display->GetClientWidth();
	swapDesc->BufferDesc.Height = display->GetClientHeight();
	swapDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc->BufferDesc.RefreshRate.Numerator = 60;
	swapDesc->BufferDesc.RefreshRate.Denominator = 1;
	swapDesc->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapDesc->SampleDesc.Count = 1;
	swapDesc->SampleDesc.Quality = 0;

	swapDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc->BufferCount = 2;
	swapDesc->OutputWindow = display->GetHWnd();
	swapDesc->Windowed = true;
	swapDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	const int featureLevelsNumber = 1;
	D3D_FEATURE_LEVEL featureLevels[featureLevelsNumber] = { D3D_FEATURE_LEVEL_11_1 };

	res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		featureLevelsNumber,
		D3D11_SDK_VERSION,
		swapDesc.get(),
		swapChain.GetAddressOf(),
		device.GetAddressOf(),
		nullptr,
		context.GetAddressOf()
	);

	if (FAILED(res)) {}
	
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backTex.GetAddressOf());
	res = device->CreateRenderTargetView(backTex.Get(), nullptr, rtv.GetAddressOf());
}

void Game::Initialize()
{
	for (auto gameObject : gameObjects)
	{
		gameObject->Initialize();
	}
}

void Game::Run()
{
	PrepareResources();
	Initialize();

	*startTime = std::chrono::steady_clock::now();
	*prevTime = *startTime;

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			UpdateInternal();
		}
	}
}

void Game::UpdateInternal()
{
	auto curTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - *prevTime).count() / 1000000.0f;
	*prevTime = curTime;
	totalTime += deltaTime;
	frameCount++;
	if (totalTime > 1.0f)
	{
		float fps = frameCount / totalTime;
		totalTime -= 1.0f;
		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(display->GetHWnd(), text);
		frameCount = 0;
	}
	PrepareFrame();
	Update();
	Draw();
	EndFrame();
}

void Game::PrepareFrame()
{
	context->ClearState();
	context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
	context->RSSetViewports(1, viewport.get());
	float backgroundColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(rtv.Get(), backgroundColor);
}

void Game::Update()
{
	if (inputDevice->IsKeyDown(Keys::Escape))
	{
		PostQuitMessage(0);
	}
}

void Game::Draw()
{
	for (auto gameObject : gameObjects)
	{
		gameObject->Draw();
	}
}

void Game::EndFrame()
{
	context->OMSetRenderTargets(0, nullptr, nullptr);
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

int Game::GetWidth() { return clientWidth; }
int Game::GetHeight() { return clientHeight; }
std::shared_ptr<DisplayWin32> Game::GetDisplay()                { return display;   }
Microsoft::WRL::ComPtr<ID3D11Device> Game::GetDevice()          { return device;    }
Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Game::GetRTV()   { return rtv;       }
Microsoft::WRL::ComPtr <IDXGISwapChain> Game::GetSwapChain()    { return swapChain; }
Microsoft::WRL::ComPtr <ID3D11DeviceContext> Game::GetContext() { return context;   }

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	return Game::GetInstance()->MessageHandler(hwnd, umessage, wparam, lparam);
}
LRESULT Game::MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage)
	{
		case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];

			if (lpb == nullptr) 
			{
				return 0;
			}

			if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			{
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
			}

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				//printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
				//	raw->data.keyboard.MakeCode,
				//	raw->data.keyboard.Flags,
				//	raw->data.keyboard.Reserved,
				//	raw->data.keyboard.ExtraInformation,
				//	raw->data.keyboard.Message,
				//	raw->data.keyboard.VKey);

				inputDevice.get()->OnKeyDown({
					raw->data.keyboard.MakeCode,
					raw->data.keyboard.Flags,
					raw->data.keyboard.VKey,
					raw->data.keyboard.Message
					});
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				//printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				inputDevice->OnMouseMove({
					raw->data.mouse.usFlags,
					raw->data.mouse.usButtonFlags,
					static_cast<int>(raw->data.mouse.ulExtraInformation),
					static_cast<int>(raw->data.mouse.ulRawButtons),
					static_cast<short>(raw->data.mouse.usButtonData),
					raw->data.mouse.lLastX,
					raw->data.mouse.lLastY
					});
			}
			delete[] lpb;
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
		default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}