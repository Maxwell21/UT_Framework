/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"

class UNpcBehaviorBlueprint;
class FNpcBehaviorGraphFactory;

class FUT_FrameworkEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TSharedPtr<FNpcBehaviorGraphFactory> NpcBehaviorGraphFactory;

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

	void RegisterAssetActions();
	void RegisterNpcBehaviorTasks();

	static inline FUT_FrameworkEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FUT_FrameworkEditorModule>("UT_FrameworkEditor");
	};

};