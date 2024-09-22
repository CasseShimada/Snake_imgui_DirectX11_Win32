// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

#include "Game.h"

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define _S(_LITERAL)    (const char*)u8##_LITERAL

// Main code
int main(int, char**)
{
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"贪吃蛇", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// 加载字体
	ImFont* sourceHanSansChineseFull = io.Fonts->AddFontFromFileTTF("C:/Users/Admin/assignments/games/Snake_imgui_DirectX11_Win32/Snake_imgui_DirectX11_Win32/Font/SourceHanSans-Bold.ttc", 25.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	IM_ASSERT(sourceHanSansChineseFull != nullptr);
	io.Fonts->Build();

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 预设的颜色选项
	const char* color_presets[] = {
	_S("默认"), _S("红色"), _S("绿色"), _S("蓝色"), _S("黄色"), _S("橘黄色"), _S("紫色"), _S("黑色"), _S("白色"), NULL
	};

	// 开始字符串
	const char* secret_code = _S("邀请码");
	char input_buffer[128] = "";

	bool show_error_popup = false;

	Game* game = new Game();

	// 游戏设置
	bool gameStart = false; // 游戏是否开始

	int speed = 3; // 速度
	std::pair<int, int> grid_size = std::pair<int, int>(12, 12); // 舞台尺寸
	int cell_size = 30; // 格子大小
	ImVec4 grid_color = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 格子颜色
	ImVec4 snake_color = ImVec4(0.6549f, 0.3843f, 0.2196f, 1.0f); // 蛇颜色

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window being minimized or screen locked
		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ———————————————————————— 自定义窗口 ————————————————————————

		ImGui::SetNextWindowSize(ImVec2(600.0f, 600.0f));
		if (gameStart == false)
		{
			if (ImGui::Begin(_S("游戏设置"), NULL, ImGuiWindowFlags_NoResize | // 游戏设置窗口
				ImGuiWindowFlags_NoCollapse)) {
				//if (ImGui::CollapsingHeader(_S("舞台设置")))
				if (ImGui::TreeNodeEx(_S("舞台设置"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::SliderInt(_S("舞台长度"), &grid_size.first, 12, 30);
					ImGui::SliderInt(_S("舞台宽度"), &grid_size.second, 12, 30);
					ImGui::SliderInt(_S("格子大小"), &cell_size, 15, 50);
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx(_S("颜色设置"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::ColorEdit3(_S("格子颜色"), (float*)&grid_color);

					ImGui::SameLine();

					static int grid_color_preset = 0; // 默认选择第一个颜色
					if (ImGui::Combo(_S("格子颜色预设"), &grid_color_preset, color_presets, IM_ARRAYSIZE(color_presets))) {
						switch (grid_color_preset) {
						case 0: grid_color = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); break; // 默认
						case 1: grid_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break; // 红色
						case 2: grid_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); break; // 绿色
						case 3: grid_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); break; // 蓝色
						case 4: grid_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); break; // 黄色
						case 5: grid_color = ImVec4(1.0f, 0.647f, 0.0f, 1.0f); break; // 橘黄色
						case 6: grid_color = ImVec4(0.5f, 0.0f, 0.5f, 1.0f); break; // 紫色
						case 7: grid_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); break; // 黑色
						case 8: grid_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break; // 白色
						}
					}

					ImGui::ColorEdit3(_S("蛇颜色"), (float*)&snake_color);
					ImGui::SameLine();

					static int snake_color_preset = 0; // 默认选择第一个颜色
					if (ImGui::Combo(_S("蛇颜色预设"), &snake_color_preset, color_presets, IM_ARRAYSIZE(color_presets))) {
						switch (snake_color_preset) {
						case 0: snake_color = ImVec4(0.6549f, 0.3843f, 0.2196f, 1.0f); break; // 默认
						case 1: snake_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break; // 红色
						case 2: snake_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); break; // 绿色
						case 3: snake_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); break; // 蓝色
						case 4: snake_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); break; // 黄色
						case 5: snake_color = ImVec4(1.0f, 0.647f, 0.0f, 1.0f); break; // 橘黄色
						case 6: snake_color = ImVec4(0.5f, 0.0f, 0.5f, 1.0f); break; // 紫色
						case 7: snake_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); break; // 黑色
						case 8: snake_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break; // 白色
						}
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx(_S("游戏设置"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::SliderInt(_S("速度"), &speed, 1, 5);
					ImGui::TreePop();
				}

				//ImGui::Checkbox(_S("游戏状态"), &gameStart);


				if (ImGui::TreeNodeEx(_S("游戏开始"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::InputText(_S("请输入邀请码"), input_buffer, IM_ARRAYSIZE(input_buffer));
					if (ImGui::Button(_S("开始游戏")))
					{
						if (strcmp(input_buffer, secret_code) == 0)
						{
							show_error_popup = false;
							game->SyncGameSettings(speed, grid_size, cell_size, grid_color, snake_color);
							game->Reset();
							gameStart = true;
							game->Start();
						}
						else {
							show_error_popup = true;
						}
					}
					ImGui::TreePop();
				}


			}ImGui::End();
		}

		if (show_error_popup) {
			ImGui::SetNextWindowSize(ImVec2(250.0f, 166.0f));
			if (ImGui::Begin(_S("啊:<"), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
			{
				ImGui::Text(_S("输入错误，请使用脑子"));
				if (ImGui::Button(_S("好的")))
				{
					show_error_popup = false;
				}
			}ImGui::End();
		}

		if (gameStart == true)
		{

			ImGui::SetNextWindowSize(ImVec2(static_cast<float>(grid_size.first * cell_size),
				static_cast<float>(grid_size.second * cell_size)));
			if (ImGui::Begin(_S("舞台"), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
				// 游戏舞台窗口

				game->Update();
				if (game->CheckGameOver() && ImGui::IsKeyDown(ImGuiKey_Escape))
				{
					gameStart = false;
				}

			}ImGui::End();

		}

		// ———————————————————————— 自定义窗口 ————————————————————————

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Present
		HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
		//HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
