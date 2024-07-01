// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyDatabaseManager.h"
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

	bool GetIsLogin() const { return this->IsLogin; }

	bool GetIsLogOut() const { return this->IsLogOut; }

	UFUNCTION(Client, Reliable)
	void ClientInitializeUI();

	void VisibleStartMenu();
	void VisibleSignup();
	void VisibleLogin();
	void VisibleOption();
	void VisiblePlayerUI();

	bool CheckIdDuplicate(const FString& MemberID);
	bool CheckNicknameDuplicate(const FString& MemberNickname);
	bool SignupPlayer(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname);
	bool LoginPlayer(const FString& MemberID, const FString& MemberPW);
	bool CheckMultipleLogin(const FString& MemberID);
	bool LogoutPlayer(const FString& MemberID);

	void SetPlayerID(const FString& _PlayerID);
	FString GetPlayerID();

	UFUNCTION(Server, Reliable, WithValidation)
	void InitializePlayerAfterLogin(TSubclassOf<APawn> PawnClass);

	UFUNCTION(Client, Reliable)
	void ClientShowLoginFailedMessage();

	void InitializePlayerAfterLogin_Implementation(TSubclassOf<APawn> PawnClass);
	bool InitializePlayerAfterLogin_Validate(TSubclassOf<APawn> PawnClass);
	void ClientShowLoginFailedMessage_Implementation();

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

	UPROPERTY()
	UMyDatabaseManager* Database;

	UPROPERTY()
	FString PlayerID;

	UPROPERTY()
	bool IsLogin;

	UPROPERTY()
	bool IsLogOut;

	UFUNCTION()
	void InitializeUI();

	UFUNCTION()
	void InitializeDatabase();
};
