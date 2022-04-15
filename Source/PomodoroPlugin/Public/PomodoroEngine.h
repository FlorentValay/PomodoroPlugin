// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PomodoroState.h"

DECLARE_EVENT_OneParam(FPomodoroEngine, FTimespanElapsed, bool)

DECLARE_DELEGATE_OneParam(FElapsedTimespanHandleDelegate, bool)

/**
 * Controls the pomodoro behavior
 */
class POMODOROPLUGIN_API FPomodoroEngine final : public TSharedFromThis<FPomodoroEngine>
{
	public:
	/**
	 * @brief Standard constructor for FPomodoroEngine.
	 */
	FPomodoroEngine();
	
	/**
	 * @brief Standard destructor for FPomodoroEngine.
	 */
	~FPomodoroEngine();

	/**
	 * @brief Start the engine.
	 *
	 * The configuration is not allowed in this state.
	 */
	void Start();

	/**
	 * @brief Stop the engine.
	 *
	 * In this state the engine will start from the very beginning.
	 * The configuration is allowed in this state.
	 */
	void Stop();
	
	/**
	 * @brief Pause the engine.
	 *
	 * In this state the engine is ready to start where it was previously paused.
	 * Editing the configuration is not allowed in this state.
	 */
	void Pause();

	
	/**
	 * @brief Used to configure the number of cycle of engine
	 * @param NewCycleCount New number of cycle the engine will do before looping.
	 */
	void SetCycleCount(int32 NewCycleCount);

	/**
	 * @brief Used to configure the working timespan length
	 * @param Hour Hour part of the working timespan length.
	 * @param Minute Minute part of the working timespan length.
	 * @param Second Second part of the working timespan length.
	 */
	void SetWorkingTimespan(int32 Hour, int32 Minute, int32 Second);

	/**
	 * @brief Used to configure the short resting timespan length
	 * @param Hour Hour part of the short resting timespan length.
	 * @param Minute Minute part of the short resting timespan length.
	 * @param Second Second part of the short resting timespan length.
	 */
	void SetShortRestingTimespan(int32 Hour, int32 Minute, int32 Second);
	
	/**
	 * @brief Used to configure the long resting timespan length
	 * @param Hour Hour part of the long resting timespan length.
	 * @param Minute Minute part of the long resting timespan length.
	 * @param Second Second part of the long resting timespan length.
	 */
	void SetLongRestingTimespan(int32 Hour, int32 Minute, int32 Second);

	/**
	 * @brief Reset the engine configuration to standard config. 
	 */
	void ResetConfig();

	/**
	 * @brief Reload the last saved engine configuration. 
	 */
	void ReloadConfig();

	/**
	 * @brief Save the current engine configuration in file to be used again. 
	 */
	void SaveConfig() const;

	
	/**
	 * @brief Return a text representation of the remaining timespan.
	 * @return Text representation of the remaining timespan.
	 */
	FText GetTimerText() const;

	/**
	 * @brief Indicate current cycle the engine is running.
	 * @return Current cycle the engine is running.
	 */
	int32 GetCurrentCycle() const;
	
	/**
	 * @brief Indicate how many cycle the engine got (last one is used for long resting time)
	 * @return How many cycle the engine got.
	 */
	int32 GetCycleCount() const;

	/**
	 * @brief Give the working timespan
	 * @return A timespan representing the working timespan
	 */
	FTimespan GetWorkingTimespan() const;

	/**
	 * @brief Give the short resting timespan
	 * @return A timespan representing the short resting timespan
	 */
	FTimespan GetShortRestingTimespan() const;
	
	/**
	 * @brief Give the long resting timespan
	 * @return A timespan representing the long resting timespan
	 */
	FTimespan GetLongRestingTimespan() const;
	
	/**
	 * @brief Give the current state of the engine.
	 * @return An enum representing current state.
	 */
	EPomodoroState GetState() const;
	
	/**
	 * @brief Indicate if the current timespan is a working timespan.
	 * @return True if current timespan is a working timespan, otherwise false.
	 */
	bool IsWorkingTime() const;

	
	/**
	 * @brief Used to bind object to TimespanElapsed event.
	 * @param Delegate The delegate generated from the bound object.
	 */
	void BindOnTimeSpanElapsed(FElapsedTimespanHandleDelegate Delegate);

private:

	/**
	 * @brief Current cycle the engine is running
	 */
	int CurrentCycle;
	
	/**
	 * @brief Indicate how many cycle the engine got (last one is used for long resting time)
	 */
	int CycleCount;

	/**
	 * @brief Indicate if the current timespan is a working timespan
	 */
	bool WorkingTime;

	/**
	 * @brief Indicate the current state of the timer.
	 */
	EPomodoroState State;

	/**
	 * @brief Handle used to control engine start, stop and pause.
	 */
	FTimerHandle TimerHandle;
	
	/**
	 * @brief Delegate for TimespanElapsed event.
	 */
	FTimespanElapsed ElapsedTimespanHandle;

	/**
	 * @brief Indicate how many time remains before the timespan end.
	 */
	FTimespan RemainingTimespan;

	/**
	 * @brief Text representation of RemainingTimespan. 
	 */
	FText TimerText;

	/**
	 * @brief Represent the lenght of working timespan.
	 */
	FTimespan WorkingTimespan;

	/**
	 * @brief Represent the lenght of short resting timespan.
	 */
	FTimespan ShortRestingTimespan;
	
	/**
	 * @brief Represent the lenght of long resting timespan.
	 */
	FTimespan LongRestingTimespan;
	
	
	/**
	 * @brief Triggered every second to update engine.
	 */
	void OnTick();
	
	/**
	 * @brief Called when the timespan is elapsed.
	 *
	 * This function will update the pomodoro to use the appropriate next timespan.
	 * Also it will trigger all the event that relate on elapsed timespan event.
	 */
	void OnElapsedTimespan();

	/**
	 * @brief Called to update the Timer text
	 */
	void UpdateTimerText();
};
