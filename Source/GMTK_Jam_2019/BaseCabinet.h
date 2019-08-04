// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseCabinet.generated.h"


namespace CabinetState
{
	const extern FName Active;
	const extern FName Inactive;
	const extern FName Fail;

	const extern TArray<FName> States;

}

namespace CabinetResult
{
	const extern FName Success;
	const extern FName Fail;

	const extern TArray<FName> Results;
}

USTRUCT(BlueprintType)
struct FCabinetResult
{
	GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Cabinet")
	class ABaseCabinet* Cabinet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cabinet")
	FName result;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCabinetResultDelegate, FCabinetResult, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCabinetDelegate, ABaseCabinet*, cabinet);

UCLASS()
class GMTK_JAM_2019_API ABaseCabinet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseCabinet();

	UPROPERTY(EditAnywhere, Category = "Cabinet")
		bool StartActivated = false;

    UPROPERTY(EditAnywhere, Category = "Cabinet")
		bool AutoRefreshWidget = true;

	UPROPERTY(BlueprintAssignable)
		FCabinetResultDelegate OnCabinetResult;

	UPROPERTY(BlueprintAssignable)
		FCabinetDelegate OnCabinetStateChanged;

	UPROPERTY(EditAnywhere, Category = "ScreenWidgets")
		TSubclassOf<class UUserWidget> ActiveScreen;

	UPROPERTY(EditAnywhere, Category = "ScreenWidgets")
		TSubclassOf<class UUserWidget> InactiveScreen;

	UPROPERTY(EditAnywhere, Category = "ScreenWidgets")
		TSubclassOf<class UUserWidget> FailScreen;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ScreenWidgets")
		class UWidgetComponent* GetWidgetComponent();

	UFUNCTION(BlueprintCallable)
	FName GetCurrentState() { return m_currentState; }

	UFUNCTION(BlueprintCallable)
		float GetTimeInCurrentState() { return m_timeInCurrentState; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cabinet")
		void ActivateCabinet();

	
protected:
	FName m_currentState;
	float m_timeInCurrentState;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* update the cabinet based on current state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cabinet")
		void UpdateState(float dt);

	/* assess current state and populate pending state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cabinet")
		void GetPendingState(FName& outPendingState);

	/* assess current state and broadcast result if there is one */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cabinet")
		void CheckForResult();

	UFUNCTION(BlueprintCallable)
		void BroadcastResult(FCabinetResult result);

	UFUNCTION(BlueprintCallable)
		virtual void ChangeState(FName newState);

	UFUNCTION(BlueprintCallable)
		virtual bool IsValidState(FName state);

	UFUNCTION(BlueprintCallable)
		virtual bool IsValidResult(FName result);

private:
	void RefreshScreenWidget();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
