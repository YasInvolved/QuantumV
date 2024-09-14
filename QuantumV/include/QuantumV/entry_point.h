#pragma once

extern QuantumV::Application* QuantumV::CreateApplication();

int main(int argc, char** argv) {
	auto app = QuantumV::CreateApplication();
	app->Run();
	delete app;
	return 0;
}