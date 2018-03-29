/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NavigableWidgetLibrary.h"
#include "NavigableWidget.h"
#include "NavigableVerticalBox.h"
#include "NavigableWidgetInterface.h"
#include "UObjectIterator.h"

void UNavigableWidgetLibrary::SwitchNavigableContainer(TScriptInterface<INavigableWidgetInterface> Container, bool PreserveForCancel /*= false*/)
{
	if (!Container || !Container->ContainNavigableWidget())
		return;

	INavigableWidgetInterface* CurrentContainer = (INavigableWidgetInterface*)(UNavigableWidgetLibrary::GetActiveNavigableContainer().GetInterface());
	TScriptInterface<INavigableWidgetInterface> CurrentContainerInterface = UNavigableWidgetLibrary::GetActiveNavigableContainer();

	/**
	 * if we are not preserving the current container
	 */
	if (PreserveForCancel == false)
	{
		// unfocus all + unbind 
		if (CurrentContainer)
			CurrentContainer->Shutdown();
	}

	// Prepare new container
	Container->Initialize();
	Container->IsActive = true;

	// Set the current container has parent of the new one
	if (PreserveForCancel && CurrentContainer)
	{
		// remove active flag to the preserved container
		CurrentContainer->IsActive = false;
		Container->Parent = CurrentContainerInterface;
	}

	/**
	 * Get widget to focus (in case of the container is a preserved container we get the last focused widget as new widget to focus
	 */
	UNavigableWidget* WidgetToFocus = (Container->GetFocusedNavigationWidget())
		? Container->GetFocusedNavigationWidget()
		: Container->GetFirstNavigableWidget()
	;

	UNavigableWidgetLibrary::FocusNavigableWidget(Container, WidgetToFocus);
}

TScriptInterface<INavigableWidgetInterface> UNavigableWidgetLibrary::GetActiveNavigableContainer()
{
	TScriptInterface<INavigableWidgetInterface> Property;
	for (TObjectIterator<UObject> Itr; Itr; ++Itr)
	{
		UObject* Object = *Itr;
		if (INavigableWidgetInterface* NavigableInterface = Cast<INavigableWidgetInterface>(Object))
		{
			if (NavigableInterface->IsActive)
			{	
				Property.SetInterface(NavigableInterface);
				Property.SetObject(Object);
				
				return Property;
			}
		}
	}

	return Property;
}

void UNavigableWidgetLibrary::FocusNavigableWidget(TScriptInterface<INavigableWidgetInterface> Container, UNavigableWidget* NavigableWidget)
{
	if (!Container)
		return;

	// unfocus current
	if (Container->GetFocusedNavigationWidget())
		Container->GetFocusedNavigationWidget()->LoseFocus();

	// focus new 
	if (NavigableWidget)
		NavigableWidget->ReceiveFocus();
}

void UNavigableWidgetLibrary::UnFocusAllNavigableWidget()
{
	for (TObjectIterator<UObject> Itr; Itr; ++Itr)
	{
		UObject* Object = *Itr;
		if (INavigableWidgetInterface* NavigableInterface = Cast<INavigableWidgetInterface>(Object))
			NavigableInterface->UnFocusAllNavigableWidget();
	}
}

UNavigableWidget* UNavigableWidgetLibrary::GetFocusedNavigableWidget(TScriptInterface<INavigableWidgetInterface> Container)
{
	if (Container)
		return Container->GetFocusedNavigationWidget();

	return nullptr;
}

bool UNavigableWidgetLibrary::DisableInput(TScriptInterface<INavigableWidgetInterface> Container)
{
	INavigableWidgetInterface* CurrentContainer = (INavigableWidgetInterface*)(Container.GetInterface());

	if (CurrentContainer)
	{
		if (CurrentContainer->Parent)
			DisableInput(CurrentContainer->Parent);

		CurrentContainer->Shutdown();

		return true;
	}

	return false;
}

bool UNavigableWidgetLibrary::EnableInput(TScriptInterface<INavigableWidgetInterface> Container)
{
	INavigableWidgetInterface* CurrentContainer = (INavigableWidgetInterface*)(Container.GetInterface());

	if (CurrentContainer)
	{
		SwitchNavigableContainer(Container);

		return true;
	}

	return false;
}

void UNavigableWidgetLibrary::InvalidateConfirmState(TScriptInterface<INavigableWidgetInterface> Container)
{
	if (Container)
		Container->InvalidateConfirm();
}
