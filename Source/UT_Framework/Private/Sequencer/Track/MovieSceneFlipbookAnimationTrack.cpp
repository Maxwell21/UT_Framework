/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "MovieSceneFlipbookAnimationTrack.h"
#include "MovieSceneEvaluationCustomVersion.h"
#include "MovieSceneEvaluationTreePopulationRules.h"
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

UMovieSceneSection* UMovieSceneFlipbookAnimationTrack::AddNewAnimationOnRow(FFrameNumber KeyTime, UPaperFlipbook* AnimSequence, int32 RowIndex)
{
	UMovieSceneFlipbookAnimationSection* NewSection = Cast<UMovieSceneFlipbookAnimationSection>(CreateNewSection());
	{
		NewSection->InitialPlacement(AnimationSections, KeyTime, KeyTime.Value + AnimSequence->GetTotalDuration(), SupportsMultipleRows());
		NewSection->Params.Animation = AnimSequence;
	}

	AddSection(*NewSection);

	return NewSection;
}

TArray<UMovieSceneSection*> UMovieSceneFlipbookAnimationTrack::GetAnimSectionsAtTime(FFrameNumber Time)
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

FMovieSceneEvalTemplatePtr UMovieSceneFlipbookAnimationTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
	return FMovieSceneFlipbookAnimationSectionTemplate(*CastChecked<const UMovieSceneFlipbookAnimationSection>(&InSection));
}

void UMovieSceneFlipbookAnimationTrack::SortSections()
{
	AnimationSections.Sort([](UMovieSceneSection& A,  UMovieSceneSection& B) {return ((A).GetTrueRange().GetLowerBoundValue() < (B).GetTrueRange().GetLowerBoundValue());});
}

UMovieSceneSection* UMovieSceneFlipbookAnimationTrack::CreateNewSection()
{
	return NewObject<UMovieSceneFlipbookAnimationSection>(this);
}

bool UMovieSceneFlipbookAnimationTrack::PopulateEvaluationTree(TMovieSceneEvaluationTree<FMovieSceneTrackEvaluationData>& OutData) const
{
	using namespace UE::MovieScene;

	if (!bUseLegacySectionIndexBlend)
	{
		FEvaluationTreePopulationRules::HighPassPerRow(AnimationSections, OutData);
	}
	else
	{
		// Use legacy blending... when there's overlapping, the section that makes it into the evaluation tree is
		// the one that appears later in the container arary of section data.
		//
		auto SortByLatestInArrayAndRow = [](const FEvaluationTreePopulationRules::FSortedSection& A, const FEvaluationTreePopulationRules::FSortedSection& B)
		{
			if (A.Row() == B.Row())
			{
				return A.Index > B.Index;
			}
			
			return A.Row() < B.Row();
		};

		UE::MovieScene::FEvaluationTreePopulationRules::HighPassCustomPerRow(AnimationSections, OutData, SortByLatestInArrayAndRow);
	}
	return true;
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

bool UMovieSceneFlipbookAnimationTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
	return SectionClass == UMovieSceneFlipbookAnimationSection::StaticClass();
}

#if WITH_EDITORONLY_DATA

FText UMovieSceneFlipbookAnimationTrack::GetDefaultDisplayName() const
{
	return LOCTEXT("TrackName", "Animation");
}

void UMovieSceneFlipbookAnimationTrack::OnSectionMoved(UMovieSceneSection& Section,
	const FMovieSceneSectionMovedParams& Params)
{
	SortSections();
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
