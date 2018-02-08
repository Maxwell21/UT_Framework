/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "BlueprintEditor.h"

class UStateMachineBlueprint;
class UStateMachineGraph;
class IDetailsView;
class SDockableTab;
class UEdGraph;

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API FStateMachineEditor : public FBlueprintEditor
{

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/** The StateMachineBlueprint asset being inspected */
	UStateMachineBlueprint* StateMachineBlueprint;

	/** List of open tool panels; used to ensure only one exists at any one time */
	TMap< FName, TWeakPtr<class SDockableTab> > SpawnedToolPanels;

	/** Graph Editor */
	TSharedPtr<class SGraphEditor> GraphEditor;

	/** Property View */
	TSharedPtr<class IDetailsView> DetailsView;

	/** Command list for this editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;

	/** The toolbar builder associated with this editor */
	TSharedPtr<class FStateMachineBlueprintEditorToolbar> WidgetToolbar;

public:

	/**	The tab ids for all the tabs used */
	static const FName GraphCanvasTabId;
	static const FName PropertiesTabId;

	/************************************************************************/
	/* METHODS                                                              */
	/************************************************************************/

	FStateMachineEditor();
	virtual ~FStateMachineEditor();

	/** FBlueprintEditor */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void SaveAsset_Execute() override;
	virtual UBlueprint* GetBlueprintObj() const override;

	virtual UStateMachineBlueprint* GetStateMachineBlueprint();
	virtual UStateMachineGraph* GetStateMachineGraph();

	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);

	TSharedPtr<class FStateMachineBlueprintEditorToolbar> GetWidgetToolbarBuilder() { return WidgetToolbar; }

	/**
	* Edits the specified state machine behavior object
	*
	* @param	Mode					Asset editing mode for this editor (standalone or world-centric)
	* @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	* @param	ObjectToEdit			The state machine blueprint to edit
	*/
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit);
	virtual void RegisterApplicationModes(const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode, bool bNewlyCreated = false) override;
	void CreateInternalWidgets();

	TSharedRef<SGraphEditor> CreateGraphEditorWidget();
	FGraphPanelSelectionSet GetSelectedNodes() const;

	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;
	void OnStraightenConnections();

protected:

	/** FNotifyHook interface */
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

	/** Called when the selection changes in the GraphEditor */
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

};
