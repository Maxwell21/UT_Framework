/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "NpcEditor.h"
#include "BlueprintEditorModes.h"

class FNpcBehaviorGraphApplicationMode : public FBlueprintEditorApplicationMode
{

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	static const FName GraphMode;

	TWeakPtr<class FNpcEditor> MyNpcEditor;

	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FNpcBehaviorGraphApplicationMode(TSharedPtr<class FNpcEditor> InNpcEditor, FName InModeName);

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
