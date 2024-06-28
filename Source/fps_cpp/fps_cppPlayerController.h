// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "fps_cppPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API Afps_cppPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	Afps_cppPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable)
	void ClientInitializeUI();

	void VisibleSignup();
	void VisibleLogin();
	void VisibleOption();
	void VisiblePlayerUI();

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> StartWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoginWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SignupWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> OptionWidgetClass;

private:
	UPROPERTY(Transient)
	UUserWidget* PlayerUIWidgetInstance;

	UPROPERTY(Transient)
	UUserWidget* SignupWidgetInstance;

	UPROPERTY(Transient)
	UUserWidget* LoginWidgetInstance;

	UPROPERTY(Transient)
	UUserWidget* StartWidgetInstance;

	UPROPERTY(Transient)
	UUserWidget* OptionWidgetInstance;

	UFUNCTION()
	void InitializeUI();
};
