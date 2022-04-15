// Copyright Epic Games, Inc. All Rights Reserved.

#include "PomodoroPluginCommands.h"

#define LOCTEXT_NAMESPACE "FPomodoroPluginModule"

void FPomodoroPluginCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "PomodoroPlugin", "Bring up PomodoroPlugin window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
