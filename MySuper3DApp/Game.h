#pragma once
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "SimpleMath.h"
#include "SimpleMath.inl"
#include <iostream>
#include <chrono>
#include <vector>

#include "GameObject.h"
#include "DisplayWin32.h"
#include "InputDevice.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class DisplayWin32;
class RenderComponent;
class InputDevice;

class Game
{
protected:

	LPCWSTR	name;
	int clientWidth;
	int clientHeight;
	static Game* instance;

	std::shared_ptr<DisplayWin32> display;
	std::shared_ptr<D3D11_VIEWPORT> viewport;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	std::shared_ptr<DXGI_SWAP_CHAIN_DESC> swapDesc;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backTex;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;

	Game(LPCWSTR name, int clientWidth, int clientHeight);

	void UpdateInternal();
	void PrepareResources();
	void Initialize();
	void PrepareFrame();
	virtual void Update();
	void Draw();
	void EndFrame();

public:

	Game(const Game&) = delete;
	void operator = (const Game&) = delete;
	static Game* GetInstance();

	std::vector<GameObject*> gameObjects;
	std::shared_ptr<InputDevice> inputDevice;

	std::shared_ptr<std::chrono::time_point<std::chrono::steady_clock>> startTime;
	std::shared_ptr<std::chrono::time_point<std::chrono::steady_clock>> prevTime;
	HRESULT res;
	float totalTime;
	float deltaTime;
	unsigned int frameCount;

	static Game* CreateInstance(LPCWSTR name, int screenWidth, int screenHeight);

	virtual void Run();

	int GetWidth();
	int GetHeight();
	std::shared_ptr<DisplayWin32> GetDisplay();
	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice();
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> GetContext();
	Microsoft::WRL::ComPtr <IDXGISwapChain> GetSwapChain();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRTV();

	LRESULT MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
};
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
