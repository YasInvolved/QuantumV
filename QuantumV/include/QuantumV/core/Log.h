#pragma once

#include "Base.h"
#include <spdlog/spdlog.h>

namespace QuantumV {
	class QV_API Log {
	public:
		static void Init();

		static Ref<spdlog::logger>& getCoreLogger() { return CoreLogger; }
		static Ref<spdlog::logger>& getClientLogger() { return ClientLogger; }
	private:
		static Ref<spdlog::logger> CoreLogger;
		static Ref<spdlog::logger> ClientLogger;
	};
}

// core functions
#define QV_CORE_TRACE(...)		::QuantumV::Log::getCoreLogger()->trace(__VA_ARGS__)
#define QV_CORE_WARN(...)		::QuantumV::Log::getCoreLogger()->warn(__VA_ARGS__)
#define QV_CORE_ERROR(...)		::QuantumV::Log::getCoreLogger()->error(__VA_ARGS__)
#define QV_CORE_CRITICAL(...)	::QuantumV::Log::getCoreLogger()->critical(__VA_ARGS__)

// client functions
#define QV_CLIENT_TRACE(...)	::QuantumV::Log::getClientLogger()->trace(__VA_ARGS__)
#define QV_CLIENT_WARN(...)		::QuantumV::Log::getClientLogger()->warn(__VA_ARGS__)
#define QV_CLIENT_ERROR(...)	::QuantumV::Log::getClientLogger()->error(__VA_ARGS__)
#define QV_CLIENT_CRITICAL(...)	::QuantumV::Log::getClientLogger()->critical(__VA_ARGS__)