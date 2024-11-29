// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingSystem/Colorbound_TSTask_AOE.h"
#include "Core/ColorboundCharacterBase.h"

FVector UColorbound_TSTask_AOE::GetSourceOffset_Implementation(const FTargetingRequestHandle& TargetingHandle) const
{
	if (bUseRelativeOffset)
	{
		const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle);
		if (const AColorboundCharacterBase* SourceActor = SourceContext ? Cast<AColorboundCharacterBase>(SourceContext->SourceActor) : nullptr)
		{
			return	SourceActor->GetForwardVector() * DefaultSourceOffset.X +
				SourceActor->GetRightVector() * DefaultSourceOffset.Y +
				SourceActor->GetActorUpVector() * DefaultSourceOffset.Z;
		}
	}

	return DefaultSourceOffset;
}
