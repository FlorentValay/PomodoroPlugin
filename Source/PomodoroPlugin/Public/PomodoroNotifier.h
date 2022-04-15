// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PomodoroEngine.h"

/**
 * Notifier used to announce when a timespan has ended
 */
class POMODOROPLUGIN_API FPomodoroNotifier final : public TSharedFromThis<FPomodoroNotifier>
{
public:

	/**
	 * @brief Delegate used to detect when timespan is elapsed
	 */
	FElapsedTimespanHandleDelegate ElapsedTimespanHandleDelegate;	
	
	/**
	 * @brief Standard constructor for FPomodoroNotifier
	 */
	FPomodoroNotifier();
	
	/**
	 * @brief Standard destructor for FPomodoroNotifier
	 */
	~FPomodoroNotifier();
	
	/**
	 * @brief Launch a notification indicating that the timespan
	 * has ended with a message and a sound
	 * @param bWorkingTime Was the timespan a working timespan 
	 */
	void Notify(const bool bWorkingTime);
	
	/**
	 * @brief Set the state of sound notification
	 * @param NewValue The new state as folowed :
	 * Checked		-> Sound is activated
	 * Unchecked	-> Sound is not activated
	 * Undefined	-> An error has occured
	 */
	void SetNotificationSoundState(const ECheckBoxState NewValue);
	
	/**
	 * @brief Used to get the current state of sound notification
	 * @return A CheckBoxState representing the state of the sound notification :
	 * Checked		-> Sound is activated
	 * Unchecked	-> Sound is not activated
	 * Undefined	-> An error has occured
	 */
	ECheckBoxState GetNotificationSoundState() const;

	/**
	 * @brief Reset the current configuration of this notifier
	 */
	void ResetConfig();

	/**
	 * @brief Reload the current configuration of this notifier to the last one saved
	 */
	void ReloadConfig();
	
	/**
	 * @brief Save the current configuration of this notifier
	 */
	void SaveConfig() const;
	
private:
	
	/**
	 * @brief Some cheering message to go back to work.
	 */
	TArray<FText> WorkingMessages;

	/**
	 * @brief Some cheering message to go rest after a working timespan
	 */
	TArray<FText> RestingMessages;

	/**
	 * @brief Used to know if the sound is activated for notifications
	 */
	ECheckBoxState ActivateSound = ECheckBoxState::Checked;
};
