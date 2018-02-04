/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "SStateMachineGraphNode.h"

class UStateMachineGraphNode;
class UStateMachineGraphNode_Transition;

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API SStateMachineGraphNode_Transition : public SStateMachineGraphNode
{

public:

	SLATE_BEGIN_ARGS(SStateMachineGraphNode_Transition) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	void Construct(const FArguments& InArgs, UStateMachineGraphNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
 	// End of SGraphNode interface

	// SWidget interface
	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

	// SNodePanel::SNode interface
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter) override;
	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;
	// End of SNodePanel::SNode interface

	// Calculate position for multiple nodes to be placed between a start and end point, by providing this nodes index and max expected nodes 
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;
	FSlateColor GetTransitionColor() const;

	static FLinearColor StaticGetTransitionColor(UStateMachineGraphNode_Transition* TransNode, bool bIsHovered);

};
