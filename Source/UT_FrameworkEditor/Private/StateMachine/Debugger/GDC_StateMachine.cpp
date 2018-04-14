/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#if WITH_GAMEPLAY_DEBUGGER

#include "GDC_StateMachine.h"
#include "StateMachine.h"
#include "DebugRenderSceneProxy.h"
#include "UObjectIterator.h"

FGDC_StateMachine::FGDC_StateMachine()
{
}

TSharedRef<FGameplayDebuggerCategory> FGDC_StateMachine::MakeInstance()
{
	return MakeShareable(new FGDC_StateMachine());
}

void FGDC_StateMachine::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	if (DebugActor)
	{
		DataPack.StateMachinesDebug = TArray<FRepData::FStateMachineDebug>();
		for (TObjectIterator<UStateMachine> Itr; Itr; ++Itr)
		{
			UStateMachine* StateMachine = *Itr;
			if (StateMachine)
			{
				DataPack.OwnerName = DebugActor->GetName();
				if (Cast<AActor>(StateMachine->OwnerObject) && Cast<AActor>(StateMachine->OwnerObject) == DebugActor)
				{
					FRepData::FStateMachineDebug DebugData = FRepData::FStateMachineDebug();
					DebugData.CurrentState = StateMachine->CurrentState.Name;

					DataPack.StateMachinesDebug.Add(DebugData);
				}
			}
		}
	}
}

void FGDC_StateMachine::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	//DISPLAY ALL ACTIVE STATE MACHINE
	CanvasContext.Printf(TEXT("{red}STATE MACHINE : "));
	CanvasContext.Printf(TEXT("{red}------------------"));
	CanvasContext.Printf(TEXT("{gray}Owner: {green}%s"), *DataPack.OwnerName);
	{
		for (int32 Idx = 0; Idx < DataPack.StateMachinesDebug.Num(); Idx++)
		{
			const FRepData::FStateMachineDebug& SmDebug = DataPack.StateMachinesDebug[Idx];
			CanvasContext.Printf(TEXT("{gray}------------------"));
			CanvasContext.Printf(TEXT("{gray}Current state : %s"), *SmDebug.CurrentState);
			CanvasContext.Printf(TEXT("{gray}------------------"));
		}
	}
}

#endif
