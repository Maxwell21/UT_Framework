/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "GameFramework/PlayerInput.h"
#include "Components/VerticalBox.h"
#include "NavigableWidgetInterface.h"
#include "NavigableVerticalBox.generated.h"

class UNavigableWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNavigableVerticalBoxChangedChild, int32, ChildIndex);

/**
* A navigable vertical box widget is a layout panel allowing child widgets to be automatically laid out
* vertically and user can navigate through their navigable child widgets
*
* * Many Children
* * Flows Vertical
*/
UCLASS(AutoExpandCategories = ("Umbra Framework | Navigation"))
class UT_FRAMEWORK_API UNavigableVerticalBox : public UVerticalBox, public INavigableWidgetInterface
{
	GENERATED_UCLASS_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/**
	* Axis used for going to the next widget available
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FName> NavigateNextAxis;

	/**
	 * Keys used for going to the next widget available
	 */
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> NavigateNextKeys;

	/**
	* Axis used for going to the previous widget available
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FName> NavigatePreviousAxis;

	/**
	* Keys used for going to the previous widget available
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> NavigatePreviousKeys;

	/**
	* Actions used for call event when is focused
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FName> ConfirmActions;

	/**
	* Keys used for call event when is focused
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> ConfirmKeys;

	/**
	* Actions used for call event when player cancel 
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FName> CancelActions;

	/**
	* Keys used for call event when player cancel
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> CancelKeys;

	/**
	* When it's first or last widget if true the next focusable widget will be the opposite
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	bool BoundOpposite = true;

	/**
	 * All childs contain
	 */
	UPROPERTY()
	TArray<UNavigableWidget*> NavigableWidgets;

	/**
	 * User has pressed the confirm button
	 */
	bool HasConfirmed = false;

	UPROPERTY(Category = "Umbra Framework | Navigation", BlueprintAssignable)
	FNavigableVerticalBoxChangedChild OnVerticalBoxChangedChild;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
protected:

	// UPanelWidget
	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;
	// End UPanelWidget

	// INavigableWidgetInterface
	virtual void BindInputs() override;
	virtual void UnBindInputs() override;
	virtual UNavigableWidget* GetFocusedNavigationWidget() override;
	virtual UNavigableWidget* GetFirstNavigableWidget() override;
	virtual APlayerController* GetOwningPlayerController() override;
	virtual bool ContainNavigableWidget() override;
	virtual void InvalidateConfirm() override;
	// End INavigableWidgetInterface

	virtual void HandleNextKeyPressed();
	virtual void HandlePreviousKeyPressed();
	virtual void HandleConfirmKeyPressed();
	virtual void HandleCancelKeyPressed();
	virtual void UnFocusAllNavigableWidget();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

};
