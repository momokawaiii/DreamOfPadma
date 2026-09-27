#include "UI/Screens/PadmaTarotSession.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FPadmaTarotSession::Load(const FString& Json,FString& Error)
{
 Cards.Reset();Spreads.Reset();for(auto& R:Results)R.Reset();Selected=5;DrawSerial=0;
 TSharedPtr<FJsonObject> Root;
 if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Root)||!Root.IsValid()) {Error=TEXT("Invalid source scene JSON");return false;}
 const TSharedPtr<FJsonObject>* Catalog=nullptr;
 if(!Root->TryGetObjectField(TEXT("catalog"),Catalog)){Error=TEXT("Missing original activity catalog; run AuthorDivinationOriginals.py");return false;}
 const TArray<TSharedPtr<FJsonValue>> *CardRows=nullptr,*SpreadRows=nullptr;
 if(!(*Catalog)->TryGetArrayField(TEXT("cards"),CardRows)||!(*Catalog)->TryGetArrayField(TEXT("spreads"),SpreadRows)||CardRows->Num()!=8||SpreadRows->Num()!=6)
 {Error=TEXT("Expected eight activity cards and six spreads");return false;}
 for(const auto& V:*CardRows)
 {
  auto J=V->AsObject();FPadmaTarotCard C;
  C.Art=FName(*J->GetStringField(TEXT("id")));C.Name=J->GetStringField(TEXT("name"));C.Character=J->GetStringField(TEXT("operator"));
  C.Upright=J->GetStringField(TEXT("upright"));C.Reversed=J->GetStringField(TEXT("reversed"));Cards.Add(C);
 }
 auto Layouts=[](const TArray<TSharedPtr<FJsonValue>>& Rows)
 {
  TArray<FPadmaTarotLayout> Result;
  for(const auto& V:Rows)
  {
   auto J=V->AsObject();auto Vec=[&](const TCHAR* Key){const auto& A=J->GetArrayField(Key);return FVector2D(A[0]->AsNumber(),A[1]->AsNumber());};
   FPadmaTarotLayout D;D.Position=Vec(TEXT("position"));D.Size=Vec(TEXT("size"));D.Label=Vec(TEXT("label"));const auto& R=J->GetArrayField(TEXT("rotation"));D.Rotation=FVector(R[0]->AsNumber(),R[1]->AsNumber(),R[2]->AsNumber());Result.Add(D);
  }
  return Result;
 };
 const int32 Counts[]={3,4,4,4,5,2};
 for(const auto& V:*SpreadRows)
 {
  auto J=V->AsObject();FPadmaTarotSpread S;S.Id=J->GetStringField(TEXT("id"));S.Name=J->GetStringField(TEXT("name"));S.English=J->GetStringField(TEXT("english"));S.TitleArt=FName(*J->GetStringField(TEXT("titleArt")));
  for(const auto& Slot:J->GetArrayField(TEXT("slots"))){auto D=Slot->AsObject();S.Slots.Add({D->GetStringField(TEXT("name")),D->GetStringField(TEXT("english"))});}
  S.Selection=Layouts(J->GetArrayField(TEXT("selection")));S.Result=Layouts(J->GetArrayField(TEXT("result")));
  if(S.Slots.Num()!=Counts[Spreads.Num()]||S.Selection.Num()!=S.Slots.Num()||S.Result.Num()!=S.Slots.Num())
  {Error=TEXT("Original spread slots/layouts disagree: ")+S.Id;Cards.Reset();Spreads.Reset();return false;}
  Spreads.Add(MoveTemp(S));
 }
 for(int32 I=0;I<6;++I){Selected=I;GenerateCards();Revealed[I]=false;}
 // The supplied all-owned daily-reading reference shows reversed Strength and Pope.
 Results[5]={{5,true},{4,true}};SetNormalRevealed(true);Revealed[5]=true;Selected=5;return true;
}
bool FPadmaTarotSession::Select(int32 Index)
{
 if(!IsReady()||!Spreads.IsValidIndex(Index)||(Index==5&&!IsDailyUnlocked()))return false;
 Selected=Index;return true;
}
bool FPadmaTarotSession::IsDailyUnlocked()const
{for(int32 I=0;I<5;++I)if(!Revealed[I])return false;return IsReady();}
void FPadmaTarotSession::SetNormalRevealed(bool Value)
{for(int32 I=0;I<5;++I)Revealed[I]=Value;if(!Value){Revealed[5]=false;Results[5].Reset();Selected=0;}}
bool FPadmaTarotSession::CanDivine()const
{return IsReady()&&(Selected==5?IsDailyUnlocked():!Revealed[Selected]);}
bool FPadmaTarotSession::Draw()
{
 if(!CanDivine())return false;
 if(Selected==5)GenerateCards();
 Revealed[Selected]=true;return true;
}
void FPadmaTarotSession::GenerateCards()
{
 FRandomStream Random(0x54A11+Selected*97+(++DrawSerial)*7919);
 TArray<int32> Pool;for(int32 I=0;I<Cards.Num();++I)Pool.Add(I);
 for(int32 I=Pool.Num()-1;I>0;--I)Pool.Swap(I,Random.RandRange(0,I));
 auto& R=Results[Selected];R.Reset();
 for(int32 I=0;I<Spreads[Selected].Slots.Num();++I)R.Add({Pool[I],Random.RandRange(0,1)!=0});
}
