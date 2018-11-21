#include "ShadowApp.h"
#include "Win32Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	ShadowMapApp sample(1280, 720, L"D3D12 SnowPlum");
	return Win32Application::Init(&sample, hInstance, showCmd);
}