/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "K2Node_ConstructStateMachine.h"
#include "StateMachineCallbackProxy.h"
#include "EdGraphSchema_K2.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_ConstructStateMachine::UK2Node_ConstructStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachineCallbackProxy, CreateProxyObjectForConstructStateMachine);
	ProxyFactoryClass = UStateMachineCallbackProxy::StaticClass();
	ProxyClass = UStateMachineCallbackProxy::StaticClass();
}

FText UK2Node_ConstructStateMachine::GetTooltipText() const
{
	return LOCTEXT("K2Node_ConstructStateMachine_Tooltip", "Construct a state machine");
}

FText UK2Node_ConstructStateMachine::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ConstructStateMachine", "Construct State Machine");
}

FText UK2Node_ConstructStateMachine::GetMenuCategory() const
{
	return LOCTEXT("ConstructStateMachineCategory", "Umbra Framework | State Machine");
}

#undef LOCTEXT_NAMESPACE
