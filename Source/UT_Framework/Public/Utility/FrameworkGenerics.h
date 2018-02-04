/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FrameworkGenerics.generated.h"

USTRUCT()
struct UT_FRAMEWORK_API FGenericNodeData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 NodePosX;

	UPROPERTY()
	int32 NodePosY;

	FGenericNodeData()
		: NodePosX(0)
		, NodePosY(0)
	{
	}
};