/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "NavigableWidgetInterface.generated.h"

class UNavigableWidget;
class UInputComponent;
class APlayerController;

UINTERFACE(Blueprintable)
class UNavigableWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UT_FRAMEWORK_API INavigableWidgetInterface
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/
	
	UInputComponent* InputComponent;

	bool Initialized;

	bool IsActive;

	TScriptInterface<INavigableWidgetInterface> Parent;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Initialize();

	virtual void Shutdown();

	virtual void InitializeInputComponent();

	virtual void BindInputs() = 0;

	virtual void UnBindInputs() = 0;

	virtual UNavigableWidget* GetFocusedNavigationWidget();

	virtual UNavigableWidget* GetFirstNavigableWidget() = 0;
	
	virtual APlayerController* GetOwningPlayerController() = 0;

	virtual void UnFocusAllNavigableWidget() = 0;

	virtual bool ContainNavigableWidget() = 0;

	virtual void InvalidateConfirm() = 0;
};
