#pragma once

extern QuantumV::Application* QuantumV::CreateApplication();

#ifdef QV_PLATFORM_WINDOWS
	#ifdef QV_DEBUG

	int main(int argc, char** argv) {
		QuantumV::Log::Init();
		auto app = QuantumV::CreateApplication();
		app->Run();
		delete app;
		return 0;
	}

	#else

	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, PSTR cmdLine, int nCmdShow) {
		QuantumV::Log::Init();
		auto app = QuantumV::CreateApplication();
		app->Run();
		delete app;
		return 0;
	}

	#endif
#else

	#error platforms other than windows are not supported for now
	int main(int argc, char** argv) {
		QuantumV::Log::Init();
		auto app = QuantumV::CreateApplication();
		app->Run();
		delete app;
		return 0;
	}

#endif