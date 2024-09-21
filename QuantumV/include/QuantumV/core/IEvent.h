#pragma once

namespace QuantumV {
	class IEvent {
	public:
		virtual ~IEvent() = default;
		virtual const char* GetName() const = 0;
	};
}