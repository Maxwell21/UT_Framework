/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadSystemInterface.generated.h"

UINTERFACE(MinimalAPI)
class ULoadSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UT_FRAMEWORK_API ILoadSystemInterface
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	UFUNCTION(Category = "Umbra Framework | LoadSystem", BlueprintImplementableEvent)
	void OnLevelLoaded();

	UFUNCTION(Category = "Umbra Framework | LoadSystem", BlueprintImplementableEvent)
	void OnLevelShown();

};
