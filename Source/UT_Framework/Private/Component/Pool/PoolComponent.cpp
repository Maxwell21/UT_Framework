/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "PoolComponent.h"

// Sets default values for this component's properties
UPoolComponent::UPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UPoolComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPoolComponent::PoolActivate()
{
	this->OnPoolActivate.Broadcast();
	GetOwner()->GetRootComponent()->Deactivate();
}

void UPoolComponent::PoolDeactivate()
{
	this->OnPoolDeactivate.Broadcast();
	GetOwner()->GetRootComponent()->Activate();
}
