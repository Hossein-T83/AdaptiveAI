// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpponentAdapatation.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogOpponentAdapatation)

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "GameplayDebuggerCategory_Adaptation.h"
#endif

class FOpponentAdapatationModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		UE_LOG(LogOpponentAdapatation, Warning, TEXT("StartupModule running"));
#if WITH_GAMEPLAY_DEBUGGER
		UE_LOG(LogOpponentAdapatation, Warning, TEXT("registering Adaptation category"));

		IGameplayDebugger& Debugger = IGameplayDebugger::Get();
		Debugger.RegisterCategory(TEXT("Adaptation"),
			IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Adaptation::MakeInstance),
			EGameplayDebuggerCategoryState::EnabledInGameAndSimulate);
		Debugger.NotifyCategoriesChanged();
#endif
	}

	virtual void ShutdownModule() override
	{
#if WITH_GAMEPLAY_DEBUGGER
		if (IGameplayDebugger::IsAvailable())
		{
			IGameplayDebugger::Get().UnregisterCategory(TEXT("Adaptation"));
		}
#endif
	}
};
IMPLEMENT_PRIMARY_GAME_MODULE(FOpponentAdapatationModule, OpponentAdapatation, "OpponentAdapatation");