// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PomodoroConfig.generated.h"

/**
 * Used to save and load pomodoro configuration
 */
UCLASS(Config=Editor, HideDropdown)
class POMODOROPLUGIN_API UPomodoroConfig : public UObject
{
	GENERATED_BODY()

	public:
	/**
	* Default constructor
	*
	* Load last saved configuration from config file.
	*/
	UPomodoroConfig();

	
	/**
	* Save the given engine configuration into config file.
	* 
	* @param NewWorkingTimespan Duration of the working timespan
	* @param NewShortRestingTimespan Duration of the short resting timespan 
	* @param NewLongRestingTimespan Duration of the long resting timespan
	* @param NewCycleLenght Number of iteration
	*/
	void SaveEngineConfig(FTimespan NewWorkingTimespan, FTimespan NewShortRestingTimespan, FTimespan NewLongRestingTimespan,
			int32 NewCycleLenght);

	/**
	* Used to get the engine current configuration.
	* 
	* @return A tuple containing all the data in the given order :
	* - working timespan
	* - short resting timespan
	* - long resting timespan
	* - cycle length
	*/
	TTuple<FTimespan, FTimespan, FTimespan, int32> LoadEngineConfig() const;

	
	/**
	 * @brief Save the given notifier configuration into config file.
	 * @param ActivateSoundNotification Activation of notification sound
	 */
	void SaveNotificationConfig(ECheckBoxState ActivateSoundNotification);

	/**
	* Used to get the notifier current configuration.
	* 
	* @return A tuple containing all the data in the given order :
	* - Activation of notification sound
	*/
	TTuple<ECheckBoxState> LoadNotificationConfig() const;
	
	private:
	/** Path of the file used to save the config */
	const FString ConfigPath = (FPaths::ProjectConfigDir() + TEXT("PomodoroConfig.ini"));

	/** Working time span */
	UPROPERTY(Config)
	FTimespan WorkingTime;

	/** Short resting time span */
	UPROPERTY(Config)
	FTimespan ShortRestingTimespan;

	/** Long resting time span */
	UPROPERTY(Config)
	FTimespan LongRestingTimespan;

	/** Number of cycle iterations */
	UPROPERTY(Config)
	int32 CycleLength;

	/** Allowing the notification sound */
	UPROPERTY(config)
	bool NotificationSound;
};
