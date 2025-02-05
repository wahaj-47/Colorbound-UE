#include "ColorboundGameplayTags.h"

namespace ColorboundGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StatusTag_Hit, "Status.Hit", "On actor hit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StatusTag_Knockback, "Status.Knockback", "On actor knocked back.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Combo_WindowOpen, "Event.Combo.WindowOpen", "On combo window open.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Combo_WindowClosed, "Event.Combo.WindowClosed", "On combo window closed.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityTag_HitReact, "Ability.HitReact", "Hit React Ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityTag_Knockback, "Ability.Knockback", "Knockback Ability.");

}