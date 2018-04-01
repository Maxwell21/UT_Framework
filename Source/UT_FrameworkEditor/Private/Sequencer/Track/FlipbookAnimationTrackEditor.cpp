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
#include "../MovieSceneTools/Public/FloatCurveKeyArea.h"
#include "../MovieSceneTools/Public/CommonMovieSceneTools.h"
#include "../MovieSceneTools/Public/MatineeImportTools.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

namespace FlipbookAnimationEditorConstants
{
	// @todo Sequencer Allow this to be customizable
	const uint32 AnimationTrackHeight = 20;
}

#define LOCTEXT_NAMESPACE "FlipbookAnimationTrackEditor"

FFlipbookAnimationSection::FFlipbookAnimationSection( UMovieSceneSection& InSection )
	: Section(*CastChecked<UMovieSceneFlipbookAnimationSection>(&InSection))
	, InitialStartOffsetDuringResize(0.f)
	, InitialStartTimeDuringResize(0.f)
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

void FFlipbookAnimationSection::GenerateSectionLayout( class ISectionLayoutBuilder& LayoutBuilder ) const
{
// 	WeightArea = MakeShareable( new FFloatCurveKeyArea( &Section.Params.Weight, &Section ) );
// 
// 	LayoutBuilder.AddKeyArea( "Weight", NSLOCTEXT( "FlipbookAnimationSection", "WeightArea", "Weight" ), WeightArea.ToSharedRef() );
}

int32 FFlipbookAnimationSection::OnPaintSection( FSequencerSectionPainter& Painter ) const
{
	const ESlateDrawEffect DrawEffects = Painter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	
	const FTimeToPixel& TimeToPixelConverter = Painter.GetTimeConverter();

	int32 LayerId = Painter.PaintSectionBackground();

	static const FSlateBrush* GenericDivider = FEditorStyle::GetBrush("Sequencer.GenericDivider");

	// Add lines where the animation starts and ends/loops
	float AnimPlayRate = FMath::IsNearlyZero(Section.Params.PlayRate) ? 1.0f : Section.Params.PlayRate;
	float SeqLength = (Section.Params.GetSequenceLength() - (Section.Params.StartOffset + Section.Params.EndOffset)) / AnimPlayRate;

	if (!FMath::IsNearlyZero(SeqLength, KINDA_SMALL_NUMBER) && SeqLength > 0)
	{
		float MaxOffset = Section.GetRange().Size<float>();
		float OffsetTime = SeqLength;

		while (OffsetTime < MaxOffset)
		{
			float OffsetPixel = TimeToPixelConverter.TimeToPixel(Section.GetStartTime() + OffsetTime) - TimeToPixelConverter.TimeToPixel(Section.GetStartTime());

			FSlateDrawElement::MakeBox(
				Painter.DrawElements,
				LayerId,
				Painter.SectionGeometry.MakeChild(
					FVector2D(2.f, Painter.SectionGeometry.Size.Y-2.f),
					FSlateLayoutTransform(FVector2D(OffsetPixel, 1.f))
				).ToPaintGeometry(),
				GenericDivider,
				DrawEffects
			);

			OffsetTime += SeqLength;
		}
	}

	return LayerId;
}

void FFlipbookAnimationSection::BeginResizeSection()
{
	InitialStartOffsetDuringResize = Section.Params.StartOffset;
	InitialStartTimeDuringResize = Section.GetStartTime();
}

void FFlipbookAnimationSection::ResizeSection(ESequencerSectionResizeMode ResizeMode, float ResizeTime)
{
	// Adjust the start offset when resizing from the beginning
	if (ResizeMode == SSRM_LeadingEdge)
	{
		float StartOffset = (ResizeTime - InitialStartTimeDuringResize) * Section.Params.PlayRate;
		StartOffset += InitialStartOffsetDuringResize;

		// Ensure start offset is not less than 0 and adjust ResizeTime
		if (StartOffset < 0)
		{
			ResizeTime = ResizeTime - (StartOffset / Section.Params.PlayRate);

			StartOffset = 0.f;
		}

		Section.Params.StartOffset = StartOffset;
	}

	ISequencerSection::ResizeSection(ResizeMode, ResizeTime);
}

void FFlipbookAnimationSection::BeginSlipSection()
{
	BeginResizeSection();
}

void FFlipbookAnimationSection::SlipSection(float SlipTime)
{
	float StartOffset = (SlipTime - InitialStartTimeDuringResize) * Section.Params.PlayRate;
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
	
	return MakeShareable( new FFlipbookAnimationSection(SectionObject) );
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

				AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FFlipbookAnimationTrackEditor::AddKeyInternal, Object, AnimSequence, Track));

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
		AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FFlipbookAnimationTrackEditor::AddKeyInternal, Object, AnimSequence, Track) );
	}
}

FKeyPropertyResult FFlipbookAnimationTrackEditor::AddKeyInternal( float KeyTime, UObject* Object, class UPaperFlipbook* AnimSequence, UMovieSceneTrack* Track )
{
	FKeyPropertyResult KeyPropertyResult;

	FFindOrCreateHandleResult HandleResult = FindOrCreateHandleToObject( Object );
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

			Cast<UMovieSceneFlipbookAnimationTrack>(Track)->AddNewAnimation( KeyTime, AnimSequence );
			KeyPropertyResult.bTrackModified = true;
		}
	}

	return KeyPropertyResult;
}

void CopyInterpAnimControlTrack(TSharedRef<ISequencer> Sequencer, UInterpTrackAnimControl* MatineeAnimControlTrack, UMovieSceneFlipbookAnimationTrack* FlipbookAnimationTrack)
{
	float EndPlaybackRange = Sequencer.Get().GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange().GetUpperBoundValue();

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
