// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MainHUD.h"
#include "MainPlayerController.h"
#include "MainPawn.h"

const float AMainHUD::MinHudScale = 0.5f;

AMainHUD::AMainHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> TargetBoxOB(TEXT("/Game/UI/HUDtarget"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ProgressBarOB(TEXT("/Game/UI/HUDbar"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> WeaponsOB(TEXT("/Game/UI/T_Weapons"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HitTextureOb(TEXT("/Game/UI/HitIndicator"));

	static ConstructorHelpers::FObjectFinder<UFont> FontOB(TEXT("/Game/UI/Roboto-Black"));


	TargetBoxTexture = TargetBoxOB.Object;
	ProgressBarTexture = ProgressBarOB.Object;
	WeaponsTexture = WeaponsOB.Object;
	HitNotifyTexture = HitTextureOb.Object;

	Font = FontOB.Object;

	FriendlyTargetBoxIcon = UCanvas::MakeIcon(TargetBoxTexture, 2, 2, 50, 50);
	EnemyTargetBoxIcon = UCanvas::MakeIcon(TargetBoxTexture, 2, 54, 50, 50);
	CrosshairIcon = UCanvas::MakeIcon(TargetBoxTexture, 2, 106, 50, 50);
	HealthBarIcon = UCanvas::MakeIcon(TargetBoxTexture, 2, 2, 2, 50);

	B_ProgressBarIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 2, 400, 30);
	S_ProgressBarIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 34, 396, 26);
	A_ProgressBarIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 62, 396, 26);

	B_MinimapIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 90, 200, 200);
	F_MinimapIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 292, 10, 10);
	E_MinimapIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 304, 10, 10);
	S_MinimapIcon = UCanvas::MakeIcon(ProgressBarTexture, 2, 316, 15, 18);
	Alpha_F_MinimapIcon = UCanvas::MakeIcon(ProgressBarTexture, 14, 292, 10, 10);
	Alpha_E_MinimapIcon = UCanvas::MakeIcon(ProgressBarTexture, 14, 304, 10, 10);

	MiniGunIcon = UCanvas::MakeIcon(WeaponsTexture, 0, 0, 128, 128);
	MissleGunIcon = UCanvas::MakeIcon(WeaponsTexture, 128, 0, 128, 128);
	SniperGunIcon = UCanvas::MakeIcon(WeaponsTexture, 256, 0, 128, 128);

	HitNotifyIcon[0] = UCanvas::MakeIcon(HitNotifyTexture, 158, 831, 585, 392);
	HitNotifyIcon[1] = UCanvas::MakeIcon(HitNotifyTexture, 369, 434, 460, 378);
	HitNotifyIcon[2] = UCanvas::MakeIcon(HitNotifyTexture, 848, 284, 361, 395);
	HitNotifyIcon[3] = UCanvas::MakeIcon(HitNotifyTexture, 1212, 397, 427, 394);
	HitNotifyIcon[4] = UCanvas::MakeIcon(HitNotifyTexture, 1350, 844, 547, 321);
	HitNotifyIcon[5] = UCanvas::MakeIcon(HitNotifyTexture, 1232, 1241, 458, 341);
	HitNotifyIcon[6] = UCanvas::MakeIcon(HitNotifyTexture, 862, 1384, 353, 408);
	HitNotifyIcon[7] = UCanvas::MakeIcon(HitNotifyTexture, 454, 1251, 371, 410);

	Offsets[0] = FVector2D(173, 0);
	Offsets[1] = FVector2D(120, 125);
	Offsets[2] = FVector2D(0, 173);
	Offsets[3] = FVector2D(-120, 125);
	Offsets[4] = FVector2D(-173, 0);
	Offsets[5] = FVector2D(-120, -125);
	Offsets[6] = FVector2D(0, -173);
	Offsets[7] = FVector2D(120, -125);

	HitNotifyDisplayTime = 0.75f;

	UIscale = 1.0f;

}

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Canvas == nullptr)
	{
		return;
	}

	//UIscale = Canvas->ClipY / 1080.0f;

	FCanvasTextItem FPSTextItem(FVector2D(Canvas->ClipX - 30.0f, 20.0f), FText::GetEmpty(), Font, FColor::Black);
	FPSTextItem.Text = FText::FromString(FString::FromInt(1 / GetWorld()->GetDeltaSeconds()));
	FPSTextItem.Scale = FVector2D(1 * UIscale, 1 * UIscale);
	Canvas->DrawItem(FPSTextItem);

	AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	AMainPlayerController* PC = Cast<AMainPlayerController>(GetOwningPlayerController());

	UIscale = FMath::Max(UIscale, MinHudScale);

	if (Pawn != nullptr && PC != nullptr)
	{
		FreindlyPawnsInSightSphere = Pawn->GetFriendlysOverlapPawn();
		EnemyPawnsInSightSphere = Pawn->GetEnemyOverlapPawn();
		EnemyActorsInSightSphere = Pawn->GetEnemyOverlapActor();
		FreindlyActorsInSightSphere = Pawn->GetFriendlysOverlapActor();

		for (AMainPawn* temp : FreindlyPawnsInSightSphere)
		{
			FVector2D ViewportPosition;

			if (temp != nullptr && PC)
			{
				if (PC->ProjectWorldLocationToScreen(temp->GetActorLocation(), ViewportPosition) == true)
				{
					DrawTargetBox(ViewportPosition.X, ViewportPosition.Y, temp, FriendlyTargetBoxIcon, FColor::Green);
				}
			}
		}

		for (AMainPawn* temp : EnemyPawnsInSightSphere)
		{
			FVector2D ViewportPosition;

			if (temp != nullptr && PC)
			{
				if (GetOwningPlayerController()->ProjectWorldLocationToScreen(temp->GetActorLocation(), ViewportPosition) == true)
				{
					DrawTargetBox(ViewportPosition.X, ViewportPosition.Y, temp, FriendlyTargetBoxIcon, FColor::Red);
				}
			}
		}

		for (AActor* temp : EnemyActorsInSightSphere)
		{
			FVector2D ViewportPosition;

			if (temp != nullptr && PC)
			{
				if (GetOwningPlayerController()->ProjectWorldLocationToScreen(temp->GetActorLocation(), ViewportPosition) == true)
				{
					DrawTargetBox(ViewportPosition.X, ViewportPosition.Y, temp, EnemyTargetBoxIcon, FColor::Green);
				}
			}
		}

		for (AActor* temp : FreindlyActorsInSightSphere)
		{
			FVector2D ViewportPosition;

			if (temp != nullptr && PC)
			{
				if (GetOwningPlayerController()->ProjectWorldLocationToScreen(temp->GetActorLocation(), ViewportPosition) == true)
				{
					DrawTargetBox(ViewportPosition.X, ViewportPosition.Y, temp, EnemyTargetBoxIcon, FColor::Red);
				}
			}
		}

		AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

		AActor* temp;
		if (Pawn->getWeapon() != nullptr)
		{
			if (Pawn->getWeapon()->getTarget() != nullptr)
			{
				temp = Pawn->getWeapon()->getTarget();
			}
			else
			{
				temp = nullptr;
			}

			if (temp != nullptr)
			{
				DrawCrosshair(FColor::Red);
			}
			else
			{
				DrawCrosshair(FColor::White);
			}
		}
	}


	Canvas->SetDrawColor(FColor::White);
	//DrawHealth();
	DrawSpeed();
	DrawMinimap();
	DrawWeapons();
	DrawHitIndicator();
}

void AMainHUD::DrawHealth()
{
	AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	FCanvasTextItem HealthTextItem(FVector2D(Canvas->ClipX / 2 - 150 * UIscale / 2, Canvas->ClipY / 2 - 50 * UIscale / 2), FText::GetEmpty(), Font, FLinearColor::White);
	HealthTextItem.Text = FText::FromString(FString::FromInt(Pawn->Health) + "%");
	HealthTextItem.Scale = FVector2D(1.5f * UIscale, 1.5f * UIscale);

	Canvas->DrawItem(HealthTextItem);
}

void AMainHUD::DrawSpeed()
{
	//AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwningPlayerController());
	AMainPawn* Pawn = Cast<AMainPawn>(GetOwningPawn());
	if (Pawn)
	{
		float Speed = Pawn->GetSpeed() / 2000.0f;

		const float BarPositionX = (20.0f + B_MinimapIcon.UL + 10.0f) * UIscale;
		const float BarPositionY = Canvas->ClipY - (20.0f + B_ProgressBarIcon.VL) * UIscale;

		Canvas->DrawIcon(B_ProgressBarIcon, BarPositionX, BarPositionY, UIscale);

		FCanvasTileItem TileItem(FVector2D(BarPositionX + 2, BarPositionY + 2), S_ProgressBarIcon.Texture->Resource, FVector2D(S_ProgressBarIcon.UL * Speed * UIscale, S_ProgressBarIcon.VL *UIscale), FLinearColor::White);

		const float Width = S_ProgressBarIcon.Texture->GetSurfaceWidth();
		const float Height = S_ProgressBarIcon.Texture->GetSurfaceHeight();
		TileItem.UV0 = FVector2D(S_ProgressBarIcon.U / Width, S_ProgressBarIcon.V / Height);
		TileItem.UV1 = TileItem.UV0 + FVector2D(S_ProgressBarIcon.UL / Width, S_ProgressBarIcon.VL / Height);

		TileItem.BlendMode = SE_BLEND_Translucent;

		FString SpeedText = FString::FromInt(Pawn->GetSpeed() / 10);
		FCanvasTextItem TextItem(FVector2D((20.0f + B_MinimapIcon.UL + 10.0f + S_ProgressBarIcon.UL + 5.0f) * UIscale, (Canvas->ClipY - 20.0f - B_ProgressBarIcon.VL - 5.0f) * UIscale), FText::GetEmpty(), Font, FLinearColor::White);
		TextItem.Text = FText::FromString(SpeedText);
		TextItem.Scale = FVector2D(1 * UIscale, 1 * UIscale);

		Canvas->DrawItem(TextItem);
		Canvas->DrawItem(TileItem);
	}
}

void AMainHUD::DrawMinimap()
{
	Canvas->DrawIcon(B_MinimapIcon, 20.0f*UIscale, Canvas->ClipY - (20.0f + B_MinimapIcon.VL)*UIscale, UIscale);

	TArray<AActor*> TempActors;
	TArray<AMainPawn*> TempPawns;

	for (AActor* temp : FreindlyActorsInSightSphere)
	{
		TempActors.Add(temp);
	}

	for (AActor* temp : EnemyActorsInSightSphere)
	{
		TempActors.Add(temp);
	}

	for (AMainPawn* temp : FreindlyPawnsInSightSphere)
	{
		TempPawns.Add(temp);
	}

	for (AMainPawn* temp : EnemyPawnsInSightSphere)
	{
		TempPawns.Add(temp);
	}

	for (AActor* temp : TempActors)
	{
		if (temp)
		{
			AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

			if (Pawn != nullptr)
			{

				float M_Xpos = FMath::GetMappedRangeValue(FVector2D(-4000, 4000), FVector2D(-100, 100), temp->GetActorLocation().X - Pawn->GetActorLocation().X);
				float M__Ypos = FMath::GetMappedRangeValue(FVector2D(-4000, 4000), FVector2D(-100, 100), temp->GetActorLocation().Y - Pawn->GetActorLocation().Y);

				float Rot = (Pawn->GetActorRotation().Yaw)* PI / 180;

				float Xpos = M_Xpos * cos(Rot) + M__Ypos * sin(Rot);
				float Ypos = -M_Xpos * sin(Rot) + M__Ypos * cos(Rot);


				if (FVector2D(Xpos, Ypos).Size() >= 100)
				{
					FVector2D Clamped = 100 * FVector2D(Xpos, Ypos) / FVector2D(Xpos, Ypos).Size();
					Xpos = Clamped.X;
					Ypos = Clamped.Y;
				}

				if (Pawn)
				{
					if (Pawn->ActorHasTag("RED"))
					{
						if (temp->ActorHasTag("BLUE"))
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
						else
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
					}
					else if (Pawn->ActorHasTag("BLUE"))
					{
						if (temp->ActorHasTag("RED"))
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
						else
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
					}
				}
			}
		}
	}

	for (AMainPawn* temp : TempPawns)
	{
		AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

		if (Pawn != nullptr)
		{
			if (temp)
			{
				float M_Xpos = FMath::GetMappedRangeValue(FVector2D(-4000, 4000), FVector2D(-100, 100), temp->GetActorLocation().X - Pawn->GetActorLocation().X);
				float M__Ypos = FMath::GetMappedRangeValue(FVector2D(-4000, 4000), FVector2D(-100, 100), temp->GetActorLocation().Y - Pawn->GetActorLocation().Y);

				float Rot = (Pawn->GetActorRotation().Yaw)* PI / 180;

				float Xpos = M_Xpos * cos(Rot) + M__Ypos * sin(Rot);
				float Ypos = -M_Xpos * sin(Rot) + M__Ypos * cos(Rot);


				if (FVector2D(Xpos, Ypos).Size() >= 100)
				{
					FVector2D Clamped = 100 * FVector2D(Xpos, Ypos) / FVector2D(Xpos, Ypos).Size();
					Xpos = Clamped.X;
					Ypos = Clamped.Y;
				}

				if (Pawn)
				{
					if (Pawn->ActorHasTag("RED"))
					{
						if (temp->ActorHasTag("BLUE"))
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
						else
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
					}
					else if (Pawn->ActorHasTag("BLUE"))
					{
						if (temp->ActorHasTag("RED"))
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_E_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - E_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - E_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
						else
						{
							if (FVector2D(Xpos, Ypos).Size() < 99)
							{
								Canvas->DrawIcon(F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
							else
							{
								Canvas->DrawIcon(Alpha_F_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - F_MinimapIcon.UL / 2 + Ypos)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - F_MinimapIcon.VL / 2 - Xpos)*UIscale, UIscale);
							}
						}
					}
				}
			}
		}
	}

	Canvas->DrawIcon(S_MinimapIcon, (B_MinimapIcon.UL / 2 + 20.0f - S_MinimapIcon.UL / 2)*UIscale, (Canvas->ClipY - 20.0f - B_MinimapIcon.VL / 2 - S_MinimapIcon.VL / 2)*UIscale, UIscale);
}

void AMainHUD::DrawTargetBox(float PositionX, float PositionY, AActor* Target, FCanvasIcon Icon, FColor Color)
{

	AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (Pawn != nullptr)
	{

		if (Target->IsA(AMainPawn::StaticClass()))
		{
			AMainPawn* castedTemp = Cast<AMainPawn>(Target);

			FCanvasTextItem HealthTextItem(FVector2D(PositionX - (Icon.UL) * UIscale / 3, PositionY - Icon.VL * UIscale / 2 - 15.0f), FText::GetEmpty(), Font, Color);
			HealthTextItem.Text = FText::FromString(FString::FromInt(castedTemp->Health) + "%");
			HealthTextItem.Scale = FVector2D(1 * UIscale, 1 * UIscale);

			FCanvasTextItem NameTextItem(FVector2D(PositionX + (Icon.UL / 2) * UIscale / 2 + 13.0f, PositionY - Icon.VL * UIscale / 2), FText::GetEmpty(), Font, Color);
			NameTextItem.Text = FText::FromString(Target->GetName());
			NameTextItem.Scale = FVector2D(1 * UIscale, 1 * UIscale);

			FCanvasTextItem DistanceTextItem(FVector2D(PositionX + (Icon.UL / 2)  * UIscale / 2 + 13.0f, PositionY - Icon.VL  * UIscale / 2 + 13.0f), FText::GetEmpty(), Font, Color);
			DistanceTextItem.Text = FText::FromString(FString::FromInt(FVector(Target->GetActorLocation() - Pawn->GetActorLocation()).Size()));
			DistanceTextItem.Scale = FVector2D(1 * UIscale, 1 * UIscale);

			Canvas->SetDrawColor(Color);
			Canvas->DrawItem(HealthTextItem);
			Canvas->DrawItem(NameTextItem);
			Canvas->DrawItem(DistanceTextItem);
			Canvas->DrawIcon(Icon, PositionX - Icon.UL * UIscale / 2, PositionY - Icon.VL * UIscale / 2, UIscale);
		}
	}



	/////////////////////////////////
	//////////  HealthBar  /////////
	///////////////////////////////

	//FCanvasTileItem TileItem(FVector2D(PositionX - (Icon.UL) * UIscale / 2 - 8.0f, PositionY + Icon.VL * UIscale / 2), HealthBarIcon.Texture->Resource, FVector2D(HealthBarIcon.UL * UIscale, HealthBarIcon.VL * (-castedTemp->Health / castedTemp->MaxHealth) * UIscale), Color);

	//const float Width = HealthBarIcon.Texture->GetSurfaceWidth();
	//const float Height = HealthBarIcon.Texture->GetSurfaceHeight();
	//TileItem.UV0 = FVector2D(HealthBarIcon.U / Width, HealthBarIcon.V / Height);
	//TileItem.UV1 = TileItem.UV0 + FVector2D(HealthBarIcon.UL / Width, HealthBarIcon.VL / Height);

	//TileItem.BlendMode = SE_BLEND_Translucent;
	//Canvas->DrawItem(TileItem);



}

void AMainHUD::DrawCrosshair(FColor Color)
{
	AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	FRotator rotation;

	if (Pawn != nullptr)
	{
		FVector2D ViewportPosition;
		if (Pawn->getWeapon() == nullptr || Pawn->getWeapon()->getTarget() == nullptr && didhaveTarget == true)
		{
			location = FMath::VInterpTo(location, Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 2000, GetWorld()->DeltaTimeSeconds, 10.0f);

			FVector Dis = location - (Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 2000);
			Dis.Y = 0;

			if (Dis.Size() < 20.0f)
			{
				location = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 2000;
				didhaveTarget = false;
			}
		}
		else if (Pawn->getWeapon() == nullptr || Pawn->getWeapon()->getTarget() == nullptr && didhaveTarget == false)
		{
			location = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 2000;
		}

		if (Pawn->getWeapon()->getTarget() && Pawn->getWeapon() != nullptr)
		{
			FVector Dis = location - (Pawn->getWeapon()->getTarget()->GetActorLocation());
			Dis.Y = 0;

			if (Dis.Size() < 20.0f)
			{
				location = Pawn->getWeapon()->getTarget()->GetActorLocation();
			}
			else
			{
				location = FMath::VInterpTo(location, Pawn->getWeapon()->getTarget()->GetActorLocation(), GetWorld()->DeltaTimeSeconds, 10.0f);
			}

			didhaveTarget = true;
		}

		rotation = Pawn->GetActorRotation();

		GetOwningPlayerController()->ProjectWorldLocationToScreen(location, ViewportPosition);

		Canvas->SetDrawColor(Color);
		Canvas->DrawIcon(CrosshairIcon, ViewportPosition.X - CrosshairIcon.UL * UIscale / 2, ViewportPosition.Y - CrosshairIcon.VL * UIscale / 2, UIscale);
	}

}

void AMainHUD::DrawWeapons()
{
	AMainPawn* Pawn = Cast<AMainPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Pawn && Pawn->getWeapon() != nullptr)
	{
		if (Pawn->getWeapon()->ProjectileType == EWeaponProjectile::EBullet)
		{
			Canvas->DrawIcon(MiniGunIcon, Canvas->ClipX - 128 * 1 * 0.5 * UIscale, 0, 0.5 * UIscale);
		}

		if (Pawn->getWeapon()->ProjectileType == EWeaponProjectile::EProjectile)
		{
			Canvas->DrawIcon(MissleGunIcon, Canvas->ClipX - 128 * 1 * 0.5 * UIscale, 0, 0.5 * UIscale);
		}

		if (Pawn->getWeapon()->ProjectileType == EWeaponProjectile::ESniper)
		{
			Canvas->DrawIcon(SniperGunIcon, Canvas->ClipX - 128 * 1 * 0.5 * UIscale, 0, 0.5 * UIscale);
		}
	}
}

void AMainHUD::NotifyWeaponHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	AMainPawn* MyPawn = (PlayerOwner) ? Cast<AMainPawn>(PlayerOwner->GetPawn()) : NULL;
	if (MyPawn)
	{
		if (CurrentTime - LastHitTime > HitNotifyDisplayTime)
		{
			for (uint8 i = 0; i < 8; i++)
			{
				HitNotifyData[i].HitPercentage = 0;
			}
		}

		FVector ImpulseDir;
		FHitResult Hit;
		DamageEvent.GetBestHitInfo(this, PawnInstigator, Hit, ImpulseDir);

		//check hit vector against pre-defined direction vectors - left, front, right, back
		const FVector HitVector = FRotationMatrix(PlayerOwner->GetControlRotation()).InverseTransformVector(-ImpulseDir);

		FVector Dirs2[8] = {
			FVector(0, -1, 0) /*left*/,
			FVector(1, -1, 0) /*front left*/,
			FVector(1, 0, 0) /*front*/,
			FVector(1, 1, 0) /*front right*/,
			FVector(0, 1, 0) /*right*/,
			FVector(-1, 1, 0) /*back right*/,
			FVector(-1, 0, 0), /*back*/
			FVector(-1, -1, 0) /*back left*/
		};
		int32 DirIndex = -1;
		float HighestModifier = 0;

		for (uint8 i = 0; i < 8; i++)
		{
			//Normalize our direction vectors
			Dirs2[i].Normalize();
			const float DirModifier = FMath::Max(0.0f, FVector::DotProduct(Dirs2[i], HitVector));
			if (DirModifier > HighestModifier)
			{
				DirIndex = i;
				HighestModifier = DirModifier;
			}
		}
		if (DirIndex > -1)
		{
			const float DamageTakenPercentage = (DamageTaken / MyPawn->Health);
			HitNotifyData[DirIndex].HitPercentage += DamageTakenPercentage * 2;
			HitNotifyData[DirIndex].HitPercentage = FMath::Clamp(HitNotifyData[DirIndex].HitPercentage, 0.0f, 1.0f);
			HitNotifyData[DirIndex].HitTime = CurrentTime;
		}

	}

	LastHitTime = CurrentTime;
}

void AMainHUD::DrawHitIndicator()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastHitTime <= HitNotifyDisplayTime)
	{
		const float StartX = Canvas->ClipX / 2.0f;
		const float StartY = Canvas->ClipY / 2.0f;

		for (uint8 i = 0; i < 8; i++)
		{
			const float TimeModifier = FMath::Max(0.0f, 1 - (CurrentTime - HitNotifyData[i].HitTime) / HitNotifyDisplayTime);
			const float Alpha = TimeModifier * HitNotifyData[i].HitPercentage;
			Canvas->SetDrawColor(255, 255, 255, FMath::Clamp(FMath::TruncToInt(Alpha * 255 * 1.5f), 0, 255));
			Canvas->DrawIcon(HitNotifyIcon[i],
				StartX + (HitNotifyIcon[i].U - HitNotifyTexture->GetSizeX() / 2 + Offsets[i].X) * UIscale,
				StartY + (HitNotifyIcon[i].V - HitNotifyTexture->GetSizeY() / 2 + Offsets[i].Y) * UIscale,
				UIscale);
		}
	}
}

