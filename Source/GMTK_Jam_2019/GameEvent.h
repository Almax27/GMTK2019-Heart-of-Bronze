// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameEvent.generated.h"

/**
 * 
 */
UCLASS(abstract, editinlinenew, Blueprintable, BlueprintType)
class GMTK_JAM_2019_API UGameEvent : public UObject
{
	GENERATED_BODY()
	
public:

    UGameEvent();

    virtual UWorld* GetWorld() const override;

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Game Event")
    bool CanBegin() const;

    UFUNCTION(BlueprintCallable, Category = "Game Event")
    bool IsActive() const;

    UFUNCTION(BlueprintCallable, Category = "Game Event")
    void Begin(UWorld* World);

    UFUNCTION(BlueprintCallable, Category = "Game Event")
    void Tick(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Game Event")
    void End();

protected: //Logical events

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Game Event")
    void OnBegin();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Game Event")
    void OnTick(float DeltaTime);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Game Event")
    void OnEnd();

public:

    //Should this event be triggered immediately or pushed onto the event queue
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event")
    bool bIsInstant;

    //Should this event be triggered immediately or pushed onto the event queue
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event")
    bool bCanInterupt;

    //Priority in the queue. Higher priorities will bubble up.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event")
    int32 QueuePriority;

private:

    UPROPERTY(Transient, VisibleInstanceOnly, Category = "Game Event")
    bool bIsActive;

    UPROPERTY(Transient)
    TWeakObjectPtr<UWorld> WorldPtr;
};
