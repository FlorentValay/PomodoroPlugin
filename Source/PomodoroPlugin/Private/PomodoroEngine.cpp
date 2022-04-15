// Fill out your copyright notice in the Description page of Project Settings.


#include "PomodoroEngine.h"
#include "PomodoroConfig.h"

#define LOCTEXT_NAMESPACE "FPomodoroPluginModule"

FPomodoroEngine::FPomodoroEngine()
{
	ReloadConfig();
	
	CurrentCycle = 0;
	RemainingTimespan = FTimespan::Zero();
	WorkingTime = false;
	State = Stopped;

	UpdateTimerText();
}

FPomodoroEngine::~FPomodoroEngine()
{
	Stop();
	ElapsedTimespanHandle.Clear();
}

void FPomodoroEngine::Start()
{
	// If pomodoro is already running, do nothing 
	if(State == Running)
	{
		return;
	}

	// If Editor is valid
	if(IsValid(GEditor))
	{
		FTimerDelegate Delegate;
		Delegate.BindRaw(this, &FPomodoroEngine::OnTick);
		
		// If the previous state was "Stopped"
		if(State == Stopped)
		{
			CurrentCycle = 0;
			WorkingTime = true;
			RemainingTimespan = WorkingTimespan;
			UpdateTimerText();
			GEditor->GetTimerManager()->SetTimer(TimerHandle, Delegate, 1, true, 1);
		}
		
		// If the previous state was : "Paused"
		else
		{
			GEditor->GetTimerManager()->SetTimer(TimerHandle, Delegate, 1, true, 0);
		}
		
		State = Running;
	}
}

void FPomodoroEngine::Stop()
{
	// If pomodoro is already stopped, do nothing
	if(State == Stopped)
	{
		return;
	}

	// If Editor is valid
	if(IsValid(GEditor))
	{
		GEditor->GetTimerManager()->ClearTimer(TimerHandle);
		RemainingTimespan = FTimespan::Zero();
		UpdateTimerText();
		State = Stopped;
	}
}

void FPomodoroEngine::Pause()
{
	// If pomodoro is already paused, do nothing
	if(State == Paused)
	{
		return;
	}

	// If Editor is valid
	if(IsValid(GEditor))
	{
		GEditor->GetTimerManager()->ClearTimer(TimerHandle);
		State = Paused;
	}
}

void FPomodoroEngine::SetCycleCount(const int32 NewCycleCount)
{
	CycleCount = NewCycleCount;
}

void FPomodoroEngine::SetWorkingTimespan(const int32 Hour, const int32 Minute, const int32 Second)
{
	WorkingTimespan = FTimespan(Hour, Minute, Second);
}

void FPomodoroEngine::SetShortRestingTimespan(const int32 Hour, const int32 Minute, const int32 Second)
{
	ShortRestingTimespan = FTimespan(Hour, Minute, Second);
}

void FPomodoroEngine::SetLongRestingTimespan(const int32 Hour, const int32 Minute, const int32 Second)
{
	LongRestingTimespan = FTimespan(Hour, Minute, Second);
}

void FPomodoroEngine::ResetConfig()
{
	CycleCount = 4;
	WorkingTimespan = FTimespan::FromMinutes(25);
	ShortRestingTimespan = FTimespan::FromMinutes(5);
	LongRestingTimespan = FTimespan::FromMinutes(20);
}

void FPomodoroEngine::ReloadConfig()
{
	TTuple<FTimespan, FTimespan, FTimespan, int32> Data = NewObject<UPomodoroConfig>()->LoadEngineConfig();

	WorkingTimespan = Data.Get<0>();
	ShortRestingTimespan = Data.Get<1>();
	LongRestingTimespan = Data.Get<2>();
	CycleCount = Data.Get<3>();
}

void FPomodoroEngine::SaveConfig() const
{
	NewObject<UPomodoroConfig>()->SaveEngineConfig(WorkingTimespan, ShortRestingTimespan, LongRestingTimespan, CycleCount);
}

FText FPomodoroEngine::GetTimerText() const
{
	return TimerText;
}

int32 FPomodoroEngine::GetCurrentCycle() const
{
	return CurrentCycle + 1;
}

int32 FPomodoroEngine::GetCycleCount() const
{
	return CycleCount;
}

FTimespan FPomodoroEngine::GetWorkingTimespan() const
{
	return WorkingTimespan;
}

FTimespan FPomodoroEngine::GetShortRestingTimespan() const
{
	return ShortRestingTimespan;
}

FTimespan FPomodoroEngine::GetLongRestingTimespan() const
{
	return LongRestingTimespan;
}

EPomodoroState FPomodoroEngine::GetState() const
{
	return State;
}

bool FPomodoroEngine::IsWorkingTime() const
{
	return WorkingTime;
}

void FPomodoroEngine::BindOnTimeSpanElapsed(const FElapsedTimespanHandleDelegate Delegate)
{
	ElapsedTimespanHandle.Add(Delegate);
}

void FPomodoroEngine::OnTick()
{
	// Update remaining timespan
	RemainingTimespan -= FTimespan::FromSeconds(1);

	// If current timespan is elapsed
	if(RemainingTimespan < FTimespan::Zero())
	{
		OnElapsedTimespan();
	}

	UpdateTimerText();
}

void FPomodoroEngine::OnElapsedTimespan()
{
	// If the elapsed timespan was a working timespan
	if(WorkingTime)
	{
		// If on the last cycle, use a long resting time
		if(CurrentCycle == CycleCount - 1)
		{
			RemainingTimespan = LongRestingTimespan;
		}
		// Otherwise, use a short resting time 
		else
		{
			RemainingTimespan = ShortRestingTimespan;
		}
	}
	// If the elapsed timespan was a resting timespan
	else
	{
		CurrentCycle = (CurrentCycle + 1) % CycleCount;
		RemainingTimespan = WorkingTimespan;
	}
	ElapsedTimespanHandle.Broadcast(WorkingTime);
	WorkingTime = !WorkingTime;
}

void FPomodoroEngine::UpdateTimerText()
{
	TimerText = FText::Format(FTextFormat::FromString(TEXT("{0} : {1} : {2}")),
		RemainingTimespan.GetHours() < 10 ?
			FText::Format(FText::FromString(TEXT("0{0}")), RemainingTimespan.GetHours()) :
			FText::Format(FText::FromString(TEXT("{0}")), RemainingTimespan.GetHours()),
		RemainingTimespan.GetMinutes() < 10 ?
			FText::Format(FText::FromString(TEXT("0{0}")), RemainingTimespan.GetMinutes()) :
			FText::Format(FText::FromString(TEXT("{0}")), RemainingTimespan.GetMinutes()),
		RemainingTimespan.GetSeconds() < 10 ?
			FText::Format(FText::FromString(TEXT("0{0}")), RemainingTimespan.GetSeconds()) :
			FText::Format(FText::FromString(TEXT("{0}")), RemainingTimespan.GetSeconds())
	);
}

#undef LOCTEXT_NAMESPACE