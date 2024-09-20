#include <chrono>

namespace QuantumV::Utils {
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<float>;

	class Timer {
	public:
		void Start();
		void Update();
		float Stop();
		float GetDeltaTime() const;
	private:
		TimePoint m_startTime;
		TimePoint m_lastTime;
		TimePoint m_stopTime;
		bool m_isStarted = false;
	};
}