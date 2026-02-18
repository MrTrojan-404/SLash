#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_SwappingWeapons UMETA(DisplayName = "Swapping Weapons"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EPlayerWeaponState : uint8
{
	EPWS_HasNoWeapon UMETA(DisplayName = "HasNoWeapon"),
	EPWS_HasOneWeapon UMETA(DisplayName = "HasOneWeapon"),
	EPWS_HasTwoWeapons UMETA(DisplayName = "HasTwoWeapons"),
	
	EPWS_MAX UMETA(DisplayName = "DefaultMAX")

};

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequippied"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	
	ECS_MAX UMETA(DisplayName = "DefaultMAX")

};