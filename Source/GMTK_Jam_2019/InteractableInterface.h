// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GMTK_JAM_2019_API IInteractableInterface
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnInteract(const FHitResult& Hit);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnBeginHighlight(USceneComponent* Component);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnEndHighlight(USceneComponent* Component);
};
