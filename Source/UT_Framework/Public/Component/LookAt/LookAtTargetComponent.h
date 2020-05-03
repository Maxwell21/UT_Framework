/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LookAtTargetComponent.generated.h"

UCLASS(ClassGroup=(UmbraTools), meta=(BlueprintSpawnableComponent))
class UT_FRAMEWORK_API ULookAtTargetComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	/************************************************************************/
    /* PROPERTIES                                                           */
    /************************************************************************/

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, EditAnywhere)
	float TriggerDistance = 500.f;

	UPROPERTY(Category = "LookAt", BlueprintReadOnly, EditAnywhere)
	int32 Priority = 1;
	
	UPROPERTY(Category = "LookAt", BlueprintReadOnly, EditAnywhere)
	FName SocketOrBone;
	
	/************************************************************************/
    /* FUNCTIONS                                                            */
    /************************************************************************/

	// Sets default values for this component's properties
	ULookAtTargetComponent();

	UFUNCTION(Category = "LookAt", BlueprintPure)
    virtual FVector GetSourceLocation() const;
};
