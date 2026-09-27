#pragma once
#include "CoreMinimal.h"
class UMaterial;
namespace PadmaClothGraph
{
// Explicit one-time graph migration; preserves parameters and fails closed on unsupported graph shapes.
bool Encapsulate(UMaterial *Material, FString &Message);
} // namespace PadmaClothGraph
