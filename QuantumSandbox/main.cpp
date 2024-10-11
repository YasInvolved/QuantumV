#include <QuantumV.h>
#include <iostream>

class QuantumSandbox : public QuantumV::Application {
public:
	QuantumSandbox() {
		// add gameobjects and listeners
	}

	void OnEvent(QuantumV::IEvent* event) override {
	}

private:
};

QuantumV::Application* QuantumV::CreateApplication() {
	return new QuantumSandbox;
}