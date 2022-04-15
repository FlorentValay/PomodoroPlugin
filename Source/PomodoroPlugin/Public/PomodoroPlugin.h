// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PomodoroEngine.h"
#include "PomodoroNotifier.h"

class FToolBarBuilder;
class FMenuBuilder;

class FPomodoroPluginModule final : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	/**
	 * @brief Pomodoro engine that will run the timer
	 */
	TSharedPtr<FPomodoroEngine> Engine;
	
	/**
	 * @brief Notifier that will display information related to Pomodoro Engine.
	 */
	TSharedPtr<FPomodoroNotifier> Notifier;

	TSharedPtr<class FUICommandList> PluginCommands;
	
	void RegisterMenus();
	
	/**
	 * @brief Function triggered when the plugin tab is spawned.
	 * @param SpawnTabArgs Argument given when requesting the tab.
	 * @return The content of the plugin tab.
	 */
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs) const;
	
	/**
	 * @brief Used to generate the informational part of plugin tab  
	 * @return The information part of the tab
	 */
	TSharedRef<class SBoxPanel> SpawnInfo() const;
	
	/**
	 * @brief Used to generate the button part of plugin tab  
	 * @return The button part of the tab
	 */
	TSharedRef<class SBoxPanel> SpawnButtons() const;
	
	/**
	 * @brief Used to generate the engine configuration part of plugin tab  
	 * @return The engine configuration part of the tab
	 */
	TSharedRef<class SBoxPanel> SpawnTimerConfig() const;

	/**
	 * @brief Used to generate the notifier configuration part of plugin tab  
	 * @return The notifier configuration part of the tab
	 */
	TSharedRef<class SBoxPanel> SpawnNotificationConfig() const;
};
