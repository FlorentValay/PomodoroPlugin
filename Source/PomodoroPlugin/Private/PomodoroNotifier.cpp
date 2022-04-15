// Fill out your copyright notice in the Description page of Project Settings.


#include "PomodoroNotifier.h"

#include "PomodoroConfig.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FPomodoroNotifier"

FPomodoroNotifier::FPomodoroNotifier()
{
	WorkingMessages = TArray<FText>();
	WorkingMessages.Add(LOCTEXT("WorkingMessage1", "It's time to go back to work !"));
	WorkingMessages.Add(LOCTEXT("WorkingMessage2", "Let's go working !"));
	WorkingMessages.Add(LOCTEXT("WorkingMessage3", "Aw crap, here we go again..."));
    
	RestingMessages = TArray<FText>();
	RestingMessages.Add(LOCTEXT("RestingMessage1", "Take some rest !"));
	RestingMessages.Add(LOCTEXT("RestingMessage2", "Remember to drink !"));
	RestingMessages.Add(LOCTEXT("RestingMessage3", "You should go out !"));

	ElapsedTimespanHandleDelegate.BindRaw(this, &FPomodoroNotifier::Notify);
}

FPomodoroNotifier::~FPomodoroNotifier()
{
}

void FPomodoroNotifier::Notify(const bool bWorkingTime)
{
	// Play Sound
	if (ActivateSound == ECheckBoxState::Checked && GEditor)
	{
		GEditor->PlayEditorSound(TEXT("/PomodoroPlugin/BellRinging_Cue.BellRinging_Cue"));
	}
	
	// Prepare text to display depending on the parameter 
	FText TextToDisplay;
	if(bWorkingTime)
	{
		TextToDisplay = RestingMessages[FMath::RandRange(0, RestingMessages.Num() - 1)];
	}
	else
	{
		TextToDisplay = WorkingMessages[FMath::RandRange(0, WorkingMessages.Num() - 1)];
	}

	// Display the notification 
	FNotificationInfo Info(TextToDisplay);
	Info.FadeInDuration = 0.1f;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = 1.5f;
	Info.bUseThrobber = false;
	Info.bUseSuccessFailIcons = true;
	Info.bUseLargeFont = true;
	Info.bFireAndForget = false;
	Info.bAllowThrottleWhenFrameRateIsLow = false;
	const TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
	NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
	NotificationItem->ExpireAndFadeout();
}

void FPomodoroNotifier::SetNotificationSoundState(const ECheckBoxState NewValue)
{
	ActivateSound = NewValue;
}

ECheckBoxState FPomodoroNotifier::GetNotificationSoundState() const
{
	return ActivateSound;
}

void FPomodoroNotifier::ResetConfig()
{
	ActivateSound = ECheckBoxState::Checked;
}

void FPomodoroNotifier::ReloadConfig()
{
	TTuple<ECheckBoxState> Data = NewObject<UPomodoroConfig>()->LoadNotificationConfig();
	ActivateSound = Data.Get<0>();
}

void FPomodoroNotifier::SaveConfig() const
{
	NewObject<UPomodoroConfig>()->SaveNotificationConfig(ActivateSound);
}

#undef LOCTEXT_NAMESPACE
