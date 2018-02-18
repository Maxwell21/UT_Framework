/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "TextLayout.h"
#include "ButtonText.generated.h"

class STextBlock;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UButtonText : public UButton
{
	GENERATED_UCLASS_BODY()

public:

	/************************************************************************/
	/* BINDINGS                                                             */
	/************************************************************************/
	
	PROPERTY_BINDING_IMPLEMENTATION(FText, Text);
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, TextColorAndOpacity);
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, TextColorAndOpacityHover);
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, TextColorAndOpacityPressed);
	PROPERTY_BINDING_IMPLEMENTATION(FLinearColor, TextShadowColorAndOpacity);

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/** The text to display */
	UPROPERTY(Category = "Text", EditAnywhere, meta = (MultiLine = "true"))
	FText Text;

	/** A bindable delegate to allow logic to drive the text of the widget */
	UPROPERTY()
	FGetText TextDelegate;

	/** The text justification*/
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETextJustify::Type> TextJustification;

	/** The text margin */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	FMargin TextMargin;

	/** The color of the text */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	FSlateColor TextColorAndOpacity;

	/** A bindable delegate for the ColorAndOpacity. */
	UPROPERTY()
	FGetSlateColor TextColorAndOpacityDelegate;

	/** The color of the text */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	FSlateColor TextColorAndOpacityHover;

	/** A bindable delegate for the TextColorAndOpacityHover. */
	UPROPERTY()
	FGetSlateColor TextColorAndOpacityHoverDelegate;

	/** The color of the text */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	FSlateColor TextColorAndOpacityPressed;

	/** A bindable delegate for the TextColorAndOpacityPressed. */
	UPROPERTY()
	FGetSlateColor TextColorAndOpacityPressedDelegate;

	/** The font to render the text with */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	FSlateFontInfo TextFont;

	/** The direction the shadow is cast */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly)
	FVector2D TextShadowOffset;

	/** The color of the shadow */
	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Shadow Color"))
	FLinearColor TextShadowColorAndOpacity;

	/** A bindable delegate for the ShadowColorAndOpacity. */
	UPROPERTY()
	FGetLinearColor TextShadowColorAndOpacityDelegate;

protected:

	/** Cached Normal button style */
	FSlateBrush NormalButtonStateCached;

	/** Cached pointer to the underlying slate button owned by this UWidget */
	TSharedPtr<STextBlock> MyTextBlock;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface
	
	/** Force state style to display */
	UFUNCTION(Category = "Button", BlueprintCallable)
	virtual void ForceNormal();
	
	UFUNCTION(Category = "Button", BlueprintCallable)
	virtual void ForceHover();

	UFUNCTION(Category = "Button", BlueprintCallable)
	virtual void ForcePressed();

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

};
