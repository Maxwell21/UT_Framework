/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphApplicationMode.h"
#include "NpcBehaviorBlueprintEditorToolbar.h"
#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "NpcEditor"

const FName FNpcBehaviorGraphApplicationMode::GraphMode("GraphName");

FNpcBehaviorGraphApplicationMode::FNpcBehaviorGraphApplicationMode(TSharedPtr<class FNpcEditor> InNpcEditor, FName InModeName)
	: FBlueprintEditorApplicationMode(InNpcEditor, InModeName, FNpcBehaviorGraphApplicationMode::GetLocalizedMode, false, false)
{
	MyNpcEditor = InNpcEditor;

	TabLayout = FTabManager::NewLayout("WidgetBlueprintEditor_Graph_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.186721f)
				->SetHideTabWell(true)
				->AddTab(InNpcEditor->GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.15f)
					->Split
					(
						FTabManager::NewStack()->SetSizeCoefficient(0.5f)
						->AddTab(FBlueprintEditorTabs::MyBlueprintID, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()->SetSizeCoefficient(0.5f)
						->AddTab(FBlueprintEditorTabs::DetailsID, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.70f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.80f)
						->AddTab("Document", ETabState::ClosedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.20f)
						->AddTab(FBlueprintEditorTabs::CompilerResultsID, ETabState::ClosedTab)
						->AddTab(FBlueprintEditorTabs::FindResultsID, ETabState::ClosedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.15f)
					->Split
					(
						FTabManager::NewStack()
						->AddTab(FBlueprintEditorTabs::PaletteID, ETabState::ClosedTab)
					)
				)
			)
		);

	ToolbarExtender = MakeShareable(new FExtender);
	InNpcEditor->GetWidgetToolbarBuilder()->AddWidgetBlueprintEditorModesToolbar(ToolbarExtender);
	InNpcEditor->GetToolbarBuilder()->AddCompileToolbar(ToolbarExtender);
	InNpcEditor->GetToolbarBuilder()->AddScriptingToolbar(ToolbarExtender);
	InNpcEditor->GetToolbarBuilder()->AddBlueprintGlobalOptionsToolbar(ToolbarExtender);
	InNpcEditor->GetToolbarBuilder()->AddDebuggingToolbar(ToolbarExtender);
}

FText FNpcBehaviorGraphApplicationMode::GetLocalizedMode(const FName InMode)
{
	static TMap<FName, FText> LocModes;

	if (LocModes.Num() == 0)
		LocModes.Add(GraphMode, NSLOCTEXT("NpcBehaviorBlueprintMode", "GraphMode", "Blueprint"));

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);

	return *OutDesc;
}

UNpcBehaviorBlueprint* FNpcBehaviorGraphApplicationMode::GetBlueprint() const
{
	if (FNpcEditor* Editor = MyNpcEditor.Pin().Get() )
		return Cast<UNpcBehaviorBlueprint>(Editor->GetBlueprintObj());

	return nullptr;
}

TSharedPtr<FNpcEditor> FNpcBehaviorGraphApplicationMode::GetBlueprintEditor() const
{
	return MyNpcEditor.Pin();
}

void FNpcBehaviorGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FNpcEditor> BP = GetBlueprintEditor();

	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(CoreTabFactories);
	BP->PushTabFactories(BlueprintEditorTabFactories);
	BP->PushTabFactories(TabFactories);
}

void FNpcBehaviorGraphApplicationMode::PreDeactivateMode()
{
	//FBlueprintEditorApplicationMode::PreDeactivateMode();
}

void FNpcBehaviorGraphApplicationMode::PostActivateMode()
{
	FBlueprintEditorApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE