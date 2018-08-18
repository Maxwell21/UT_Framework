/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "FlipbookAnimationTrackEditor.h"
#include "Track/MovieSceneFlipbookAnimationTrack.h"
#include "Section/MovieSceneFlipbookAnimationSection.h"
#include "Rendering/DrawElements.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "AssetData.h"
#include "Modules/ModuleManager.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "SequencerSectionPainter.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "AssetRegistryModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Matinee/InterpTrackAnimControl.h"
#include "SequencerUtilities.h"
#include "ISectionLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "MovieSceneTrackEditor.h"
#include "MovieSceneTools/Public/CommonMovieSceneTools.h"
#include "MovieSceneTools/Public/MatineeImportTools.h"
#include "Fonts/FontMeasure.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

namespace FlipbookAnimationEditorConstants
{
	// @todo Sequencer Allow this to be customizable
	const uint32 AnimationTrackHeight = 20;
}

#define LOCTEXT_NAMESPACE "FlipbookAnimationTrackEditor"

FFlipbookAnimationSection::FFlipbookAnimationSection(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer)
	: Section(*CastChecked<UMovieSceneFlipbookAnimationSection>(&InSection))
	, Sequencer(InSequencer)
	, InitialStartOffsetDuringResize(0.f)
	, InitialStartTimeDuringResize(0)
{ }

UMovieSceneSection* FFlipbookAnimationSection::GetSectionObject()
{ 
	return &Section;
}

FText FFlipbookAnimationSection::GetSectionTitle() const
{
	if (Section.Params.Animation != nullptr)
	{
		return FText::FromString( Section.Params.Animation->GetName() );
	}
	return LOCTEXT("NoAnimationSection", "No Animation");
}

float FFlipbookAnimationSection::GetSectionHeight() const
{
	return (float)FlipbookAnimationEditorConstants::AnimationTrackHeight;
}

int32 FFlipbookAnimationSection::OnPaintSection( FSequencerSectionPainter& Painter ) const
{
	const ESlateDrawEffect DrawEffects = Painter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	const FTimeToPixel& TimeToPixelConverter = Painter.GetTimeConverter();

	int32 LayerId = Painter.PaintSectionBackground();

	static const FSlateBrush* GenericDivider = FEditorStyle::GetBrush("Sequencer.GenericDivider");

	if (!Section.HasStartFrame() || !Section.HasEndFrame())
	{
		return LayerId;
	}

	// Add lines where the animation starts and ends/loops
	float AnimPlayRate = FMath::IsNearlyZero(Section.Params.PlayRate) ? 1.0f : Section.Params.PlayRate;
	float SeqLength = (Section.Params.GetSequenceLength() - (Section.Params.StartOffset + Section.Params.EndOffset)) / AnimPlayRate;

	FFrameRate TickResolution = TimeToPixelConverter.GetTickResolution();
	if (!FMath::IsNearlyZero(SeqLength, KINDA_SMALL_NUMBER) && SeqLength > 0)
	{
		float MaxOffset = Section.GetRange().Size<FFrameTime>() / TickResolution;
		float OffsetTime = SeqLength;
		float StartTime = Section.GetInclusiveStartFrame() / TickResolution;

		while (OffsetTime < MaxOffset)
		{
			float OffsetPixel = TimeToPixelConverter.SecondsToPixel(StartTime + OffsetTime) - TimeToPixelConverter.SecondsToPixel(StartTime);

			FSlateDrawElement::MakeBox(
				Painter.DrawElements,
				LayerId,
				Painter.SectionGeometry.MakeChild(
					FVector2D(2.f, Painter.SectionGeometry.Size.Y - 2.f),
					FSlateLayoutTransform(FVector2D(OffsetPixel, 1.f))
				).ToPaintGeometry(),
				GenericDivider,
				DrawEffects
			);

			OffsetTime += SeqLength;
		}
	}

	TSharedPtr<ISequencer> SequencerPtr = Sequencer.Pin();
	if (Painter.bIsSelected && SequencerPtr.IsValid())
	{
		FFrameTime CurrentTime = SequencerPtr->GetLocalTime().Time;
		if (Section.GetRange().Contains(CurrentTime.FrameNumber) && Section.Params.Animation != nullptr)
		{
			const float Time = TimeToPixelConverter.FrameToPixel(CurrentTime);

			// Draw the current time next to the scrub handle
			const float AnimTime = Section.MapTimeToAnimation(CurrentTime, TickResolution);
			int32 FrameIdx = Section.Params.Animation->GetKeyFrameIndexAtTime(AnimTime);
			int32 FrameTime = Section.Params.Animation->GetKeyFrameChecked(FrameIdx).FrameRun;
			FString FrameString = FString::FromInt(FrameTime);

			const FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Bold", 10);
			const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
			FVector2D TextSize = FontMeasureService->Measure(FrameString, SmallLayoutFont);

			// Flip the text position if getting near the end of the view range
			static const float TextOffsetPx = 10.f;
			bool  bDrawLeft = (Painter.SectionGeometry.Size.X - Time) < (TextSize.X + 22.f) - TextOffsetPx;
			float TextPosition = bDrawLeft ? Time - TextSize.X - TextOffsetPx : Time + TextOffsetPx;
			//handle mirrored labels
			const float MajorTickHeight = 9.0f;
			FVector2D TextOffset(TextPosition, Painter.SectionGeometry.Size.Y - (MajorTickHeight + TextSize.Y));

			const FLinearColor DrawColor = FEditorStyle::GetSlateColor("SelectionColor").GetColor(FWidgetStyle());
			const FVector2D BoxPadding = FVector2D(4.0f, 2.0f);
			// draw time string

			FSlateDrawElement::MakeBox(
				Painter.DrawElements,
				LayerId + 5,
				Painter.SectionGeometry.ToPaintGeometry(TextOffset - BoxPadding, TextSize + 2.0f * BoxPadding),
				FEditorStyle::GetBrush("WhiteBrush"),
				ESlateDrawEffect::None,
				FLinearColor::Black.CopyWithNewOpacity(0.5f)
			);

			FSlateDrawElement::MakeText(
				Painter.DrawElements,
				LayerId + 6,
				Painter.SectionGeometry.ToPaintGeometry(TextOffset, TextSize),
				FrameString,
				SmallLayoutFont,
				DrawEffects,
				DrawColor
			);
		}
	}

	return LayerId;
}

void FFlipbookAnimationSection::BeginResizeSection()
{
	InitialStartOffsetDuringResize = Section.Params.StartOffset;
	InitialStartTimeDuringResize = Section.HasStartFrame() ? Section.GetInclusiveStartFrame() : 0;
}

void FFlipbookAnimationSection::ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeFrameNumber)
{
	// Adjust the start offset when resizing from the beginning
	if (ResizeMode == SSRM_LeadingEdge)
	{
		FFrameRate FrameRate = Section.GetTypedOuter<UMovieScene>()->GetTickResolution();
		float      StartOffset = (ResizeFrameNumber - InitialStartTimeDuringResize) / FrameRate * Section.Params.PlayRate;

		StartOffset += InitialStartOffsetDuringResize;

		// Ensure start offset is not less than 0 and adjust ResizeTime
		if (StartOffset < 0)
		{
			ResizeFrameNumber = ResizeFrameNumber - ((StartOffset * Section.Params.PlayRate) * FrameRate).RoundToFrame();

			StartOffset = 0.f;
		}

		Section.Params.StartOffset = StartOffset;
	}

	ISequencerSection::ResizeSection(ResizeMode, ResizeFrameNumber);
}

void FFlipbookAnimationSection::BeginSlipSection()
{
	BeginResizeSection();
}

void FFlipbookAnimationSection::SlipSection(double SlipTime)
{
	float StartOffset = (SlipTime - InitialStartTimeDuringResize / Section.GetTypedOuter<UMovieScene>()->GetTickResolution()) * Section.Params.PlayRate;
	StartOffset += InitialStartOffsetDuringResize;

	// Ensure start offset is not less than 0
	if (StartOffset < 0)
	{
		StartOffset = 0.f;
	}

	Section.Params.StartOffset = StartOffset;

	ISequencerSection::SlipSection(SlipTime);
}

FFlipbookAnimationTrackEditor::FFlipbookAnimationTrackEditor( TSharedRef<ISequencer> InSequencer )
	: FMovieSceneTrackEditor( InSequencer ) 
{ }

TSharedRef<ISequencerTrackEditor> FFlipbookAnimationTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> InSequencer )
{
	return MakeShareable( new FFlipbookAnimationTrackEditor( InSequencer ) );
}

bool FFlipbookAnimationTrackEditor::SupportsType( TSubclassOf<UMovieSceneTrack> Type ) const
{
	return Type == UMovieSceneFlipbookAnimationTrack::StaticClass();
}

TSharedRef<ISequencerSection> FFlipbookAnimationTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding )
{
	check( SupportsType( SectionObject.GetOuter()->GetClass() ) );
	
	return MakeShareable(new FFlipbookAnimationSection(SectionObject, GetSequencer()));
}

void FFlipbookAnimationTrackEditor::AddKey(const FGuid& ObjectGuid)
{
	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Collect a full list of assets with the specified class
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass(UPaperFlipbook::StaticClass()->GetFName(), AssetDataList, true);

	if (AssetDataList.Num())
	{
		TSharedPtr< SWindow > Parent = FSlateApplication::Get().GetActiveTopLevelWindow(); 
		if (Parent.IsValid())
		{
			FSlateApplication::Get().PushMenu(
				Parent.ToSharedRef(),
				FWidgetPath(),
				BuildAnimationSubMenu(ObjectGuid, nullptr),
				FSlateApplication::Get().GetCursorPos(),
				FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
			);
		}
	}
}

bool FFlipbookAnimationTrackEditor::HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid)
{
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();

	if (Asset->IsA<UPaperFlipbook>() && SequencerPtr.IsValid())
	{
		UPaperFlipbook* AnimSequence = Cast<UPaperFlipbook>(Asset);
		
		if (TargetObjectGuid.IsValid())
		{
			if (AnimSequence)
			{
				UObject* Object = SequencerPtr->FindSpawnedObjectOrTemplate(TargetObjectGuid);
				
				UMovieSceneTrack* Track = nullptr;

				const FScopedTransaction Transaction(LOCTEXT("AddAnimation_Transaction", "Add Animation"));

				int32 RowIndex = INDEX_NONE;
				AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FFlipbookAnimationTrackEditor::AddKeyInternal, Object, AnimSequence, Track, RowIndex));

				return true;
			}
		}
	}

	return false;
}

void FFlipbookAnimationTrackEditor::BuildObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding, const UClass* ObjectClass)
{
	if (ObjectClass->IsChildOf(UPaperFlipbookComponent::StaticClass()) || ObjectClass->IsChildOf(AActor::StaticClass()))
	{
		const TSharedPtr<ISequencer> ParentSequencer = GetSequencer();

		// Load the asset registry module
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		// Collect a full list of assets with the specified class
		TArray<FAssetData> AssetDataList;
		AssetRegistryModule.Get().GetAssetsByClass(UPaperFlipbook::StaticClass()->GetFName(), AssetDataList, true);

		if (AssetDataList.Num())
		{
			UMovieSceneTrack* Track = nullptr;

			MenuBuilder.AddSubMenu(
				LOCTEXT("AddAnimation", "Animation"), NSLOCTEXT("Sequencer", "AddAnimationTooltip", "Adds an animation track."),
				FNewMenuDelegate::CreateRaw(this, &FFlipbookAnimationTrackEditor::AddAnimationSubMenu, ObjectBinding, Track)
			);
		}
	}
}

TSharedRef<SWidget> FFlipbookAnimationTrackEditor::BuildAnimationSubMenu(FGuid ObjectBinding, UMovieSceneTrack* Track)
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	AddAnimationSubMenu(MenuBuilder, ObjectBinding, Track);

	return MenuBuilder.MakeWidget();
}

bool FFlipbookAnimationTrackEditor::ShouldFilterAsset(const FAssetData& AssetData)
{
	return false;
}

void FFlipbookAnimationTrackEditor::AddAnimationSubMenu(FMenuBuilder& MenuBuilder, FGuid ObjectBinding, UMovieSceneTrack* Track)
{
	FAssetPickerConfig AssetPickerConfig;
	{
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateRaw( this, &FFlipbookAnimationTrackEditor::OnAnimationAssetSelected, ObjectBinding, Track);
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
		AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateRaw(this, &FFlipbookAnimationTrackEditor::ShouldFilterAsset);
		AssetPickerConfig.Filter.bRecursiveClasses = true;
		AssetPickerConfig.Filter.ClassNames.Add(UPaperFlipbook::StaticClass()->GetFName());
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TSharedPtr<SBox> MenuEntry = SNew(SBox)
		.WidthOverride(300.0f)
		.HeightOverride(300.f)
		[
			ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
		];

	MenuBuilder.AddWidget(MenuEntry.ToSharedRef(), FText::GetEmpty(), true);
}

void FFlipbookAnimationTrackEditor::OnAnimationAssetSelected(const FAssetData& AssetData, FGuid ObjectBinding, UMovieSceneTrack* Track)
{
	FSlateApplication::Get().DismissAllMenus();

	UObject* SelectedObject = AssetData.GetAsset();
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();

	if (SelectedObject && SelectedObject->IsA(UPaperFlipbook::StaticClass()) && SequencerPtr.IsValid())
	{
		UPaperFlipbook* AnimSequence = CastChecked<UPaperFlipbook>(AssetData.GetAsset());

		UObject* Object = SequencerPtr->FindSpawnedObjectOrTemplate(ObjectBinding);
		int32 RowIndex = INDEX_NONE;
		AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FFlipbookAnimationTrackEditor::AddKeyInternal, Object, AnimSequence, Track, RowIndex));
	}
}

FKeyPropertyResult FFlipbookAnimationTrackEditor::AddKeyInternal(FFrameNumber KeyTime, UObject* Object, class UPaperFlipbook* AnimSequence, UMovieSceneTrack* Track, int32 RowIndex)
{
	FKeyPropertyResult KeyPropertyResult;

	FFindOrCreateHandleResult HandleResult = FindOrCreateHandleToObject(Object);
	FGuid ObjectHandle = HandleResult.Handle;
	KeyPropertyResult.bHandleCreated |= HandleResult.bWasCreated;
	if (ObjectHandle.IsValid())
	{
		if (!Track)
		{
			Track = AddTrack(GetSequencer()->GetFocusedMovieSceneSequence()->GetMovieScene(), ObjectHandle, UMovieSceneFlipbookAnimationTrack::StaticClass(), NAME_None);
			KeyPropertyResult.bTrackCreated = true;
		}

		if (ensure(Track))
		{
			Track->Modify();

			UMovieSceneSection* NewSection = Cast<UMovieSceneFlipbookAnimationTrack>(Track)->AddNewAnimationOnRow(KeyTime, AnimSequence, RowIndex);
			KeyPropertyResult.bTrackModified = true;

			GetSequencer()->EmptySelection();
			GetSequencer()->SelectSection(NewSection);
			GetSequencer()->ThrobSectionSelection();
		}
	}

	return KeyPropertyResult;
}

void CopyInterpAnimControlTrack(TSharedRef<ISequencer> Sequencer, UInterpTrackAnimControl* MatineeAnimControlTrack, UMovieSceneFlipbookAnimationTrack* FlipbookAnimationTrack)
{
//	float EndPlaybackRange = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange().GetUpperBoundValue();

// 	if (FMatineeImportTools::CopyInterpAnimControlTrack(MatineeAnimControlTrack, FlipbookAnimationTrack, EndPlaybackRange))
// 	{
// 		Sequencer.Get().NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
// 	}
}

void FFlipbookAnimationTrackEditor::BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track )
{
	UInterpTrackAnimControl* MatineeAnimControlTrack = nullptr;
	for ( UObject* CopyPasteObject : GUnrealEd->MatineeCopyPasteBuffer )
	{
		MatineeAnimControlTrack = Cast<UInterpTrackAnimControl>( CopyPasteObject );
		if ( MatineeAnimControlTrack != nullptr )
		{
			break;
		}
	}
	UMovieSceneFlipbookAnimationTrack* FlipbookAnimationTrack = Cast<UMovieSceneFlipbookAnimationTrack>( Track );
	MenuBuilder.AddMenuEntry(
		NSLOCTEXT( "Sequencer", "PasteMatineeFlipbookAnimControlTrack", "Paste Matinee Flipbook Animation Track" ),
		NSLOCTEXT( "Sequencer", "PasteMatineeFlipbookAnimControlTrackTooltip", "Pastes keys from a Matinee float track into this track." ),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic( &CopyInterpAnimControlTrack, GetSequencer().ToSharedRef(), MatineeAnimControlTrack, FlipbookAnimationTrack),
			FCanExecuteAction::CreateLambda( [=]()->bool { return MatineeAnimControlTrack != nullptr && MatineeAnimControlTrack->AnimSeqs.Num() > 0 && FlipbookAnimationTrack != nullptr; } ) ) );
}

TSharedPtr<SWidget> FFlipbookAnimationTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params)
{
	// Create a container edit box
	return SNew(SHorizontalBox)

	// Add the animation combo box
	+SHorizontalBox::Slot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	[
		FSequencerUtilities::MakeAddButton(LOCTEXT("AnimationText", "Animation"), FOnGetContent::CreateSP(this, &FFlipbookAnimationTrackEditor::BuildAnimationSubMenu, ObjectBinding, Track), Params.NodeIsHovered)
	];
}


#undef LOCTEXT_NAMESPACE
