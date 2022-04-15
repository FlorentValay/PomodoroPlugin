// Fill out your copyright notice in the Description page of Project Settings.


#include "PomodoroConfig.h"

UPomodoroConfig::UPomodoroConfig()
{
	if(FPaths::FileExists(ConfigPath))
	{
		LoadConfig(StaticClass(), *ConfigPath);
	}
	else
	{
		WorkingTime = FTimespan(0, 20, 0);
		ShortRestingTimespan = FTimespan(0, 5, 0);
		LongRestingTimespan = FTimespan(0, 15, 0);
		CycleLength = 4;
		NotificationSound = true;
	}
}

void UPomodoroConfig::SaveEngineConfig(const FTimespan NewWorkingTimespan, const FTimespan NewShortRestingTimespan, const FTimespan NewLongRestingTimespan, const int32 NewCycleLenght)
{
	WorkingTime = NewWorkingTimespan;
	ShortRestingTimespan = NewShortRestingTimespan;
	LongRestingTimespan = NewLongRestingTimespan;
	CycleLength = NewCycleLenght;
	
	SaveConfig(CPF_Config, *ConfigPath);
}

TTuple<FTimespan, FTimespan, FTimespan, int32> UPomodoroConfig::LoadEngineConfig() const
{
	return TTuple<FTimespan, FTimespan, FTimespan, int32>(
		WorkingTime, ShortRestingTimespan, LongRestingTimespan, CycleLength);
}

void UPomodoroConfig::SaveNotificationConfig(const ECheckBoxState ActivateSoundNotification)
{
	NotificationSound = ActivateSoundNotification == ECheckBoxState::Checked;
	SaveConfig(CPF_Config, *ConfigPath);
}

TTuple<ECheckBoxState> UPomodoroConfig::LoadNotificationConfig() const
{
	ECheckBoxState SoundNotification;
	if(NotificationSound)
	{
		SoundNotification = ECheckBoxState::Checked;
	}
	else
	{
		SoundNotification = ECheckBoxState::Unchecked;
	}
	return TTuple<ECheckBoxState>(SoundNotification);
}

