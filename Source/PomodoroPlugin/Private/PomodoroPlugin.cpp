// Copyright Epic Games, Inc. All Rights Reserved.

#include "PomodoroPlugin.h"
#include "PomodoroPluginStyle.h"
#include "PomodoroPluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SSpinBox.h"
#include "ToolMenus.h"
#include "Misc/FileHelper.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName PomodoroPluginTabName("PomodoroPlugin");

#define LOCTEXT_NAMESPACE "FPomodoroPluginModule"

void FPomodoroPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPomodoroPluginStyle::Initialize();
	FPomodoroPluginStyle::ReloadTextures();

	FPomodoroPluginCommands::Register();
	
	Engine = MakeShared<FPomodoroEngine>();

	Notifier = MakeShared<FPomodoroNotifier>();
	Engine->BindOnTimeSpanElapsed(Notifier->ElapsedTimespanHandleDelegate);
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPomodoroPluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPomodoroPluginModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPomodoroPluginModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PomodoroPluginTabName, FOnSpawnTab::CreateRaw(this, &FPomodoroPluginModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPomodoroPluginTabTitle", "PomodoroPlugin"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FPomodoroPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPomodoroPluginStyle::Shutdown();

	FPomodoroPluginCommands::Unregister();
	
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PomodoroPluginTabName);
}

TSharedRef<SDockTab> FPomodoroPluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs) const
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		.ShouldAutosize(true)
		[
			// Put your tab content here!
			SNew(SBox)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SpawnInfo()
				]
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.Padding(FMargin(0, 5))
				[
					SNew(SBorder)
					.Padding(FMargin(10))
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0.0f, 0.0f, 0.0f, 5.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("ControlLabel","Timer control"))
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SpawnButtons()
						]
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(0, 5))
				[
					SNew(SBorder)
					.Padding(FMargin(10))
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0.0f, 0.0f, 0.0f, 5.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("EngineOptionLabel","Timer options"))
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SpawnTimerConfig()
						]
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(0, 5))
				[
					SNew(SBorder)
					.Padding(FMargin(10))
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0.0f, 0.0f, 0.0f, 5.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("NotificationOptionLabel","Notification options"))
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SpawnNotificationConfig()
						]
					]
				]
			]
		];
}

TSharedRef<SBoxPanel> FPomodoroPluginModule::SpawnInfo() const
{
	return SNew(SHorizontalBox)

	+SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(10,0)
	.HAlign(HAlign_Left)
	[
		SNew(STextBlock)
		.Text_Lambda([this]()
		{
			const EPomodoroState State = Engine->GetState();
			switch (State)
			{
			case Stopped:
				return LOCTEXT("StoppedState", "Stopped");
				
			case Paused:
				return LOCTEXT("PausedState", "Paused");
				
			case Running:
				return LOCTEXT("RunningState", "Running");

			default:
				return FText::FromString(TEXT("Error"));
			}
		})
	]
	
	+SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(10,0)
	.HAlign(HAlign_Center)
	[
		SNew(STextBlock)
		.Text_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				return Engine->GetTimerText();
			}
			return FText::FromString(TEXT("Can't load Pomodoro Engine"));
		})
	]

	+SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(10,0)
	.HAlign(HAlign_Right)
	[
		SNew(STextBlock)
		.Text_Lambda([this]()
		{
			if(Engine->IsWorkingTime())
			{
				return FText::Format(FTextFormat::FromString(TEXT("{0} : {1} / {2}")),
					LOCTEXT("WorkingTimeText", "Working Time "),
					Engine->GetCurrentCycle(),
					Engine->GetCycleCount());
			}
			return FText::Format(FTextFormat::FromString(TEXT("{0} : {1} / {2}")),
				LOCTEXT("RestingTimeText", "Resting Time "),
				Engine->GetCurrentCycle(),
				Engine->GetCycleCount());
		})
	];
}

TSharedRef<SBoxPanel> FPomodoroPluginModule::SpawnButtons() const
{
	return SNew(SHorizontalBox)

	+ SHorizontalBox::Slot()
	.FillWidth(1)
	.HAlign(HAlign_Left)
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonStart","Start"))
		.OnClicked_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				Engine->Start();
				return FReply::Handled();
			}
			return FReply::Unhandled();
			
		})
		.IsEnabled_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				return Engine->GetState() != Running;
			}
			return false;
		})
	]

	+ SHorizontalBox::Slot()
	.FillWidth(1)
	.HAlign(HAlign_Center)
	
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonPause","Pause"))
		.OnClicked_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				Engine->Pause();
				return FReply::Handled();
			}
			return FReply::Unhandled();	
		})
		.IsEnabled_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				return Engine->GetState() != Paused;
			}
			return false;
		})
	]

	+ SHorizontalBox::Slot()
	.FillWidth(1)
	.HAlign(HAlign_Right)
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonStop","Stop"))
		.OnClicked_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				Engine->Stop();
				return FReply::Handled();
			}
			return FReply::Unhandled();;
		})
		.IsEnabled_Lambda([this]()
		{
			if(Engine.IsValid())
			{
				return Engine->GetState() != Stopped;
			}
			return false;
		})
	];
}

TSharedRef<SBoxPanel> FPomodoroPluginModule::SpawnTimerConfig() const
{
	return SNew(SVerticalBox)
	
		// Cycle Length parameter
		+SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(0,5)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1)
			[
				SNew(STextBlock)
				.Margin(FMargin(0,3,10,3))
				.Text(LOCTEXT("CycleLength","Cycle length"))
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetCycleCount();
					}
					return 0;
				})
				.MinValue(1)
				.MaxValue(99)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						Engine->SetCycleCount(NewValue);
					}
				})
			]
		]

		// Working timespan parameter
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0,5)
		[
			SNew(SHorizontalBox)
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.FillWidth(1)
			[
				SNew(STextBlock)
				.Margin(FMargin(0,3,10,3))
				.Text(LOCTEXT("WorkingTimeSpanLength","Working Timespan Length"))
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetWorkingTimespan().GetHours();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(23)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetWorkingTimespan();
						Engine->SetWorkingTimespan(NewValue, Timespan.GetMinutes(), Timespan.GetSeconds());
					}
				})
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Margin(FMargin(5,3,5,3))
				.Text(FText::FromString(TEXT(" : ")))
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetWorkingTimespan().GetMinutes();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(59)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetWorkingTimespan();
						Engine->SetWorkingTimespan(Timespan.GetHours(), NewValue, Timespan.GetSeconds());
					}
				})
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Margin(FMargin(5,3,5,3))
				.Text(FText::FromString(TEXT(" : ")))
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetWorkingTimespan().GetSeconds();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(59)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetWorkingTimespan();
						Engine->SetWorkingTimespan(Timespan.GetHours(), Timespan.GetMinutes(), NewValue);
					}
				})
			]
		]

		// Short resting timespan parameter
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0,5)
		[
			SNew(SHorizontalBox)
		
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.FillWidth(1)
			[
				SNew(STextBlock)
				.Margin(FMargin(0,3,10,3))
				.Text(LOCTEXT("ShortRestingTimeSpanLength","Short Resting Timespan Length"))
			]
		
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetShortRestingTimespan().GetHours();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(23)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetShortRestingTimespan();
						Engine->SetShortRestingTimespan(NewValue, Timespan.GetMinutes(), Timespan.GetSeconds());
					}
				})
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Margin(FMargin(5,3,5,3))
				.Text(FText::FromString(TEXT(" : ")))
			]
		
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetShortRestingTimespan().GetMinutes();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(59)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetShortRestingTimespan();
						Engine->SetShortRestingTimespan(Timespan.GetHours(), NewValue, Timespan.GetSeconds());
					}
				})
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Margin(FMargin(5,3,5,3))
				.Text(FText::FromString(TEXT(" : ")))
			]
		
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetShortRestingTimespan().GetSeconds();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(59)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetShortRestingTimespan();
						Engine->SetShortRestingTimespan(Timespan.GetHours(), Timespan.GetMinutes(), NewValue);
					}
				})
			]
		]

		// Long Resting Timespan Length
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0,5)
		[
			SNew(SHorizontalBox)
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.FillWidth(1)
			[
				SNew(STextBlock)
				.Margin(FMargin(0,3,10,3))
				.Text(LOCTEXT("LongRestingTimeSpanLength","Long Resting Timespan Length"))
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetLongRestingTimespan().GetHours();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(23)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetLongRestingTimespan();
						Engine->SetLongRestingTimespan(NewValue, Timespan.GetMinutes(), Timespan.GetSeconds());
					}
				})
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Margin(FMargin(5,3,5,3))
				.Text(FText::FromString(TEXT(" : ")))
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetLongRestingTimespan().GetMinutes();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(59)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetLongRestingTimespan();
						Engine->SetLongRestingTimespan(Timespan.GetHours(), NewValue, Timespan.GetSeconds());
					}
				})
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Margin(FMargin(5,3,5,3))
				.Text(FText::FromString(TEXT(" : ")))
			]
	
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetLongRestingTimespan().GetSeconds();
					}
					return 0;
				})
				.MinValue(0)
				.MaxValue(59)
				.MinDesiredWidth(27)
				.IsEnabled_Lambda([this]()
				{
					if(Engine.IsValid())
					{
						return Engine->GetState() == Stopped;
					}
					return false;
				})
				.OnValueChanged_Lambda([this](const int32 NewValue)
				{
					if(Engine.IsValid())
					{
						const FTimespan Timespan = Engine->GetLongRestingTimespan();
						Engine->SetLongRestingTimespan(Timespan.GetHours(), Timespan.GetMinutes(), NewValue);	
					}
				})
			]
			
		]

	+SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				FMessageDialog Dialog;
				if(Dialog.Open(EAppMsgType::YesNo, LOCTEXT("ReloadConfigMessage", "Do you want to reload the pomodoro configuration from save ?")) == EAppReturnType::Yes)
				{
					if(Engine.IsValid())
					{
						Engine->ReloadConfig();
						return FReply::Handled();
					}
					return FReply::Unhandled();
				}
				return FReply::Handled();
			})
			.Text(LOCTEXT("ReloadConfigButton", "Reload Configuration"))
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("ResetConfigButton", "Reset Configuration"))
			.OnClicked_Lambda([this]()
			{
				FMessageDialog Dialog;
				if(Dialog.Open(EAppMsgType::YesNo, LOCTEXT("ResetConfigMessage", "Do you want to reset the pomodoro configuration ?")) == EAppReturnType::Yes)
				{
					if(Engine.IsValid())
					{
						Engine->ResetConfig();
						return FReply::Handled();
					}
					return FReply::Unhandled();
				}
				return FReply::Handled();
			})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("SaveConfigButton", "Save Configuration"))
			.OnClicked_Lambda([this]()
			{
				if(Engine.IsValid())
				{
					Engine->SaveConfig();
					return FReply::Handled();
				}
				return FReply::Unhandled();
			})
		]
	];
}

TSharedRef<SBoxPanel> FPomodoroPluginModule::SpawnNotificationConfig() const
{
	return SNew(SVerticalBox)

	// Cycle Length parameter
	+ SVerticalBox::Slot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.Padding(0,5)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ActivateSoundLabel","Activate notification sound"))
		]
		+SHorizontalBox::Slot()
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([this]()
			{
				return Notifier->GetNotificationSoundState();
			})
			.OnCheckStateChanged_Lambda([this](const ECheckBoxState Value)
			{
				Notifier->SetNotificationSoundState(Value);
			})
		]
	]

	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				FMessageDialog Dialog;
				if(Dialog.Open(EAppMsgType::YesNo, LOCTEXT("ReloadConfigMessage", "Do you want to reload the pomodoro notifier configuration from save ?")) == EAppReturnType::Yes)
				{
					if(Notifier.IsValid())
					{
						Notifier->ReloadConfig();
						return FReply::Handled();
					}
					return FReply::Unhandled();
				}
				return FReply::Handled();
			})
			.Text(LOCTEXT("ReloadConfigButton", "Reload Configuration"))
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("ResetConfigButton", "Reset Configuration"))
			.OnClicked_Lambda([this]()
			{
				FMessageDialog Dialog;
				if(Dialog.Open(EAppMsgType::YesNo, LOCTEXT("ResetConfigMessage", "Do you want to reset the pomodoro notifier configuration ?")) == EAppReturnType::Yes)
				{
					if(Notifier.IsValid())
					{
						Notifier->ResetConfig();
						return FReply::Handled();
					}
					return FReply::Unhandled();
				}
				return FReply::Handled();
			})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("SaveConfigButton", "Save Configuration"))
			.OnClicked_Lambda([this]()
			{
				if(Notifier.IsValid())
				{
					Notifier->SaveConfig();
					return FReply::Handled();
				}
				return FReply::Unhandled();
			})
		]
	];
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FPomodoroPluginModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(PomodoroPluginTabName);
}

void FPomodoroPluginModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		Section.AddMenuEntryWithCommandList(FPomodoroPluginCommands::Get().OpenPluginWindow, PluginCommands);
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
		FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPomodoroPluginCommands::Get().OpenPluginWindow));
		Entry.SetCommandList(PluginCommands);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPomodoroPluginModule, PomodoroPlugin)