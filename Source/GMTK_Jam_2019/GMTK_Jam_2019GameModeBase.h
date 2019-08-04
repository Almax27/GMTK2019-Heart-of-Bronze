// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GMTK_Jam_2019GameModeBase.generated.h"

class UGameEvent;

USTRUCT(BlueprintType)
struct FGameEventPool
{
    GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinEventFrequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxEventFrequency;

    UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite)
    TArray<UGameEvent*> GameEvents;
};

/**
 * 
 */
UCLASS()
class GMTK_JAM_2019_API AGMTK_Jam_2019GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

    AGMTK_Jam_2019GameModeBase();

    virtual void BeginPlay() override;
    virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& TickFunction) override;

    void TickGameEventQueue(float DeltaTime);
    void TickActiveGameEvents(float DeltaTime);

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SubmitGameEvent(UGameEvent* Event);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SubmitGameEventByName(FName Name);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnGameEventBegan(UGameEvent* Event);

public:

    UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite)
    TMap<FName, UGameEvent*> NamedGameEvents;

    UPROPERTY(Transient)
    TArray<UGameEvent*> GameEventQueue;

    UPROPERTY(Transient)
    UGameEvent* QueuedActiveGameEvent;

    UPROPERTY(Transient)
    TArray<UGameEvent*> ActiveGameEvents;
};
