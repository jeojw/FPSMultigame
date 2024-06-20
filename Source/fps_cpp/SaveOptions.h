// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveOptions.generated.h"

UCLASS()
class FPS_CPP_API USaveOptions : public USaveGame
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	float MasterVolumeValue;

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	float EffectVolumeValue;

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	float MusicVolumeValue;

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString TextureQualityValue;

public:
	USaveOptions();

	float GetMasterVolume() const { return MasterVolumeValue; }
	void SetMasterVolume(float _Volume) { MasterVolumeValue = _Volume; }

	float GetEffectVolume() const { return EffectVolumeValue; }
	void SetEffectVolume(float _Volume) { EffectVolumeValue = _Volume; }

	float GetMusicVolume() const { return MusicVolumeValue; }
	void SetMusicVolume(float _Volume) { MusicVolumeValue = _Volume; }

	FString GetTextureQuality() const { return TextureQualityValue; }
	void SetTextureQuality(FString _Quality) { TextureQualityValue = _Quality; }
};
