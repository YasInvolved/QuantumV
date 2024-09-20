#include "Timer.h"

namespace QuantumV::Utils {
	void Timer::Start() {
		if (!m_isStarted) {
			m_isStarted = true;
			m_startTime = Clock::now();
			m_lastTime = m_startTime;
		}
	}

	void Timer::Update() {
		if (m_isStarted) {
			m_lastTime = Clock::now();
		}
	}

	float Timer::GetDeltaTime() const {
		if (m_isStarted) {
			TimePoint now = Clock::now();
			Duration deltaTime = now - m_lastTime;

			return deltaTime.count();
		}
		else {
			return 0;
		}
	}

	float Timer::Stop() {
		if (m_isStarted) {
			m_stopTime = Clock::now();
			Duration timeElapsed = m_stopTime - m_startTime;
			m_isStarted = false;

			return timeElapsed.count();
		}
		else {
			return 0;
		}
	}
}