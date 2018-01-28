/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "NpcEditor.h"
#include "BlueprintEditorModes.h"

class FNpcBehaviorBlueprintApplicationMode : public FBlueprintEditorApplicationMode
{

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	static const FName NpcMode;

	TWeakPtr<class FNpcEditor> MyNpcEditor;

	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	FNpcBehaviorBlueprintApplicationMode(TSharedPtr<class FNpcEditor> InNpcEditor, FName InModeName);

	static FText GetLocalizedMode(const FName InMode);

protected:

	class UNpcBehaviorBlueprint* GetBlueprint() const;
	
	TSharedPtr<class FNpcEditor> GetBlueprintEditor() const;

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface

};
