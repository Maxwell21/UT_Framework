/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraphApplicationMode.h"
#include "StateMachineBlueprintEditorToolbar.h"
#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "EditorStyleSet.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "StateMachineEditor"

const FName FStateMachineGraphApplicationMode::GraphMode("GraphName");

FStateMachineGraphApplicationMode::FStateMachineGraphApplicationMode(TSharedPtr<class FStateMachineEditor> InStateMachineEditor, FName InModeName)
	: FBlueprintEditorApplicationMode(InStateMachineEditor, InModeName, FStateMachineGraphApplicationMode::GetLocalizedMode, false, false)
{
	MyStateMachineEditor = InStateMachineEditor;

	TabLayout = FTabManager::NewLayout("WidgetBlueprintEditor_Graph_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.186721f)
				->SetHideTabWell(true)
				->AddTab(InStateMachineEditor->GetToolbarTabId(), ETabState::OpenedTab)
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
	InStateMachineEditor->GetWidgetToolbarBuilder()->AddWidgetBlueprintEditorModesToolbar(ToolbarExtender);

	if (UToolMenu* Toolbar = InStateMachineEditor->RegisterModeToolbarIfUnregistered(GetModeName()))
	{
		InStateMachineEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		InStateMachineEditor->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
		InStateMachineEditor->GetToolbarBuilder()->AddBlueprintGlobalOptionsToolbar(Toolbar);
		InStateMachineEditor->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
	}
}

FText FStateMachineGraphApplicationMode::GetLocalizedMode(const FName InMode)
{
	static TMap<FName, FText> LocModes;

	if (LocModes.Num() == 0)
		LocModes.Add(GraphMode, NSLOCTEXT("StateMachineBlueprintMode", "GraphMode", "Blueprint"));

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);

	return *OutDesc;
}

UStateMachineBlueprint* FStateMachineGraphApplicationMode::GetBlueprint() const
{
	if (FStateMachineEditor* Editor = MyStateMachineEditor.Pin().Get() )
		return Cast<UStateMachineBlueprint>(Editor->GetBlueprintObj());

	return nullptr;
}

TSharedPtr<FStateMachineEditor> FStateMachineGraphApplicationMode::GetBlueprintEditor() const
{
	return MyStateMachineEditor.Pin();
}

void FStateMachineGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FStateMachineEditor> BP = GetBlueprintEditor();

	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(CoreTabFactories);
	BP->PushTabFactories(BlueprintEditorTabFactories);
	BP->PushTabFactories(TabFactories);
}

void FStateMachineGraphApplicationMode::PreDeactivateMode()
{
	//FBlueprintEditorApplicationMode::PreDeactivateMode();
}

void FStateMachineGraphApplicationMode::PostActivateMode()
{
	FBlueprintEditorApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE