/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "PoolManager.generated.h"

class UPoolContainer;

UCLASS()
class UT_FRAMEWORK_API APoolManager : public AActor
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "PoolManager", EditAnywhere, BlueprintReadOnly)
	UPoolContainer* Container;

	UPROPERTY(Category = "PoolManager", BlueprintReadOnly)
	UPoolContainer* RuntimeContainer;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// Sets default values for this actor's properties
	APoolManager();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	 
public:

	UFUNCTION(Category = "PoolManager", BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static AActor* SpawnActorFromPool(UObject* WorldContextObject, FName Key, FTransform Transform);

	UFUNCTION(Category = "PoolManager", BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static inline APoolManager* GetPoolManager(UObject* WorldContextObject)
	{	
		class UWorld* World = WorldContextObject->GetWorld();
		for (TActorIterator<APoolManager> ActorItr(World); ActorItr; ++ActorItr)
		{
			return *ActorItr;
		}

		return nullptr;		
	};
};
