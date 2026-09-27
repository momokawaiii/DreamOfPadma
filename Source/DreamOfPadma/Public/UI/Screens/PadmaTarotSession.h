#pragma once
#include "CoreMinimal.h"

struct FPadmaTarotCard
{
 FName Art;
 FString Name, Character, Upright, Reversed;
};
struct FPadmaTarotLayout
{
 FVector2D Position,Size,Label;
 FVector Rotation;
};
struct FPadmaTarotSlot { FString Name,English; };
struct FPadmaTarotSpread
{
 FString Id,Name,English;
 FName TitleArt;
 TArray<FPadmaTarotSlot> Slots;
 TArray<FPadmaTarotLayout> Selection,Result;
};
struct FPadmaTarotDraw { int32 Card=0;bool bReversed=false; };

/** Local presentation session. All eight original activity cards are owned; no run/reward writes. */
class DREAMOFPADMA_API FPadmaTarotSession
{
public:
 bool Load(const FString& SceneJson,FString& Error);
 bool Select(int32 Index);
 bool Draw();
 bool CanDivine()const;
 bool NeedsConfirmation()const{return Selected==5&&!Results[5].IsEmpty();}
 bool IsRevealed(int32 Index)const{return Index>=0&&Index<6&&Revealed[Index];}
 bool IsDailyUnlocked()const;
 void SetNormalRevealed(bool Value);
private:
 bool Revealed[6]={false,false,false,false,false,false};
 void GenerateCards();
public:
 const TArray<FPadmaTarotDraw>& CurrentCards()const{return Results[Selected];}
 const FPadmaTarotSpread& CurrentSpread()const{return Spreads[Selected];}
 bool IsReady()const{return Cards.Num()==8&&Spreads.Num()==6;}
 int32 Selected=5;
 TArray<FPadmaTarotCard> Cards;
 TArray<FPadmaTarotSpread> Spreads;
private:
 TArray<FPadmaTarotDraw> Results[6];
 int32 DrawSerial=0;
};
