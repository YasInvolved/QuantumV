#include <chrono>

namespace QuantumV::Utils {
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	class Timer {
	public:
		Timer() : isRunning(false) {}

		void Start();
		void Stop();
		float GetElapsedTime() const;
	private:
		Clock clock;
		TimePoint startTime;
		TimePoint stopTime;
		bool isRunning;
	};
}