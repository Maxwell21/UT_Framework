/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "StateMachineFactory.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UStateMachineFactory : public UFactory
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	TSubclassOf<class UStateMachine> ParentClass;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	UStateMachineFactory(const class FObjectInitializer& Object);

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FName GetNewAssetThumbnailOverride() const override;
	//~ Begin UFactory Interface	
	
};
