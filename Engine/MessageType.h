#pragma once
enum class EMessageType
{
	ColliderAdded,
	ColliderRemoved,
	EnemyDied,
	MainMenu,
	Credits,
	Options,
	LoadLevel,
	StartGame,
	LevelSelect,
	Quit,
	AbilityOneCooldown,
	AbilityTwoCooldown,
	AbilityThreeCooldown,
	Resume,
	PlayerHealthChanged,
	PlayerResourceChanged,
	PlayerDied,
	BossFightStart,
	BossPhase2,
	BossPhase3,
	BossDied,
	BossHealthChanged,
	EnemyHealthChanged,
	PlayerTakeDamage,
	LoadDialogue,
	IntroStarted,
	IntroEnded,
	PlayAmbienceCastle,
	PlayAmbienceCave1,
	PlayAmbienceCave2,
	PlayAmbienceDungeon,
	PlayAmbienceSwamp1,
	PlayAmbienceSwamp2,
	UIButtonPress,
	PlayerExperienceChanged,
	PlayVoiceLine,
	StopDialogue,
	FadeInComplete,
	FadeOutComplete,
	AttackHits,
	DemonIdle1, 
	DemonIdle2, 
	LightAttack, 
	HeavyAttack, 
	HitDestructible,
	BossMeleeAttack, 
	HealingAura, 
	ShieldSpell,
	Count
};