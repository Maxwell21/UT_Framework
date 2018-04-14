/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "CommonFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateMapRangeTimer, float, TimeElapsed, float, InValue, float, OutValue);

USTRUCT()
struct FTimerMapRangeData
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	float TimeElapsed;
	
	UPROPERTY()
	float TimerValue;

	UPROPERTY()
	FTimerHandle TimerHandler;

};

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UCommonFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

protected:

	static TMap<FName, FTimerMapRangeData> TimeMapRangeData;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

public:

	/**
	 * Create a timer which map value on range and return output value between them
	 */
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintCallable, meta = (WorldContext="WorldContextObject"))
	static FName SetMapRangeTimerByEvent(const UObject* WorldContextObject, FDelegateMapRangeTimer Event, float Time, float Value, float InA, float InB, float OutA, float OutB, bool Reverse, bool Looping);

	/**
	 * Pause a map range timer
	 */
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void PauseMapRangeTimer(const UObject* WorldContextObject, FName TimerKey);

	/**
	* UnPause a map range timer
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void UnPauseMapRangeTimer(const UObject* WorldContextObject, FName TimerKey);

	/**
	* Clean a valid map range timer
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void CleanMapRangeTimer(const UObject* WorldContextObject, FName TimerKey);

protected:

	/**
	* Clean all invalid or inactive timers
	*/
	static void RefreshAndCleanMapRangeTimer(const UObject* WorldContextObject);

	/**
	* Generate a unique key whe SetMapRangeTimerByEvent is called
	*/
	static FName GenerateTimerUniqueKey();
};
