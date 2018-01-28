/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "UT_FrameworkEditor.h"
#include "NpcEditor.h"
#include "NpcBehaviorBlueprint.h"
#include "NpcBehaviorGraphFactory.h"
#include "NpcBehaviorUtilities.h"

// Tasks
#include "NpcBehaviorTaskFactory_Wait.h"
#include "NpcBehaviorTaskFactory_MoveTo.h"
#include "NpcBehaviorTaskFactory_Restart.h"
#include "NpcBehaviorTaskFactory_RandomBranch.h"

// Styles
#include "FrameworkStyle.h"

// AssetTools
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "AssetTypeCategories.h"
#include "AssetTypeActions_NpcBehavior.h"

#define LOCTEXT_NAMESPACE "FUT_FrameworkEditorModule"

void FUT_FrameworkEditorModule::StartupModule()
{
	// Styles
	FFrameworkStyle::Initialize();

	// Extenders 
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager());
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager());

	// Factories
	NpcBehaviorGraphFactory = MakeShareable(new FNpcBehaviorGraphFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(NpcBehaviorGraphFactory);

	// NpcBehaviorTasks
	this->RegisterNpcBehaviorTasks();

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
	FEdGraphUtilities::UnregisterVisualNodeFactory(NpcBehaviorGraphFactory);
}

TSharedRef<FAssetEditorToolkit> FUT_FrameworkEditorModule::CreateNpcEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UNpcBehaviorBlueprint* Blueprint)
{
	TSharedRef<FNpcEditor> NpcEditor(new FNpcEditor());
	NpcEditor->InitEditor(Mode, InitToolkitHost, Blueprint);

	return NpcEditor;
}

void FUT_FrameworkEditorModule::RegisterAssetActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_NpcBehavior));
}

void FUT_FrameworkEditorModule::RegisterNpcBehaviorTasks()
{
	FNpcBehaviorUtilities::RegisterTask(MakeShareable(new FNpcBehaviorTaskFactory_Wait));
	FNpcBehaviorUtilities::RegisterTask(MakeShareable(new FNpcBehaviorTaskFactory_MoveTo));
	FNpcBehaviorUtilities::RegisterTask(MakeShareable(new FNpcBehaviorTaskFactory_Restart));
	FNpcBehaviorUtilities::RegisterTask(MakeShareable(new FNpcBehaviorTaskFactory_RandomBranch));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUT_FrameworkEditorModule, UT_FrameworkEditor)