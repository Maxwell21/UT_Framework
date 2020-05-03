/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LookAtComponent.generated.h"

class ULookAtTargetComponent;

UCLASS(ClassGroup=(UmbraTools), meta=(BlueprintSpawnableComponent))
class UT_FRAMEWORK_API ULookAtComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/************************************************************************/
    /* PROPERTIES                                                           */
    /************************************************************************/

	UPROPERTY(Category = "LookAt", BlueprintReadWrite, EditAnywhere)
    FName SocketBoneSource;
	
	UPROPERTY(Category = "LookAt", BlueprintReadWrite, EditAnywhere)
	float InterpolationSpeed = 6.f;

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, EditAnywhere)
    float ClampRightAxis = 65.f;

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, EditAnywhere)
	float ClampUpAxis = 65.f;

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, EditAnywhere)
    bool DisplayDebug = false;

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, VisibleInstanceOnly)
	TArray<ULookAtTargetComponent*> Targets;
	
	UPROPERTY(Category = "LookAt", BlueprintReadOnly, VisibleInstanceOnly)
    ULookAtTargetComponent* CurrentTarget;

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, VisibleInstanceOnly)
    FRotator CurrentTargetRotation;

	bool CheckLock = false;
	
	/************************************************************************/
    /* FUNCTIONS                                                            */
    /************************************************************************/

	// Sets default values for this component's properties
	ULookAtComponent();

protected:
	
	virtual void SeekingNewTarget();

	virtual void SmoothLookAtTarget(float DeltaTime);
	
	virtual bool IsInRange(ULookAtTargetComponent* Target) const;

	virtual bool TargetIsLowerThanAnother(ULookAtTargetComponent* TargetA, ULookAtTargetComponent* TargetB) const;

	virtual void HandleEnableTick();

	virtual void DrawDebug();

public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Category = "LookAt", BlueprintCallable)
	virtual bool RegisterTarget(ULookAtTargetComponent* NewTarget);

	UFUNCTION(Category = "LookAt", BlueprintCallable)
	virtual bool UnregisterTarget(ULookAtTargetComponent* ExistingTarget);

	UFUNCTION(Category = "LookAt", BlueprintPure)
	virtual FRotator GetTargetRotation() const;

	UFUNCTION(Category = "LookAt", BlueprintPure)
    virtual FVector GetSourceLocation() const;
};
