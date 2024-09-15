#pragma once

extern QuantumV::Application* QuantumV::CreateApplication();

int main(int argc, char** argv) {
	QuantumV::Log::Init();
	auto app = QuantumV::CreateApplication();
	app->Run();
	delete app;
	return 0;
}