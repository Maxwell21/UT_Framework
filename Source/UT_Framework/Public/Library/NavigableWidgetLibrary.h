/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NavigableWidgetLibrary.generated.h"

class UNavigableWidget;
class UNavigableWidgetInterface;

/**
 * Helpers for manipulate navigable widgets via umbra framework
 */
UCLASS()
class UT_FRAMEWORK_API UNavigableWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Used for changing navigable container
	*
	* @param INavigableWidgetInterface Container
	*/
	UFUNCTION(Category = "Umbra Framework | WidgetLibrary", BlueprintCallable)
	static void SwitchNavigableContainer(TScriptInterface<INavigableWidgetInterface> Container);

	/**
	* Used for changing navigable container
	*
	* @param INavigableWidgetInterface Container
	*/
	UFUNCTION(Category = "Umbra Framework | WidgetLibrary", BlueprintCallable)
	static TScriptInterface<INavigableWidgetInterface> GetActiveNavigableContainer();

	/**
	* Used for changing focus on a navigable widget inside a navigable container
	*
	* @param INavigableWidgetInterface Container
	* @param UNavigableWidget* NavigableWidget
	*/
	UFUNCTION(Category = "Umbra Framework | WidgetLibrary", BlueprintCallable)
	static void FocusNavigableWidget(TScriptInterface<INavigableWidgetInterface> Container, UNavigableWidget* NavigableWidget);

	/**
	* Used for unfocus all navigable widgets for all containers
	*/
	UFUNCTION(Category = "Umbra Framework | WidgetLibrary", BlueprintCallable)
	static void UnFocusAllNavigableWidget();
	
	/**
	* Get a valid navigable widget focused inside a container
	*
	* @param INavigableWidgetInterface Container
	*
	* @return UNavigableWidget*
	*/
	UFUNCTION(Category = "Umbra Framework | WidgetLibrary", BlueprintCallable)
	static UNavigableWidget* GetFocusedNavigableWidget(TScriptInterface<INavigableWidgetInterface> Container);

};
