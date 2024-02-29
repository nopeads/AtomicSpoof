#include "Core.hpp"

#include "d3d_Hook.hpp"
#include "Menu.hpp"
#include "Classes.hpp"
#include "Settings.hpp"
#include "Visuals.hpp"
#include "Player.hpp"
#include "Aimbot.hpp"
#include "Weapon.hpp"
#include "NoClip.hpp"
#include "vehicule.hpp"
#include "auth.hpp"
#include "Fonts.hpp"
#include <imguinotify.hpp>
#include <program.hpp>

#pragma comment(lib, "proxine.lib")

//#include "Snow.hpp"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// you need those for snowflake
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080
bool IsValid = false;

void Kouloumbou()
{
	std::exit(-1);
}
void InitImGui()
{
	using namespace DirectX;

	ImGui::CreateContext();

	ImGuiIO* io = &ImGui::GetIO();
	io->ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io->IniFilename = nullptr;
	io->LogFilename = nullptr;

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	
	ImFontConfig rubik;
	rubik.FontDataOwnedByAtlas = false;
	

	io->Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(Rubik), sizeof(Rubik), 15.5f, &rubik);
	io->Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 25.f, &icons_config, icons_ranges);
	io->Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 25.f, &icons_config, icons_ranges);
	Menu::BiggestIcon = io->Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 40.f, NULL, icons_ranges);
	Menu::BiggestFont = io->Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(Rubik), sizeof(Rubik), 21.f, &rubik);
	Menu::littleFont = io->Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(Rubik), sizeof(Rubik), 17.5f, &rubik);

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(pDevice, pContext);
	
	//Snowflake::CreateSnowFlakes(snow, SNOW_LIMIT, 5.f/*minimum size*/, 25.f/*maximum size*/, 0/*imgui window x position*/, 0/*imgui window y position*/, WINDOW_WIDTH, WINDOW_HEIGHT, Snowflake::vec3(0.f, 0.005f)/*gravity*/, IM_COL32(255,255, 255, 100)/*color*/);

}


LRESULT __stdcall WindowHandler(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Menu::Open)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}

	return SAFE_CALL(CallWindowProcA)(DirectX::WindowEx, hWnd, uMsg, wParam, lParam);
}


bool BindD3DInfo(IDXGISwapChain* pSwapChain)
{
	if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&DirectX::pDevice)))
	{
		DirectX::pDevice->GetImmediateContext(&DirectX::pContext);
		DXGI_SWAP_CHAIN_DESC sd;

		pSwapChain->GetDesc(&sd);
		DirectX::Window = sd.OutputWindow;
		
	
		ID3D11Texture2D* pBackBuffer;

		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		DirectX::pDevice->CreateRenderTargetView(pBackBuffer, 0, &DirectX::renderTargetView);
		pBackBuffer->Release();

		DirectX::WindowEx = (WNDPROC)LI_FN(SetWindowLongPtrA).safe_cached()(DirectX::Window, GWLP_WNDPROC, (LONG_PTR)WindowHandler);

		InitImGui();

		Menu::FirstTime = false;

		Menu::Style();

		return true;
	}

	return false;
}
bool AuthConnected = false;

char Licence[50] = "";
char UserName[20] = "";
char RgPassWord[20] = "";
char RgUserName[20] = "";
static int Tabs = 2;
static int Checks = 0;
HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (Menu::FirstTime)
	{

		if (!BindD3DInfo(pSwapChain))
			return DirectX::OriginalPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (SAFE_CALL(GetAsyncKeyState)(VK_INSERT) & 1)
	{

		Menu::Open = !Menu::Open;
	}
	if (SAFE_CALL(GetAsyncKeyState)(0x58) & 1)
	{
		Settings::Player::NoClip = !Settings::Player::NoClip;
	}
	ImGui::GetIO().MouseDrawCursor = Menu::Open;
	ImGui::GetIO().WantCaptureKeyboard = Menu::Open;
	if (AuthConnected)
	{
		

		Visuals::Hook();
		Players::Hook();
		if (!Settings::Aimbot::silentaim)
		{
			Aimbot::Hook();

		}
		if (Settings::Aimbot::silentaim && Settings::Aimbot::aimmousewhilesilent)
		{
			Aimbot::Hook();
		}
		if (Settings::Aimbot::Aimbot && Settings::Aimbot::silentaim)
		{
		//	Aimbot::aimbot_silent();

		}

		Weapon::Hook();
		NoClip::Hook();
		Vehicule::Hook();

		if (Settings::Aimbot::Draw_Fov)
		{

			//	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(1920 / 2, 1080 / 2), Option::AimbotFov, IM_COL32(255, 255, 255, 255), 40.f,1.f);
			ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(FiveM::WindowSize.x / 2, FiveM::WindowSize.y / 2), Settings::Aimbot::AimbotFov, IM_COL32(0, 0, 0, 140), 40.F);
		}

		if (Settings::Aimbot::crosshair)
		{
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(955, 540), ImVec2(965, 540), ImColor(255, 255, 255, 255), 1);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(960, 535), ImVec2(960, 545), ImColor(255, 255, 255, 255), 1);

		}
		POINT mouse;
		RECT rc = { 0 };
		GetWindowRect(DirectX::Window, &rc);

		if (Menu::Open)
		{
			//Player::OnlinePlayerHook();
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImColor(0, 0, 0, 100));
			//Snowflake::Update(snow, Snowflake::vec3(mouse.x, mouse.y)/*mouse x and y*/, Snowflake::vec3(rc.left, rc.top)/*hWnd x and y positions*/); // you can change a few things inside the update function
			Menu::Drawing();


		}
	}
	else
	{
		if (Menu::Open)
		{


			ImGui::SetNextWindowSize(ImVec2(350, 215));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12);

			ImGui::SetNextWindowBgAlpha(1.0f);


			if (ImGui::Begin(E("Auth"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
			{
				ImGui::SetCursorPosX(150);

				ImGui::SetCursorPosY(42);

		
			
				if (Tabs == 1)
				{


				
				}

				if (Tabs == 2)
				{

					std::string i2;
					i2 = PassWord;
					int caca = 1;

					IsValid = true;
					AuthConnected = true;
					caca = 2;
					ImGui::Spacing();

					ImGui::Separator();

					ImGui::SetCursorPosX( 30.5f);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9.5f);

					ImGui::InputText(E("Key##rg"), PassWord, IM_ARRAYSIZE(PassWord), ImGuiInputTextFlags_Password);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9.5f);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.5f);

					if (ImGui::Button(E("Login##Log"), ImVec2(320, 25)))
					{

						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, E("Connecting.. :)") });

					
						std::string i2;
						i2 = PassWord;
						int caca = 1;

						IsValid = true;
						AuthConnected = true;
						caca = 2;
						/*std::tuple<std::string, std::string, std::string> response = program::login(i2, userid, ProgramID, ProgramName.c_str(), ProgramEncryption);
						if (std::get<0>(response) == Response1)
						{
							IsValid = true;
							AuthConnected = true;
							caca = 2;
		
						}
						if (std::get<0>(response) == Response2)
						{

							IsValid = true;
							AuthConnected = true;
							caca = 3;

						}
						if(std::get<0>(response) != Response1)
						{
							IsValid = true;
							AuthConnected = true;
							caca = 3;
						}
						if (caca == 3)
						{
							Kouloumbou();

						}*/

					}
				}
				ImGui::PopStyleVar(5);
				ImGui::End();
			}

		}
	}


	ImGui::Render();

	DirectX::pContext->OMSetRenderTargets(1, &DirectX::renderTargetView, 0);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return DirectX::OriginalPresent(pSwapChain, SyncInterval, Flags);
}

bool Core::Init()
{
	std::this_thread::sleep_for(std::chrono::seconds(20)); // paste from hx 

	while (!DirectX::OverlayHooked)
	{
		if (Hook::Init())
		{
			DirectX::OverlayHooked = Hook::Present((void**)&DirectX::OriginalPresent, PresentHook);
		}
	}

	return true;
}
