// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NavigationComponent.generated.h"

USTRUCT(BlueprintType)
struct FMissileData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Lane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

	FMissileData()
	{
		Lane = 0;
		Damage = 0.f;
		Speed = 0.f;
	}

	FMissileData(int lane, float damage, float speed)
	{
		Lane = lane;
		Damage = damage;
		Speed = speed;
	}

	void CopyFrom(FMissileData data)
	{
		Lane = data.Lane;
		Damage = data.Damage;
		Speed = data.Speed;
	}

};

USTRUCT(BlueprintType)
struct FMissile
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMissileData MissileData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Progress = 0.f;

	FMissile()
	{
		MissileData = FMissileData();
	}

	FMissile(FMissileData data, float startingProgress = 0.f)
	{
		MissileData.CopyFrom(data);
		Progress = startingProgress;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNavMissileDelegate, FMissile, missile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNavThreatDelegate, bool, isThreatened);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTK_JAM_2019_API UNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNavigationComponent();
	
	// accessors
	UFUNCTION(BlueprintCallable)
	int GetNumberOfLanes() { return m_numberOfLanes; }
	UFUNCTION(BlueprintCallable)
	int GetCurrentLane() { return m_currentLane; }
	UFUNCTION(BlueprintCallable)
	bool GetIsThreatened() { return m_isThreatened; }

	UFUNCTION(BlueprintCallable)
	TArray<FMissile> GetIncomingMissiles() const { return m_incomingMissiles;}

	UFUNCTION(BlueprintCallable)
		TArray<FMissile> GetAllMissilesInLane(int laneIndex) const;

	UFUNCTION(BlueprintCallable)
		TArray<FMissile> GetVisibleMissilesInLane(int laneIndex) const;

	UFUNCTION(BlueprintCallable)
	void ActivateNavigation(float missileSpeed, float missileSalvoSize);

	// lane movement
	UFUNCTION(BlueprintCallable)
	void ChangeLane(bool moveRight);

	bool CanMoveLeft();
	bool CanMoveRight();
	bool IsValidLane(int number);
	bool IsLaneEmpty(int lane); // returns true if there are no active missiles in this area

	// missiles
	/* spawn a missile salvo at the current salvo size*/
	bool SpawnMissileSalvo();
	/* spawn a missile salvo at the provided salvo size*/
	bool SpawnMissileSalvo(int salvoSize);

	/* Spawn at the specified lane using the default data*/
	bool SpawnMissile(int atLane);
	/* Spawn at the specified lane using the provided damage and speed */
	bool SpawnMissile(int atLane, float damage, float speed, float startingProgress = 0.f);
	/* Spawn from the provided data */
	bool SpawnMissile(FMissileData fromData, float startingProgress = 0.f);

	UFUNCTION(BlueprintCallable)
	void UpdateNavigationComponent(float dt);

	void UpdateIncomingMissiles(float dt);

	UPROPERTY(BlueprintAssignable)
	FNavMissileDelegate MissileImpactEvent;

	UPROPERTY(BlueprintAssignable)
	FNavMissileDelegate MissileOnScreenEvent;
	
	UPROPERTY(BlueprintAssignable)
	FNavMissileDelegate MissileHalfwayEvent;
	
	UPROPERTY(BlueprintAssignable)
	FNavThreatDelegate ThreatenedStateChangedEvent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	void CheckForMissileImpact();
	void ProcessMissileImpact(int missileIndex);
	TArray<int> GenerateLanePool();

	const int m_numberOfLanes = 3;
	const float m_minMissileStartProgress = 0.5f;
	const float m_missileSpacingVariance = 0.05f;
	int m_currentLane;
	bool m_isThreatened;
	int m_missileSalvoSize;

	TArray<FMissile> m_incomingMissiles;
	FMissileData m_defaultMissileData;



};
