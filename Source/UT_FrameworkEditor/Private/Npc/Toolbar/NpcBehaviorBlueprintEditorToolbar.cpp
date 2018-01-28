/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorBlueprintEditorToolbar.h"
#include "NpcEditor.h"
#include "NpcBehaviorBlueprintApplicationMode.h"
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

#define LOCTEXT_NAMESPACE "NpcEditor"

/************************************************************************/
/* SBlueprintModeSeparator                                              */
/************************************************************************/

class SBlueprintModeSeparator : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SBlueprintModeSeparator) {}
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
/* FNpcBehaviorBlueprintEditorToolbar                                   */
/************************************************************************/

FNpcBehaviorBlueprintEditorToolbar::FNpcBehaviorBlueprintEditorToolbar(TSharedPtr<FNpcEditor>& InNpcEditor)
	: NpcEditor(InNpcEditor)
{

}

void FNpcBehaviorBlueprintEditorToolbar::AddWidgetBlueprintEditorModesToolbar(TSharedPtr<FExtender> Extender)
{
	TSharedPtr<FNpcEditor> BlueprintEditorPtr = NpcEditor.Pin();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		BlueprintEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FNpcBehaviorBlueprintEditorToolbar::FillWidgetBlueprintEditorModesToolbar));
}

void FNpcBehaviorBlueprintEditorToolbar::FillWidgetBlueprintEditorModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	TSharedPtr<FNpcEditor> BlueprintEditorPtr = NpcEditor.Pin();
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
			SNew(SModeWidget, FNpcBehaviorBlueprintApplicationMode::GetLocalizedMode(FNpcBehaviorBlueprintApplicationMode::NpcMode), FNpcBehaviorBlueprintApplicationMode::NpcMode)
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("NpcModeButtonTooltip", "Switch to Blueprint Npc Behavior Mode"),
				NULL,
				TEXT("Shared/Editors/BlueprintEditor"),
				TEXT("NpcMode")))
			.IconImage(FFrameworkStyle::GetBrush("FrameworkNpcToolbar.Noc"))
			.SmallIconImage(FFrameworkStyle::GetBrush("FrameworkNpcToolbar.NpcSmall"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("NpcMode")))
		);

		BlueprintEditorPtr->AddToolbarWidget(SNew(SBlueprintModeSeparator));

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
			.IconImage(FFrameworkStyle::GetBrush("FrameworkNpcToolbar.Bp"))
			.SmallIconImage(FFrameworkStyle::GetBrush("FrameworkNpcToolbar.BpSmall"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GraphMode")))
		);
		
		// Right side padding
		BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
	}
}

#undef LOCTEXT_NAMESPACE
