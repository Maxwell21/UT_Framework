/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LatentActions.h"
#include "SpriteFunctionLibrary.generated.h"

class UPaperFlipbookComponent;
class UPaperFlipbook;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API USpriteFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	/**
	 * Play flipbook animation and continue process only on complete
	 *
	 * UPaperFlipbookComponent* Target
	 * UPaperFlipbook* Flipbook
	 * float Rate
	 */
	UFUNCTION(Category = "Umbra Framework | SpriteFunctionLibrary", BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"))
	static void	PlayFlipbookMontage(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UPaperFlipbookComponent* Target, UPaperFlipbook* Flipbook, float Rate = 1.f);
};

/**
 * FFlipbookMontageAction
 * Waiting for montage been completed
 */
class FFlipbookMontageAction : public FPendingLatentAction
{
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/
	
	float TimeRemaining;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FFlipbookMontageAction(float Duration, const FLatentActionInfo& LatentInfo)
		: TimeRemaining(Duration)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		TimeRemaining -= Response.ElapsedTime();
		Response.FinishAndTriggerIf(TimeRemaining <= 0.0f, ExecutionFunction, OutputLink, CallbackTarget);
	}

#if WITH_EDITOR

	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		return FText::Format(NSLOCTEXT("FlipbookMontageAction", "FFlipbookMontageActionTime", "Animation time remaining (%.3f seconds left)"), FText::AsNumber(TimeRemaining)).ToString();
	}

#endif

};
