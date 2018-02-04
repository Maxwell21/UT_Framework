/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"

class FExtender;
class FToolBarBuilder;
class FNpcEditor;

/**
 * Handles all of the toolbar related construction for the npc behavior blueprint editor.
 */
class FNpcBehaviorBlueprintEditorToolbar : public TSharedFromThis<FNpcBehaviorBlueprintEditorToolbar>
{

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	TWeakPtr<FNpcEditor> NpcEditor;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/** Constructor */
	FNpcBehaviorBlueprintEditorToolbar(TSharedPtr<FNpcEditor>& InNpcEditor);
	
	/**
	 * Builds the modes toolbar for the widget blueprint editor.
	 */
	void AddWidgetBlueprintEditorModesToolbar(TSharedPtr<FExtender> Extender);

	void FillWidgetBlueprintEditorModesToolbar(FToolBarBuilder& ToolbarBuilder);

};
