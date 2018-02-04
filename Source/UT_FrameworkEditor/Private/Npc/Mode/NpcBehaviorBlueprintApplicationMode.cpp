/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorBlueprintApplicationMode.h"
#include "NpcBehaviorBlueprintEditorToolbar.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "NpcEditor"

const FName FNpcBehaviorBlueprintApplicationMode::NpcMode("NpcName");

FNpcBehaviorBlueprintApplicationMode::FNpcBehaviorBlueprintApplicationMode(TSharedPtr<class FNpcEditor> InNpcEditor, FName InModeName)
	: FBlueprintEditorApplicationMode(InNpcEditor, InModeName, FNpcBehaviorBlueprintApplicationMode::GetLocalizedMode, false, false)
{
	MyNpcEditor = InNpcEditor;

	TabLayout = FTabManager::NewLayout("Standalone_NpcEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(InNpcEditor->GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab(FNpcEditor::PropertiesTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->AddTab(FNpcEditor::GraphCanvasTabId, ETabState::OpenedTab)
				)
			)
		);

	ToolbarExtender = MakeShareable(new FExtender);
	InNpcEditor->GetWidgetToolbarBuilder()->AddWidgetBlueprintEditorModesToolbar(ToolbarExtender);
}

FText FNpcBehaviorBlueprintApplicationMode::GetLocalizedMode(const FName InMode)
{
	static TMap<FName, FText> LocModes;

	if (LocModes.Num() == 0)
		LocModes.Add(NpcMode, NSLOCTEXT("NpcBehaviorBlueprintMode", "NpcMode", "Npc"));

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);

	return *OutDesc;
}

UNpcBehaviorBlueprint* FNpcBehaviorBlueprintApplicationMode::GetBlueprint() const
{
	if (FNpcEditor* Editor = MyNpcEditor.Pin().Get() )
		return Cast<UNpcBehaviorBlueprint>(Editor->GetBlueprintObj());

	return nullptr;
}

TSharedPtr<FNpcEditor> FNpcBehaviorBlueprintApplicationMode::GetBlueprintEditor() const
{
	return MyNpcEditor.Pin();
}

void FNpcBehaviorBlueprintApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FNpcEditor> BP = GetBlueprintEditor();

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_NpcEditor", "Npc Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	InTabManager->RegisterTabSpawner(FNpcEditor::GraphCanvasTabId, FOnSpawnTab::CreateSP(BP.Get(), &FNpcEditor::SpawnTab_GraphCanvas))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Npc Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FNpcEditor::PropertiesTabId, FOnSpawnTab::CreateSP(BP.Get(), &FNpcEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(TabFactories);
}

void FNpcBehaviorBlueprintApplicationMode::PreDeactivateMode()
{
	//FBlueprintEditorApplicationMode::PreDeactivateMode();
}

void FNpcBehaviorBlueprintApplicationMode::PostActivateMode()
{
	//FBlueprintEditorApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE