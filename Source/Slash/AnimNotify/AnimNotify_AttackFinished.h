// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackFinished.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UAnimNotify_AttackFinished : public UAnimNotify
{
	GENERATED_BODY()

	
public:
	[[deprecated]] virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
