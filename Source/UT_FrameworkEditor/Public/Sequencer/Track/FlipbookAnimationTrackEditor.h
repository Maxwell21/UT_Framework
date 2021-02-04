/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Widgets/SWidget.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"

struct FAssetData;
class FFloatCurveKeyArea;
class FMenuBuilder;
class FSequencerSectionPainter;
class UMovieSceneFlipbookAnimationSection;

/**
 * Tools for animation tracks
 */
class FFlipbookAnimationTrackEditor : public FMovieSceneTrackEditor
{

public:

	/**
	 * Constructor
	 *
	 * @param InSequencer The sequencer instance to be used by this tool
	 */
	FFlipbookAnimationTrackEditor( TSharedRef<ISequencer> InSequencer );

	/** Virtual destructor. */
	virtual ~FFlipbookAnimationTrackEditor() { }

	/**
	* Keeps track of how many skeletal animation track editors we have*
	*/
	static int32 NumberActive;

	/**
	 * Creates an instance of this class.  Called by a sequencer 
	 *
	 * @param OwningSequencer The sequencer instance to be used by this tool
	 * @return The new instance of this class
	 */
	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer );

	// ISequencerTrackEditor interface
	//virtual void AddKey(const FGuid& ObjectGuid) override;
	virtual void BuildObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass) override;
	virtual bool HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding ) override;
	virtual bool SupportsType( TSubclassOf<UMovieSceneTrack> Type ) const override;
	virtual void BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track ) override;
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
	virtual void OnInitialize() override;
	virtual void OnRelease() override;

private:

	/** Animation sub menu */
	TSharedRef<SWidget> BuildAnimationSubMenu(FGuid ObjectBinding, UMovieSceneTrack* Track);
	void AddAnimationSubMenu(FMenuBuilder& MenuBuilder, FGuid ObjectBinding, UMovieSceneTrack* Track);

	/** Animation sub menu filter function */
	bool ShouldFilterAsset(const FAssetData& AssetData);

	/** Animation asset selected  */
	void OnAnimationAssetSelected(const FAssetData& AssetData, FGuid ObjectBinding, UMovieSceneTrack* Track);

	/** Delegate for AnimatablePropertyChanged in AddKey */
	FKeyPropertyResult AddKeyInternal(FFrameNumber KeyTime, UObject* Object, class UPaperFlipbook* AnimSequence, UMovieSceneTrack* Track, int32 RowIndex);

};


/** Class for animation sections */
class FFlipbookAnimationSection
	: public ISequencerSection
	, public TSharedFromThis<FFlipbookAnimationSection>
{

public:

	/** Constructor. */
	FFlipbookAnimationSection(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer);

	/** Virtual destructor. */
	virtual ~FFlipbookAnimationSection() { }

	// ISequencerSection interface

	virtual UMovieSceneSection* GetSectionObject() override;
	virtual FText GetSectionTitle() const override;
	virtual float GetSectionHeight() const override;
	virtual int32 OnPaintSection(FSequencerSectionPainter& Painter) const override;
	virtual void BeginResizeSection() override;
	virtual void ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeFrameNumber) override;
	virtual void BeginSlipSection() override;
	virtual void SlipSection(FFrameNumber SlipTime) override;

private:

	/** The section we are visualizing */
	UMovieSceneFlipbookAnimationSection& Section;

	/** Used to draw animation frame, need selection state and local time*/
	TWeakPtr<ISequencer> Sequencer;

	/** Cached start offset value valid only during resize */
	FFrameNumber InitialStartOffsetDuringResize;
	
	/** Cached start time valid only during resize */
	FFrameNumber InitialStartTimeDuringResize;
};
