// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "OptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UOptionWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* OptionCanvas;

	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	USlider* MasterVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* EffectVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* MusicVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* TextureComboBox;

	UPROPERTY(meta = (BindWidget))
	UButton* ApplyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancleButton;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnMusicVolumeChanged(float Value);

	UFUNCTION()
	void OnEffectsVolumeChanged(float Value);

private:
	void AdjustVolume(FName SoundClassName, float Volume);
	void TextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void LoadOptions();
	void ApplyChangedOptions();
	void CancelChangedOptions();
};
