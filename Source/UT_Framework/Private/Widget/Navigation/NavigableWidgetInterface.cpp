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
	// force unbind in case of that container has been preserved
	this->UnBindInputs();
	this->InitializeInputComponent();
	this->BindInputs();
	this->Initialized = true;
}

void INavigableWidgetInterface::Shutdown()
{
	this->UnBindInputs();
	this->UnFocusAllNavigableWidget();
	this->Initialized = false;
	this->IsActive = false;
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
