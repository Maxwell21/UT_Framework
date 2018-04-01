/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#include "Section/MovieSceneFlipbookAnimationSection.h"
#include "MovieSceneFlipbookAnimationTemplate.generated.h"

USTRUCT()
struct FMovieSceneFlipbookAnimationSectionTemplateParameters : public FMovieSceneFlipbookAnimationParams
{
	GENERATED_BODY()

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	float SectionStartTime;

	UPROPERTY()
	float SectionEndTime;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FMovieSceneFlipbookAnimationSectionTemplateParameters() {}
	FMovieSceneFlipbookAnimationSectionTemplateParameters(const FMovieSceneFlipbookAnimationParams& BaseParams, float InSectionStartTime, float InSectionEndTime)
		: FMovieSceneFlipbookAnimationParams(BaseParams)
		, SectionStartTime(InSectionStartTime)
		, SectionEndTime(InSectionEndTime)
	{}

	float MapTimeToAnimation(float InPosition) const;
};

USTRUCT()
struct FMovieSceneFlipbookAnimationSectionTemplate : public FMovieSceneEvalTemplate
{
	GENERATED_BODY()
	
	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	FMovieSceneFlipbookAnimationSectionTemplateParameters Params;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FMovieSceneFlipbookAnimationSectionTemplate() {}
	FMovieSceneFlipbookAnimationSectionTemplate(const UMovieSceneFlipbookAnimationSection& Section);

	virtual UScriptStruct& GetScriptStructImpl() const override { return *StaticStruct(); }
	virtual void Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;

};
