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

USTRUCT()
struct FActionKeysCache
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	TArray<FKey> Cachedkeys;

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
	static TMap<FName, FActionKeysCache> CachedActions;
	static TMap<FName, FActionKeysCache> CachedAxis;

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

	/**
	* Get all keys for action mapping name given
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<FKey> GetPlayerInputActionKeys(const UObject* WorldContextObject, bool &IsValid, FName ActionName, int32 PlayerIndex = 0);

	/**
	* Returns how long the given key/button has been down.  Returns 0 if it's up or it just went down this frame.
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static float GetPlayerInputActionTimeDown(const UObject* WorldContextObject, FName ActionName, int32 PlayerIndex = 0);

	/**
	* Get all keys for axis mapping name given
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<FKey> GetPlayerInputAxisKeys(const UObject* WorldContextObject, bool &IsValid, FName AxisName, int32 PlayerIndex = 0);

	/**
	* Returns true if the given action mapping name was up last frame and down this frame
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static bool WasPlayerInputActionJustPressed(const UObject* WorldContextObject, FName ActionName, int32 PlayerIndex = 0);

	/**
	* Returns true if the given action mapping name was down last frame and down this frame
	*/
	UFUNCTION(Category = "Umbra Framework | CommonLibrary", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static bool WasPlayerInputActionJustReleased(const UObject* WorldContextObject, FName ActionName, int32 PlayerIndex = 0);

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
