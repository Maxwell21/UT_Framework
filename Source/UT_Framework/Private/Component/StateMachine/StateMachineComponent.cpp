/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (StateMachine == nullptr)
	{
		this->StateMachine = UStateMachine::ConstructStateMachine(Template, this, UGameplayStatics::GetPlayerController(this, 0));

		if (this->StateMachine)
			this->BindDelegates();
	}

	if (StateMachine && AutoStart)
		this->Start();
}

void UStateMachineComponent::BeginDestroy()
{
	this->UnbindDelegates();
	this->Stop();

	Super::BeginDestroy();
}

FState UStateMachineComponent::GetCurrentState()
{
	if (this->StateMachine)
		return this->StateMachine->GetCurrentState();

	return FState();
}

void UStateMachineComponent::Start()
{
	if (StateMachine)
		this->StateMachine->Start();
}

void UStateMachineComponent::Stop()
{
	if (this->StateMachine)
		StateMachine->Stop();
}

FString UStateMachineComponent::GetCurrentStateName()
{
	if (this->StateMachine)
		return this->StateMachine->GetCurrentStateName();

	return FString();
}

UStateMachine* UStateMachineComponent::GetMachine()
{
	return this->StateMachine;
}

void UStateMachineComponent::Pause()
{
	if (this->StateMachine)
		this->StateMachine->Pause();
}

void UStateMachineComponent::UnPaused()
{
	if (this->StateMachine)
		this->StateMachine->UnPaused();
}

bool UStateMachineComponent::IsPaused() const
{
	if (this->StateMachine)
		this->StateMachine->IsPaused();
	
	return false;
}

bool UStateMachineComponent::IsValid() const
{
	return this->StateMachine->IsValidLowLevel();
}

void UStateMachineComponent::BindDelegates()
{
	this->StateMachine->OnInit.AddDynamic(this, &UStateMachineComponent::OnInitTriggered);
	this->StateMachine->OnStart.AddDynamic(this, &UStateMachineComponent::OnStartTriggered);
	this->StateMachine->OnStop.AddDynamic(this, &UStateMachineComponent::OnStopTriggered);
	this->StateMachine->OnPause.AddDynamic(this, &UStateMachineComponent::OnPauseTriggered);
	this->StateMachine->OnUnPause.AddDynamic(this, &UStateMachineComponent::OnUnPauseTriggered);
}

void UStateMachineComponent::UnbindDelegates()
{
	if (this->OnInit.IsBound())
		this->OnInit.RemoveDynamic(this, &UStateMachineComponent::OnInitTriggered);

	if (this->OnStart.IsBound())
		this->OnStart.RemoveDynamic(this, &UStateMachineComponent::OnStartTriggered);

	if (this->OnStop.IsBound())
		this->OnStop.RemoveDynamic(this, &UStateMachineComponent::OnStopTriggered);

	if (this->OnPause.IsBound())
		this->OnPause.RemoveDynamic(this, &UStateMachineComponent::OnPauseTriggered);

	if (this->OnUnPause.IsBound())
		this->OnUnPause.RemoveDynamic(this, &UStateMachineComponent::OnUnPauseTriggered);
}

void UStateMachineComponent::OnInitTriggered()
{
	this->OnInit.Broadcast();
}

void UStateMachineComponent::OnStartTriggered()
{
	this->OnStart.Broadcast();
}

void UStateMachineComponent::OnStopTriggered()
{
	this->StateMachine = nullptr;
	this->UnbindDelegates();
	this->OnStop.Broadcast();
}

void UStateMachineComponent::OnPauseTriggered()
{
	this->OnPause.Broadcast();
}

void UStateMachineComponent::OnUnPauseTriggered()
{
	this->OnUnPause.Broadcast();
}
