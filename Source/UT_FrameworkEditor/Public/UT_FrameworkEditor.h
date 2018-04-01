/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "KismetCompilerModule.h"

class UNpcBehaviorBlueprint;
class UStateMachineBlueprint;
class FNpcBehaviorGraphFactory;
class FStateMachineGraphFactory;
struct FStateMachineGraphPinConnectionFactory;

class FUT_FrameworkEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility, public IBlueprintCompiler
{

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TSharedPtr<FNpcBehaviorGraphFactory> NpcBehaviorGraphFactory;
	TSharedPtr<FStateMachineGraphFactory> StateMachineGraphFactory;
	TSharedPtr<FStateMachineGraphPinConnectionFactory> StateMachineGraphPinConnectionFactory;

	FDelegateHandle FlipbookTrackCreateEditorHandle;

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Gets the extensibility managers for outside entities to extend gui page editor's menus and toolbars */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; };
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; };

	TSharedRef<FAssetEditorToolkit> CreateNpcEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UNpcBehaviorBlueprint* Blueprint);
	TSharedRef<FAssetEditorToolkit> CreateStateMachineEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UStateMachineBlueprint* Blueprint);

	void RegisterAssetActions();
	void RegisterNpcBehaviorTasks();

	/** IBlueprintCompiler implementation */
	virtual bool CanCompile(const UBlueprint* Blueprint) override;
	virtual void Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results, TArray<UObject *>* ObjLoaded) override;

	static inline FUT_FrameworkEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FUT_FrameworkEditorModule>("UT_FrameworkEditor");
	};
};