/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "StateMachineEditor.h"
#include "BlueprintEditorModes.h"

class FStateMachineBlueprintApplicationMode : public FBlueprintEditorApplicationMode
{

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	static const FName StateMachineMode;

	TWeakPtr<class FStateMachineEditor> MyStateMachineEditor;

	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	FStateMachineBlueprintApplicationMode(TSharedPtr<class FStateMachineEditor> InStateMachineEditor, FName InModeName);

	static FText GetLocalizedMode(const FName InMode);

protected:

	class UStateMachineBlueprint* GetBlueprint() const;
	
	TSharedPtr<class FStateMachineEditor> GetBlueprintEditor() const;

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface

};
