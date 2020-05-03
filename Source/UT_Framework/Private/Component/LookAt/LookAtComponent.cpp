/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "Component/LookAt/LookAtComponent.h"
#include "Component/LookAt/LookAtTargetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
ULookAtComponent::ULookAtComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool ULookAtComponent::RegisterTarget(ULookAtTargetComponent* NewTarget)
{
	bool Result = false;
	if (NewTarget->IsValidLowLevel() && !Targets.Contains(NewTarget))
		Result = Targets.AddUnique(NewTarget) > -1;

	HandleEnableTick();
	return Result;
}

bool ULookAtComponent::UnregisterTarget(ULookAtTargetComponent* ExistingTarget)
{
	bool Result = false;
	if (Targets.Contains(ExistingTarget))
		Result = Targets.Remove(ExistingTarget) > 0;

	HandleEnableTick();
	return Result;
}

void ULookAtComponent::SeekingNewTarget()
{
	if (CheckLock)
		return;

	CheckLock = true;
	ULookAtTargetComponent* Buffer = nullptr;
	for (int i = 0; i < Targets.Num(); ++i)
	{
		// Check if the target is in detection range
		if (Targets.IsValidIndex(i) && Targets[i]->IsValidLowLevel() && IsInRange(Targets[i]))
		{
			// We don't have any buffer we just set the new target as buffer
			if (!Buffer)
				Buffer = Targets[i];
			// If we have already a target buffer set we check that the current looping target is lower than buffer in distance
			else if (Buffer && TargetIsLowerThanAnother(Targets[i], Buffer))
				Buffer = Targets[i];
		}
	}

	// Update the new target
	CurrentTarget = Buffer;
	CheckLock = false;
}

void ULookAtComponent::SmoothLookAtTarget(float DeltaTime)
{
	const FRotator OwnerRotation = GetOwner()->GetActorRotation();
	const FRotator TargetRotation = (CurrentTarget != nullptr)
		? UKismetMathLibrary::FindLookAtRotation(GetSourceLocation(), CurrentTarget->GetSourceLocation())
		: FRotator();

	// Compute yaw
	float Yaw = UKismetMathLibrary::Abs(TargetRotation.Yaw) > 0 ? TargetRotation.Yaw - OwnerRotation.Yaw : 0.f; 
	float ClampedYaw = (ClampRightAxis > 0)
		? UKismetMathLibrary::ClampAngle(Yaw, ClampRightAxis * -1, ClampRightAxis)
		: Yaw;

	// Compute pitch
	float Pitch = (TargetRotation.Pitch - OwnerRotation.Pitch) * -1.f; 
	float ClampedPitch = (ClampRightAxis > 0)
		? UKismetMathLibrary::ClampAngle(Pitch, ClampRightAxis * -1, ClampRightAxis)
		: Pitch;
	
	FRotator BufferTarget = FRotator(
		0.f,
		ClampedYaw,
		ClampedPitch
	);
	
	CurrentTargetRotation = UKismetMathLibrary::RInterpTo(CurrentTargetRotation, BufferTarget, DeltaTime, InterpolationSpeed);
}

bool ULookAtComponent::IsInRange(ULookAtTargetComponent* Target) const
{
	return GetOwner()->GetDistanceTo(Target->GetOwner()) <= Target->TriggerDistance;
}

bool ULookAtComponent::TargetIsLowerThanAnother(ULookAtTargetComponent* TargetA, ULookAtTargetComponent* TargetB) const
{
	return
		GetOwner()->GetDistanceTo(TargetA->GetOwner()) <= GetOwner()->GetDistanceTo(TargetB->GetOwner())
		&& TargetA->Priority >= TargetB->Priority 
	;
}

void ULookAtComponent::HandleEnableTick()
{
	SetComponentTickEnabled(Targets.Num() > 0);
}

void ULookAtComponent::DrawDebug()
{
	if (!DisplayDebug)
		return;

	UKismetSystemLibrary::DrawDebugSphere(
		GetOwner(),
		CurrentTarget != nullptr ? CurrentTarget->GetSourceLocation() : GetSourceLocation(),
		20.f,
		12,
		FLinearColor::Green,
		0.1f
	);
}

// Called every frame
void ULookAtComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SeekingNewTarget();
	SmoothLookAtTarget(DeltaTime);
	DrawDebug();
}

FRotator ULookAtComponent::GetTargetRotation() const
{
	return CurrentTargetRotation;
}

FVector ULookAtComponent::GetSourceLocation() const
{
	FVector Result;
	const USkeletalMeshComponent* SkeletalComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
    if (SocketBoneSource != NAME_None && SkeletalComponent)
    	Result = SkeletalComponent->GetSocketLocation(SocketBoneSource);
	else
		Result = GetOwner()->GetActorLocation();

	return Result;
}
