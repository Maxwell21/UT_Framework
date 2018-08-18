/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "UT_FrameworkEditor.h"

// StateMachine
#include "StateMachineEditor.h"
#include "StateMachineBlueprintCompiler.h"
#include "StateMachineBlueprint.h"
#include "StateMachineGraphFactory.h"

// Styles
#include "FrameworkStyle.h"

// Sequencer
#include "ISequencerModule.h"
#include "FlipbookAnimationTrackEditor.h"

// AssetTools
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "AssetTypeCategories.h"
#include "AssetTypeActions_StateMachine.h"

// Debuggers
#include "GameplayDebugger.h"
#include "GDC_StateMachine.h"


#define LOCTEXT_NAMESPACE "FUT_FrameworkEditorModule"

void FUT_FrameworkEditorModule::StartupModule()
{
	// Styles
	FFrameworkStyle::Initialize();

	// Extenders 
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager());
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager());

	// Factories
	StateMachineGraphFactory = MakeShareable(new FStateMachineGraphFactory());
	StateMachineGraphPinConnectionFactory = MakeShareable(new FStateMachineGraphPinConnectionFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(StateMachineGraphFactory);
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(StateMachineGraphPinConnectionFactory);

	// Sequencer
	ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>("Sequencer");
	FlipbookTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FFlipbookAnimationTrackEditor::CreateTrackEditor));

	// Compilers
	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetCompilers().Add(this);

	// Debuggers
	#if WITH_GAMEPLAY_DEBUGGER
		//If the gameplay debugger is available, register the category and notify the editor about the changes
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.RegisterCategory("UmbraFramework", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGDC_StateMachine::MakeInstance), EGameplayDebuggerCategoryState::EnabledInGameAndSimulate);
		GameplayDebuggerModule.NotifyCategoriesChanged();
	#endif

	// AssetsActions
	this->RegisterAssetActions();
}

void FUT_FrameworkEditorModule::ShutdownModule()
{
	// Styles
	FFrameworkStyle::Shutdown();

	// Extenders
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();

	// Factories
	FEdGraphUtilities::UnregisterVisualNodeFactory(StateMachineGraphFactory);
	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(StateMachineGraphPinConnectionFactory);

	// Sequencer
	if (FModuleManager::Get().IsModuleLoaded("Sequencer"))
	{
		ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>("Sequencer");

		SequencerModule.UnRegisterTrackEditor(FlipbookTrackCreateEditorHandle);
	}

	// Debuggers
	#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("UmbraFramework");
	}
	#endif
}

TSharedRef<FAssetEditorToolkit> FUT_FrameworkEditorModule::CreateStateMachineEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UStateMachineBlueprint* Blueprint)
{
	TSharedRef<FStateMachineEditor> StateMachineEditor(new FStateMachineEditor());
	StateMachineEditor->InitEditor(Mode, InitToolkitHost, Blueprint);

	return StateMachineEditor;
}

void FUT_FrameworkEditorModule::RegisterAssetActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_StateMachine));
}

bool FUT_FrameworkEditorModule::CanCompile(const UBlueprint* Blueprint)
{
	return Cast<UStateMachineBlueprint>(Blueprint) != nullptr;
}

void FUT_FrameworkEditorModule::Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results, TArray<UObject *>* ObjLoaded)
{
	if (UStateMachineBlueprint* StateMachineBlueprint = CastChecked<UStateMachineBlueprint>(Blueprint))
	{
		FStateMachineBlueprintCompiler Compiler(StateMachineBlueprint, Results, CompileOptions, ObjLoaded);
		Compiler.Compile();
		check(Compiler.NewClass);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUT_FrameworkEditorModule, UT_FrameworkEditor)