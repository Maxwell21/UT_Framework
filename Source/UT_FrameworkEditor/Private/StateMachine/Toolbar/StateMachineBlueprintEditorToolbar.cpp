/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineBlueprintEditorToolbar.h"
#include "StateMachineEditor.h"
#include "StateMachineBlueprintApplicationMode.h"
#include "BlueprintEditorModes.h"
#include "Types/ISlateMetaData.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SToolTip.h"
#include "EditorStyleSet.h"
#include "IDocumentation.h"
#include "BlueprintEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "FrameworkStyle.h"

#define LOCTEXT_NAMESPACE "StateMachineEditor"

/************************************************************************/
/* SStateMachineBlueprintModeSeparator                                  */
/************************************************************************/

class SStateMachineBlueprintModeSeparator : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SStateMachineBlueprintModeSeparator) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArg)
	{
		SBorder::Construct(
			SBorder::FArguments()
			.BorderImage(FEditorStyle::GetBrush("BlueprintEditor.PipelineSeparator"))
			.Padding(0.0f)
			);
	}

	// SWidget interface
	virtual FVector2D ComputeDesiredSize(float) const override
	{
		const float Height = 20.0f;
		const float Thickness = 16.0f;
		return FVector2D(Thickness, Height);
	}
	// End of SWidget interface
};

/************************************************************************/
/* FStateMachineBlueprintEditorToolbar                                  */
/************************************************************************/

FStateMachineBlueprintEditorToolbar::FStateMachineBlueprintEditorToolbar(TSharedPtr<FStateMachineEditor>& InStateMachineEditor)
	: StateMachineEditor(InStateMachineEditor)
{

}

void FStateMachineBlueprintEditorToolbar::AddWidgetBlueprintEditorModesToolbar(TSharedPtr<FExtender> Extender)
{
	TSharedPtr<FStateMachineEditor> BlueprintEditorPtr = StateMachineEditor.Pin();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		BlueprintEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FStateMachineBlueprintEditorToolbar::FillWidgetBlueprintEditorModesToolbar));
}

void FStateMachineBlueprintEditorToolbar::FillWidgetBlueprintEditorModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	TSharedPtr<FStateMachineEditor> BlueprintEditorPtr = StateMachineEditor.Pin();
	UBlueprint* BlueprintObj = BlueprintEditorPtr->GetBlueprintObj();

	if( !BlueprintObj ||
		(!FBlueprintEditorUtils::IsLevelScriptBlueprint(BlueprintObj) 
		&& !FBlueprintEditorUtils::IsInterfaceBlueprint(BlueprintObj)
		&& !BlueprintObj->bIsNewlyCreated)
		)
	{
		TAttribute<FName> GetActiveMode(BlueprintEditorPtr.ToSharedRef(), &FBlueprintEditor::GetCurrentMode);
		FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(BlueprintEditorPtr.ToSharedRef(), &FBlueprintEditor::SetCurrentMode);

		// Left side padding
		BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

		BlueprintEditorPtr->AddToolbarWidget(
			SNew(SModeWidget, FStateMachineBlueprintApplicationMode::GetLocalizedMode(FStateMachineBlueprintApplicationMode::StateMachineMode), FStateMachineBlueprintApplicationMode::StateMachineMode)
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("StateMachineModeButtonTooltip", "Switch to Blueprint StateMachine Mode"),
				NULL,
				TEXT("Shared/Editors/BlueprintEditor"),
				TEXT("StateMachineMode")))
			.IconImage(FFrameworkStyle::GetBrush("FrameworkStateMachineToolbar.StateMachine"))
			.SmallIconImage(FFrameworkStyle::GetBrush("FrameworkStateMachineToolbar.StateMachineSmall"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("StateMachineMode")))
		);

		BlueprintEditorPtr->AddToolbarWidget(SNew(SStateMachineBlueprintModeSeparator));

		BlueprintEditorPtr->AddToolbarWidget(
			SNew(SModeWidget, FBlueprintEditorApplicationModes::GetLocalizedMode(FBlueprintEditorApplicationModes::StandardBlueprintEditorMode), FBlueprintEditorApplicationModes::StandardBlueprintEditorMode)
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.CanBeSelected(BlueprintEditorPtr.Get(), &FBlueprintEditor::IsEditingSingleBlueprint)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("GraphModeButtonTooltip", "Switch to Graph Editing Mode"),
				NULL,
				TEXT("Shared/Editors/BlueprintEditor"),
				TEXT("GraphMode")))
			.ToolTipText(LOCTEXT("GraphModeButtonTooltip", "Switch to Graph Editing Mode"))
			.IconImage(FFrameworkStyle::GetBrush("FrameworkGraphToolbar.Bp"))
			.SmallIconImage(FFrameworkStyle::GetBrush("FrameworkGraphToolbar.BpSmall"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GraphMode")))
		);
		
		// Right side padding
		BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
	}
}

#undef LOCTEXT_NAMESPACE
