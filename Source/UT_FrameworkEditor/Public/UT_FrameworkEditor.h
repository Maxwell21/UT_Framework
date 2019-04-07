/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "KismetCompilerModule.h"

class UStateMachineBlueprint;
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

	TSharedRef<FAssetEditorToolkit> CreateStateMachineEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UStateMachineBlueprint* Blueprint);

	void RegisterAssetActions();

	/** IBlueprintCompiler implementation */
	virtual bool CanCompile(const UBlueprint* Blueprint) override;
	virtual void Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results) override;
	static TSharedPtr<FKismetCompilerContext> GetCompilerForBlueprint(UBlueprint* BP, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions);

	static inline FUT_FrameworkEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FUT_FrameworkEditorModule>("UT_FrameworkEditor");
	};
};