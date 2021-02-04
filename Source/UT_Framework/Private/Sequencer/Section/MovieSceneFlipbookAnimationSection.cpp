/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "MovieSceneFlipbookAnimationSection.h"
#include "Evaluation/MovieSceneFlipbookAnimationTemplate.h"
#include "MessageLog.h"
#include "MovieScene.h"
#include "SequencerObjectVersion.h"

#define LOCTEXT_NAMESPACE "MovieSceneFlipbookAnimationSection"

namespace
{
	FName DefaultSlotName( "DefaultSlot" );
}

FFrameNumber GetFirstLoopStartOffsetAtTrimTime(FQualifiedFrameTime TrimTime, const FMovieSceneFlipbookAnimationParams& Params, FFrameNumber StartFrame, FFrameRate FrameRate)
{
	const float AnimPlayRate = FMath::IsNearlyZero(Params.PlayRate) ? 1.0f : Params.PlayRate;
	const float AnimPosition = (TrimTime.Time - StartFrame) / TrimTime.Rate * AnimPlayRate;
	const float SeqLength = Params.GetSequenceLength() - FrameRate.AsSeconds(Params.StartOffset + Params.EndOffset) / AnimPlayRate;

	FFrameNumber NewOffset = FrameRate.AsFrameNumber(FMath::Fmod(AnimPosition, SeqLength));
	NewOffset += Params.FirstLoopStartFrameOffset;

	const FFrameNumber SeqLengthInFrames = FrameRate.AsFrameNumber(SeqLength);
	while (NewOffset >= SeqLengthInFrames)
		NewOffset -= SeqLengthInFrames;

	return NewOffset;
}

FMovieSceneFlipbookAnimationParams::FMovieSceneFlipbookAnimationParams()
{
	Animation = nullptr;
	StartOffset = FFrameNumber(0);
	EndOffset = FFrameNumber(0);
	PlayRate = 1.f;
	bReverse = false;
}

FFrameNumber GetStartOffsetAtTrimTime(FQualifiedFrameTime TrimTime, const FMovieSceneFlipbookAnimationParams& Params, FFrameNumber StartFrame, FFrameRate FrameRate)
{
	float AnimPlayRate = FMath::IsNearlyZero(Params.PlayRate) ? 1.0f : Params.PlayRate;
	float AnimPosition = (TrimTime.Time - StartFrame) / TrimTime.Rate * AnimPlayRate;
	float SeqLength = Params.GetSequenceLength() - FrameRate.AsSeconds(Params.StartOffset + Params.EndOffset) / AnimPlayRate;

	FFrameNumber NewOffset = FrameRate.AsFrameNumber(FMath::Fmod(AnimPosition, SeqLength));
	NewOffset += Params.StartOffset;

	return NewOffset;
}

UMovieSceneFlipbookAnimationSection::UMovieSceneFlipbookAnimationSection(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	BlendType = EMovieSceneBlendType::Absolute;
	EvalOptions.EnableAndSetCompletionMode(EMovieSceneCompletionMode::RestoreState);

#if WITH_EDITOR
	PreviousPlayRate = Params.PlayRate;
#endif
}

void UMovieSceneFlipbookAnimationSection::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FSequencerObjectVersion::GUID);
	Super::Serialize(Ar);
}

void UMovieSceneFlipbookAnimationSection::PostLoad()
{
	// if version is less than this
	if (GetLinkerCustomVersion(FSequencerObjectVersion::GUID) < FSequencerObjectVersion::ConvertEnableRootMotionToForceRootLock)
	{
		UPaperFlipbook* Flipbook = Params.Animation;
		if (Flipbook)
		{
			// this is not ideal, but previously single player node was using this flag to whether or not to extract root motion
			// with new anim sequencer instance, this would break because we use the instance flag to extract root motion or not
			// so instead of setting that flag, we use bForceRootLock flag to asset
			// this can have side effect, where users didn't want that to be on to start with
			// So we'll notify users to let them know this has to be saved
			Flipbook->MarkPackageDirty();
			// warning to users
#if WITH_EDITOR			
			if (!IsRunningGame())
			{
				static FName NAME_LoadErrors("LoadErrors");
				FMessageLog LoadErrors(NAME_LoadErrors);

				TSharedRef<FTokenizedMessage> Message = LoadErrors.Warning();
				Message->AddToken(FTextToken::Create(LOCTEXT("RootMotionFixUp1", "The Animation ")));
				Message->AddToken(FAssetNameToken::Create(Flipbook->GetPathName(), FText::FromString(GetNameSafe(Flipbook))));
				Message->AddToken(FTextToken::Create(LOCTEXT("RootMotionFixUp2", "will be set to ForceRootLock on. Please save the animation if you want to keep this change.")));
				Message->SetSeverity(EMessageSeverity::Warning);
				LoadErrors.Notify();
			}
#endif // WITH_EDITOR

			UE_LOG(LogMovieScene, Warning, TEXT("%s Animation has set ForceRootLock to be used in Sequencer. If this animation is used in anywhere else using root motion, that will cause conflict."), *Flipbook->GetName());
		}
	}

	Super::PostLoad();
}

/*
FMovieSceneEvalTemplatePtr UMovieSceneFlipbookAnimationSection::GenerateTemplate() const
{
	return FMovieSceneFlipbookAnimationSectionTemplate(*this);
}
*/

float UMovieSceneFlipbookAnimationSection::MapTimeToAnimation(FFrameTime InPosition, FFrameRate InFrameRate) const
{
	FMovieSceneFlipbookAnimationSectionTemplateParameters TemplateParams(Params, GetInclusiveStartFrame(), GetExclusiveEndFrame());
	return TemplateParams.MapTimeToAnimation(InPosition, InFrameRate);
}

TOptional<TRange<FFrameNumber>> UMovieSceneFlipbookAnimationSection::GetAutoSizeRange() const
{
	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();

	FFrameTime AnimationLength = Params.GetSequenceLength() * FrameRate;
	int32 IFrameNumber = AnimationLength.FrameNumber.Value + (int)(AnimationLength.GetSubFrame() + 0.5f);

	return TRange<FFrameNumber>(GetInclusiveStartFrame(), GetInclusiveStartFrame() + IFrameNumber + 1);
}

UMovieSceneSection* UMovieSceneFlipbookAnimationSection::SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys)
{
	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();

	FFrameNumber NewOffset = HasStartFrame() ? GetStartOffsetAtTrimTime(SplitTime, Params, GetInclusiveStartFrame(), FrameRate) : 0;
	NewOffset += Params.StartOffset;

	UMovieSceneSection* NewSection = Super::SplitSection(SplitTime, bDeleteKeys);
	if (NewSection != nullptr)
	{
		UMovieSceneFlipbookAnimationSection* NewFlipbookSection = Cast<UMovieSceneFlipbookAnimationSection>(NewSection);
		NewFlipbookSection->Params.StartOffset = NewOffset;
	}
	return NewSection;
}

void UMovieSceneFlipbookAnimationSection::TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys)
{
	SetFlags(RF_Transactional);

	if (TryModify())
	{
		if (bTrimLeft)
		{
			FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();

			Params.FirstLoopStartFrameOffset = HasStartFrame() ? GetFirstLoopStartOffsetAtTrimTime(TrimTime, Params, GetInclusiveStartFrame(), FrameRate) : 0;
		}

		Super::TrimSection(TrimTime, bTrimLeft, bDeleteKeys);
	}
}

void UMovieSceneFlipbookAnimationSection::GetSnapTimes(TArray<FFrameNumber>& OutSnapTimes, bool bGetSectionBorders) const
{
	Super::GetSnapTimes(OutSnapTimes, bGetSectionBorders);
}

TOptional<FFrameTime> UMovieSceneFlipbookAnimationSection::GetOffsetTime() const
{
	return TOptional<FFrameTime>(Params.FirstLoopStartFrameOffset);
}

#if WITH_EDITOR

void UMovieSceneFlipbookAnimationSection::PreEditChange(FProperty* PropertyAboutToChange)
{
	// Store the current play rate so that we can compute the amount to compensate the section end time when the play rate changes
	PreviousPlayRate = Params.PlayRate;

	Super::PreEditChange(PropertyAboutToChange);
}

void UMovieSceneFlipbookAnimationSection::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

#undef LOCTEXT_NAMESPACE 