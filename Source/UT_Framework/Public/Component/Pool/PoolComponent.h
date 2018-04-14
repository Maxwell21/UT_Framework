/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPoolEvent);

class UPoolRuntimeData;

UCLASS( ClassGroup=(UmbraTools), meta=(BlueprintSpawnableComponent) )
class UT_FRAMEWORK_API UPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "PoolComponent", VisibleAnywhere, BlueprintReadOnly)
	UPoolRuntimeData* PoolRuntimeData;

	UPROPERTY(Category = "PoolComponent", BlueprintAssignable)
	FPoolEvent OnPoolActivate;

	UPROPERTY(Category = "PoolComponent", BlueprintAssignable)
	FPoolEvent OnPoolDeactivate;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	// Sets default values for this component's properties
	UPoolComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual void PoolActivate();

	virtual void PoolDeactivate();

};
