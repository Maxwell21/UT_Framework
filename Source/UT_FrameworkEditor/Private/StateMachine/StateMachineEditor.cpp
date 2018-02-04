/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineEditor.h"
#include "StateMachineBlueprint.h"
#include "StateMachineGraph.h"
#include "StateMachineGraphSchema.h"
#include "StateMachineGraphNode.h"
#include "StateMachineState_Default.h"
#include "StateMachineBlueprintApplicationMode.h"
#include "StateMachineGraphApplicationMode.h"
#include "StateMachineBlueprintEditorToolbar.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "KismetEditorUtilities.h"
#include "BlueprintEditorUtils.h"
#include "BlueprintUtilities.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "GenericCommands.h"
#include "ModuleManager.h"
#include "SDockTab.h"
#include "Editor.h"
#include "ApplicationMode.h"

#define LOCTEXT_NAMESPACE "StateMachineEditor"

const FName FStateMachineEditor::GraphCanvasTabId(TEXT("StateMachine_GraphCanvas"));
const FName FStateMachineEditor::PropertiesTabId(TEXT("StateMachine_Properties"));

FStateMachineEditor::FStateMachineEditor()
{
	this->StateMachineBlueprint = nullptr;
}

FStateMachineEditor::~FStateMachineEditor()
{
}

FName FStateMachineEditor::GetToolkitFName() const
{
	return FName("StateMachineEditor");
}

FText FStateMachineEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "State Machine Editor");
}

FString FStateMachineEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "StateMachine").ToString();
}

FLinearColor FStateMachineEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

void FStateMachineEditor::SaveAsset_Execute()
{
	GetStateMachineGraph()->NotifyNodesGraphChanged();
	FAssetEditorToolkit::SaveAsset_Execute();
}

TSharedRef<SDockTab> FStateMachineEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == GraphCanvasTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("GraphCanvasTitle", "Graph"))
		[
			GraphEditor.ToSharedRef()
		];

	return SpawnedTab;
}

TSharedRef<SDockTab> FStateMachineEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("StateMachinePropertiesTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];

	return SpawnedTab;
}

void FStateMachineEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	StateMachineBlueprint = CastChecked<UStateMachineBlueprint>(ObjectToEdit);
	if (!StateMachineBlueprint->StateMachineGraph)
	{
		UStateMachineGraph* StateMachineGraph = CastChecked<UStateMachineGraph>(FBlueprintEditorUtils::CreateNewGraph(StateMachineBlueprint, NAME_None, UStateMachineGraph::StaticClass(), UStateMachineGraphSchema::StaticClass()));
		if (StateMachineGraph)
			StateMachineBlueprint->StateMachineGraph = StateMachineGraph;
	}

	GetStateMachineGraph()->Init(StateMachineBlueprint);
	CreateInternalWidgets();

	TSharedPtr<FStateMachineEditor> ThisPtr(SharedThis(this));
	WidgetToolbar = MakeShareable(new FStateMachineBlueprintEditorToolbar(ThisPtr));

	TArray<UBlueprint*> Blueprints = { StateMachineBlueprint };
	InitBlueprintEditor(Mode, InitToolkitHost, Blueprints, false);
}

void FStateMachineEditor::RegisterApplicationModes(const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode, bool bNewlyCreated/* = false*/)
{
	if (InBlueprints.Num() == 1)
	{
		TSharedPtr<FStateMachineEditor> ThisPtr(SharedThis(this));

		// Create the modes and activate one (which will populate with a real layout)
		TArray< TSharedRef<FApplicationMode> > TempModeList;
		TempModeList.Add(MakeShareable(new FStateMachineBlueprintApplicationMode(ThisPtr, FStateMachineBlueprintApplicationMode::StateMachineMode)));
		TempModeList.Add(MakeShareable(new FStateMachineGraphApplicationMode(ThisPtr, FStateMachineGraphApplicationMode::GraphMode)));

		for (TSharedRef<FApplicationMode>& AppMode : TempModeList)
		{
			AddApplicationMode(AppMode->GetModeName(), AppMode);
		}

 		SetCurrentMode(FStateMachineBlueprintApplicationMode::StateMachineMode);
	}
}

void FStateMachineEditor::CreateInternalWidgets()
{
	GraphEditor = CreateGraphEditorWidget();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(true, false, true, FDetailsViewArgs::HideNameArea, true);
	DetailsViewArgs.NotifyHook = this;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(GetStateMachineBlueprint()->GeneratedClass);
}

TSharedRef<SGraphEditor> FStateMachineEditor::CreateGraphEditorWidget()
{
	if (!GraphEditorCommands.IsValid())
	{
		GraphEditorCommands = MakeShareable(new FUICommandList);

		// Editing commands
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FStateMachineEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FStateMachineEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FStateMachineEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FStateMachineEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().StraightenConnections,
			FExecuteAction::CreateSP(this, &FStateMachineEditor::OnStraightenConnections)
		);
	}

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_StateMachine", "STATE MACHINE");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FStateMachineEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FStateMachineEditor::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(StateMachineBlueprint->StateMachineGraph)
		.GraphEvents(InEvents)
		.ShowGraphStateOverlay(false);
}

void FStateMachineEditor::SelectAllNodes()
{
	if (GraphEditor.IsValid())
		GraphEditor->SelectAllNodes();
}

bool FStateMachineEditor::CanSelectAllNodes() const
{
	return true;
}

FGraphPanelSelectionSet FStateMachineEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (GraphEditor.IsValid())
	{
		CurrentSelection = GraphEditor->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FStateMachineEditor::DeleteSelectedNodes()
{
	if (!GraphEditor.IsValid())
		return;

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	GraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = this->GetSelectedNodes();
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
				GetStateMachineGraph()->NotifyNodesGraphChanged();
			}
		}
	}
}

bool FStateMachineEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
			return true;
	}

	return false;
}

void FStateMachineEditor::OnStraightenConnections()
{
	if (GraphEditor.IsValid())
	{
		const FScopedTransaction Transaction(FGraphEditorCommands::Get().StraightenConnections->GetLabel());

		if (UEdGraphPin* Pin = GraphEditor->GetGraphPinForMenu())
			GraphEditor->StraightenConnections(Pin, Pin->LinkedTo[0]);
		else
			GraphEditor->StraightenConnections();

		GetStateMachineGraph()->NotifyNodesGraphChanged();
	}
}

UBlueprint* FStateMachineEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<UStateMachineBlueprint>()) 
			return (UBlueprint*)EditingObjs[i];
	}

	return nullptr;
}

UStateMachineBlueprint* FStateMachineEditor::GetStateMachineBlueprint()
{
	return this->StateMachineBlueprint;
}

UStateMachineGraph* FStateMachineEditor::GetStateMachineGraph()
{
	return Cast<UStateMachineGraph>(this->StateMachineBlueprint->StateMachineGraph);
}

void FStateMachineEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	if (GraphEditor.IsValid() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		GraphEditor->NotifyGraphChanged();
		GetStateMachineGraph()->NotifyNodesGraphChanged();
		for (auto const& Node : GetStateMachineGraph()->Nodes)
		{
			if (auto SmNode = Cast<UStateMachineGraphNode>(Node))
			{
				SmNode->OnPropertyUpdated(PropertyChangedEvent, PropertyThatChanged);
				SmNode->OnNotifyGraphChanged();
			}
		}
	}
}

void FStateMachineEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if (NewSelection.Num() >= 1)
	{
		for (TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
			UStateMachineGraphNode* GraphNode = CastChecked<UStateMachineGraphNode>(*SetIt);
			GraphNode->OnNotifyGraphChanged();
			Selection.Add(GraphNode->State);
		}
		DetailsView->SetObjects(Selection);
	}
	else
		DetailsView->SetObject(GetStateMachineBlueprint()->GeneratedClass);
}

#undef LOCTEXT_NAMESPACE
