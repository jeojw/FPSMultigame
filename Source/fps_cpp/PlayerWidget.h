// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "fps_cppCharacter.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};
