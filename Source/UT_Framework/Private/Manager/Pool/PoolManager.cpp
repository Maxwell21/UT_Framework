/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "PoolManager.h"
#include "PoolContainer.h"

// Sets default values
APoolManager::APoolManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


// Called when the game starts or when spawned
void APoolManager::BeginPlay()
{
	Super::BeginPlay();

	// Create the runtime container
	this->RuntimeContainer = Cast<UPoolContainer>(DuplicateObject(this->Container, this));
	this->RuntimeContainer->Init();
}

AActor* APoolManager::SpawnActorFromPool(UObject* WorldContextObject, FName Key, FTransform Transform)
{
	if (APoolManager* PoolManager = GetPoolManager(WorldContextObject))
	{
		if (AActor* Actor = PoolManager->RuntimeContainer->GetByKey(Key))
		{
			Actor->SetActorTransform(Transform);
			return Actor;
		}
	}

	return nullptr;
}