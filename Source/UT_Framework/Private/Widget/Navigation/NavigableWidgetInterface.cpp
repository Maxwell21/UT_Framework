/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NavigableWidgetInterface.h"
#include "NavigableWidget.h"
#include "NavigableWidgetInterface.h"
#include "Components/InputComponent.h"
#include "UObjectGlobals.h"

void INavigableWidgetInterface::Initialize()
{
	this->InitializeInputComponent();
}

void INavigableWidgetInterface::InitializeInputComponent()
{
	if (!this->InputComponent)
	{
		if (APlayerController* Controller = this->GetOwningPlayerController())
		{
			this->InputComponent = NewObject<UInputComponent>(Controller->GetPawn(), NAME_None, RF_Transient);
			this->InputComponent->bBlockInput = false;
			Controller->PushInputComponent(InputComponent);
		}
	}
}

UNavigableWidget* INavigableWidgetInterface::GetFocusedNavigationWidget()
{
	return nullptr;
}
