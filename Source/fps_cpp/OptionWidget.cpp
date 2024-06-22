// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionWidget.h"
#include "Sound/SoundClass.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "SaveOptions.h"

void UOptionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnMasterVolumeChanged);
    }

    if (MusicVolumeSlider)
    {
        MusicVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnMusicVolumeChanged);
    }

    if (EffectVolumeSlider)
    {
        EffectVolumeSlider->OnValueChanged.AddDynamic(this, &UOptionWidget::OnEffectsVolumeChanged);
    }

    if (TextureComboBox)
    {
        TextureComboBox->AddOption(TEXT("Low"));
        TextureComboBox->AddOption(TEXT("Medium"));
        TextureComboBox->AddOption(TEXT("High"));
        TextureComboBox->AddOption(TEXT("Epic"));
        TextureComboBox->AddOption(TEXT("Cinematic"));

        // 기본값 설정 (선택되지 않은 상태로 두려면 이 줄을 생략)
        TextureComboBox->SetSelectedOption(TEXT("High"));

        TextureComboBox->OnSelectionChanged.AddDynamic(this, &UOptionWidget::TextureQualityChanged);
    }

    if (ApplyButton)
    {
        ApplyButton->OnClicked.AddDynamic(this, &UOptionWidget::ApplyChangedOptions);
    }

    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UOptionWidget::CancelChangedOptions);
    }

    LoadOptions();

    PlayerController = Cast<Afps_cppPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void UOptionWidget::OnMasterVolumeChanged(float Value)
{
    AdjustVolume("Master", Value);
}

void UOptionWidget::OnMusicVolumeChanged(float Value)
{
    AdjustVolume("Music", Value);
}

void UOptionWidget::OnEffectsVolumeChanged(float Value)
{

}

void UOptionWidget::AdjustVolume(FName SoundClassName, float Volume)
{
    if (USoundClass* SoundClass = FindObject<USoundClass>(nullptr, *SoundClassName.ToString()))
    {
        SoundClass->Properties.Volume = Volume;
    }
}

void UOptionWidget::TextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    int32 QualityLevel = 2;

    if (SelectedItem == "Low")
    {
        QualityLevel = 0;
    }
    else if (SelectedItem == "Medium")
    {
        QualityLevel = 1;
    }
    else if (SelectedItem == "High")
    {
        QualityLevel = 2;
    }
    else if (SelectedItem == "Epic")
    {
        QualityLevel = 3;
    }
    else if (SelectedItem == "Cinematic")
    {
        QualityLevel = 4;
    }

    // Set the texture quality
    Scalability::FQualityLevels QualityLevels = Scalability::GetQualityLevels();
    QualityLevels.TextureQuality = QualityLevel;
    Scalability::SetQualityLevels(QualityLevels);
    Scalability::SaveState(SelectedItem);
}

void UOptionWidget::ApplyChangedOptions()
{
    USaveOptions* SaveOptionsInstance = Cast<USaveOptions>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));

    if (!SaveOptionsInstance)
    {
        SaveOptionsInstance = Cast<USaveOptions>(UGameplayStatics::CreateSaveGameObject(USaveOptions::StaticClass()));
    }

    if (SaveOptionsInstance)
    {
        SaveOptionsInstance->SetMasterVolume(MasterVolumeSlider->GetValue());
        SaveOptionsInstance->SetEffectVolume(EffectVolumeSlider->GetValue());
        SaveOptionsInstance->SetMusicVolume(MusicVolumeSlider->GetValue());
        SaveOptionsInstance->SetTextureQuality(TextureComboBox->GetSelectedOption());

        UGameplayStatics::SaveGameToSlot(SaveOptionsInstance, TEXT("SettingsSlot"), 0);

    }
}

void UOptionWidget::LoadOptions()
{
    USaveOptions* LoadOptionsInstance = Cast<USaveOptions>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));

    if (LoadOptionsInstance)
    {
        MasterVolumeSlider->SetValue(LoadOptionsInstance->GetMasterVolume());
        EffectVolumeSlider->SetValue(LoadOptionsInstance->GetEffectVolume());
        MusicVolumeSlider->SetValue(LoadOptionsInstance->GetMusicVolume());
        TextureComboBox->SetSelectedOption(LoadOptionsInstance->GetTextureQuality());

        OnMasterVolumeChanged(LoadOptionsInstance->GetMasterVolume());
        OnEffectsVolumeChanged(LoadOptionsInstance->GetEffectVolume());
        OnMusicVolumeChanged(LoadOptionsInstance->GetMusicVolume());
        TextureQualityChanged(LoadOptionsInstance->GetTextureQuality(), ESelectInfo::Direct);
    }

    else
    {
        MasterVolumeSlider->SetValue(0.7f);
        EffectVolumeSlider->SetValue(1.0f);
        MusicVolumeSlider->SetValue(1.0f);
        TextureComboBox->SetSelectedOption(TEXT("High"));

        OnMasterVolumeChanged(0.7f);
        OnEffectsVolumeChanged(1.0f);
        OnMusicVolumeChanged(1.0f);
        TextureQualityChanged(TEXT("High"), ESelectInfo::Direct);

        ApplyChangedOptions();
    }
}

void UOptionWidget::CancelChangedOptions()
{
    LoadOptions();
}