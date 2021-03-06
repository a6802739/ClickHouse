#pragma once

#include <time.h>
#include <mutex>
#include <Poco/ScopedLock.h>
#include <common/Common.h>

#ifdef __APPLE__
#include <common/apple_rt.h>
#endif

/** Отличается от Poco::Stopwatch только тем, что использует clock_gettime вместо gettimeofday,
  * возвращает наносекунды вместо микросекунд, а также другими незначительными отличиями.
  */
class Stopwatch
{
public:
	/** CLOCK_MONOTONIC работает сравнительно эффективно (~15 млн. вызовов в сек.) и не приводит к системному вызову.
	  * Поставьте CLOCK_MONOTONIC_COARSE, если нужна больше производительность, но достаточно погрешности в несколько мс.
	  */
	Stopwatch(clockid_t clock_type_ = CLOCK_MONOTONIC) : clock_type(clock_type_) { restart(); }

	void start() 					{ setStart(); is_running = true; }
	void stop() 					{ updateElapsed(); is_running = false; }
	void restart() 					{ elapsed_ns = 0; start(); }
	UInt64 elapsed() const 			{ updateElapsed(); return elapsed_ns; }
	double elapsedSeconds() const	{ updateElapsed(); return static_cast<double>(elapsed_ns) / 1000000000ULL; }

private:
	mutable UInt64 start_ns;
	mutable UInt64 elapsed_ns;
	clockid_t clock_type;
	bool is_running;

	void setStart()
	{
		struct timespec ts;
		clock_gettime(clock_type, &ts);
		start_ns = ts.tv_sec * 1000000000ULL + ts.tv_nsec;
	}

	void updateElapsed() const
	{
		if (is_running)
		{
			struct timespec ts;
			clock_gettime(clock_type, &ts);
			UInt64 current_ns = ts.tv_sec * 1000000000ULL + ts.tv_nsec;
			elapsed_ns += current_ns - start_ns;
			start_ns = current_ns;
		}
	}
};


class StopwatchWithLock : public Stopwatch
{
public:
	/** If specified amount of time has passed and timer is not locked right now, then restarts timer and returns true.
	  * Otherwise returns false.
	  * This is done atomically.
	  */
	bool lockTestAndRestart(double seconds)
	{
		std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
		if (!lock.try_lock())
			return false;

		if (elapsedSeconds() >= seconds)
		{
			restart();
			return true;
		}
		else
			return false;
	}

	struct Lock
	{
		StopwatchWithLock * parent = nullptr;
		std::unique_lock<std::mutex> lock;

		Lock() {}

		operator bool() const { return parent != nullptr; }

		Lock(StopwatchWithLock * parent, std::unique_lock<std::mutex> && lock)
			: parent(parent), lock(std::move(lock))
		{
		}

		Lock(Lock &&) = default;

		~Lock()
		{
			if (parent)
				parent->restart();
		}
	};

	/** If specified amount of time has passed and timer is not locked right now, then returns Lock object,
	  *  which locks timer and, on destruction, restarts timer and releases the lock.
	  * Otherwise returns object, that is implicitly casting to false.
	  * This is done atomically.
	  *
	  * Usage:
	  * if (auto lock = timer.lockTestAndRestartAfter(1))
	  *		/// do some work, that must be done in one thread and not more frequently than each second.
	  */
	Lock lockTestAndRestartAfter(double seconds)
	{
		std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
		if (!lock.try_lock())
			return {};

		if (elapsedSeconds() >= seconds)
			return Lock(this, std::move(lock));

		return {};
	}

private:
	std::mutex mutex;
};
