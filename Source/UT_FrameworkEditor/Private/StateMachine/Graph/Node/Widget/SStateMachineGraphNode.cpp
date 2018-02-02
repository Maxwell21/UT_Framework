/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "SStateMachineGraphNode.h"
#include "StateMachineGraphNode.h"
#include "SStateMachineGraphNode_Transition.h"
#include "StateMachineTransition_Default.h"
#include "StateMachineUtilities.h"
#include "Widgets/SBoxPanel.h"
#include "SImage.h"
#include "SSpacer.h"
#include "SInlineEditableTextBlock.h"
#include "Editor/KismetWidgets/Public/SLevelOfDetailBranchNode.h"
#include "FrameworkStyle.h"
#include "GraphEditorSettings.h"
#include "SGraphPin.h"
#include "SGraphNode.h"

#define LOCTEXT_NAMESPACE "StateMachineGraphEditor"

/************************************************************************/
/* SStateMachineGraphNode_OutputPin	                                    */
/************************************************************************/

void SStateMachineGraphNode_OutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	typedef SStateMachineGraphNode_OutputPin ThisClass;

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &ThisClass::GetPinBorder)
		.BorderBackgroundColor(this, &ThisClass::GetPinColor)
		.OnMouseButtonDown(this, &ThisClass::OnPinMouseDown)
		.Cursor(this, &ThisClass::GetPinCursor)
	);
}

const FSlateBrush* SStateMachineGraphNode_OutputPin::GetPinBorder() const
{
	return (IsHovered())
		? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.StateMachine.NodePinHoverCue"))
		: FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
}

/** SGraphPin interface */
TSharedRef<SWidget>	SStateMachineGraphNode_OutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SStateMachineGraphNode_OutputPin::GetPinImage() const
{
	return (IsHovered())
		? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.StateMachine.NodePinHoverCue"))
		: FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
}

/************************************************************************/
/* SStateMachineGraphNode	                                            */
/************************************************************************/

void SStateMachineGraphNode::Construct(const FArguments & InArgs, UStateMachineGraphNode * InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SStateMachineGraphNode::UpdateGraphNode()
{
	Cast<UStateMachineGraphNode>(this->GraphNode)->SaveNode();
	
	InputPins.Empty();
	OutputPins.Empty();
	
	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.StateMachine.NodeBody")))
			.Padding(0)
			.BorderBackgroundColor( this, &SStateMachineGraphNode::GetBorderBackgroundColor )
			[
				SNew(SOverlay)

				// PIN AREA
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(5.0f)
				[
					SNew(SBorder)
					.BorderImage(FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.StateMachine.NodeBodyColorSpill")))
					.BorderBackgroundColor( TitleShadowColor )
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							// POPUP ERROR MESSAGE
							SAssignNew(ErrorText, SErrorText )
							.BackgroundColor( this, &SStateMachineGraphNode::GetErrorColor )
							.ToolTipText( this, &SStateMachineGraphNode::GetErrorMsgToolTip )
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(NodeTypeIcon)
						]
						+SHorizontalBox::Slot()
						.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.Style( FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText" )
								.Text( NodeTitle.Get(), &SNodeTitle::GetHeadTitle )
								.OnVerifyTextChanged(this, &SStateMachineGraphNode::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SStateMachineGraphNode::OnNameTextCommited)
								.IsReadOnly( this, &SStateMachineGraphNode::IsNameReadOnly )
								.IsSelected(this, &SStateMachineGraphNode::IsSelectedExclusively)
							]
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle.ToSharedRef()
							]
						]
					]
				]
			]
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SStateMachineGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	if (PinToAdd->GetDirection() == EGPD_Output)
	{
		RightNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}

void SStateMachineGraphNode::CreatePinWidgets()
{
	UStateMachineGraphNode* StateMachineGraphNode = CastChecked<UStateMachineGraphNode>(GraphNode);

	for (UEdGraphPin*& Pin : StateMachineGraphNode->Pins)
	{
		if (!Pin->bHidden)
			this->AddPin(SNew(SStateMachineGraphNode_OutputPin, Pin));
	}
}

const FSlateBrush* SStateMachineGraphNode::GetShadowBrush(bool bSelected) const
{
	return bSelected ? FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.StateMachine.NodeBodySelected")) : FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.RegularNodeShadow"));
}

void SStateMachineGraphNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (UStateMachineGraphNode* StateMachineGraphNode = CastChecked<UStateMachineGraphNode>(GraphNode))
	{
		FStateMachineUtilities::RenameBoundingGraph(StateMachineGraphNode->State, StateMachineGraphNode->State->StateName, InText.ToString());

		TArray<UStateMachineGraphNode_Transition*> Transitions;
		StateMachineGraphNode->GetTransitionList(Transitions);
		for (const UStateMachineGraphNode_Transition* TransitionNode : Transitions)
		{
			if (TransitionNode->Transition)
				FStateMachineUtilities::RenameBoundingGraph(TransitionNode->Transition);
		}
	}

	SGraphNode::OnNameTextCommited(InText, CommitInfo);
}

bool SStateMachineGraphNode::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	return InText.ToString().IsEmpty() == false;
}

FSlateColor SStateMachineGraphNode::GetNodeColor() const
{
	return FSlateColor(GraphNode->GetNodeTitleColor());
}

FSlateColor SStateMachineGraphNode::GetNodeBackgroundColor() const
{
	return FSlateColor(FLinearColor(0.05098f, 0.05098f, 0.05098f));
}

FText SStateMachineGraphNode::GetNodeTitle() const
{
	return GraphNode->GetNodeTitle(ENodeTitleType::FullTitle);
}

FText SStateMachineGraphNode::GetPreviewCornerText() const
{
	UStateMachineGraphNode* StateMachineGraphNode = CastChecked<UStateMachineGraphNode>(GraphNode);

	return FText::Format(NSLOCTEXT("SStateMachineGraphNode", "PreviewCornerStateText", "{0} state"), FText::FromString(StateMachineGraphNode->GetStateName()));
}

const FSlateBrush* SStateMachineGraphNode::GetNameIcon() const
{
	return FFrameworkStyle::GetBrush(TEXT("FrameworkGraph.StateMachine.Icon"));
}

FSlateColor SStateMachineGraphNode::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);

	return InactiveStateColor;
}

#undef LOCTEXT_NAMESPACE
