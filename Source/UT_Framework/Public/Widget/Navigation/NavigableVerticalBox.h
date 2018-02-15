/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "Components/VerticalBox.h"
#include "NavigableWidgetInterface.h"
#include "NavigableVerticalBox.generated.h"

class UNavigableWidget;

/**
* A navigable vertical box widget is a layout panel allowing child widgets to be automatically laid out
* vertically and user can navigate through their navigable child widgets
*
* * Many Children
* * Flows Vertical
*/
UCLASS()
class UT_FRAMEWORK_API UNavigableVerticalBox : public UVerticalBox, public INavigableWidgetInterface
{
	GENERATED_UCLASS_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/**
	 * Keys used for going to the next widget available
	 */
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> NavigateNextKeys;

	/**
	* Keys used for going to the previous widget available
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> NavigatePreviousKeys;

	/**
	* Keys used for call event when is focused
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	TArray<FInputActionKeyMapping> Confirmkeys;

	/**
	* When it's first or last widget if true the next focusable widget will be the opposite
	*/
	UPROPERTY(Category = "Umbra Framework | Navigation", EditAnywhere, BlueprintReadWrite)
	bool BoundOpposite = true;

	UPROPERTY()
	TArray<UNavigableWidget*> NavigableWidgets;

	bool HasConfirmed = false;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
protected:

	// UPanelWidget
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

	// INavigableWidgetInterface
	virtual void BindInputs() override;
	virtual void UnBindInputs() override;
	virtual UNavigableWidget* GetFocusedNavigationWidget() override;
	virtual UNavigableWidget* GetFirstNavigableWidget() override;
	virtual APlayerController* GetOwningPlayerController() override;
	virtual bool ContainNavigableWidget() override;
	// End INavigableWidgetInterface

	virtual void HandleNextKeyPressed();
	virtual void HandlePreviousKeyPressed();
	virtual void HandleConfirmKeyPressed();
	virtual void UnFocusAllNavigableWidget();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

};
