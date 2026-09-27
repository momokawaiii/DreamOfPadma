#include "UI/Screens/PadmaACTTrainingAnalyticsWidget.h"

#include "Gameplay/ACT/Authoring/PadmaACTAuthoring.h"
#include "Gameplay/ACT/Runtime/PadmaACTMelee.h"
#include "Gameplay/ACT/Runtime/PadmaACTTrainingAnalytics.h"
#include "Gameplay/Combat/PadmaCombatComponent.h"
#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "Brushes/SlateColorBrush.h"
#include "Rendering/DrawElements.h"

namespace
{
	const FLinearColor PanelBackground(0.006f, 0.018f, 0.026f, 0.97f);
	const FLinearColor CardBackground(0.012f, 0.047f, 0.060f, 0.88f);
	const FLinearColor Cyan(0.18f, 0.77f, 0.84f, 1.f);
	const FLinearColor White(0.89f, 0.95f, 0.96f, 1.f);
	const FLinearColor Muted(0.43f, 0.62f, 0.66f, 1.f);
	const FLinearColor Orange(1.f, 0.48f, 0.08f, 1.f);
	const FLinearColor Yellow(1.f, 0.82f, 0.18f, 1.f);
	const FLinearColor Red(1.f, 0.25f, 0.16f, 1.f);

	const FSlateBrush* WhiteBrush()
	{
		return FCoreStyle::Get().GetBrush("WhiteBrush");
	}

	TSharedRef<SWidget> Text(const FString& Value, int32 Size = 14, FLinearColor Color = White, bool bWrap = false)
	{
		return SNew(STextBlock)
			.Text(FText::FromString(Value))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", Size))
			.ColorAndOpacity(Color)
			.AutoWrapText(bWrap);
	}

	TSharedRef<SWidget> LiveText(TFunction<FString()> Value, int32 Size = 12, FLinearColor Color = White)
	{
		return SNew(STextBlock)
			.Text_Lambda([Value = MoveTemp(Value)]() { return FText::FromString(Value()); })
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", Size))
			.ColorAndOpacity(Color).AutoWrapText(true);
	}

	TSharedRef<SWidget> Line()
	{
		return SNew(SBox).HeightOverride(1.f)
			[SNew(SBorder).BorderImage(WhiteBrush()).BorderBackgroundColor(FLinearColor(0.13f, 0.42f, 0.46f, 0.75f))];
	}


    class STrainingModule : public SBorder
    {
    public:
        void Construct(const SBorder::FArguments& Args) { SBorder::Construct(Args);SetCanTick(true); }
        virtual void Tick(const FGeometry& Geometry, double Now, float Delta) override
        {
            SBorder::Tick(Geometry,Now,Delta);
            if (LastTick<0 || Now-LastTick>.25) Age=0;
            LastTick=Now;Age=FMath::Min(.18f,Age+Delta);
            const float T=Age/.18f;
            const float Ease=1.f-FMath::Pow(1.f-T,3.f);
            SetRenderTransformPivot(FVector2D(.5f,.5f));
            SetRenderTransform(FSlateRenderTransform(FScale2D(1.f+.012f*FMath::Sin(T*PI)),FVector2D(0,(1.f-Ease)*8.f)));
        }
    private:
        double LastTick=-1;
        float Age=0;
    };
	TSharedRef<SWidget> Frame(TSharedRef<SWidget> Content, FMargin Padding = FMargin(10), FLinearColor Background = CardBackground)
	{
		return SNew(STrainingModule)
			.Padding(1.f)
			.BorderImage(WhiteBrush())
			.BorderBackgroundColor(FLinearColor(0.11f, 0.48f, 0.54f, 0.72f))
			[SNew(SBorder).Padding(Padding).BorderImage(WhiteBrush()).BorderBackgroundColor(Background)[Content]];
	}

	FString DamageKindText(EPadmaACTDamageKind Kind)
	{
		switch (Kind)
		{
		case EPadmaACTDamageKind::Magical: return TEXT("法术");
		case EPadmaACTDamageKind::TrueDamage: return TEXT("真实");
		default: return TEXT("物理");
		}
	}

	FLinearColor DamageKindColor(EPadmaACTDamageKind Kind)
	{
		return Kind == EPadmaACTDamageKind::TrueDamage ? Yellow : Kind == EPadmaACTDamageKind::Magical ? Cyan : Orange;
	}


	FString ChineseName(const FString& Value)
	{
		static const TMap<FString,FString> Names = {
			{TEXT("GuiQiongYu"),TEXT("归穹宇")},{TEXT("JianTianHe"),TEXT("见天河")},{TEXT("LieFengShuang"),TEXT("冽风霜")},
			{TEXT("Attack01"),TEXT("破飞霞·一段")},{TEXT("Attack02"),TEXT("破飞霞·二段")},{TEXT("Attack03"),TEXT("破飞霞·三段")},{TEXT("Attack04"),TEXT("破飞霞·四段")},{TEXT("Attack05"),TEXT("破飞霞·五段")},
			{TEXT("PerfectDodge"),TEXT("完美闪避")},{TEXT("Dodge"),TEXT("闪避")},{TEXT("Execution"),TEXT("处决")},{TEXT("Plunge"),TEXT("下落攻击")},{TEXT("Jump"),TEXT("跳跃")},
			{TEXT("Attack"),TEXT("普通攻击")},{TEXT("Skill"),TEXT("技能")},{TEXT("Legacy"),TEXT("基础动作")},
			{TEXT("Primary"),TEXT("鼠标左键")},{TEXT("SkillE"),TEXT("E")},{TEXT("SkillQ"),TEXT("Q")},{TEXT("SkillR"),TEXT("R")},
			{TEXT("Combo2"),TEXT("连击二段")},{TEXT("Combo3"),TEXT("连击三段")},{TEXT("Combo4"),TEXT("连击四段")},{TEXT("Combo5"),TEXT("连击五段")},{TEXT("PerfectInternal"),TEXT("完美闪避触发")},
			{TEXT("Health"),TEXT("生命")},{TEXT("Shield"),TEXT("护盾")},{TEXT("None"),TEXT("无")},
			{TEXT("Chen Qianyu"),TEXT("陈千语")},
			{TEXT("blade-dummy"),TEXT("高防御 / 低血量")},{TEXT("blade-dummy-2"),TEXT("低防御 / 高血量")},{TEXT("blade-dummy-3"),TEXT("高法抗 / 高血量")}
		};
		FString Key=Value; Key.RemoveFromStart(TEXT("Chen."));
		if (const FString* Found=Names.Find(Key)) return *Found;
		return Value;
	}

	FString UnitLabel(UPadmaCombatComponent* Battle, FName Id)
	{
		if (ChineseName(Id.ToString()) != Id.ToString()) return ChineseName(Id.ToString());
		if (Battle)
		{
			if (const auto* Unit = Battle->GetUnit(Id)) return ChineseName(Unit->Spec.DisplayName.ToString());
		}
		return Id.ToString();
	}

	template <typename T>
	FString SoftAssetName(const TSoftObjectPtr<T>& Asset)
	{
		return Asset.IsNull() ? TEXT("未设置") : Asset.ToSoftObjectPath().GetAssetName();
	}

	FString EnumDisplayName(const UEnum* Enum, int64 Value)
	{
		return Enum ? ChineseName(Enum->GetNameStringByValue(Value)) : TEXT("未加载");
	}
}

class SPadmaACTTrainingAnalyticsPanel : public SCompoundWidget
{
#if WITH_DEV_AUTOMATION_TESTS
	friend class FPadmaACTTrainingPanelTest;
#endif
private:
	enum class EPage : uint8
	{
		Overview,
		Character,
		Details,
		Appearance
	};

public:
	SLATE_BEGIN_ARGS(SPadmaACTTrainingAnalyticsPanel) {}
		SLATE_ARGUMENT(UPadmaACTTrainingAnalyticsComponent*, Analytics)
		SLATE_ARGUMENT(UPadmaCombatComponent*, Battle)
		SLATE_ARGUMENT(bool, WorldPresentation)
		SLATE_EVENT(FSimpleDelegate, Close)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
		Analytics = Args._Analytics;
		Battle = Args._Battle;
		Close = Args._Close;
		bWorldPresentation=Args._WorldPresentation;
		SetCanTick(true);
		RefreshTargetOptions();

		SAssignNew(Layout, SOverlay);
		if (!bWorldPresentation) Layout->AddSlot()
			[SAssignNew(Blur, SBackgroundBlur)
				.Visibility(EVisibility::HitTestInvisible)
				.BlurStrength(0.f)
				[SNew(SBorder).BorderImage(WhiteBrush()).BorderBackgroundColor(FLinearColor(0.005f, 0.025f, 0.032f, 0.40f))]];
		Layout->AddSlot()
			[SNew(SBorder).Visibility(EVisibility::HitTestInvisible).BorderImage(WhiteBrush()).BorderBackgroundColor(FLinearColor(0.002f, 0.012f, 0.017f, 0.44f))];

		SAssignNew(ContentBox, SBox)[BuildBody()];
		SAssignNew(PanelFrame, SBorder)
			.BorderImage(WhiteBrush()).BorderBackgroundColor(Cyan)
			[SNew(SBorder).Padding(1.f).BorderImage(WhiteBrush()).BorderBackgroundColor(PanelBackground)[ContentBox.ToSharedRef()]];
		SAssignNew(PanelSlot, SBox).WidthOverride(1120.f).HeightOverride(720.f)[PanelFrame.ToSharedRef()];

		// World presentation is rendered and hit-tested by the lab WidgetComponent.
		Layout->AddSlot().HAlign(bWorldPresentation ? HAlign_Center : HAlign_Right).VAlign(VAlign_Center).Padding(FMargin(0.f, 28.f, 42.f, 28.f))[PanelSlot.ToSharedRef()];
		ChildSlot[Layout.ToSharedRef()];
		SetVisibility(EVisibility::Hidden);
		SetOpen(false);
	}

	void SetOpen(bool bInOpen)
	{
		if (bOpen == bInOpen) return;
		bOpen = bInOpen;
		if (bOpen)
		{
			SetVisibility(EVisibility::Visible);
			if (Analytics.IsValid() && Analytics->GetRevision() != LastRevision)
			{
				LastRevision = Analytics->GetRevision();
				RefreshContent();
			}
		}
	}

	virtual void Tick(const FGeometry& Geometry, double CurrentTime, float DeltaTime) override
	{
		SCompoundWidget::Tick(Geometry, CurrentTime, DeltaTime);
		const float Speed = 1.f / 0.18f;
		Animation = FMath::Clamp(Animation + (bOpen ? 1.f : -1.f) * DeltaTime * Speed, 0.f, 1.f);
		const float Ease = 1.f - FMath::Pow(1.f - Animation, 3.f);
		SetRenderOpacity(Animation);
		if (PanelSlot.IsValid()) PanelSlot->SetRenderTransform(FSlateRenderTransform(FVector2D(0.f, (1.f - Ease) * 72.f)));
		if (Blur.IsValid()) Blur->SetBlurStrength(Animation * 5.f);
		if (!bOpen && Animation <= 0.f) SetVisibility(EVisibility::Hidden);

		if (Analytics.IsValid() && Analytics->GetRevision() != LastRevision)
		{
			LastRevision = Analytics->GetRevision();
			RefreshContent();
		}
	}

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& Cull,
        FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool Enabled) const override
    {
        const int32 Top=SCompoundWidget::OnPaint(Args,Geometry,Cull,Elements,Layer,Style,Enabled);
        if (!bWorldPresentation || Animation<=0) return Top;
        const FVector2D Size=Geometry.GetLocalSize();
        const float Strength=Animation<1.f ? .15f : .018f;
        for (float Y=20; Y<Size.Y; Y+=18.f)
        {
            TArray<FVector2D> Points={FVector2D(20,Y),FVector2D(Size.X-20,Y)};
            FSlateDrawElement::MakeLines(Elements,Top+1,Geometry.ToPaintGeometry(),Points,
                ESlateDrawEffect::None,FLinearColor(.1f,.8f,1.f,Strength),false,1.f);
        }
        if (Animation<1.f)
        {
            const float Y=Size.Y*(1.f-Animation);
            TArray<FVector2D> Points={FVector2D(20,Y),FVector2D(Size.X-20,Y)};
            FSlateDrawElement::MakeLines(Elements,Top+1,Geometry.ToPaintGeometry(),Points,
                ESlateDrawEffect::None,FLinearColor(.2f,.9f,1.f,.35f),false,2.f);
        }
        return Top+1;
    }

private:
	TSharedRef<SWidget> BuildHeader()
	{
		TSharedRef<SHorizontalBox> Header = SNew(SHorizontalBox);
		Header->AddSlot().FillWidth(1.f).VAlign(VAlign_Center)
			[SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()[Text(TEXT("ACT 训练场"), 22, White)]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 0.f)[Text(TEXT("陈千语 · 战斗分析 · 伤害记录"), 11, Muted)]];
		Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(12.f, 0.f)
			[SNew(SBorder).Padding(FMargin(9.f, 5.f)).BorderImage(WhiteBrush()).BorderBackgroundColor(FLinearColor(0.12f, 0.52f, 0.57f, 0.28f))
				[Text(TEXT("实时数据 / 最近 10 秒平均伤害"), 11, Cyan)]];
		Header->AddSlot().AutoWidth().VAlign(VAlign_Center)
			[SNew(SButton).ContentPadding(FMargin(12.f, 5.f)).ButtonColorAndOpacity(FLinearColor(0.12f, 0.08f, 0.03f, 0.90f))
				.OnClicked_Lambda([this]() { Close.ExecuteIfBound(); return FReply::Handled(); })[Text(TEXT("关闭  [B]"), 12, Orange)]];
		return Header;
	}

	FString PageName(EPage Page) const
	{
		switch (Page)
		{
		case EPage::Character: return TEXT("角色");
		case EPage::Details: return TEXT("明细");
		case EPage::Appearance: return TEXT("外观");
		default: return TEXT("总览");
		}
	}

	TSharedRef<SWidget> BuildNavigationButton(EPage Page)
	{
		return SNew(SButton)
			.ContentPadding(FMargin(14.f, 4.f, 14.f, 2.f))
			.ButtonColorAndOpacity(FLinearColor::Transparent)
			.OnClicked_Lambda([this, Page]() { SelectPage(Page); return FReply::Handled(); })
			[SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[SNew(STextBlock)
					.Text(FText::FromString(PageName(Page)))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 13))
					.ColorAndOpacity_Lambda([this, Page]() { return FSlateColor(SelectedPage == Page ? Orange : Muted); })]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 5.f, 0.f, 0.f)
				[SNew(SBox).HeightOverride(2.f)
					[SNew(SBorder).Visibility_Lambda([this, Page]() { return SelectedPage == Page ? EVisibility::Visible : EVisibility::Hidden; })
						.BorderImage(WhiteBrush()).BorderBackgroundColor(Orange)]]];
	}

	TSharedRef<SWidget> BuildNavigation()
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()[BuildNavigationButton(EPage::Overview)]
			+ SHorizontalBox::Slot().AutoWidth()[BuildNavigationButton(EPage::Character)]
			+ SHorizontalBox::Slot().AutoWidth()[BuildNavigationButton(EPage::Details)]
			+ SHorizontalBox::Slot().AutoWidth()[BuildNavigationButton(EPage::Appearance)]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center).Padding(12.f, 0.f)
				[Text(TEXT("实时训练分析 · [B] 开关面板"), 10, Muted)];
	}

	bool RefreshTargetOptions()
	{
		TArray<FName> CurrentIds;
		if (Battle.IsValid())
		{
			for (const auto& UnitRef : Battle->GetUnits())
			{
				const APadmaCombatUnit* Unit = UnitRef.Get();
				if (Unit && !Unit->Spec.bPlayer) CurrentIds.Add(Unit->Spec.Id);
			}
		}

		bool bChanged = CurrentIds.Num() != TargetOptions.Num();
		if (!bChanged)
		{
			for (int32 Index = 0; Index < CurrentIds.Num(); ++Index)
			{
				if (!TargetOptions[Index].IsValid() || *TargetOptions[Index] != CurrentIds[Index])
				{
					bChanged = true;
					break;
				}
			}
		}
		if (!bChanged && SelectedTargetId.IsNone() && !CurrentIds.IsEmpty()) bChanged = true;
		if (!bChanged) return false;

		TargetOptions.Reset();
		for (const FName Id : CurrentIds) TargetOptions.Add(MakeShared<FName>(Id));
		bool bSelectedStillExists = false;
		for (const TSharedPtr<FName>& Option : TargetOptions)
		{
			if (Option.IsValid() && *Option == SelectedTargetId)
			{
				SelectedTargetOption = Option;
				bSelectedStillExists = true;
				break;
			}
		}
		if (!bSelectedStillExists)
		{
			SelectedTargetOption = TargetOptions.IsEmpty() ? nullptr : TargetOptions[0];
			SelectedTargetId = SelectedTargetOption.IsValid() ? *SelectedTargetOption : NAME_None;
		}
		if (TargetCombo.IsValid())
		{
			TargetCombo->RefreshOptions();
			TargetCombo->SetSelectedItem(SelectedTargetOption);
		}
		return true;
	}

	TArray<const APadmaCombatUnit*> GetTrainingTargets() const
	{
		TArray<const APadmaCombatUnit*> Result;
		if (!Battle.IsValid()) return Result;
		for (const auto& UnitRef : Battle->GetUnits())
		{
			const APadmaCombatUnit* Unit = UnitRef.Get();
			if (Unit && !Unit->Spec.bPlayer) Result.Add(Unit);
		}
		return Result;
	}

	const UPadmaACTCharacterDefinition* GetCharacterDefinition() const
	{
		if (!Battle.IsValid()) return nullptr;
		const APadmaCombatUnit* Player = Battle->GetPlayerUnit();
		return Player ? Player->Spec.Presentation.ACTDefinition.LoadSynchronous() : nullptr;
	}

	TSharedRef<SWidget> BuildMetric(const FString& Label, TFunction<FString()> Value, FLinearColor Accent) const
	{
		return Frame(SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()[Text(Label, 10, Muted)]
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 0.f)[LiveText(MoveTemp(Value), 22, Accent)], FMargin(10.f, 8.f), FLinearColor(0.008f, 0.035f, 0.045f, 0.92f));
	}

	TSharedRef<SWidget> BuildOverviewSummary()
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 5.f, 0.f)[BuildMetric(TEXT("训练木桩"), [this]() { return FString::Printf(TEXT("%02d"), GetTrainingTargets().Num()); }, Orange)]
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(5.f, 0.f)[BuildMetric(TEXT("命中次数"), [this]() { return FString::Printf(TEXT("%04d"), Analytics.IsValid() ? Analytics->GetHitCount(NAME_None) : 0); }, Cyan)]
			+ SHorizontalBox::Slot().FillWidth(1.f).Padding(5.f, 0.f, 0.f, 0.f)[BuildMetric(TEXT("效果记录"), [this]() { return FString::Printf(TEXT("%04d"), Analytics.IsValid() ? Analytics->GetGameplayEffectRecords().Num() : 0); }, Yellow)];
	}

	FString TargetMetrics(FName Id) const
	{
		const APadmaCombatUnit* Unit = Battle.IsValid() ? Battle->GetUnit(Id) : nullptr;
		if (!Unit) return TEXT("目标不可用");
		return FString::Printf(TEXT("生命     %0.0f / %0.0f\n防御     %0.0f       法抗  %0.0f\n属性  %s\n总伤害  %0.1f       秒伤  %0.1f\n命中     %d         上次伤害 %0.1f\n处决免疫  %s"),
			Unit->Health(), Unit->Spec.MaxHealth, Unit->Spec.Defense, Unit->Spec.MagicDefense, *Unit->Spec.Attribute.ToString(),
			Analytics.IsValid() ? Analytics->GetTotalDamage(Id) : 0.f,
			Analytics.IsValid() ? Analytics->GetDPS(Id) : 0.f,
			Analytics.IsValid() ? Analytics->GetHitCount(Id) : 0,
			Analytics.IsValid() ? Analytics->GetLastDamage(Id) : 0.f,
			Unit->Spec.bExecutionImmune ? TEXT("是") : TEXT("否"));
	}

	TSharedRef<SWidget> BuildOverviewCards()
	{
		TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(7.f, 0.f));
		int32 Column = 0;
		for (const APadmaCombatUnit* Unit : GetTrainingTargets())
		{
			const FName Id = Unit->Spec.Id;
			const FLinearColor Accent = Column == 0 ? Orange : Column == 1 ? Cyan : Yellow;
			Grid->AddSlot(Column++, 0)
				[Frame(SNew(SVerticalBox)
					+ SVerticalBox::Slot().AutoHeight()[LiveText([this, Id]() { return UnitLabel(Battle.Get(), Id); }, 16, Accent)]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 6.f)
					[SNew(SProgressBar).Percent_Lambda([this, Id]() -> TOptional<float>
					{
						const APadmaCombatUnit* Current = Battle.IsValid() ? Battle->GetUnit(Id) : nullptr;
						return Current ? FMath::Clamp(Current->Health() / FMath::Max(1.f, Current->Spec.MaxHealth), 0.f, 1.f) : 0.f;
					}).FillColorAndOpacity(Accent)]
					+ SVerticalBox::Slot().AutoHeight()[LiveText([this, Id]() { return TargetMetrics(Id); })], FMargin(10.f, 9.f), CardBackground)];
		}
		if (Column == 0) Grid->AddSlot(0, 0)[Text(TEXT("暂无训练木桩"), 13, Red)];
		return Grid;
	}

	TSharedRef<SWidget> BuildOverviewPage()
	{
		SAssignNew(OverviewSummaryBox, SBox)[BuildOverviewSummary()];
		SAssignNew(OverviewCardsBox, SBox)[BuildOverviewCards()];
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()[Text(TEXT("总览 · 三个训练木桩"), 13, Cyan)]
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 7.f, 0.f, 0.f)[OverviewSummaryBox.ToSharedRef()]
			+ SVerticalBox::Slot().FillHeight(1.f).Padding(0.f, 10.f, 0.f, 0.f)
				[SNew(SScrollBox).Orientation(Orient_Vertical) + SScrollBox::Slot()[OverviewCardsBox.ToSharedRef()]];
	}

	TSharedRef<SWidget> BuildCharacterPage()
	{
		const UPadmaACTCharacterDefinition* Character = GetCharacterDefinition();
		TSharedRef<SVerticalBox> Rows = SNew(SVerticalBox);
		Rows->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)
			[Text(Character ? FString::Printf(TEXT("%s  //  %s"), *ChineseName(Character->DisplayName.ToString()), *Character->DefinitionId.ToString()) : TEXT("未加载角色配置"), 14, White)];

		UDataTable* SkillTable = Character ? Character->SkillTable.LoadSynchronous() : nullptr;
		if (!SkillTable || SkillTable->GetRowMap().IsEmpty())
		{
			Rows->AddSlot().AutoHeight()[Frame(Text(TEXT("未加载技能数据。"), 12, Red, true))];
		}
		else
		{
			for (const auto& Pair : SkillTable->GetRowMap())
			{
				const FPadmaACTSkillRow* Row = SkillTable->FindRow<FPadmaACTSkillRow>(Pair.Key, TEXT("ACT Training Character Page"));
				if (!Row) continue;
				const UPadmaACTSkillDefinition* Skill = Row->Definition.LoadSynchronous();
				const FString SkillName = ChineseName(Row->SkillId.ToString());
				const FString ActionKind = Skill ? EnumDisplayName(StaticEnum<EPadmaACTActionKind>(), static_cast<int64>(Skill->ActionKind)) : TEXT("未加载");
				const FString SkillInfo = FString::Printf(TEXT("按键  %s    实现标识  %s    动作类型  %s\n威力  %s x%0.2f    冷却  %0.2f 秒    心流消耗  %0.1f    计算力消耗  %0.1f\n动画  %s    起始段  %s    下一连招  %s    使用主目标  %s"),
					*ChineseName(Row->ActivationBindingId.ToString()), Skill ? *Skill->AbilityImplementationId.ToString() : TEXT("未加载"), *ActionKind,
					Skill ? *DamageKindText(Skill->DamageKind) : TEXT("未加载"), Skill ? Skill->DamageScale : 0.f,
					Skill ? Skill->CooldownSeconds : 0.f, Skill ? Skill->FlowCost : 0.f, Skill ? Skill->CalculationCost : 0.f,
					Skill ? *SoftAssetName(Skill->Montage) : TEXT("未加载"), Skill ? *Skill->StartSection.ToString() : TEXT("-"),
					Skill ? *ChineseName(Skill->NextComboId.ToString()) : TEXT("-"), Skill && Skill->bUsePrimaryTarget ? TEXT("是") : TEXT("否"));
				Rows->AddSlot().AutoHeight().Padding(0.f, 3.f)
					[Frame(SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight()[Text(SkillName, 15, Skill ? Cyan : Red)]
						+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 5.f, 0.f, 0.f)[Text(SkillInfo, 11, White, true)], FMargin(10.f, 8.f))];
			}
		}
		return SNew(SScrollBox).Orientation(Orient_Vertical) + SScrollBox::Slot()[Rows];
	}

	TSharedRef<SWidget> BuildTargetSelector()
	{
		return Frame(SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 14.f, 0.f)[Text(TEXT("选择木桩"), 11, Muted)]
			+ SHorizontalBox::Slot().FillWidth(1.f)
				[SAssignNew(TargetCombo, SComboBox<TSharedPtr<FName>>)
                    .Method(EPopupMethod::UseCurrentWindow)
					.OptionsSource(&TargetOptions)
					.InitiallySelectedItem(SelectedTargetOption)
					.OnGenerateWidget_Lambda([this](TSharedPtr<FName> Item)
					{
						return Text(Item.IsValid() ? UnitLabel(Battle.Get(), *Item) : TEXT("未加载"), 12, White);
					})
					.OnSelectionChanged_Lambda([this](TSharedPtr<FName> Item, ESelectInfo::Type)
					{
						if (!Item.IsValid()) return;
						SelectedTargetOption = Item;
						SelectedTargetId = *Item;
						RefreshDetailsPage();
					})
					[SNew(STextBlock).Text_Lambda([this]() { return FText::FromString(UnitLabel(Battle.Get(), SelectedTargetId)); }).Font(FCoreStyle::GetDefaultFontStyle("Regular", 13)).ColorAndOpacity(White)]]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(12.f, 0.f, 0.f, 0.f)[Text(TEXT("伤害与效果记录"), 10, Cyan)], FMargin(10.f, 7.f));
	}

	TSharedRef<SWidget> BuildDetailsTargetSummary()
	{
		return Frame(SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()[LiveText([this]() { return UnitLabel(Battle.Get(), SelectedTargetId); }, 16, Orange)]
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 5.f, 0.f, 0.f)[LiveText([this]() { return TargetMetrics(SelectedTargetId); })]);
	}

	void RefreshDamageRows(TSharedPtr<SVerticalBox> Rows, FName TargetId)
	{
		if (!Rows.IsValid()) return;
		Rows->ClearChildren();
		Rows->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)[Text(TEXT("伤害结算记录"), 13, Orange)];
		const auto Samples = Analytics.IsValid() ? Analytics->GetRecentDamage(TargetId, 18) : TArray<FPadmaACTTrainingDamageSample>();
		if (Samples.IsEmpty()) Rows->AddSlot().AutoHeight()[Text(TEXT("等待命中……"), 12, Muted, true)];
		for (const auto& Sample : Samples)
		{
			const FString Row = FString::Printf(TEXT("%s  ->  %s   %-4s  %7.1f%s\n%s  |  时间=%05.2f  生命=%0.0f -> %0.0f%s  吸收=%0.1f"),
				*UnitLabel(Battle.Get(), Sample.SourceId), *UnitLabel(Battle.Get(), Sample.TargetId), *DamageKindText(Sample.DamageKind), Sample.Amount,
				Sample.bTrueDamage ? TEXT(" 真伤") : TEXT(""), *ChineseName(Sample.ActionId.ToString()), Sample.TimeSeconds, Sample.HealthBefore, Sample.HealthAfter,
				Sample.bBlocked ? TEXT("  格挡") : TEXT(""), Sample.Absorbed);
			Rows->AddSlot().AutoHeight().Padding(0.f, 2.f)[Text(Row, 11, DamageKindColor(Sample.DamageKind), true)];
		}
	}

	void RefreshEffectRows(TSharedPtr<SVerticalBox> Rows, FName TargetId)
	{
		if (!Rows.IsValid()) return;
		Rows->ClearChildren();
		Rows->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)[Text(TEXT("战斗效果记录"), 13, Cyan)];
		TArray<FPadmaGameplayEffectRecord> Records;
		if (Analytics.IsValid())
		{
			for (int32 Index = Analytics->GetGameplayEffectRecords().Num() - 1; Index >= 0 && Records.Num() < 18; --Index)
			{
				const FPadmaGameplayEffectRecord& Record = Analytics->GetGameplayEffectRecords()[Index];
				if (TargetId.IsNone() || Record.TargetId == TargetId) Records.Add(Record);
			}
		}
		if (Records.IsEmpty()) Rows->AddSlot().AutoHeight()[Text(TEXT("暂无效果记录。"), 12, Muted, true)];
		for (const auto& Record : Records)
		{
			const FString SourceTags = Record.SourceTags.IsEmpty() ? TEXT("-") : Record.SourceTags.ToStringSimple();
			const FString TargetTags = Record.TargetTags.IsEmpty() ? TEXT("-") : Record.TargetTags.ToStringSimple();
			const FString MagnitudeText = FString::Printf(TEXT("%0.1f"), Record.Magnitude);
			const FString TimeText = FString::Printf(TEXT("%05.2f"), Record.TimeSeconds);
			FString Row = UnitLabel(Battle.Get(), Record.SourceId);
			Row += TEXT("  ->  ");
			Row += UnitLabel(Battle.Get(), Record.TargetId);
			Row += TEXT("\n");
			Row += Record.GameplayEffectId.ToString();
			Row += TEXT("  |  ");
			Row += ChineseName(Record.ActionId.ToString());
			Row += TEXT("  ");
			Row += ChineseName(Record.AttributeId.ToString());
			Row += Record.Magnitude >= 0.f ? TEXT("+") : TEXT("");
			Row += MagnitudeText;
			Row += TEXT("  时间=");
			Row += TimeText;
			Row += Record.bOverride ? TEXT("  覆盖") : TEXT("");
			Row += TEXT("\n来源标签：");
			Row += SourceTags;
			Row += TEXT("  目标标签：");
			Row += TargetTags;
			Rows->AddSlot().AutoHeight().Padding(0.f, 2.f)[Text(Row, 11, Cyan, true)];
		}
	}

	TSharedRef<SWidget> BuildDetailsPage()
	{
		SAssignNew(DetailsTargetSummaryBox, SBox)[BuildDetailsTargetSummary()];
		SAssignNew(DetailsDamageRows, SVerticalBox);
		SAssignNew(DetailsEffectRows, SVerticalBox);
		RefreshDamageRows(DetailsDamageRows, SelectedTargetId);
		RefreshEffectRows(DetailsEffectRows, SelectedTargetId);
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()[Text(TEXT("明细 · 当前木桩"), 13, Cyan)]
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 7.f, 0.f, 0.f)[DetailsTargetSummaryBox.ToSharedRef()]
			+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 7.f, 0.f, 0.f)[BuildTargetSelector()]
			+ SVerticalBox::Slot().FillHeight(1.f).Padding(0.f, 8.f, 0.f, 0.f)
				[SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 5.f, 0.f)[SNew(SScrollBox).Orientation(Orient_Vertical) + SScrollBox::Slot()[Frame(DetailsDamageRows.ToSharedRef(), FMargin(10.f), FLinearColor(0.006f, 0.028f, 0.035f, 0.91f))]]
					+ SHorizontalBox::Slot().FillWidth(1.f).Padding(5.f, 0.f, 0.f, 0.f)[SNew(SScrollBox).Orientation(Orient_Vertical) + SScrollBox::Slot()[Frame(DetailsEffectRows.ToSharedRef(), FMargin(10.f), FLinearColor(0.006f, 0.028f, 0.035f, 0.91f))]]];
	}

	TSharedRef<SWidget> BuildAppearancePage()
	{
		const UPadmaACTCharacterDefinition* Character = GetCharacterDefinition();
		const UPadmaACTMeleeDefinition* Profile = Character ? Character->MeleeProfile.LoadSynchronous() : nullptr;
		TSharedRef<SVerticalBox> Rows = SNew(SVerticalBox);
		Rows->AddSlot().AutoHeight()[Text(TEXT("外观 · 武器配置"), 13, Cyan)];
		Rows->AddSlot().AutoHeight().Padding(0.f, 7.f, 0.f, 0.f)
			[Frame(Text(Profile ? FString::Printf(TEXT("近战配置  %s\n武器收放  %s    持武停留  %0.2f 秒    收武停留  %0.2f 秒    消隐  %0.2f 秒"),
				*Profile->GetName(), Profile->bWeaponLifecycle ? TEXT("开启") : TEXT("关闭"), Profile->DrawnHoldSeconds, Profile->StowedHoldSeconds, Profile->WeaponDissolveSeconds) : TEXT("未加载近战配置"), 12, Profile ? White : Red, true))];

		if (!Profile || Profile->Weapons.IsEmpty())
		{
			Rows->AddSlot().AutoHeight().Padding(0.f, 5.f)[Frame(Text(TEXT("未加载武器配置。"), 12, Red, true))];
		}
		else
		{
			for (int32 Index = 0; Index < Profile->Weapons.Num(); ++Index)
			{
				const FPadmaACTMeleeWeapon& Weapon = Profile->Weapons[Index];
				const FString WeaponText = FString::Printf(TEXT("模型  %s\n持武挂点  %s    收武挂点  %s\n收武位置  %s"),
					*SoftAssetName(Weapon.Mesh), *Weapon.Socket.ToString(), *Weapon.StowedSocket.ToString(),
					Weapon.StowedSocket.IsNone() ? TEXT("无") : TEXT("已配置"));
				Rows->AddSlot().AutoHeight().Padding(0.f, 3.f)
					[Frame(SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight()[Text(FString::Printf(TEXT("武器槽 %02d"), Index + 1), 15, Index == 0 ? Orange : Cyan)]
						+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 5.f, 0.f, 0.f)[Text(WeaponText, 11, White, true)]
						+ SVerticalBox::Slot().AutoHeight()[LiveText([this, Index]()
						{
							const APadmaCombatUnit* Player = Battle.IsValid() ? Battle->GetPlayerUnit() : nullptr;
							const auto* Melee = Player ? Player->FindComponentByClass<UPadmaACTMeleeComponent>() : nullptr;
							const auto* WeaponComponent = Melee ? Melee->GetWeapon(Index) : nullptr;
							return FString(TEXT("当前状态  ")) + (WeaponComponent ? (WeaponComponent->IsVisible() ? TEXT("显示") : TEXT("隐藏")) : TEXT("未加载"));
						}, 11)], FMargin(10.f, 8.f))];
			}
		}
		Rows->AddSlot().AutoHeight().Padding(0.f, 8.f, 0.f, 0.f)
			[SNew(SButton).IsEnabled(false).ContentPadding(FMargin(12.f, 6.f)).ButtonColorAndOpacity(FLinearColor(0.08f, 0.12f, 0.13f, 0.85f))[Text(TEXT("更换武器（尚未接通）"), 11, Muted)]];
		return SNew(SScrollBox).Orientation(Orient_Vertical) + SScrollBox::Slot()[Rows];
	}

	TSharedRef<SWidget> BuildPage()
	{
		OverviewSummaryBox.Reset();
		OverviewCardsBox.Reset();
		DetailsTargetSummaryBox.Reset();
		DetailsDamageRows.Reset();
		DetailsEffectRows.Reset();
		TargetCombo.Reset();
		switch (SelectedPage)
		{
		case EPage::Character: return BuildCharacterPage();
		case EPage::Details: return BuildDetailsPage();
		case EPage::Appearance: return BuildAppearancePage();
		default: return BuildOverviewPage();
		}
	}

	void SelectPage(EPage Page)
	{
		if (SelectedPage == Page) return;
		SelectedPage = Page;
		if (PageBox.IsValid()) PageBox->SetContent(BuildPage());
	}

	void RefreshDetailsPage()
	{
		RefreshDamageRows(DetailsDamageRows, SelectedTargetId);
		RefreshEffectRows(DetailsEffectRows, SelectedTargetId);
	}

	void RefreshContent()
	{
		const bool bTargetsChanged = RefreshTargetOptions();
		// Metrics are bound to live state. Damage never replaces the cards, page,
		// scroll owners or dropdown; only the receipt row containers change.
		// The lab creates its overlay before starting the battle. Build target
		// cards when that roster arrives (or changes), not on ordinary damage.
		if (bTargetsChanged && OverviewCardsBox.IsValid()) OverviewCardsBox->SetContent(BuildOverviewCards());
		RefreshDetailsPage();
	}

	TSharedRef<SWidget> BuildBody()
	{
		TSharedRef<SVerticalBox> Body = SNew(SVerticalBox);
		Body->AddSlot().AutoHeight()[BuildHeader()];
		Body->AddSlot().AutoHeight().Padding(0.f, 9.f, 0.f, 0.f)[BuildNavigation()];
		Body->AddSlot().AutoHeight().Padding(0.f, 8.f, 0.f, 0.f)[Line()];
		SAssignNew(PageBox, SBox)[BuildPage()];
		Body->AddSlot().FillHeight(1.f).Padding(0.f, 10.f, 0.f, 0.f)[PageBox.ToSharedRef()];
		Body->AddSlot().AutoHeight().Padding(0.f, 8.f, 0.f, 0.f)
			[Text(TEXT("[B] 关闭    [F8] 重置    |    训练数据"), 10, Muted)];
		return Body;
	}

	TWeakObjectPtr<UPadmaACTTrainingAnalyticsComponent> Analytics;
	TWeakObjectPtr<UPadmaCombatComponent> Battle;
	FSimpleDelegate Close;
	TSharedPtr<SOverlay> Layout;
	TSharedPtr<SBackgroundBlur> Blur;
	TSharedPtr<SBox> PanelSlot;
	TSharedPtr<SBorder> PanelFrame;
	TSharedPtr<SBox> ContentBox;
	TSharedPtr<SBox> PageBox;
	TSharedPtr<SBox> OverviewSummaryBox;
	TSharedPtr<SBox> OverviewCardsBox;
	TSharedPtr<SBox> DetailsTargetSummaryBox;
	TSharedPtr<SVerticalBox> DetailsDamageRows;
	TSharedPtr<SVerticalBox> DetailsEffectRows;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> TargetCombo;
	TArray<TSharedPtr<FName>> TargetOptions;
	TSharedPtr<FName> SelectedTargetOption;
	FName SelectedTargetId;
	EPage SelectedPage = EPage::Overview;
	bool bOpen = false;
	bool bWorldPresentation = false;
	float Animation = 0.f;
	int32 LastRevision = INDEX_NONE;
};

void UPadmaACTTrainingAnalyticsWidget::Configure(UPadmaACTTrainingAnalyticsComponent* InAnalytics, UPadmaCombatComponent* InBattle, TFunction<void()> InClose)
{
	Analytics = InAnalytics;
	Battle = InBattle;
	Close = MoveTemp(InClose);
}

void UPadmaACTTrainingAnalyticsWidget::SetOpen(bool bInOpen)
{
	bOpen = bInOpen;
	if (Panel.IsValid()) Panel->SetOpen(bOpen);
}

TSharedRef<SWidget> UPadmaACTTrainingAnalyticsWidget::RebuildWidget()
{
	Super::RebuildWidget();
	FSimpleDelegate CloseDelegate = FSimpleDelegate::CreateLambda([this]()
	{
		if (Close) Close();
	});
	SAssignNew(Panel, SPadmaACTTrainingAnalyticsPanel)
		.Analytics(Analytics.Get())
		.Battle(Battle.Get())
		.Close(CloseDelegate)
        .WorldPresentation(bWorldPresentation);
	Panel->SetOpen(bOpen);
	return Panel.ToSharedRef();
}

void UPadmaACTTrainingAnalyticsWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Panel.Reset();
	Super::ReleaseSlateResources(bReleaseChildren);
}

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaACTTrainingPanelTest, "DreamOfPadma.ACT.ChenTrainingPanel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPadmaACTTrainingPanelTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues Values;
	Values.AllowAudioPlayback(false).CreatePhysicsScene(false).ShouldSimulatePhysics(false)
		.EnableTraceCollision(false).CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("Panel test world"), World)) return false;
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	ON_SCOPE_EXIT { World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
	AActor* Host = World->SpawnActor<AActor>();
	auto* Battle = NewObject<UPadmaCombatComponent>(Host);
	Host->AddInstanceComponent(Battle); Battle->RegisterComponent();
	auto* Analytics = NewObject<UPadmaACTTrainingAnalyticsComponent>(Host);
	Host->AddInstanceComponent(Analytics); Analytics->RegisterComponent(); Analytics->BindBattle(Battle);
	Battle->PayCost = [](float, float, FName, FString&) { return true; };
	FPadmaCombatSetup Setup;
	Setup.Mode = EPadmaCombatMode::ACT;
	Setup.ArenaMinimum = FVector2D(-1000, -1000); Setup.ArenaMaximum = FVector2D(1000, 1000);
	FPadmaCombatUnitSpec Player;
	Player.Id = TEXT("panel-player"); Player.bPlayer = true;
	Player.Health = Player.MaxHealth = 1000; Player.Attack = 20; Player.AttackRange = 500;
	Setup.Units.Add(Player);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		auto Target = Player; Target.bPlayer = false;
		Target.Id = FName(*FString::Printf(TEXT("fixture-%d"), Index));
		Target.DisplayName = FText::FromName(Target.Id);
		Target.Location = FVector(200 + Index * 80, 0, 0);
		Target.bCanPursueInACT = false; Target.bExecutionImmune = Index != 1;
		Setup.Units.Add(Target);
	}
	TSharedPtr<SPadmaACTTrainingAnalyticsPanel> Panel = SNew(SPadmaACTTrainingAnalyticsPanel).Analytics(Analytics).Battle(Battle);
	FString Failure;
	if (!TestTrue(TEXT("Panel battle starts"), Battle->StartBattle(Setup, Failure))) return false;
	Analytics->Reset();
	// Attach the real authored character definition for the read-only page without
	// requiring its movement/animation runtime in this isolated Slate test world.
	Battle->GetPlayerUnit()->Spec.Presentation.ACTDefinition = TSoftObjectPtr<UPadmaACTCharacterDefinition>(
		FSoftObjectPath(TEXT("/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN.DA_ACTCharacter_CHEN")));
	Panel->SetOpen(true);
	const auto Geometry = FGeometry::MakeRoot(FVector2D(1600, 900), FSlateLayoutTransform());
	Panel->Tick(Geometry, 0, .3f);
	const auto Frame = Panel->PanelFrame;
	const auto Page = Panel->PageBox->GetChildren()->GetChildAt(0);
	const auto Cards = Panel->OverviewCardsBox->GetChildren()->GetChildAt(0);
	TestEqual(TEXT("Deferred battle start populates three target cards"), Cards->GetChildren()->Num(), 3);
	TestTrue(TEXT("Real battle damage settles"), Battle->Attack({ TEXT("fixture-0") }, Failure));
	Panel->Tick(Geometry, .3, .016f);
	TestTrue(TEXT("Damage retains panel frame"), Frame == Panel->PanelFrame);
	TestTrue(TEXT("Damage retains page and scroll ownership"), Page == Panel->PageBox->GetChildren()->GetChildAt(0));
	TestTrue(TEXT("Damage retains overview cards"), Cards == Panel->OverviewCardsBox->GetChildren()->GetChildAt(0));
	TestEqual(TEXT("Damage cannot restart opening animation"), Panel->Animation, 1.f);
	TestTrue(TEXT("Execution immunity is read from the unit"), Panel->TargetMetrics(TEXT("fixture-1")).Contains(TEXT("处决免疫  否")));

	// Native SButton keyboard activation exercises its actual delegate.
	auto CharacterButton = StaticCastSharedRef<SButton>(Panel->BuildNavigationButton(SPadmaACTTrainingAnalyticsPanel::EPage::Character));
	CharacterButton->OnKeyDown(Geometry, FKeyEvent(EKeys::Enter, FModifierKeysState(), 0, false, 0, 0));
	CharacterButton->OnKeyUp(Geometry, FKeyEvent(EKeys::Enter, FModifierKeysState(), 0, false, 0, 0));
	TestTrue(TEXT("Character navigation activates"), Panel->SelectedPage == SPadmaACTTrainingAnalyticsPanel::EPage::Character);
	const auto* Character = Panel->GetCharacterDefinition();
	TestNotNull(TEXT("Real Chen definition resolves"), Character);
	TestTrue(TEXT("Real Chen skill table contains authored actions"), Character && Character->SkillTable.LoadSynchronous() && Character->SkillTable.Get()->GetRowMap().Num() >= 13);
	Panel->SelectPage(SPadmaACTTrainingAnalyticsPanel::EPage::Details);
	TestEqual(TEXT("Three real combo options"), Panel->TargetOptions.Num(), 3);
	const auto Combo = Panel->TargetCombo;
	for (const auto& Option : Panel->TargetOptions)
	{
		Combo->SetSelectedItem(Option);
		TestEqual(TEXT("Combo delegates select actual fixture"), Panel->SelectedTargetId, *Option);
	}
	Battle->GetPlayerUnit()->AttackCooldown = 0;
	Battle->Attack({ TEXT("fixture-2") }, Failure);
	Panel->Tick(Geometry, .4, .016f);
	TestTrue(TEXT("Receipt refresh retains combo"), Panel->TargetCombo == Combo);
	TestEqual(TEXT("Receipt refresh retains selection"), Panel->SelectedTargetId, FName(TEXT("fixture-2")));
	TestEqual(TEXT("Selected receipt stream has one hit plus heading"), Panel->DetailsDamageRows->NumSlots(), 2);
	TestTrue(TEXT("Selected effect stream contains real GE"), Panel->DetailsEffectRows->NumSlots() > 1);
	Analytics->Reset(); Panel->Tick(Geometry, .5, .016f);
	TestEqual(TEXT("Reset leaves empty receipt message plus heading"), Panel->DetailsDamageRows->NumSlots(), 2);
	TestEqual(TEXT("Reset leaves empty GE message plus heading"), Panel->DetailsEffectRows->NumSlots(), 2);
	TestTrue(TEXT("Reset retains combo owner"), Panel->TargetCombo == Combo);
	Panel->SelectPage(SPadmaACTTrainingAnalyticsPanel::EPage::Appearance);
	TestTrue(TEXT("Real melee profile resolves"), Character && Character->MeleeProfile.LoadSynchronous());
	Panel->SetOpen(false); Panel->Tick(Geometry, .6, .3f);
	TestTrue(TEXT("Close completes and hides overlay"), Panel->GetVisibility() == EVisibility::Hidden);
	Panel.Reset(); Battle->ExitBattle();
	return true;
}
#endif
