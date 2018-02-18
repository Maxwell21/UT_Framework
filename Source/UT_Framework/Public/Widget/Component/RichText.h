/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SWidget.h"
#include "Components/TextWidgetTypes.h"
#include "RichText.generated.h"

class SRichTextBlock;
class URichTextBlockDecorator;

/**
 * The rich text block
 *
 * * Fancy Text
 * * No Children
 */
UCLASS()
class UT_FRAMEWORK_API URichText : public UTextLayoutWidget
{
	GENERATED_BODY()

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

protected:

	/** The text to display */
	UPROPERTY(EditAnywhere, Category = Content, meta = (MultiLine = "true"))
	FText Text;

	/** A bindable delegate to allow logic to drive the text of the widget */
	UPROPERTY()
	FGetText TextDelegate;

	/** The default font for the text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateFontInfo Font;

	/** The default color for the text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, Instanced, Category = Decorators)
	TArray<URichTextBlockDecorator*> Decorators;

	PROPERTY_BINDING_IMPLEMENTATION(FText, Text);

	FTextBlockStyle DefaultStyle;

	/** Native Slate Widget */
	TSharedPtr<SRichTextBlock> MyRichTextBlock;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	URichText(const FObjectInitializer& ObjectInitializer);
	
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

#if WITH_EDITOR
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	virtual void OnCreationFromPalette() override;
	// End UWidget interface
#endif

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

};
