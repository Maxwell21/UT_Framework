/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachine.h"
#include "StateMachineBlueprint.h"
#include "StateMachineState_Entry.h"
#include "StateMachineState_Default.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

/************************************************************************/
/* FTransition                                                          */
/************************************************************************/

FTransition::FTransition()
{
	this->Name = "";
	this->FromState = "";
	this->ToState = "";
}

bool FTransition::IsValid()
{
	return !this->Name.IsEmpty() && !this->FromState.IsEmpty() && !this->ToState.IsEmpty();
}

/************************************************************************/
/* FState                                                               */
/************************************************************************/

FState::FState()
{
	this->Name = "";
	this->BeginFunctionName = "";
	this->UpdateFunctionName = "";
	this->FinishFunctionName = "";
	this->TransitionNames = TArray<FString>();
}

bool FState::IsValid()
{
	return Name.IsEmpty() == false;
}

/************************************************************************/
/* UStateMachine                                                        */
/************************************************************************/

UStateMachine* UStateMachine::ConstructStateMachine(TSubclassOf<UStateMachine> Template, UObject* Owner, APlayerController* Controller)
{
	if (!Template || !Owner)
		return nullptr;

	if (UStateMachine* StateMachine = NewObject<UStateMachine>(Owner->GetWorld(), Template, NAME_None, RF_Standalone))
	{
		if (Controller)
			StateMachine->PlayerController = Controller;

		StateMachine->Init(Owner);
		
		return StateMachine;
	}

	return nullptr;
}

void UStateMachine::Init(UObject* Owner)
{
	if (!Owner)
		return;

	this->OwnerObject = Owner;
	this->OnInit.Broadcast();
}

void UStateMachine::CheckTransitionForCurrentState()
{
	// If current state is not valid or current state transitions is empty we don't have to check transitions
	if (!CurrentState.IsValid() || CurrentState.TransitionNames.Num() == 0)
		return;

	for (const auto& TransitionName : this->CurrentState.TransitionNames)
	{
		bool Result = false;
		this->ExecuteInnerFunction(*TransitionName, &Result);

		// Result is true we move to the new state
		if (Result)
		{
			FTransition* Transition = this->Transitions.Find(TransitionName);
			FState State = this->GetStateByName(Transition->ToState);

			// we notify the current state to finish is state and start the new one
			this->FinishState();
			this->SetCurrentState(State);
			
			// play new current state
			this->BeginState();

			return;
		}
	}
}

void UStateMachine::Start()
{
	if (UStateMachineBlueprintGeneratedClass* This = this->GetGeneratedClass())
	{
 		if (this->GetRootState().IsValid())
		{
			this->OnStateMachineStart();
			this->SetCurrentState(this->GetRootState());
			this->BeginState();

			this->OnStart.Broadcast();
		}
	}
}

void UStateMachine::Stop()
{
	this->UseTick = false;
	this->CurrentState = FState();
	this->OnStop.Broadcast();
	this->ConditionalBeginDestroy();
	GEngine->ForceGarbageCollection(true);
}

void UStateMachine::ExecuteInnerFunction(FName FunctionName, void* Params/* = nullptr*/)
{
	if (UFunction* Function = this->FindFunction(FunctionName))
		this->ProcessEvent(Function, Params);
}

UWorld* UStateMachine::GetOwnerWorldObject()
{
	return GEngine->GetWorldFromContextObject(this->OwnerObject, EGetWorldErrorMode::ReturnNull);
}

UStateMachineBlueprintGeneratedClass* UStateMachine::GetGeneratedClass()
{
	return Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass());
}

FState UStateMachine::GetStateByName(FString Name)
{
	if (Name.IsEmpty())
		return FState();

	for (auto State : this->States)
	{
		if (State.Name == Name)
			return State;
	}

	return FState();
}

void UStateMachine::SetCurrentState(FState State)
{
	this->CurrentState = State;
}

FState UStateMachine::GetCurrentState()
{
	return this->CurrentState;
}

FString UStateMachine::GetCurrentStateName()
{
	return this->GetCurrentState().Name;
}

void UStateMachine::SetRootState(FString Name)
{
	for (int32 Index = 0; Index < this->States.Num(); ++Index)
	{
		if (States[Index].Name == Name)
			States[Index].Root = true;
		else
			States[Index].Root = false;
	}
}

FState UStateMachine::GetRootState()
{
	for (auto RootState : this->States)
	{
		if (RootState.Root)
			return RootState;
	}

	return FState();
}

int32 UStateMachine::StateExist(FString Name)
{
	if (Name.IsEmpty())
		return -1;

	for (int32 Index = 0; Index < this->States.Num(); ++Index)
	{
		if (this->States[Index].Name == Name)
			return Index;
	}

	return -1;
}

bool UStateMachine::AddOrUpdateState(FState State, FString OldName/*= ""*/)
{
	if (State.IsValid() == false)
		return false;

	int32 Index = this->StateExist(OldName);
	if (Index != -1 && this->States.IsValidIndex(Index))
	{
		this->States[Index] = State;

		return true;
	}
	else
	{
		this->States.Add(State);

		return true;
	}
}

bool UStateMachine::RemoveStateByName(FString OldName)
{
	if (OldName.IsEmpty())
		return false;
	
	int32 Index = this->StateExist(OldName);
	if (Index != -1 && this->States.IsValidIndex(Index))
	{
		this->States.RemoveAt(Index);

		return true;
	}
	return false;
}

FTransition UStateMachine::GetTransitionByName(FString Name)
{
	if (Name.IsEmpty())
		return FTransition();

	return *this->Transitions.Find(Name);
}

bool UStateMachine::AddOrUpdateTransition(FTransition Transition, FString OldName/*= ""*/)
{
	if (Transition.IsValid() == false)
		return false;

	if (FTransition* ExistingTransition = this->Transitions.Find(OldName))
	{
		// Replace old transition
		this->Transitions.Remove(OldName);
		this->Transitions.Add(Transition.Name, Transition);

		// Replace linking between State --> Transition
		int32 IndexState = this->StateExist(ExistingTransition->FromState);
		if (IndexState != -1)
		{
			this->States[IndexState].TransitionNames.Remove(OldName);
			this->States[IndexState].TransitionNames.Add(Transition.Name);
		}
		
		return true;
	}
	else
	{
		// link transition and from state
		int32 IndexState = this->StateExist(Transition.FromState);
		if (IndexState != -1)
			this->States[IndexState].TransitionNames.Add(Transition.Name);

		this->Transitions.Add(Transition.Name, Transition);
		
		return true;
	}
}

bool UStateMachine::RemoveTransitionByName(FString Name)
{
	if (Name.IsEmpty())
		return false;

	if (FTransition* Transition = this->Transitions.Find(Name))
	{
		// Remove Transition name in State (Break connection)
		int32 IndexState = this->StateExist(Transition->FromState);
		if (IndexState != -1)
			this->States[IndexState].TransitionNames.Remove(Name);

		// Remove Transition
		this->Transitions.Remove(Name);

		return true;
	}

	return false;
}

void UStateMachine::Pause()
{
	this->Paused = true;
	this->UseTick = false;
	this->OnPause.Broadcast();
}

void UStateMachine::UnPaused()
{
	this->Paused = false;
	this->UseTick = true;
	this->OnUnPause.Broadcast();
}

bool UStateMachine::IsPaused() const
{
	return Paused;
}

void UStateMachine::BeginState()
{
	this->ExecuteInnerFunction(*this->CurrentState.BeginFunctionName);
}

void UStateMachine::UpdateState(float DeltaTime)
{
	this->CheckTransitionForCurrentState();
	this->ExecuteInnerFunction(*this->CurrentState.UpdateFunctionName, &DeltaTime);
}

void UStateMachine::FinishState()
{
	this->ExecuteInnerFunction(*this->CurrentState.FinishFunctionName);
}

TStatId UStateMachine::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UStateMachine, STATGROUP_Tickables);
}

bool UStateMachine::IsTickable() const
{
	return this->UseTick;
}

void UStateMachine::Tick(float DeltaTime)
{
	this->Delta = DeltaTime;
	this->UpdateState(DeltaTime);
}
