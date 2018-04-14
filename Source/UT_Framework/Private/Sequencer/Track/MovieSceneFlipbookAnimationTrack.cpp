/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "MovieSceneFlipbookAnimationTrack.h"
#include "MovieSceneEvaluationCustomVersion.h"
#include "MovieSceneFlipbookAnimationSection.h"
#include "Compilation/MovieSceneCompilerRules.h"
#include "Evaluation/MovieSceneEvaluationTrack.h"
#include "Evaluation/MovieSceneFlipbookAnimationTemplate.h"
#include "Compilation/IMovieSceneTemplateGenerator.h"

#define LOCTEXT_NAMESPACE "MovieSceneFlipbookAnimationTrack"

/* UMovieSceneFlipbookAnimationTrack structors
 *****************************************************************************/

UMovieSceneFlipbookAnimationTrack::UMovieSceneFlipbookAnimationTrack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bUseLegacySectionIndexBlend(false)
{
#if WITH_EDITORONLY_DATA
	TrackTint = FColor(124, 15, 124, 65);
#endif

	SupportedBlendTypes.Add(EMovieSceneBlendType::Absolute);

	EvalOptions.bEvaluateNearestSection_DEPRECATED = EvalOptions.bCanEvaluateNearestSection = true;
}

/* UMovieSceneFlipbookAnimationTrack interface
 *****************************************************************************/

void UMovieSceneFlipbookAnimationTrack::AddNewAnimation(float KeyTime, UPaperFlipbook* AnimSequence)
{
	UMovieSceneFlipbookAnimationSection* NewSection = Cast<UMovieSceneFlipbookAnimationSection>(CreateNewSection());
	{
		NewSection->InitialPlacement(AnimationSections, KeyTime, KeyTime + AnimSequence->GetTotalDuration(), SupportsMultipleRows());
		NewSection->Params.Animation = AnimSequence;
	}

	AddSection(*NewSection);
}

TArray<UMovieSceneSection*> UMovieSceneFlipbookAnimationTrack::GetAnimSectionsAtTime(float Time)
{
	TArray<UMovieSceneSection*> Sections;
	for (auto Section : AnimationSections)
	{
		if (Section->IsTimeWithinSection(Time))
		{
			Sections.Add(Section);
		}
	}

	return Sections;
}

/* UMovieSceneTrack interface
 *****************************************************************************/

void UMovieSceneFlipbookAnimationTrack::PostLoad()
{
	Super::PostLoad();

	if (GetLinkerCustomVersion(FMovieSceneEvaluationCustomVersion::GUID) < FMovieSceneEvaluationCustomVersion::AddBlendingSupport)
	{
		bUseLegacySectionIndexBlend = true;
	}
}

const TArray<UMovieSceneSection*>& UMovieSceneFlipbookAnimationTrack::GetAllSections() const
{
	return AnimationSections;
}

bool UMovieSceneFlipbookAnimationTrack::SupportsMultipleRows() const
{
	return true;
}

UMovieSceneSection* UMovieSceneFlipbookAnimationTrack::CreateNewSection()
{
	return NewObject<UMovieSceneFlipbookAnimationSection>(this);
}

void UMovieSceneFlipbookAnimationTrack::RemoveAllAnimationData()
{
	AnimationSections.Empty();
}

bool UMovieSceneFlipbookAnimationTrack::HasSection(const UMovieSceneSection& Section) const
{
	return AnimationSections.Contains(&Section);
}

void UMovieSceneFlipbookAnimationTrack::AddSection(UMovieSceneSection& Section)
{
	AnimationSections.Add(&Section);
}

void UMovieSceneFlipbookAnimationTrack::RemoveSection(UMovieSceneSection& Section)
{
	AnimationSections.Remove(&Section);
}

bool UMovieSceneFlipbookAnimationTrack::IsEmpty() const
{
	return AnimationSections.Num() == 0;
}

TRange<float> UMovieSceneFlipbookAnimationTrack::GetSectionBoundaries() const
{
	TArray<TRange<float>> Bounds;

	for (auto Section : AnimationSections)
	{
		Bounds.Add(Section->GetRange());
	}

	return TRange<float>::Hull(Bounds);
}

#if WITH_EDITORONLY_DATA

FText UMovieSceneFlipbookAnimationTrack::GetDefaultDisplayName() const
{
	return LOCTEXT("TrackName", "Animation");
}

#endif

FMovieSceneTrackRowSegmentBlenderPtr UMovieSceneFlipbookAnimationTrack::GetRowSegmentBlender() const
{
	// Apply an upper bound exclusive blend
	struct FFlipbookAnimationRowCompilerRules : FMovieSceneTrackRowSegmentBlender
	{
		bool bUseLegacySectionIndexBlend;
		FFlipbookAnimationRowCompilerRules(bool bInUseLegacySectionIndexBlend) : bUseLegacySectionIndexBlend(bInUseLegacySectionIndexBlend) {}

		virtual void Blend(FSegmentBlendData& BlendData) const override
		{
			// Run the default high pass filter for overlap priority
			MovieSceneSegmentCompiler::FilterOutUnderlappingSections(BlendData);

			if (bUseLegacySectionIndexBlend)
			{
				// Weed out based on array index (legacy behaviour)
				MovieSceneSegmentCompiler::BlendSegmentLegacySectionOrder(BlendData);
			}
		}

	};

	return FFlipbookAnimationRowCompilerRules(bUseLegacySectionIndexBlend);
}

#undef LOCTEXT_NAMESPACE
