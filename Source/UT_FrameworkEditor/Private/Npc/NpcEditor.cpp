/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcEditor.h"
#include "NpcBehaviorBlueprint.h"
#include "NpcBehaviorGraph.h"
#include "NpcBehaviorGraphSchema.h"
#include "NpcBehaviorGraphNode.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorBlueprintApplicationMode.h"
#include "NpcBehaviorGraphApplicationMode.h"
#include "NpcBehaviorBlueprintEditorToolbar.h"
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

#define LOCTEXT_NAMESPACE "NpcEditor"

const FName FNpcEditor::GraphCanvasTabId(TEXT("NpcEditor_GraphCanvas"));
const FName FNpcEditor::PropertiesTabId(TEXT("NpcEditor_Properties"));

FNpcEditor::FNpcEditor()
{
	this->NpcBlueprint = nullptr;
}

FNpcEditor::~FNpcEditor()
{
	if (UNpcBehaviorBlueprint* Blueprint = Cast<UNpcBehaviorBlueprint>(GetBlueprintObj()))
	{
		Blueprint->OnChanged().RemoveAll(this);
		Blueprint->OnCompiled().RemoveAll(this);
	}

	GEditor->OnObjectsReplaced().RemoveAll(this);

	DetailsView.Reset();
}

FName FNpcEditor::GetToolkitFName() const
{
	return FName("NpcEditor");
}

FText FNpcEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Npc Editor");
}

FString FNpcEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Npc").ToString();
}

FLinearColor FNpcEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

void FNpcEditor::SaveAsset_Execute()
{
	GetNpcBehaviorGraph()->NotifyNodesGraphChanged();
	FAssetEditorToolkit::SaveAsset_Execute();
}

TSharedRef<SDockTab> FNpcEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == GraphCanvasTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("GraphCanvasTitle", "Graph"))
		[
			GraphEditor.ToSharedRef()
		];

	return SpawnedTab;
}

TSharedRef<SDockTab> FNpcEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("NpcBehaviorPropertiesTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];

	return SpawnedTab;
}

void FNpcEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	NpcBlueprint = CastChecked<UNpcBehaviorBlueprint>(ObjectToEdit);
	NpcBlueprint->SetFlags(RF_Transactional);
	if (!NpcBlueprint->NpcBehaviorGraph)
	{
		UNpcBehaviorGraph* NpcBehaviorGraph = CastChecked<UNpcBehaviorGraph>(FBlueprintEditorUtils::CreateNewGraph(NpcBlueprint, NAME_None, UNpcBehaviorGraph::StaticClass(), UNpcBehaviorGraphSchema::StaticClass()));
		if (NpcBehaviorGraph)
			NpcBlueprint->NpcBehaviorGraph = NpcBehaviorGraph;
	}

	GetNpcBehaviorGraph()->Init(NpcBlueprint);
	CreateInternalWidgets();

	TSharedPtr<FNpcEditor> ThisPtr(SharedThis(this));
	WidgetToolbar = MakeShareable(new FNpcBehaviorBlueprintEditorToolbar(ThisPtr));

	TArray<UBlueprint*> Blueprints = { NpcBlueprint };
	InitBlueprintEditor(Mode, InitToolkitHost, Blueprints, false);
}

void FNpcEditor::RegisterApplicationModes(const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode, bool bNewlyCreated/* = false*/)
{
	if (InBlueprints.Num() == 1)
	{
		TSharedPtr<FNpcEditor> ThisPtr(SharedThis(this));

		// Create the modes and activate one (which will populate with a real layout)
		TArray< TSharedRef<FApplicationMode> > TempModeList;
		TempModeList.Add(MakeShareable(new FNpcBehaviorBlueprintApplicationMode(ThisPtr, FNpcBehaviorBlueprintApplicationMode::NpcMode)));
		TempModeList.Add(MakeShareable(new FNpcBehaviorGraphApplicationMode(ThisPtr, FNpcBehaviorGraphApplicationMode::GraphMode)));

		for (TSharedRef<FApplicationMode>& AppMode : TempModeList)
		{
			AddApplicationMode(AppMode->GetModeName(), AppMode);
		}

 		SetCurrentMode(FNpcBehaviorBlueprintApplicationMode::NpcMode);
	}
}

void FNpcEditor::CreateInternalWidgets()
{
	GraphEditor = CreateGraphEditorWidget();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(true, false, true, FDetailsViewArgs::HideNameArea, true);
	DetailsViewArgs.NotifyHook = this;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(GetNpcBlueprint()->GeneratedClass);
}

TSharedRef<SGraphEditor> FNpcEditor::CreateGraphEditorWidget()
{
	if (!GraphEditorCommands.IsValid())
	{

		GraphEditorCommands = MakeShareable(new FUICommandList);

		// Editing commands
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FNpcEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FNpcEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FNpcEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FNpcEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().StraightenConnections,
			FExecuteAction::CreateSP(this, &FNpcEditor::OnStraightenConnections)
		);
	}

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_Npc", "NPC");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FNpcEditor::OnSelectedNodesChanged);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(NpcBlueprint->NpcBehaviorGraph)
		.GraphEvents(InEvents)
		.ShowGraphStateOverlay(false);
}

void FNpcEditor::SelectAllNodes()
{
	if (GraphEditor.IsValid())
		GraphEditor->SelectAllNodes();
}

bool FNpcEditor::CanSelectAllNodes() const
{
	return true;
}

FGraphPanelSelectionSet FNpcEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (GraphEditor.IsValid())
	{
		CurrentSelection = GraphEditor->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FNpcEditor::DeleteSelectedNodes()
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
				GetNpcBehaviorGraph()->NotifyNodesGraphChanged();
			}
		}
	}
}

bool FNpcEditor::CanDeleteNodes() const
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

void FNpcEditor::OnStraightenConnections()
{
	if (GraphEditor.IsValid())
	{
		const FScopedTransaction Transaction(FGraphEditorCommands::Get().StraightenConnections->GetLabel());

		if (UEdGraphPin* Pin = GraphEditor->GetGraphPinForMenu())
			GraphEditor->StraightenConnections(Pin, Pin->LinkedTo[0]);
		else
			GraphEditor->StraightenConnections();

		GetNpcBehaviorGraph()->NotifyNodesGraphChanged();
	}
}

UBlueprint* FNpcEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<UNpcBehaviorBlueprint>()) 
			return (UBlueprint*)EditingObjs[i];
	}

	return nullptr;
}

UNpcBehaviorBlueprint* FNpcEditor::GetNpcBlueprint()
{
	return this->NpcBlueprint;
}

UNpcBehaviorGraph* FNpcEditor::GetNpcBehaviorGraph()
{
	return Cast<UNpcBehaviorGraph>(this->NpcBlueprint->NpcBehaviorGraph);
}

void FNpcEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	if (GraphEditor.IsValid() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		GraphEditor->NotifyGraphChanged();
		GetNpcBehaviorGraph()->NotifyNodesGraphChanged();
		for (auto const& Node : GetNpcBehaviorGraph()->Nodes)
		{
			if (auto NpcNode = Cast<UNpcBehaviorGraphNode>(Node))
			{
				NpcNode->OnPropertyUpdated(PropertyChangedEvent, PropertyThatChanged);
				NpcNode->OnNotifyGraphChanged();
			}
		}
	}
}

void FNpcEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if (NewSelection.Num() >= 1)
	{
		for (TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
			UNpcBehaviorGraphNode* GraphNode = CastChecked<UNpcBehaviorGraphNode>(*SetIt);
			GraphNode->OnNotifyGraphChanged();
			Selection.Add(GraphNode->Task);
		}
		DetailsView->SetObjects(Selection);
	}
	else
		DetailsView->SetObject(GetNpcBlueprint()->GeneratedClass);
}

#undef LOCTEXT_NAMESPACE
