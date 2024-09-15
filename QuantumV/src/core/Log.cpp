#include "QuantumV/core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <vector>

namespace QuantumV {
	Ref<spdlog::logger> Log::CoreLogger;
	Ref<spdlog::logger> Log::ClientLogger;

	void Log::Init() {
		std::vector<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("QuantumV.log", true));

		log_sinks[0]->set_pattern("%^[%T] <%n> %v%$");
		log_sinks[1]->set_pattern("%^[%T] <%n> %v%$");
		
		CoreLogger = std::make_shared<spdlog::logger>("QuantumCore", std::begin(log_sinks), std::end(log_sinks));
		spdlog::register_logger(CoreLogger);
		CoreLogger->set_level(spdlog::level::trace);
		CoreLogger->flush_on(spdlog::level::trace);

		ClientLogger = std::make_shared<spdlog::logger>("QuantumClient", std::begin(log_sinks), std::end(log_sinks));
		spdlog::register_logger(ClientLogger);
		ClientLogger->set_level(spdlog::level::trace);
		ClientLogger->flush_on(spdlog::level::trace);
	}
}