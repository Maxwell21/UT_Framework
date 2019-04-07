/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "MovieSceneFlipbookAnimationTemplate.h"
#include "Compilation/MovieSceneCompilerRules.h"
#include "MovieSceneEvaluation.h"
#include "IMovieScenePlayer.h"
#include "ObjectKey.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"

bool ShouldUsePreviewFlipbookPlayback(IMovieScenePlayer& Player, UObject& RuntimeObject)
{
	// we also use PreviewSetAnimPosition in PIE when not playing, as we can preview in PIE
	bool bIsNotInPIEOrNotPlaying = (RuntimeObject.GetWorld() && !RuntimeObject.GetWorld()->HasBegunPlay()) || Player.GetPlaybackStatus() != EMovieScenePlayerStatus::Playing;
	return GIsEditor && bIsNotInPIEOrNotPlaying;
}

bool CanPlayAnimation(UPaperFlipbookComponent* PaperFlipbookComponent, UPaperFlipbook* Flipbook)
{
	return PaperFlipbookComponent && Flipbook;
}

void ResetFlipbookInstance(UObject& ObjectToRestore, IMovieScenePlayer& Player)
{
	//CastChecked<UPaperFlipbookComponent>(&ObjectToRestore)->Stop();
}

// struct FPreAnimatedAnimationTokenProducer : IMovieScenePreAnimatedTokenProducer
// {
// 	virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject& Object) const
// 	{
// 		struct FToken : IMovieScenePreAnimatedToken
// 		{
// 			FToken(USkeletalMeshComponent* InComponent)
// 			{
// 				// Cache this object's current update flag and animation mode
// 				MeshComponentUpdateFlag = InComponent->MeshComponentUpdateFlag;
// 				AnimationMode = InComponent->GetAnimationMode();
// 			}
// 
// 			virtual void RestoreState(UObject& ObjectToRestore, IMovieScenePlayer& Player)
// 			{
// 				USkeletalMeshComponent* Component = CastChecked<USkeletalMeshComponent>(&ObjectToRestore);
// 
// 				UAnimSequencerInstance* SequencerInst = Cast<UAnimSequencerInstance>(Component->GetAnimInstance());
// 				if (SequencerInst)
// 				{
// 					SequencerInst->ResetNodes();
// 				}
// 
// 				UAnimSequencerInstance::UnbindFromSkeletalMeshComponent(Component);
// 
// 				// Reset the mesh component update flag and animation mode to what they were before we animated the object
// 				Component->MeshComponentUpdateFlag = MeshComponentUpdateFlag;
// 				if (Component->GetAnimationMode() != AnimationMode)
// 				{
// 					// this SetAnimationMode reinitializes even if the mode is same
// 					// if we're using same anim blueprint, we don't want to keep reinitializing it. 
// 					Component->SetAnimationMode(AnimationMode);
// 				}
// 			}
// 
// 			EMeshComponentUpdateFlag::Type MeshComponentUpdateFlag;
// 			EAnimationMode::Type AnimationMode;
// 		};
// 
// 		return FToken(CastChecked<USkeletalMeshComponent>(&Object));
// 	}
// };
// 
// 
struct FMinimalAnimParameters
{
	FMinimalAnimParameters(UPaperFlipbook* InAnimation, float InEvalTime, const FMovieSceneEvaluationScope& InScope, FObjectKey InSection)
		: Animation(InAnimation)
		, EvalTime(InEvalTime)
		, EvaluationScope(InScope)
		, Section(InSection)
	{}
	
	UPaperFlipbook* Animation;
	float EvalTime;
	FMovieSceneEvaluationScope EvaluationScope;
	FObjectKey Section;
};

namespace MovieSceneFlipbook
{
	struct FBlendedAnimation
	{
		TArray<FMinimalAnimParameters> AllAnimations;

		FBlendedAnimation& Resolve(TMovieSceneInitialValueStore<FBlendedAnimation>& InitialValueStore)
		{
			return *this;
		}
	};

	void BlendValue(FBlendedAnimation& OutBlend, const FMinimalAnimParameters& InValue, float Weight, EMovieSceneBlendType BlendType, TMovieSceneInitialValueStore<FBlendedAnimation>& InitialValueStore)
	{
		OutBlend.AllAnimations.Add(InValue);
	};

	struct FComponentAnimationActuator : TMovieSceneBlendingActuator<FBlendedAnimation>
	{
		FComponentAnimationActuator() : TMovieSceneBlendingActuator<FBlendedAnimation>(GetActuatorTypeID()) {}

		static FMovieSceneBlendingActuatorID GetActuatorTypeID()
		{
			static FMovieSceneAnimTypeID TypeID = TMovieSceneAnimTypeID<FComponentAnimationActuator>();
			return FMovieSceneBlendingActuatorID(TypeID);
		}

		virtual FBlendedAnimation RetrieveCurrentValue(UObject* InObject, IMovieScenePlayer* Player) const
		{
			check(false);
			return FBlendedAnimation();
		}

		virtual void Actuate(UObject* InObject, const FBlendedAnimation& InFinalValue, const TBlendableTokenStack<FBlendedAnimation>& OriginalStack, const FMovieSceneContext& Context, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) override
		{
			ensureMsgf(InObject, TEXT("Attempting to evaluate an Animation track with a null object."));

			UPaperFlipbookComponent* PaperFlipbookComponent = PaperFlipbookComponentFromObject(InObject);
			if (!PaperFlipbookComponent)
				return;

			static FMovieSceneAnimTypeID AnimTypeID = TMovieSceneAnimTypeID<FComponentAnimationActuator>();
			//OriginalStack.SavePreAnimatedState(Player, *PaperFlipbookComponent, AnimTypeID, FPreAnimatedAnimationTokenProducer());

			const bool bPreviewPlayback = ShouldUsePreviewFlipbookPlayback(Player, *PaperFlipbookComponent);

			const EMovieScenePlayerStatus::Type PlayerStatus = Player.GetPlaybackStatus();

			// If the playback status is jumping, ie. one such occurrence is setting the time for thumbnail generation, disable anim notifies updates because it could fire audio
			const bool bFireNotifies = !bPreviewPlayback || (PlayerStatus != EMovieScenePlayerStatus::Jumping && PlayerStatus != EMovieScenePlayerStatus::Stopped);

			// When jumping from one cut to another cut, the delta time should be 0 so that anim notifies before the current position are not evaluated. Note, anim notifies at the current time should still be evaluated.
			const FFrameTime DeltaTime = Context.HasJumped() ? FFrameTime() : Context.GetRange().Size<FFrameTime>();

			const bool bResetDynamics = PlayerStatus == EMovieScenePlayerStatus::Stepping || 
										PlayerStatus == EMovieScenePlayerStatus::Jumping || 
										PlayerStatus == EMovieScenePlayerStatus::Scrubbing || 
										(DeltaTime.GetSubFrame() == 0.0f && PlayerStatus != EMovieScenePlayerStatus::Stopped); 
		
			static const bool bLooping = false;
			for (const FMinimalAnimParameters& AnimParams : InFinalValue.AllAnimations)
			{
				Player.PreAnimatedState.SetCaptureEntity(AnimParams.EvaluationScope.Key, AnimParams.EvaluationScope.CompletionMode);

				if (bPreviewPlayback)
				{
					PreviewSetAnimPosition(PersistentData, Player, PaperFlipbookComponent,
						AnimParams.Section, AnimParams.Animation, AnimParams.EvalTime,
						bLooping, bFireNotifies, DeltaTime.GetSubFrame(), Player.GetPlaybackStatus() == EMovieScenePlayerStatus::Playing, bResetDynamics);
				}
				else
				{
					SetAnimPosition(PersistentData, Player, PaperFlipbookComponent,
						AnimParams.Section, AnimParams.Animation, AnimParams.EvalTime,
						bLooping, bFireNotifies);
				}
			}

			Player.PreAnimatedState.SetCaptureEntity(FMovieSceneEvaluationKey(), EMovieSceneCompletionMode::KeepState);
		}

	private:

		static UPaperFlipbookComponent* PaperFlipbookComponentFromObject(UObject* InObject)
		{
			UPaperFlipbookComponent* PaperFlipbookComponent = Cast<UPaperFlipbookComponent>(InObject);
			if (PaperFlipbookComponent)
				return PaperFlipbookComponent;
			
			// then check to see if we are controlling an actor & if so use its first UPaperFlipbookComponent 
			else if (AActor* Actor = Cast<AActor>(InObject))
				return Actor->FindComponentByClass<UPaperFlipbookComponent>();
			
			return nullptr;
		}

		void SetAnimPosition(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player, UPaperFlipbookComponent* PaperFlipbookComponent, FObjectKey Section, UPaperFlipbook* InFlipbook, float InPosition, bool bLooping, bool bFireNotifies)
		{
			if (!CanPlayAnimation(PaperFlipbookComponent, InFlipbook))
				return;

			UPaperFlipbook* FlipbookInst = PaperFlipbookComponent->GetFlipbook();
			if (FlipbookInst)
			{
				FMovieSceneAnimTypeID AnimTypeID = SectionToAnimationIDs.GetAnimTypeID(Section);

				Player.SavePreAnimatedState(*FlipbookInst, AnimTypeID, FStatelessPreAnimatedTokenProducer(&ResetFlipbookInstance));

				// Set parameters
				PaperFlipbookComponent->SetFlipbook(InFlipbook);
				PaperFlipbookComponent->SetPlaybackPosition(InPosition, bFireNotifies);
				PaperFlipbookComponent->SetLooping(bLooping);
			}
		}

		void PreviewSetAnimPosition(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player, UPaperFlipbookComponent* PaperFlipbookComponent, FObjectKey Section, UPaperFlipbook* InFlipbook, float InPosition, bool bLooping, bool bFireNotifies, float DeltaTime, bool bPlaying, bool bResetDynamics)
		{
			if (!CanPlayAnimation(PaperFlipbookComponent, InFlipbook))
				return;

			UPaperFlipbook* FlipbookInst = PaperFlipbookComponent->GetFlipbook();
			if (FlipbookInst)
			{
				// Unique anim type ID per slot
				FMovieSceneAnimTypeID AnimTypeID = SectionToAnimationIDs.GetAnimTypeID(Section);
				Player.SavePreAnimatedState(*FlipbookInst, AnimTypeID, FStatelessPreAnimatedTokenProducer(&ResetFlipbookInstance));

				// Set parameters
				PaperFlipbookComponent->SetFlipbook(InFlipbook);
				PaperFlipbookComponent->SetPlaybackPosition(InPosition, bFireNotifies);
				PaperFlipbookComponent->SetLooping(bLooping);
			}
		}

		TMovieSceneAnimTypeIDContainer<FName> MontageSlotAnimationIDs;
		TMovieSceneAnimTypeIDContainer<FObjectKey> SectionToAnimationIDs;
	};

}	// namespace MovieScene

template<> FMovieSceneAnimTypeID GetBlendingDataType<MovieSceneFlipbook::FBlendedAnimation>()
{
	static FMovieSceneAnimTypeID TypeID = FMovieSceneAnimTypeID::Unique();
	return TypeID;
}

FMovieSceneFlipbookAnimationSectionTemplate::FMovieSceneFlipbookAnimationSectionTemplate(const UMovieSceneFlipbookAnimationSection& InSection)
	: Params(InSection.Params, InSection.GetInclusiveStartFrame().Value, InSection.GetExclusiveEndFrame().Value)
{
}

void FMovieSceneFlipbookAnimationSectionTemplate::Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
	if (Params.Animation)
	{
		// calculate the time at which to evaluate the animation
		float EvalTime = Params.MapTimeToAnimation(Context.GetTime(), Context.GetFrameRate());

		FOptionalMovieSceneBlendType BlendType = GetSourceSection()->GetBlendType();
		check(BlendType.IsValid());

		// Ensure the accumulator knows how to actually apply component transforms
		FMovieSceneBlendingActuatorID ActuatorTypeID = MovieSceneFlipbook::FComponentAnimationActuator::GetActuatorTypeID();
		FMovieSceneBlendingAccumulator& Accumulator = ExecutionTokens.GetBlendingAccumulator();
		if (!Accumulator.FindActuator<MovieSceneFlipbook::FBlendedAnimation>(ActuatorTypeID))
		{
			Accumulator.DefineActuator(ActuatorTypeID, MakeShared<MovieSceneFlipbook::FComponentAnimationActuator>());
		}

		UPaperFlipbook* Animation = Params.Animation;

		// Add the blendable to the accumulator
		FMinimalAnimParameters AnimParams(Animation, EvalTime, ExecutionTokens.GetCurrentScope(), GetSourceSection());
		ExecutionTokens.BlendToken(ActuatorTypeID, TBlendableToken<MovieSceneFlipbook::FBlendedAnimation>(AnimParams, BlendType.Get(), 1.f));
	}
}

float FMovieSceneFlipbookAnimationSectionTemplateParameters::MapTimeToAnimation(FFrameTime InPosition, FFrameRate InFrameRate) const
{
	InPosition = FMath::Clamp(InPosition, FFrameTime(SectionStartTime), FFrameTime(SectionEndTime - 1));

	const float SectionPlayRate = PlayRate;
	const float AnimPlayRate = FMath::IsNearlyZero(SectionPlayRate) ? 1.0f : SectionPlayRate;

	const float SeqLength = GetSequenceLength() - InFrameRate.AsSeconds(StartOffset + EndOffset);

	float AnimPosition = FFrameTime::FromDecimal((InPosition - SectionStartTime).AsDecimal() * AnimPlayRate) / InFrameRate;
	if (SeqLength > 0.f)
	{
		AnimPosition = FMath::Fmod(AnimPosition, SeqLength);
	}
	AnimPosition += InFrameRate.AsSeconds(StartOffset);
	if (bReverse)
	{
		AnimPosition = (SeqLength - (AnimPosition - InFrameRate.AsSeconds(StartOffset))) + InFrameRate.AsSeconds(StartOffset);
	}

	return AnimPosition;
}
