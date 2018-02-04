/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineBlueprintApplicationMode.h"
#include "StateMachineBlueprintEditorToolbar.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "StateMachineEditor"

const FName FStateMachineBlueprintApplicationMode::StateMachineMode("StateMachineName");

FStateMachineBlueprintApplicationMode::FStateMachineBlueprintApplicationMode(TSharedPtr<class FStateMachineEditor> InStateMachineEditor, FName InModeName)
	: FBlueprintEditorApplicationMode(InStateMachineEditor, InModeName, FStateMachineBlueprintApplicationMode::GetLocalizedMode, false, false)
{
	MyStateMachineEditor = InStateMachineEditor;

	TabLayout = FTabManager::NewLayout("Standalone_StateMachineEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(InStateMachineEditor->GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab(FStateMachineEditor::PropertiesTabId, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->AddTab(FStateMachineEditor::GraphCanvasTabId, ETabState::OpenedTab)
				)
			)
		);

	ToolbarExtender = MakeShareable(new FExtender);
	InStateMachineEditor->GetWidgetToolbarBuilder()->AddWidgetBlueprintEditorModesToolbar(ToolbarExtender);
}

FText FStateMachineBlueprintApplicationMode::GetLocalizedMode(const FName InMode)
{
	static TMap<FName, FText> LocModes;

	if (LocModes.Num() == 0)
		LocModes.Add(StateMachineMode, NSLOCTEXT("StateMachineBlueprintMode", "StateMachineMode", "StateMachine"));

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);

	return *OutDesc;
}

UStateMachineBlueprint* FStateMachineBlueprintApplicationMode::GetBlueprint() const
{
	if (FStateMachineEditor* Editor = MyStateMachineEditor.Pin().Get() )
		return Cast<UStateMachineBlueprint>(Editor->GetBlueprintObj());

	return nullptr;
}

TSharedPtr<FStateMachineEditor> FStateMachineBlueprintApplicationMode::GetBlueprintEditor() const
{
	return MyStateMachineEditor.Pin();
}

void FStateMachineBlueprintApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FStateMachineEditor> BP = GetBlueprintEditor();

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_StateMachineEditor", "StateMachine Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	InTabManager->RegisterTabSpawner(FStateMachineEditor::GraphCanvasTabId, FOnSpawnTab::CreateSP(BP.Get(), &FStateMachineEditor::SpawnTab_GraphCanvas))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "StateMachine Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FStateMachineEditor::PropertiesTabId, FOnSpawnTab::CreateSP(BP.Get(), &FStateMachineEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(TabFactories);
}

void FStateMachineBlueprintApplicationMode::PreDeactivateMode()
{
	//FBlueprintEditorApplicationMode::PreDeactivateMode();
}

void FStateMachineBlueprintApplicationMode::PostActivateMode()
{
	//FBlueprintEditorApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE