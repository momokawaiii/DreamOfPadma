#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PadmaACTTrainingAnalyticsWidget.generated.h"

class UPadmaACTTrainingAnalyticsComponent;
class UPadmaCombatComponent;
class SPadmaACTTrainingAnalyticsPanel;
class SWidget;

/** Native Division-style training overlay; it observes analytics and emits no gameplay commands. */
UCLASS()
class DREAMOFPADMA_API UPadmaACTTrainingAnalyticsWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void Configure(UPadmaACTTrainingAnalyticsComponent* InAnalytics, UPadmaCombatComponent* InBattle, TFunction<void()> InClose);
	void SetWorldPresentation(bool bEnabled) { bWorldPresentation=bEnabled; }
	void SetOpen(bool bInOpen);
	bool IsOpen() const { return bOpen; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	TWeakObjectPtr<UPadmaACTTrainingAnalyticsComponent> Analytics;
	TWeakObjectPtr<UPadmaCombatComponent> Battle;
	TFunction<void()> Close;
	TSharedPtr<SPadmaACTTrainingAnalyticsPanel> Panel;
	bool bOpen = false;
	bool bWorldPresentation = false;
};
