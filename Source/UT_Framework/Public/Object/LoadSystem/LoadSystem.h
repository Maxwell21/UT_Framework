/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Package.h"
#include "UObjectGlobals.h"
#include "LoadSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateLoadSystem, ULoadSystem*, LoadSystem);

class ULevelStreamingDynamic;
class ALoadSystemLevelScriptActor;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class UT_FRAMEWORK_API ULoadSystem : public UObject
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	static TMap<TSoftObjectPtr<UWorld>, ULoadSystem*> LoadedLevels;

	UPROPERTY(Category = "Umbra Framework | LoadSystem", BlueprintReadOnly)
	ULevelStreamingDynamic* LevelStreaming;

	UPROPERTY(Category = "Umbra Framework | LoadSystem", BlueprintReadOnly)
	TSoftObjectPtr<UWorld> CurrentLevel;

	UPROPERTY(Category = "Umbra Framework | LoadSystem", BlueprintReadOnly)
	bool VisibiltyOnLoad;

	UPROPERTY(BlueprintAssignable)
	FDelegateLoadSystem OnBeginLoad;

	UPROPERTY(BlueprintAssignable)
	FDelegateLoadSystem OnFailedLoad;

	UPROPERTY(BlueprintAssignable)
	FDelegateLoadSystem OnComplete;

	UPROPERTY(BlueprintAssignable)
	FDelegateLoadSystem OnShow;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Load a level asynchronously
	* 
	* @param UObject* WorldContextObject
	* @param TSoftObjectPtr<UWorld> Level
	* @param bool VisibleOnLoad
	*
	* @return ULoadSystem*
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static ULoadSystem* AsyncLoadLevel(UObject* WorldContextObject, TSoftObjectPtr<UWorld> Level, bool VisibleOnLoad);

	/**
	 * Unload a specific level by load system instance
	 *
	 * @param ULoadSystem* LoadSystemInstance
	 *
     * @return bool
	 */
	UFUNCTION(Category = "Umbra Framework | LoadSystem", BlueprintCallable)
	static bool UnloadLoadLevel(ULoadSystem* LoadSystemInstance);

	/**
	 * Unload a specific level by Level asset
	 *
	 * @param TSoftObjectPtr<UWorld> Level
	 *
     * @return bool
	 */
	UFUNCTION(Category = "Umbra Framework | LoadSystem", BlueprintCallable)
	static bool UnloadLoadLevelByAsset(TSoftObjectPtr<UWorld> Level);

	/**
	 * Show the level if VisibleOnLoad was false on loading
	 */
	UFUNCTION(Category = "Umbra Framework | LoadSystem", BlueprintCallable)
	void ShowLevel();

	/**
	 * Load the streaming level asynchronously and add it to the world
	 */
	void HandleLoading();

	virtual void BeginDestroy() override;

protected:

	/** Remove the loaded streaming level */
	bool HandleUnloading();

	/** Event called when level streaming is shown */
	UFUNCTION()
	void OnLevelStreamingShown();

	/** Event called when level streaming is loaded */
	UFUNCTION()
	void OnLevelStreamingLoaded();

	/**
	 * Get LevelScriptActor to the level streaming owned by this load system
	 *
     * @return ALoadSystemLevelScriptActor*
	 */
	ALoadSystemLevelScriptActor* GetLevelScriptActor();
};
