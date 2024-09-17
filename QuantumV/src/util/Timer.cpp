#include "Timer.h"

namespace QuantumV::Utils {
	void Timer::Start() {
		if (!isRunning) {
			startTime = Clock::now();
			isRunning = true;
		}
	}

	void Timer::Stop() {
		if (isRunning) {
			stopTime = Clock::now();
			isRunning = false;
		}
	}

	float Timer::GetElapsedTime() const {
		TimePoint endTime = isRunning ? Clock::now() : stopTime;
		std::chrono::duration<float> elapsed = endTime - startTime;

		return elapsed.count();
	}
}