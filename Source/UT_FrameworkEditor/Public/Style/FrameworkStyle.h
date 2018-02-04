/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"
#include "Styling/SlateStyle.h"

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API FFrameworkStyle
{

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

private:

	static TSharedPtr< class FSlateStyleSet > StyleSet;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

public:

	static void Initialize();

	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

	static FName GetStyleSetName();

	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = NULL);

private:

	static FString InContent(const FString& RelativePath, const TCHAR* Extension);

};
