#include <QuantumV.h>
#include <iostream>

class QuantumSandbox : public QuantumV::Application {
public:
	QuantumSandbox() {
		// add gameobjects and listeners
	}
};

QuantumV::Application* QuantumV::CreateApplication() {
	return new QuantumSandbox;
}