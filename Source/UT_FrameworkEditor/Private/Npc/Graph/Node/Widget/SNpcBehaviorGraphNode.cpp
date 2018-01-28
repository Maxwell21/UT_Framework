/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

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

/************************************************************************/
/* SNpcBehaviorGraphNode_OutputPin	                                    */
/************************************************************************/

void SNpcBehaviorGraphNode_OutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	typedef SNpcBehaviorGraphNode_OutputPin ThisClass;

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

const FSlateBrush* SNpcBehaviorGraphNode_OutputPin::GetPinBorder() const
{
	return (IsHovered())
		? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePinHoverCue"))
		: FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePin"));
}

/** SGraphPin interface */
TSharedRef<SWidget>	SNpcBehaviorGraphNode_OutputPin::GetDefaultValueWidget()
{
	return SNew(SSpacer);
}

const FSlateBrush* SNpcBehaviorGraphNode_OutputPin::GetPinImage() const
{
	return (IsHovered())
		? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePinHoverCue"))
		: FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodePin"));
}

/************************************************************************/
/* SNpcBehaviorGraphNode	                                            */
/************************************************************************/

void SNpcBehaviorGraphNode::Construct(const FArguments & InArgs, UNpcBehaviorGraphNode * InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SNpcBehaviorGraphNode::UpdateGraphNode()
{
	Cast<UNpcBehaviorGraphNode>(this->GraphNode)->SaveNode();
	
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );

	// Get node icon
	IconColor = FLinearColor::White;
	const FSlateBrush* IconBrush = NULL;
	if (GraphNode != NULL && GraphNode->ShowPaletteIconOnNode())
		IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();

	TSharedRef<SOverlay> DefaultTitleAreaWidget =
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.BorderImage(FFrameworkStyle::GetBrush("FrameworkGraph.RegularNodeTitleNormal"))
			.Padding(FMargin(10,5,30,3))
			.BorderBackgroundColor( this, &SGraphNode::GetNodeTitleColor )
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
				.AutoWidth()
				[
					SNew(SImage)
					.Image(IconBrush)
					.ColorAndOpacity(FSlateColor(FLinearColor::White))
				]
				+SHorizontalBox::Slot()
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.AutoHeight()
					[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(), "PhysicsAssetEditor.Graph.TextStyle")
						.Text(this, &SNpcBehaviorGraphNode::GetNodeTitle)
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]
				]
			]
		];

	SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

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
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					DefaultTitleAreaWidget
				]
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
						SAssignNew(LeftNodeBox, SVerticalBox)
					]
				
					+SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.Padding(4.0f)
					[
						GetNodeContent()
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

void SNpcBehaviorGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

void SNpcBehaviorGraphNode::CreatePinWidgets()
{
	UNpcBehaviorGraphNode* NpcGraphNode = CastChecked<UNpcBehaviorGraphNode>(GraphNode);

	for (UEdGraphPin*& Pin : NpcGraphNode->Pins)
	{
		if (!Pin->bHidden)
			this->AddPin(SNew(SNpcBehaviorGraphNode_OutputPin, Pin));
	}
}

const FSlateBrush* SNpcBehaviorGraphNode::GetShadowBrush(bool bSelected) const
{
	return bSelected ? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.NodeBodySelected")) : FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.RegularNodeShadow"));
}

TSharedRef<class SWidget> SNpcBehaviorGraphNode::GetNodeContent()
{
	return SNew(SBox)
		.WidthOverride(50.0f)
	;
}

FSlateColor SNpcBehaviorGraphNode::GetNodeColor() const
{
	return FSlateColor(GraphNode->GetNodeTitleColor());
}

FSlateColor SNpcBehaviorGraphNode::GetNodeBackgroundColor() const
{
	return FSlateColor(FLinearColor(0.05098f, 0.05098f, 0.05098f));
}

FText SNpcBehaviorGraphNode::GetNodeTitle() const
{
	return GraphNode->GetNodeTitle(ENodeTitleType::FullTitle);
}

#undef LOCTEXT_NAMESPACE
