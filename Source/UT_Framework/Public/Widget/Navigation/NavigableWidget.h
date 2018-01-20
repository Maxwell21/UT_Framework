/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NavigableWidget.generated.h"

class INavigableWidgetInterface;

/**
 * 
 */
UCLASS(Abstract)
class UT_FRAMEWORK_API UNavigableWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/**
	 * True if focused or false otherwise
	 */
	UPROPERTY(Category = "Umbra Framework | Widgets | Navigation", BlueprintReadOnly)
	bool IsFocused;

	UPROPERTY(Category = "Umbra Framework | Widgets | Navigation", BlueprintReadOnly)
	TScriptInterface<INavigableWidgetInterface> OwnerContainer;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Call by a container for telling this object is focus now
	*/
	UFUNCTION(Category = "Umbra Framework | Widgets | Navigation", BlueprintCallable)
	virtual void ReceiveFocus();

	/**
	* Call by a container for telling this object is losing focus
	*/
	UFUNCTION(Category = "Umbra Framework | Widgets | Navigation", BlueprintCallable)
	virtual void LoseFocus();

	/************************************************************************/
	/* EVENTS                                                               */
	/************************************************************************/

	/**
	 * Call when this object is focused
	 */
	UFUNCTION(Category = "Umbra Framework | Widgets | Navigation", BlueprintImplementableEvent)
	void NormalState();

	/**
	* Call when this object is lose focus
	*/
	UFUNCTION(Category = "Umbra Framework | Widgets | Navigation", BlueprintImplementableEvent)
	void HoverState();	

	/**
	* Call when this object is focused and the user press confirm key
	*/
	UFUNCTION(Category = "Umbra Framework | Widgets | Navigation", BlueprintImplementableEvent)
	void ConfirmState();

};
