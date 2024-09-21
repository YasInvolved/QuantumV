#pragma once

#include <QuantumV/core/Base.h>

namespace QuantumV {
	class QV_API IEvent {
	public:
		virtual ~IEvent() = default;

		virtual void OnEvent() {};
	};
}