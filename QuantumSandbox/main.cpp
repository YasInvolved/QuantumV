#include <QuantumV.h>
#include <QuantumV/resources/Mesh.h>
#include <iostream>

class QuantumSandbox : public QuantumV::Application {
public:
	QuantumSandbox() {
		// add gameobjects and listeners
		QuantumV::Ref<QuantumV::Mesh> cubeMesh = m_resourceManager->loadResource<QuantumV::Mesh>("cube", "C:\\Users\\YasInvolved\\Pictures\\cube.obj");
	}

	void OnEvent(QuantumV::IEvent* event) override {

	}

private:
};

QuantumV::Application* QuantumV::CreateApplication() {
	return new QuantumSandbox;
}