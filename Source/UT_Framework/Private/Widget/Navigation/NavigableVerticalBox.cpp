/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NavigableVerticalBox.h"
#include "NavigableWidget.h"
#include "Kismet/GameplayStatics.h"
#include "NavigableWidgetLibrary.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "UmbraFramework"

UNavigableVerticalBox::UNavigableVerticalBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNavigableVerticalBox::OnSlotAdded(UPanelSlot* InSlot)
{
	if (UNavigableWidget* Widget = Cast<UNavigableWidget>(InSlot->Content))
	{
		this->NavigableWidgets.Add(Widget);
		Widget->OwnerContainer = this;
	}

	Super::OnSlotAdded(InSlot);
}

void UNavigableVerticalBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	if (UNavigableWidget* Widget = Cast<UNavigableWidget>(InSlot->Content))
	{
		this->NavigableWidgets.Remove(Widget);
		Widget->OwnerContainer = nullptr;
	}

	Super::OnSlotRemoved(InSlot);
}

void UNavigableVerticalBox::BindInputs()
{
	if (this->InputComponent)
	{
		UPlayerInput* PlayerInput = GetOwningPlayerController()->PlayerInput;
		if (PlayerInput)
		{
			/** Prepare next axis */
			for (const FName AxisName : this->NavigateNextAxis)
			{
				/** Gather all next axis mapping */
				TArray<FInputAxisKeyMapping> Results = PlayerInput->GetKeysForAxis(AxisName);

				/** Loop and add procedurally them to the "NavigateNextKeys" array */
				for (int32 AxisIndex = Results.Num() - 1; AxisIndex >= 0; --AxisIndex)
				{
					FInputActionKeyMapping KeyMapping = FInputActionKeyMapping(Results[AxisIndex].AxisName, Results[AxisIndex].Key);
					this->NavigateNextKeys.AddUnique(KeyMapping);
				}
			}

			/** Prepare previous axis */
			for (const FName AxisName : this->NavigatePreviousAxis)
			{
				/** Gather all previous axis mapping */
				TArray<FInputAxisKeyMapping> Results = PlayerInput->GetKeysForAxis(AxisName);

				/** Loop and add procedurally them to the "NavigatePreviousKeys" array */
				for (int32 AxisIndex = Results.Num() - 1; AxisIndex >= 0; --AxisIndex)
				{
					FInputActionKeyMapping KeyMapping = FInputActionKeyMapping(Results[AxisIndex].AxisName, Results[AxisIndex].Key);
					this->NavigatePreviousKeys.AddUnique(KeyMapping);
				}
			}

			/** Prepare confirm action */
			for (const FName ActionName : this->ConfirmActions)
			{
				/** Gather all previous axis mapping */
				TArray<FInputActionKeyMapping> Results = PlayerInput->GetKeysForAction(ActionName);

				/** Loop and add procedurally them to the "ConfirmKeys" array */
				for (int32 ActionIndex = Results.Num() - 1; ActionIndex >= 0; --ActionIndex)
				{
					this->ConfirmKeys.AddUnique(Results[ActionIndex]);
				}
			}

			/** Prepare cancel action */
			for (const FName ActionName : this->CancelActions)
			{
				/** Gather all previous axis mapping */
				TArray<FInputActionKeyMapping> Results = PlayerInput->GetKeysForAction(ActionName);

				/** Loop and add procedurally them to the "CancelKeys" array */
				for (int32 ActionIndex = Results.Num() - 1; ActionIndex >= 0; --ActionIndex)
				{
					this->CancelKeys.AddUnique(Results[ActionIndex]);
				}
			}
 		}

		for (FInputActionKeyMapping KeyMapping : this->NavigateNextKeys)
		{
			this->InputComponent->BindKey(KeyMapping.Key, EInputEvent::IE_Pressed, this, &UNavigableVerticalBox::HandleNextKeyPressed);
			this->InputComponent->BindKey(KeyMapping.Key, EInputEvent::IE_Repeat, this, &UNavigableVerticalBox::HandleNextKeyPressed);
		}

		for (FInputActionKeyMapping KeyMapping : this->NavigatePreviousKeys)
		{
			this->InputComponent->BindKey(KeyMapping.Key, EInputEvent::IE_Pressed, this, &UNavigableVerticalBox::HandlePreviousKeyPressed);
			this->InputComponent->BindKey(KeyMapping.Key, EInputEvent::IE_Repeat, this, &UNavigableVerticalBox::HandlePreviousKeyPressed);
		}

		for (FInputActionKeyMapping KeyMapping : this->ConfirmKeys)
		{
			this->InputComponent->BindKey(KeyMapping.Key, EInputEvent::IE_Pressed, this, &UNavigableVerticalBox::HandleConfirmKeyPressed);
		}

		for (FInputActionKeyMapping KeyMapping : this->CancelKeys)
		{
			this->InputComponent->BindKey(KeyMapping.Key, EInputEvent::IE_Pressed, this, &UNavigableVerticalBox::HandleCancelKeyPressed);
		}
	}
}

void UNavigableVerticalBox::UnBindInputs()
{
	if (this->InputComponent)
	{
		this->HasConfirmed = false;
		this->InputComponent->DestroyComponent();
		this->InputComponent = nullptr;
	}
}

UNavigableWidget* UNavigableVerticalBox::GetFocusedNavigationWidget()
{
	for (UNavigableWidget* NavigableWidget : this->NavigableWidgets)
	{
		if (NavigableWidget->IsFocused)
			return NavigableWidget;
	}

	return nullptr;
}

UNavigableWidget* UNavigableVerticalBox::GetFirstNavigableWidget()
{
	return this->NavigableWidgets[0];
}

APlayerController* UNavigableVerticalBox::GetOwningPlayerController()
{
	return UGameplayStatics::GetPlayerController(this, 0);
}

bool UNavigableVerticalBox::ContainNavigableWidget()
{
	return this->NavigableWidgets.Num() > 0;
}

void UNavigableVerticalBox::InvalidateConfirm()
{
	this->HasConfirmed = false;
}

void UNavigableVerticalBox::HandleNextKeyPressed()
{
	if (this->HasConfirmed)
		return;

	for (int32 Index = 0; Index < this->NavigableWidgets.Num(); Index++)
	{
		if (this->NavigableWidgets[Index]->IsFocused && this->NavigableWidgets.IsValidIndex(Index + 1))
		{
			UNavigableWidgetLibrary::FocusNavigableWidget(this, this->NavigableWidgets[Index + 1]);
			return;
		}
	}

	if (this->BoundOpposite)
		UNavigableWidgetLibrary::FocusNavigableWidget(this, this->NavigableWidgets[0]);
}

void UNavigableVerticalBox::HandlePreviousKeyPressed()
{
	if (this->HasConfirmed)
		return;

	for (int32 Index = 0; Index < this->NavigableWidgets.Num(); Index++)
	{
		if (this->NavigableWidgets[Index]->IsFocused && this->NavigableWidgets.IsValidIndex(Index - 1))
		{
			UNavigableWidgetLibrary::FocusNavigableWidget(this, this->NavigableWidgets[Index -1]);
			return;
		}
	}

	if (this->BoundOpposite)
		UNavigableWidgetLibrary::FocusNavigableWidget(this, this->NavigableWidgets[this->NavigableWidgets.Num() - 1]);
}

void UNavigableVerticalBox::HandleConfirmKeyPressed()
{
	if (this->GetFocusedNavigationWidget())
	{
		this->HasConfirmed = true;
		this->GetFocusedNavigationWidget()->ConfirmState();
	}
}

void UNavigableVerticalBox::HandleCancelKeyPressed()
{
	if (this->GetFocusedNavigationWidget())
	{
		this->GetFocusedNavigationWidget()->CancelState();
		UNavigableWidgetLibrary::SwitchNavigableContainer(this->Parent, false);
	}
}

void UNavigableVerticalBox::UnFocusAllNavigableWidget()
{
	for (auto const& Widget : this->NavigableWidgets)
	{
		Widget->LoseFocus();
	}
}

#if WITH_EDITOR

const FText UNavigableVerticalBox::GetPaletteCategory()
{
	return LOCTEXT("UmbraFramework", "Umbra Framework");
}

#endif

#undef LOCTEXT_NAMESPACE