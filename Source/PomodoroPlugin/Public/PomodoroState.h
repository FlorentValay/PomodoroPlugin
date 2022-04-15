#pragma once

/**
 * @brief Represent timer state
 */
enum EPomodoroState : uint8
{
	/** Timer is currently stopped */
	Stopped = 0,
	
	/** Timer is currently paused */
	Paused = 1,

	/** Timer is currently running */
	Running = 2,
};
