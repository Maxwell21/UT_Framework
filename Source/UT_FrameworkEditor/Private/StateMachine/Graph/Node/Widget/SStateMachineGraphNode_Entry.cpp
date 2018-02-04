/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "SStateMachineGraphNode_Entry.h"
#include "SStateMachineGraphNode.h"
#include "StateMachineGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "SImage.h"
#include "SSpacer.h"
#include "SNullWidget.h"
#include "Editor/KismetWidgets/Public/SLevelOfDetailBranchNode.h"
#include "FrameworkStyle.h"
#include "GraphEditorSettings.h"

#define LOCTEXT_NAMESPACE "StateMachineGraphEditor"


/************************************************************************/
/* SStateMachineGraphNodeEntry_OutputPin	                            */
/************************************************************************/

void SStateMachineGraphNodeEntry_OutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	typedef SStateMachineGraphNodeEntry_OutputPin ThisClass;

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.BorderBackgroundColor(this, &ThisClass::GetPinColor)
		.OnMouseButtonDown(this, &ThisClass::OnPinMouseDown)
		.Cursor(this, &ThisClass::GetPinCursor)
		.Content()
		[
			SNew(SImage)
			.Image(this, &ThisClass::GetPinImage)
		]
	);
}

const FSlateBrush* SStateMachineGraphNodeEntry_OutputPin::GetPinBorder() const
{
	return (IsHovered())
		? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePinHoverCue"))
		: FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePin"))
	;
}

/** SGraphPin interface */
TSharedRef<SWidget>	SStateMachineGraphNodeEntry_OutputPin::GetDefaultValueWidget()
{
	return SNew(SSpacer);
}

const FSlateBrush* SStateMachineGraphNodeEntry_OutputPin::GetPinImage() const
{
	return (IsHovered())
		? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePinHoverCue"))
		: FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePin"))
	;
}

void SStateMachineGraphNode_Entry::Construct(const FArguments& InArgs, UStateMachineGraphNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SStateMachineGraphNode_Entry::UpdateGraphNode()
{
	Cast<UStateMachineGraphNode>(this->GraphNode)->SaveNode();
	
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
			.BorderBackgroundColor(this, &SStateMachineGraphNode::GetNodeBackgroundColor)
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
						.Text(this, &SStateMachineGraphNode::GetNodeTitle)
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

void SStateMachineGraphNode_Entry::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

void SStateMachineGraphNode_Entry::CreatePinWidgets()
{
	UStateMachineGraphNode* StateMachineGraphNode = CastChecked<UStateMachineGraphNode>(GraphNode);

	if (StateMachineGraphNode->GetOutputPin())
	{
		UEdGraphPin& OutputPin = *StateMachineGraphNode->GetOutputPin();
		if (!OutputPin.bHidden)
			this->AddPin(SNew(SStateMachineGraphNodeEntry_OutputPin, &OutputPin));
	}
}

#undef LOCTEXT_NAMESPACE
