/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "Component/RichText.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Components/RichTextBlockDecorator.h"

#define LOCTEXT_NAMESPACE "UmbraFramework"

URichText::URichText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
		Font = FSlateFontInfo(RobotoFontObj.Object, 12, FName("Regular"));
	}
	Color = FLinearColor::White;
}

void URichText::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyRichTextBlock.Reset();
}

TSharedRef<SWidget> URichText::RebuildWidget()
{
	//+ OnHyperlinkClicked = FSlateHyperlinkRun::FOnClick::CreateStatic(&RichTextHelper::OnBrowserLinkClicked, AsShared());
	//+ FHyperlinkDecorator::Create(TEXT("browser"), OnHyperlinkClicked))
	//+MakeShareable(new FDefaultRichTextDecorator(Font, Color));

	DefaultStyle.SetFont(Font);
	DefaultStyle.SetColorAndOpacity(Color);

	TArray< TSharedRef< class ITextDecorator > > CreatedDecorators;

	for (URichTextBlockDecorator* Decorator : Decorators)
	{
		if (Decorator)
			CreatedDecorators.Add(Decorator->CreateDecorator(Font, Color));
	}

	MyRichTextBlock =
		SNew(SRichTextBlock)
		.TextStyle(&DefaultStyle)
		.Decorators(CreatedDecorators);
	
	return MyRichTextBlock.ToSharedRef();
}

void URichText::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FText> TextBinding = PROPERTY_BINDING(FText, Text);

	MyRichTextBlock->SetText(TextBinding);

	Super::SynchronizeTextLayoutProperties( *MyRichTextBlock );
}

#if WITH_EDITOR

const FText URichText::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

void URichText::OnCreationFromPalette()
{
	Decorators.Add(NewObject<URichTextBlockDecorator>(this, NAME_None, RF_Transactional));
}

#endif

#undef LOCTEXT_NAMESPACE
