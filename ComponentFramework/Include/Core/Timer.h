#ifndef TIMER_H
#define TIMER_H
#include <SDL3/SDL.h>

class Timer {
private:	
	unsigned int prevTicks;
	unsigned int currentTicks;
	static unsigned int singleEventID;

public:
	Timer();
	~Timer();

	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;

	void Start();
	void UpdateFrameTicks();
	float GetDeltaTime() const;
	unsigned int GetSleepTime(const unsigned int fps_) const;
	float GetCurrentTicks() const;
};
#endif
