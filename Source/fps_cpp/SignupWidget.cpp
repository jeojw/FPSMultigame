// Fill out your copyright notice in the Description page of Project Settings.


#include "SignupWidget.h"
#include "Kismet/GameplayStatics.h"

void USignupWidget::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerController = Cast<Afps_cppPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    if (InputNewId)
    {
        InputNewId->OnTextChanged.AddDynamic(this, &USignupWidget::OnIdTextChanged);
    }

    if (InputNewPw)
    {
        InputNewPw->SetKeyboardType(EVirtualKeyboardType::Password);
        InputNewPw->SetIsPassword(true);
        InputNewPw->OnTextChanged.AddDynamic(this, &USignupWidget::OnPasswordTextChanged);
    }

    if (InputNewNickname)
    {
        InputNewNickname->OnTextChanged.AddDynamic(this, &USignupWidget::OnNicknameTextChanged);
    }

    if (CheckIdMessage)
    {
        CheckIdMessage->SetText(FText::FromString(TEXT("Please input your new ID")));
    }

    if (CheckPwMessage)
    {
        CheckPwMessage->SetText(FText::FromString(TEXT("Please input your new Password")));
    }

    if (CheckNicknameMessage)
    {
        CheckNicknameMessage->SetText(FText::FromString(TEXT("Plese input your new Nickname")));
    }

    if (CheckInfoMessage)
    {
        CheckInfoMessage->SetText(FText::FromString(TEXT("Please check every duplicates")));
    }

    if (CheckId)
    {
        CheckId->OnClicked.AddDynamic(this, &USignupWidget::CheckIdDuplicate);
    }

    if (CheckNickname)
    {
        CheckNickname->OnClicked.AddDynamic(this, &USignupWidget::CheckNicknameDuplicate);
    }

    if (SignupButton)
    {
        SignupButton->OnClicked.AddDynamic(this, &USignupWidget::Signup);
    }

    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &USignupWidget::CancelSignup);
    }

    isCheckId = false;
    isCheckNickname = false;
    isCheckAll = false;
}

void USignupWidget::OnIdTextChanged(const FText& Text)
{
    IdString = Text.ToString();
}

void USignupWidget::OnPasswordTextChanged(const FText& Text)
{
    PasswordString = Text.ToString();
}

void USignupWidget::OnNicknameTextChanged(const FText& Text)
{
    NicknameString = Text.ToString();
}

void USignupWidget::CheckIdDuplicate()
{
    if (PlayerController && InputNewId)
    {
        isCheckId = !PlayerController->CheckIdDuplicate(InputNewId->GetText().ToString());
        UpdateValidationMessages();
    }
}

void USignupWidget::CheckNicknameDuplicate()
{
    if (PlayerController && InputNewNickname)
    {
        isCheckNickname = !PlayerController->CheckNicknameDuplicate(InputNewNickname->GetText().ToString());
        UpdateValidationMessages();
    }
}

void USignupWidget::Signup()
{
    if (PlayerController && InputNewId && InputNewPw && InputNewNickname && isCheckAll)
    {
        PlayerController->SignupPlayer(InputNewId->GetText().ToString(), InputNewPw->GetText().ToString(), InputNewNickname->GetText().ToString());
        PlayerController->VisibleLogin();
    }
}

void USignupWidget::CancelSignup()
{
    if (PlayerController)
    {
        PlayerController->VisibleStartMenu();
    }
}

void USignupWidget::UpdateValidationMessages()
{
    if (InputNewId->GetText().ToString().IsEmpty())
    {
        CheckIdMessage->SetText(FText::FromString(TEXT("Please input your new ID")));
    }
    else
    {
        if (!isCheckId)
        {
            CheckIdMessage->SetText(FText::FromString(TEXT("This ID is duplicated!")));
        }
        else
        {
            CheckIdMessage->SetText(FText::FromString(TEXT("This ID is valid")));
        }
    }

    if (InputNewPw->GetText().ToString().IsEmpty())
    {
        CheckPwMessage->SetText(FText::FromString(TEXT("Please input your new Password")));
    }

    if (InputNewNickname->GetText().ToString().IsEmpty())
    {
        CheckNicknameMessage->SetText(FText::FromString(TEXT("Please input your new Nickname")));
    }
    else
    {
        if (!isCheckNickname)
        {
            CheckNicknameMessage->SetText(FText::FromString(TEXT("This Nickname is duplicated!")));
        }
        else
        {
            CheckNicknameMessage->SetText(FText::FromString(TEXT("This Nickname is valid")));
        }
    }

    isCheckAll = !IdString.IsEmpty() && !PasswordString.IsEmpty() && !NicknameString.IsEmpty() && isCheckId && isCheckNickname;

    if (!isCheckAll)
    {
        CheckInfoMessage->SetText(FText::FromString(TEXT("Please check duplicates")));
    }
    else
    {
        CheckInfoMessage->SetText(FText::FromString(TEXT("All elements are valid!")));
    }
}