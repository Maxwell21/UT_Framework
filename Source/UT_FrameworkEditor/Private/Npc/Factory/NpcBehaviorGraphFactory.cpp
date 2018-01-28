/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphFactory.h"

#include "Node/NpcBehaviorGraphNode.h"
#include "Node/NpcBehaviorGraphNode_Entry.h"
#include "Node/NpcBehaviorGraphNode_Multiple.h"

#include "Widget/SNpcBehaviorGraphNode.h"
#include "Widget/SNpcBehaviorGraphNode_Entry.h"
#include "Widget/SNpcBehaviorGraphNode_Multiple.h"

TSharedPtr<class SGraphNode> FNpcBehaviorGraphFactory::CreateNode(class UEdGraphNode* InNode) const
{
	if (UNpcBehaviorGraphNode_Entry* Node = Cast<UNpcBehaviorGraphNode_Entry>(InNode))
		return SNew(SNpcBehaviorGraphNode_Entry, Node);
	else if (UNpcBehaviorGraphNode_Multiple* Node = Cast<UNpcBehaviorGraphNode_Multiple>(InNode))
		return SNew(SNpcBehaviorGraphNode_Multiple, Node);
	else if (UNpcBehaviorGraphNode* Node = Cast<UNpcBehaviorGraphNode>(InNode))
		return SNew(SNpcBehaviorGraphNode, Node);

	return nullptr;
}
