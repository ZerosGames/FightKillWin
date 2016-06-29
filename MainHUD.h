// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
*
*/
struct FHitData
{
	/** Last hit time. */
	float HitTime;

	/** strength of hit icon */
	float HitPercentage;

	/** Initialise defaults. */
	FHitData()
	{
		HitTime = 0.0f;
		HitPercentage = 0.0f;
	}
};

UCLASS()
class MYPROJECT_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:

	AMainHUD();

	virtual void DrawHUD() override;

	void NotifyWeaponHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator);

	UPROPERTY()
		UTexture2D* TargetBoxTexture;

	UPROPERTY()
		UTexture2D* ProgressBarTexture;

	UPROPERTY()
		UTexture2D* WeaponsTexture;

	UPROPERTY()
		UTexture2D* HitNotifyTexture;

	UPROPERTY()
		FCanvasIcon FriendlyTargetBoxIcon;

	UPROPERTY()
		FCanvasIcon EnemyTargetBoxIcon;

	UPROPERTY()
		FCanvasIcon CrosshairIcon;

	UPROPERTY()
		FCanvasIcon HealthBarIcon;

	UPROPERTY()
		FCanvasIcon S_ProgressBarIcon;

	UPROPERTY()
		FCanvasIcon A_ProgressBarIcon;

	UPROPERTY()
		FCanvasIcon B_ProgressBarIcon;

	UPROPERTY()
		FCanvasIcon B_MinimapIcon;

	UPROPERTY()
		FCanvasIcon E_MinimapIcon;

	UPROPERTY()
		FCanvasIcon F_MinimapIcon;

	UPROPERTY()
		FCanvasIcon Alpha_E_MinimapIcon;

	UPROPERTY()
		FCanvasIcon Alpha_F_MinimapIcon;

	UPROPERTY()
		FCanvasIcon S_MinimapIcon;

	UPROPERTY()
		FCanvasIcon MiniGunIcon;

	UPROPERTY()
		FCanvasIcon MissleGunIcon;

	UPROPERTY()
		FCanvasIcon SniperGunIcon;

	UPROPERTY()
		FCanvasIcon HitNotifyIcon[8];

	UPROPERTY()
		UFont* Font;

	FVector2D Offsets[8];

	FHitData HitNotifyData[8];

	float LastHitTime;

	float HitNotifyDisplayTime;

private:

	void DrawSpeed();
	void DrawHealth();
	void DrawTargetBox(float, float, AActor* Target, FCanvasIcon, FColor);
	void DrawCrosshair(FColor);
	void DrawMinimap();
	void DrawWeapons();
	void DrawHitIndicator();

	void NotifyEnemyHit();


	FLinearColor TargetBoxColor;
	float UIscale;

	TArray<AActor*> EnemyActorsInSightSphere;
	TArray<AActor*> FreindlyActorsInSightSphere;
	TArray<AMainPawn*> EnemyPawnsInSightSphere;
	TArray<AMainPawn*> FreindlyPawnsInSightSphere;

	FVector location;
	bool didhaveTarget;
	static const float MinHudScale;

};
