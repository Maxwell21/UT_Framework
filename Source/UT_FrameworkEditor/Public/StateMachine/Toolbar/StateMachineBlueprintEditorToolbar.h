/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"

class FExtender;
class FToolBarBuilder;
class FStateMachineEditor;

/**
 * Handles all of the toolbar related construction for the state machine blueprint editor.
 */
class FStateMachineBlueprintEditorToolbar : public TSharedFromThis<FStateMachineBlueprintEditorToolbar>
{

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	TWeakPtr<FStateMachineEditor> StateMachineEditor;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/** Constructor */
	FStateMachineBlueprintEditorToolbar(TSharedPtr<FStateMachineEditor>& InNpcEditor);
	
	/**
	 * Builds the modes toolbar for the widget blueprint editor.
	 */
	void AddWidgetBlueprintEditorModesToolbar(TSharedPtr<FExtender> Extender);

	void FillWidgetBlueprintEditorModesToolbar(FToolBarBuilder& ToolbarBuilder);

};
