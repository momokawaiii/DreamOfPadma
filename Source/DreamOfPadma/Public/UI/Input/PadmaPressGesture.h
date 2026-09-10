#pragma once
#include "CoreMinimal.h"

/** Pointer movement cancels inspection even on a non-draggable card. */
struct FPadmaPressGesture
{
 enum class ERelease { None, Click, Drop };
 FVector2D Origin=FVector2D::ZeroVector;
 double Started=0;
 bool bHeld=false,bMoved=false,bInspected=false;
 void Begin(FVector2D Position,double Now){Origin=Position;Started=Now;bHeld=true;bMoved=false;bInspected=false;}
 void Move(FVector2D Position){if(bHeld&&(Position-Origin).Size()>12)bMoved=true;}
 bool Inspect(double Now,bool bInspectable)
 {
  if(!bHeld||bMoved||bInspected||!bInspectable||Now-Started<.5)return false;
  bInspected=true;return true;
 }
 ERelease Release(bool bDraggable)
 {
  const ERelease Result=!bHeld||bInspected?ERelease::None:bMoved?(bDraggable?ERelease::Drop:ERelease::None):ERelease::Click;
  bHeld=false;return Result;
 }
 void Cancel(){bHeld=false;}
};
