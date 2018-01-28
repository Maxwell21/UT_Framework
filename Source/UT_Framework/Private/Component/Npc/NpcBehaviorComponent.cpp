/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorComponent.h"
#include "NpcBehavior.h"
#include "AIController.h"
#include "GameplayTasksComponent.h"

// Sets default values for this component's properties
UNpcBehaviorComponent::UNpcBehaviorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UNpcBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UNpcBehaviorComponent::CreateNpcBehavior()
{
	if (AActor* Owner = this->GetOwner())
		this->NpcBehavior = NewObject<UNpcBehavior>(Owner, this->NpcBehaviorTemplate);

	return this->NpcBehavior != nullptr;
}

bool UNpcBehaviorComponent::CreateGameplayTaskComponent()
{
	if (AActor* Owner = this->GetOwner())
	{
		// check if the owner already has the component
		this->GameplayTaskComponent = Cast<UGameplayTasksComponent>(Owner->GetComponentByClass(UGameplayTasksComponent::StaticClass()));

		// The owner doesn't has it
		if (!this->GameplayTaskComponent)
		{
			// So we create it
			this->GameplayTaskComponent = NewObject<UGameplayTasksComponent>(Owner, UGameplayTasksComponent::StaticClass());
			Owner->AddInstanceComponent(this->GameplayTaskComponent);
			Owner->RegisterAllComponents();
		}
	}

	return this->GameplayTaskComponent != nullptr;
}

void UNpcBehaviorComponent::StartNpcBehavior()
{
	// Create them if not exist
	if (!this->CreateNpcBehavior() || !this->CreateGameplayTaskComponent())
		return;

	if (AActor* Owner = this->GetOwner())
	{
		AAIController* Controller = nullptr;

		// Try to get the AIController
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (Pawn->GetController()->IsA<AAIController>())
				Controller = Cast<AAIController>(Pawn->GetController());
		}
		else if (Owner->GetInstigatorController() && Cast<AAIController>(Owner->GetInstigatorController()))
			Controller = Cast<AAIController>(Owner->GetInstigatorController());
		
		// Check if the controller is set
		if (Controller)
		{
			// Start NpcBehavior
			this->NpcBehavior->Start(this, Controller);
		}
	}
}
