#include <QuantumV.h>
#include <QuantumV/events/KeyboardEvent.h>
#include <iostream>

class QuantumSandbox : public QuantumV::Application {
public:
	QuantumSandbox() {
		// add gameobjects and listeners
	}

	void OnEvent(QuantumV::IEvent* event) override {
		if (auto* keyboardEvent = dynamic_cast<QuantumV::KeyboardEvent*>(event)) {
			handleKeyboardEvent(keyboardEvent);
		}
	}

private:
	void handleKeyboardEvent(QuantumV::KeyboardEvent* event) {
		QV_CLIENT_TRACE("{} has been {}", event->GetScancode(), event->GetIsPressed() ? "pressed" : "released");
	}
};

QuantumV::Application* QuantumV::CreateApplication() {
	return new QuantumSandbox;
}