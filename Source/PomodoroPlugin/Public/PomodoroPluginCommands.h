// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PomodoroPluginStyle.h"

class FPomodoroPluginCommands : public TCommands<FPomodoroPluginCommands>
{
public:

	FPomodoroPluginCommands()
		: TCommands<FPomodoroPluginCommands>(TEXT("PomodoroPlugin"), NSLOCTEXT("Contexts", "PomodoroPlugin", "PomodoroPlugin Plugin"), NAME_None, FPomodoroPluginStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};