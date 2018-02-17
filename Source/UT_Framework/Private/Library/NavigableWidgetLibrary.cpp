/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NavigableWidgetLibrary.h"
#include "NavigableWidget.h"
#include "NavigableVerticalBox.h"
#include "NavigableWidgetInterface.h"
#include "UObjectIterator.h"

void UNavigableWidgetLibrary::SwitchNavigableContainer(TScriptInterface<INavigableWidgetInterface> Container)
{
	if (!Container || !Container->ContainNavigableWidget())
		return;

	// unfocus all + unbind 
	if (INavigableWidgetInterface* NavigableInterface = (INavigableWidgetInterface*)(UNavigableWidgetLibrary::GetActiveNavigableContainer().GetInterface()))
		NavigableInterface->Shutdown();

	// Focus first
	Container->Initialize();
	Container->IsActive = true;
	UNavigableWidgetLibrary::FocusNavigableWidget(Container, Container->GetFirstNavigableWidget());
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
