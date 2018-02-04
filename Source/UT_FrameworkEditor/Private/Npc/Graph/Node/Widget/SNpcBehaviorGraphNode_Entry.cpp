/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "SNpcBehaviorGraphNode_Entry.h"
#include "SNpcBehaviorGraphNode.h"
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

void SNpcBehaviorGraphNode_Entry::Construct(const FArguments& InArgs, UNpcBehaviorGraphNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SNpcBehaviorGraphNode_Entry::UpdateGraphNode()
{
	Cast<UNpcBehaviorGraphNode>(this->GraphNode)->SaveNode();
	
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );

	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage( FFrameworkStyle::GetBrush("FrameworkGraph.NodeBody") )
			.BorderBackgroundColor(this, &SNpcBehaviorGraphNode::GetNodeBackgroundColor)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(10)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(), "PhysicsAssetEditor.Graph.TextStyle")
						.Text(this, &SNpcBehaviorGraphNode::GetNodeTitle)
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]
				
					+SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.Padding(4.0f)
					[
						SNullWidget::NullWidget
					]
					+SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]
			]
		];

	CreatePinWidgets();
}

void SNpcBehaviorGraphNode_Entry::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	if (PinToAdd->GetDirection() == EGPD_Input)
	{
		LeftNodeBox->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				PinToAdd
			];

		InputPins.Add(PinToAdd);
	}
	else
	{
		RightNodeBox->AddSlot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				PinToAdd
			];

		OutputPins.Add(PinToAdd);
	}
}

void SNpcBehaviorGraphNode_Entry::CreatePinWidgets()
{
	UNpcBehaviorGraphNode* NpcGraphNode = CastChecked<UNpcBehaviorGraphNode>(GraphNode);

	if (NpcGraphNode->GetInputPin())
	{
		UEdGraphPin& InputPin = *NpcGraphNode->GetInputPin();
		if (!InputPin.bHidden)
			this->AddPin(SNew(SNpcBehaviorGraphNode_OutputPin, &InputPin));
	}

	if (NpcGraphNode->GetOutputPin())
	{
		UEdGraphPin& OutputPin = *NpcGraphNode->GetOutputPin();
		if (!OutputPin.bHidden)
			this->AddPin(SNew(SNpcBehaviorGraphNode_OutputPin, &OutputPin));
	}
}

#undef LOCTEXT_NAMESPACE
