/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineCallbackProxy.h"
#include "StateMachine.h"
#include "GameFramework/PlayerController.h"

UStateMachineCallbackProxy::UStateMachineCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UStateMachineCallbackProxy* UStateMachineCallbackProxy::CreateProxyObjectForConstructStateMachine(TSubclassOf<UStateMachine> Template, UObject* Owner, APlayerController* Controller)
{
	UStateMachineCallbackProxy* Proxy = NewObject<UStateMachineCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);

	if (UStateMachine* StateMachine = UStateMachine::ConstructStateMachine(Template, Owner, Controller))
	{
		Proxy->OnCompleted.Broadcast(StateMachine);
	}
	else
		Proxy->OnFailed.Broadcast(nullptr);

	return Proxy;
}
