/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "IMovieSceneTrackTemplateProducer.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneNameableTrack.h"
#include "MovieSceneFlipbookAnimationTrack.generated.h"

/**
 * Handles animation of flipbook mesh actors
 */
UCLASS(MinimalAPI)
class UMovieSceneFlipbookAnimationTrack : public UMovieSceneNameableTrack, public IMovieSceneTrackTemplateProducer
{
	GENERATED_UCLASS_BODY()

public:

	/** Adds a new animation to this track */
	virtual UMovieSceneSection* AddNewAnimationOnRow(FFrameNumber KeyTime, class UPaperFlipbook* AnimSequence, int32 RowIndex);

	/** Adds a new animation to this track on the next available/non-overlapping row */
	virtual UMovieSceneSection* AddNewAnimation(FFrameNumber KeyTime, class UPaperFlipbook* AnimSequence) { return AddNewAnimationOnRow(KeyTime, AnimSequence, INDEX_NONE); }

	/** Gets the animation sections at a certain time */
	TArray<UMovieSceneSection*> GetAnimSectionsAtTime(FFrameNumber Time);

public:

	// UMovieSceneTrack interface

	virtual void PostLoad() override;
	virtual void RemoveAllAnimationData() override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual bool IsEmpty() const override;
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual UMovieSceneSection* CreateNewSection() override;
	virtual bool PopulateEvaluationTree(TMovieSceneEvaluationTree<FMovieSceneTrackEvaluationData>& OutData) const override;
	virtual FMovieSceneTrackRowSegmentBlenderPtr GetRowSegmentBlender() const override;
	virtual bool SupportsMultipleRows() const override;

	// ~IMovieSceneTrackTemplateProducer interface
	virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;

	void SortSections();

#if WITH_EDITORONLY_DATA
	virtual FText GetDefaultDisplayName() const override;
#endif

#if WITH_EDITOR
	virtual void OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params) override;
#endif

private:

	/** List of all animation sections */
	UPROPERTY()
	TArray<UMovieSceneSection*> AnimationSections;

	UPROPERTY()
	bool bUseLegacySectionIndexBlend;
};
