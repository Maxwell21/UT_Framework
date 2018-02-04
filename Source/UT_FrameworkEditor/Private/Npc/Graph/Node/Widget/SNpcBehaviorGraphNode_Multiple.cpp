/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "SNpcBehaviorGraphNode_Multiple.h"
#include "NpcBehaviorGraphNode_Multiple.h"
#include "NpcBehaviorGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "SImage.h"
#include "SSpacer.h"
#include "SNullWidget.h"
#include "Editor/KismetWidgets/Public/SLevelOfDetailBranchNode.h"
#include "FrameworkStyle.h"
#include "GraphEditorSettings.h"

#define LOCTEXT_NAMESPACE "NpcBehaviorGraphEditor"

void SNpcBehaviorGraphNode_Multiple::Construct(const FArguments & InArgs, UNpcBehaviorGraphNode * InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SNpcBehaviorGraphNode_Multiple::CreatePinWidgets()
{
	UNpcBehaviorGraphNode_Multiple* NpcGraphNode = CastChecked<UNpcBehaviorGraphNode_Multiple>(GraphNode);

	for (UEdGraphPin*& Pin : NpcGraphNode->Pins)
	{
		if (!Pin->bHidden && Pin->Direction == EGPD_Input)
			this->AddPin(SNew(SNpcBehaviorGraphNode_OutputPin, Pin));
	}

	for (UEdGraphPin*& Pin : NpcGraphNode->MultipleOutputPins)
	{
		if (!Pin->bHidden)
			this->AddPin(SNew(SNpcBehaviorGraphNode_OutputPin, Pin));
	}
}

#undef LOCTEXT_NAMESPACE
