/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Curves/KeyHandle.h"
#include "MovieSceneSection.h"
#include "PaperFlipbook.h"
#include "MovieSceneFlipbookAnimationSection.generated.h"

USTRUCT()
struct FMovieSceneFlipbookAnimationParams
{
	GENERATED_BODY()

	/** The animation this section plays */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UPaperFlipbook* Animation;

	/** The offset into the beginning of the animation clip */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float StartOffset;

	/** The offset into the end of the animation clip */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float EndOffset;

	/** The playback rate of the animation clip */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float PlayRate;

	/** Reverse the playback of the animation clip */
	UPROPERTY(EditAnywhere, Category = "Animation")
	uint32 bReverse : 1;
	
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FMovieSceneFlipbookAnimationParams();

	/** Gets the animation duration, modified by play rate */
	float GetDuration() const { return FMath::IsNearlyZero(PlayRate) || Animation == nullptr ? 0.f : Animation->GetTotalDuration() / PlayRate; }

	/** Gets the animation sequence length, not modified by play rate */
	float GetSequenceLength() const { return Animation != nullptr ? Animation->GetTotalDuration() : 0.f; }

};

/**
 * Movie scene section that control skeletal animation
 */
UCLASS( MinimalAPI )
class UMovieSceneFlipbookAnimationSection
	: public UMovieSceneSection
{
	GENERATED_UCLASS_BODY()

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

public:

	UPROPERTY(EditAnywhere, Category = "Animation", meta=(ShowOnlyInnerProperties))
	FMovieSceneFlipbookAnimationParams Params;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	//~ MovieSceneSection interface

	virtual void MoveSection( float DeltaPosition, TSet<FKeyHandle>& KeyHandles ) override;
	virtual void DilateSection( float DilationFactor, float Origin, TSet<FKeyHandle>& KeyHandles  ) override;
	virtual UMovieSceneSection* SplitSection(float SplitTime) override;
	virtual void GetKeyHandles(TSet<FKeyHandle>& OutKeyHandles, TRange<float> TimeRange) const override;
	virtual void GetSnapTimes(TArray<float>& OutSnapTimes, bool bGetSectionBorders) const override;
	virtual TOptional<float> GetOffsetTime() const override { return TOptional<float>(Params.StartOffset); }
	virtual TOptional<float> GetKeyTime( FKeyHandle KeyHandle ) const override { return TOptional<float>(); }
	virtual void SetKeyTime( FKeyHandle KeyHandle, float Time ) override { }
	virtual FMovieSceneEvalTemplatePtr GenerateTemplate() const override;

	/** ~UObject interface */
	virtual void PostLoad() override;
	virtual void Serialize(FArchive& Ar) override;
private:

	//~ UObject interface

#if WITH_EDITOR

	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	float PreviousPlayRate;

#endif

};
