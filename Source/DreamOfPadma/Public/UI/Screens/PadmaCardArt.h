#pragma once
#include "CoreMinimal.h"
inline int32 PadmaCardArtIndex(FName Id)
{
 static const FName Ids[]={TEXT("dawn"),TEXT("earth"),TEXT("banquet"),TEXT("cherry"),TEXT("reed"),TEXT("cleopatra"),TEXT("vitruvian"),TEXT("dancer"),TEXT("door")};
 for(int32 I=0;I<UE_ARRAY_COUNT(Ids);++I)if(Ids[I]==Id)return I;return INDEX_NONE;
}
