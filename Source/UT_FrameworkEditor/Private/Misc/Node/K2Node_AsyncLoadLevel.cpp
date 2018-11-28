/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "K2Node_AsyncLoadLevel.h"
#include "StateMachineCallbackProxy.h"
#include "EdGraphSchema_K2.h"
#include "LoadSystem.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_AsyncLoadLevel::UK2Node_AsyncLoadLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(ULoadSystem, AsyncLoadLevel);
	ProxyFactoryClass = ULoadSystem::StaticClass();
	ProxyClass = ULoadSystem::StaticClass();
}

FText UK2Node_AsyncLoadLevel::GetTooltipText() const
{
	return LOCTEXT("K2Node_UK2Node_AsyncLoadLevel_Tooltip", "Load a level streaming map asynchronously");
}

FText UK2Node_AsyncLoadLevel::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AsyncLoadLevel", "Async Load Level");
}

FText UK2Node_AsyncLoadLevel::GetMenuCategory() const
{
	return LOCTEXT("AsyncLoadLevelCategory", "Umbra Framework | Load System");
}

#undef LOCTEXT_NAMESPACE
