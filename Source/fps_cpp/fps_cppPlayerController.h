// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyDatabaseManager.h"
#include "fps_cppPlayerState.h"
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

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	bool GetIsLogin() const { return this->IsLogin; }

	bool GetIsLogOut() const { return this->IsLogOut; }

	UFUNCTION(Client, Reliable)
	void ClientInitializeUI();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInitializeUI();

	void VisibleStartMenu();
	void VisibleSignup();
	void VisibleLogin();
	void VisibleOption();
	void VisiblePlayerUI();
	void VisibleLobbyUI();

	bool CheckIdDuplicate(const FString& MemberID);
	bool CheckNicknameDuplicate(const FString& MemberNickname);
	bool SignupPlayer(const FString& MemberID, const FString& MemberPW, const FString& MemberNickname);
	bool LoginPlayer(const FString& MemberID, const FString& MemberPW);
	UTexture2D* LoadTextureFromFile(const FString& FilePath);
	bool CheckMultipleLogin(const FString& MemberID);
	bool LogoutPlayer();
	
	/*void SetProfileImage();*/
	/*bool CreateRoom();
	bool EntraceRoom();*/

	UFUNCTION(Server, Reliable, WithValidation)
	void InitializePlayerAfterLogin(TSubclassOf<APawn> PawnClass);

	UFUNCTION(Client, Reliable)
	void ClientShowLoginFailedMessage();

	void InitializePlayerAfterLogin_Implementation(TSubclassOf<APawn> PawnClass);
	bool InitializePlayerAfterLogin_Validate(TSubclassOf<APawn> PawnClass);
	void ClientShowLoginFailedMessage_Implementation();

	void ClientInitializeUI_Implementation();
	void ServerInitializeUI_Implementation();
	bool ServerInitializeUI_Validate();

	UFUNCTION()
	Afps_cppPlayerState* GetFPSPlayerState() const { return Cast<Afps_cppPlayerState>(PlayerState); }

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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

private:
	UPROPERTY()
	UUserWidget* PlayerUIWidgetInstance;

	UPROPERTY()
	UUserWidget* SignupWidgetInstance;

	UPROPERTY()
	UUserWidget* LoginWidgetInstance;

	UPROPERTY()
	UUserWidget* StartWidgetInstance;

	UPROPERTY()
	UUserWidget* OptionWidgetInstance;

	UPROPERTY()
	UUserWidget* LobbyWidgetInstance;

	UPROPERTY()
	UMyDatabaseManager* Database;

	UPROPERTY()
	Afps_cppPlayerState* FPSPlayerState;

	UPROPERTY()
	bool IsLogin;

	UPROPERTY()
	bool IsLogOut;

	UFUNCTION()
	void InitializeUI();

	UFUNCTION()
	void InitializeDatabase();

	UFUNCTION()
	void InitializePlayerState();
};
