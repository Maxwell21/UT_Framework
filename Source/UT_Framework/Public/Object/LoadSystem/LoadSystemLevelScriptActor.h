/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "LoadSystemInterface.h"
#include "LoadSystemLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API ALoadSystemLevelScriptActor : public ALevelScriptActor, public ILoadSystemInterface
{
	GENERATED_BODY()
	
};
