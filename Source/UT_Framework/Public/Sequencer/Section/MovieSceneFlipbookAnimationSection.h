/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once


#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "PaperFlipbook.h"
#include "MovieSceneFlipbookAnimationSection.generated.h"

USTRUCT(BlueprintType)
struct FMovieSceneFlipbookAnimationParams
{
	GENERATED_BODY()

	/** The animation this section plays */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UPaperFlipbook* Animation;

	/** The offset into the beginning of the animation clip */
	UPROPERTY(EditAnywhere, Category = "Animation")
	FFrameNumber StartOffset;

	/** The offset into the end of the animation clip */
	UPROPERTY(EditAnywhere, Category = "Animation")
	FFrameNumber EndOffset;

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
 * Movie scene section that control flipbook animation
 */
UCLASS( MinimalAPI )
class UMovieSceneFlipbookAnimationSection : public UMovieSceneSection
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

	/** Get Frame Time as Animation Time*/
	UT_FRAMEWORK_API float MapTimeToAnimation(FFrameTime InPosition, FFrameRate InFrameRate) const;

	//~ MovieSceneSection interface
	virtual UMovieSceneSection* SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys) override;
	virtual void GetSnapTimes(TArray<FFrameNumber>& OutSnapTimes, bool bGetSectionBorders) const override;
	virtual FMovieSceneEvalTemplatePtr GenerateTemplate() const override;

	/** ~UObject interface */
	virtual void PostLoad() override;
	virtual void Serialize(FArchive& Ar) override;

#if WITH_EDITOR

private:

	//~ UObject interface
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	float PreviousPlayRate;

#endif

};
