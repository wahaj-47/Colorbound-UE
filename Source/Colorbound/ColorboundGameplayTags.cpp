#include "ColorboundGameplayTags.h"

namespace ColorboundGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StatusTag_Hit, "Status.Hit", "On actor hit.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_ComboWindowOpen, "Event.Combo.WindowOpen", "On combo window open.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_ComboWindowClosed, "Event.Combo.WindowClosed", "On combo window closed.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityTag_HitReact, "Ability.Type.Passive.HitReact", "Hit react ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityTag_Knockback, "Ability.Type.Passive.Knockback", "Knockback ability.");

}