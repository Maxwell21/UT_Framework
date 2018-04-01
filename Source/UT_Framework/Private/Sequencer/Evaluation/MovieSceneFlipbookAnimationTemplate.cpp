/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "MovieSceneFlipbookAnimationTemplate.h"
#include "Compilation/MovieSceneCompilerRules.h"
#include "Components/SkeletalMeshComponent.h"
#include "MovieSceneEvaluation.h"
#include "IMovieScenePlayer.h"
#include "ObjectKey.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"

bool ShouldUsePreviewPlayback(IMovieScenePlayer& Player, UObject& RuntimeObject)
{
	// we also use PreviewSetAnimPosition in PIE when not playing, as we can preview in PIE
	bool bIsNotInPIEOrNotPlaying = (RuntimeObject.GetWorld() && !RuntimeObject.GetWorld()->HasBegunPlay()) || Player.GetPlaybackStatus() != EMovieScenePlayerStatus::Playing;
	return GIsEditor && bIsNotInPIEOrNotPlaying;
}

bool CanPlayAnimation(UPaperFlipbookComponent* PaperFlipbookComponent, UPaperFlipbook* Flipbook)
{
	return PaperFlipbookComponent && Flipbook;
}

void ResetAnimSequencerInstance(UObject& ObjectToRestore, IMovieScenePlayer& Player)
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

namespace MovieScene
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

			const bool bPreviewPlayback = ShouldUsePreviewPlayback(Player, *PaperFlipbookComponent);

			const EMovieScenePlayerStatus::Type PlayerStatus = Player.GetPlaybackStatus();

			// If the playback status is jumping, ie. one such occurrence is setting the time for thumbnail generation, disable anim notifies updates because it could fire audio
			const bool bFireNotifies = !bPreviewPlayback || (PlayerStatus != EMovieScenePlayerStatus::Jumping && PlayerStatus != EMovieScenePlayerStatus::Stopped);

			// When jumping from one cut to another cut, the delta time should be 0 so that anim notifies before the current position are not evaluated. Note, anim notifies at the current time should still be evaluated.
			const float DeltaTime = Context.HasJumped() ? 0.f : Context.GetRange().Size<float>();

			const bool bResetDynamics = PlayerStatus == EMovieScenePlayerStatus::Stepping || 
										PlayerStatus == EMovieScenePlayerStatus::Jumping || 
										PlayerStatus == EMovieScenePlayerStatus::Scrubbing || 
										(DeltaTime == 0.0f && PlayerStatus != EMovieScenePlayerStatus::Stopped); 
		
			static const bool bLooping = false;
			for (const FMinimalAnimParameters& AnimParams : InFinalValue.AllAnimations)
			{
				Player.PreAnimatedState.SetCaptureEntity(AnimParams.EvaluationScope.Key, AnimParams.EvaluationScope.CompletionMode);

				if (bPreviewPlayback)
				{
					PreviewSetAnimPosition(PersistentData, Player, PaperFlipbookComponent,
						AnimParams.Section, AnimParams.Animation, AnimParams.EvalTime,
						bLooping, bFireNotifies, DeltaTime, Player.GetPlaybackStatus() == EMovieScenePlayerStatus::Playing, bResetDynamics);
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

				Player.SavePreAnimatedState(*FlipbookInst, AnimTypeID, FStatelessPreAnimatedTokenProducer(&ResetAnimSequencerInstance));

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
				Player.SavePreAnimatedState(*FlipbookInst, AnimTypeID, FStatelessPreAnimatedTokenProducer(&ResetAnimSequencerInstance));

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

template<> FMovieSceneAnimTypeID GetBlendingDataType<MovieScene::FBlendedAnimation>()
{
	static FMovieSceneAnimTypeID TypeID = FMovieSceneAnimTypeID::Unique();
	return TypeID;
}

FMovieSceneFlipbookAnimationSectionTemplate::FMovieSceneFlipbookAnimationSectionTemplate(const UMovieSceneFlipbookAnimationSection& InSection)
	: Params(InSection.Params, InSection.GetStartTime(), InSection.GetEndTime())
{
}

void FMovieSceneFlipbookAnimationSectionTemplate::Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
	if (Params.Animation)
	{
		// calculate the time at which to evaluate the animation
		float EvalTime = Params.MapTimeToAnimation(Context.GetTime());

		FOptionalMovieSceneBlendType BlendType = SourceSection->GetBlendType();
		check(BlendType.IsValid());

		// Ensure the accumulator knows how to actually apply component transforms

 		FMovieSceneBlendingActuatorID ActuatorTypeID = MovieScene::FComponentAnimationActuator::GetActuatorTypeID();
 		FMovieSceneBlendingAccumulator& Accumulator = ExecutionTokens.GetBlendingAccumulator();
		if (!Accumulator.FindActuator<MovieScene::FBlendedAnimation>(ActuatorTypeID))
			Accumulator.DefineActuator(ActuatorTypeID, MakeShared<MovieScene::FComponentAnimationActuator>());

		// Add the blendable to the accumulator
		FMinimalAnimParameters AnimParams(Params.Animation, EvalTime, ExecutionTokens.GetCurrentScope(), GetSourceSection());
		
		ExecutionTokens.BlendToken(ActuatorTypeID, TBlendableToken<MovieScene::FBlendedAnimation>(AnimParams, BlendType.Get(), 1.f));
	}
}

float FMovieSceneFlipbookAnimationSectionTemplateParameters::MapTimeToAnimation(float ThisPosition) const
{
	ThisPosition = FMath::Clamp(ThisPosition, SectionStartTime, SectionEndTime);

	const float SectionPlayRate = PlayRate;
	const float AnimPlayRate = FMath::IsNearlyZero(SectionPlayRate) ? 1.0f : SectionPlayRate;

	const float SeqLength = GetSequenceLength() - (StartOffset + EndOffset);

	ThisPosition = (ThisPosition - SectionStartTime) * AnimPlayRate;
	if (SeqLength > 0.f)
	{
		ThisPosition = FMath::Fmod(ThisPosition, SeqLength);
	}
	ThisPosition += StartOffset;
	if (bReverse)
	{
		ThisPosition = (SeqLength - (ThisPosition - StartOffset)) + StartOffset;
	}

	return ThisPosition;
}