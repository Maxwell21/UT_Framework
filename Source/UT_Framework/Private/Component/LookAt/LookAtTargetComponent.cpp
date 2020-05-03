/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "Component/LookAt/LookAtTargetComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
ULookAtTargetComponent::ULookAtTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FVector ULookAtTargetComponent::GetSourceLocation() const
{
	FVector Result;
	const USkeletalMeshComponent* SkeletalComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (SocketOrBone != NAME_None && SkeletalComponent)
		Result = SkeletalComponent->GetSocketLocation(SocketOrBone);
	else
		Result = GetComponentLocation();

	return Result;
}
