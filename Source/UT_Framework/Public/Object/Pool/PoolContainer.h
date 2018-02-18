/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "PoolContainer.generated.h"

class AActor;
class UWorld;

UCLASS(Blueprintable)
class UT_FRAMEWORK_API UPoolRuntimeData : public UObject
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "PoolRuntimeData", VisibleInstanceOnly, BlueprintReadOnly)
	bool Active;

	UPROPERTY(Category = "PoolRuntimeData", VisibleInstanceOnly, BlueprintReadOnly)
	AActor* Actor;

	UPROPERTY(Category = "PoolRuntimeData", VisibleInstanceOnly, BlueprintReadOnly)
	UPoolRuntimeData* Next;

};

USTRUCT(Blueprintable)
struct UT_FRAMEWORK_API FPoolData
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "PoolData", EditDefaultsOnly, BlueprintReadOnly)
	FName Key;

	UPROPERTY(Category = "PoolData", EditDefaultsOnly, BlueprintReadOnly)
	int32 Number;

	UPROPERTY(Category = "PoolData", EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> Actor;

};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class UT_FRAMEWORK_API UPoolContainer : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "PoolContainer", EditDefaultsOnly)
	TArray<FPoolData> Data;
	
	UPROPERTY(Category = "PoolContainer", BlueprintReadOnly)
	TMap<FName, UPoolRuntimeData*> RuntimeData;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Init();

	virtual UPoolRuntimeData* CreatePoolRuntimeData(TSubclassOf<AActor> ActorClass);

	virtual void AddNext(TSubclassOf<AActor> ActorClass, UPoolRuntimeData* Parent, int32& Number);

	virtual AActor* GetByKey(FName Key);

protected:

	virtual AActor* GetValidRuntimePoolData(UPoolRuntimeData* CurrentRuntimeData);
};
