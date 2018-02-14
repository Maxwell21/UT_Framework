/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "ButtonText.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Components/ButtonSlot.h"
#include "ConstructorHelpers.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "UmbraFramework"

UButtonText::UButtonText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = false;
	TextShadowOffset = FVector2D(1.0f, 1.0f);
	TextColorAndOpacity = FLinearColor::White;
	TextShadowColorAndOpacity = FLinearColor::Transparent;

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
		TextFont = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	}
}

TSharedRef<SWidget> UButtonText::RebuildWidget()
{
	MyTextBlock = SNew(STextBlock);
	MyTextBlock->SetText(Text);
	MyTextBlock->SetColorAndOpacity(TextColorAndOpacity);
	MyTextBlock->SetFont(TextFont);
	MyTextBlock->SetJustification(TextJustification);
	MyTextBlock->SetMargin(TextMargin);

	MyButton = SNew(SButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.IsFocusable(IsFocusable)
		.Content()
		[
			MyTextBlock.ToSharedRef()
		]
	;

	if (GetChildrenCount() > 0)
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());

	return MyButton.ToSharedRef();
}

void UButtonText::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FText> RenderText = (TextDelegate.IsBound()) ? PROPERTY_BINDING(FText, Text) : Text;
	
	MyTextBlock->SetText(RenderText);
	MyTextBlock->SetColorAndOpacity(TextColorAndOpacity);
	MyTextBlock->SetFont(TextFont);
	MyTextBlock->SetJustification(TextJustification);
	MyTextBlock->SetMargin(TextMargin);
}

#if WITH_EDITOR

const FText UButtonText::GetPaletteCategory()
{
	return LOCTEXT("UmbraFramework", "Umbra Framework");
}

#endif

#undef LOCTEXT_NAMESPACE
