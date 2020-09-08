//
//  BattleSceneUI.h
//  sanguoClient
//
//  Created by fuchenhao on 3/31/15.
//
//

#pragma once

//#include <vector>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "BattleEvent.h"

#define SKILL_ICON_PADDING           15
#define SKILL_ICON_MOVE_DISTANCE     16
#define SKILL_ICON_MOVE_TIME         0.5f
#define BATTLE_END_DELAY_TIME        0.5f
#define RETURN_TO_LUA_DELAY          1.5f

//using namespace cocos2d;
//using namespace ui;


class BattleSceneUI : public cocos2d::Layer, public entityx::Receiver<BattleSceneUI>
{
public:

	enum ButtonAnimationStatus
	{
		ANIMATION_DISABLE,
		ANIMATION_READY
	};

	enum AnimationFrame
	{
		DOWN_START_FRAME = 1,
		DOWN_END_FRAME = 21,
		UP_START_FRAME = 70,
		UP_END_FRAME = 142
	};

	enum PausePanelStatus
	{
		OPENING,
		OPEN,
		CLOSING,
		CLOSE
	};

	struct HeroInfo
	{
		float currentHp;
		float currentMp;
		float prevHp;
		float prevMp;
		float maxHp;
		float maxMp;
	};

	BattleSceneUI();
	virtual ~BattleSceneUI();

	virtual void update(float dt);

	void receive(const BattleEvent::LockStatusChange& e);
	void receive(const BattleEvent::FireSkill& e);
	void receive(const BattleEvent::PlayHeroDeadEffect& e);
	void receive(const BattleEvent::PlayBattleResultEffect& e);
	void receive(const BattleEvent::ShowBattleSceneUI& e);
	void receive(const BattleEvent::HideBattleSceneUI& e);
	void receive(const BattleEvent::StartBattleComplete& e);
	void receive(const BattleEvent::UpdateBuffIcon& e);

	inline cocos2d::Node* getMap() { return m_map; };
	inline cocos2d::Node* getMapBg() { return m_mapBg; };

	CREATE_FUNC(BattleSceneUI);

private:
	// ���õ����
	cocos2d::Layer* m_swallowTouchLayer = nullptr;
	void initSwallowTouchLayer();

	// shared
	cocos2d::ui::Widget* m_battleSceneUI;
	cocostudio::timeline::ActionTimeline* m_battleSceneUITimeline;
	bool checkTouchType(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type, bool closeStrategyPanel = true, float orginScale = 1.0f, float targetScale = 1.2f);

	// battle start
	Node* m_battleStartAnimation = nullptr;
	cocostudio::timeline::ActionTimeline* m_battleStartAnimationTimeline = nullptr;
	bool m_isPlayingBattleStartAnimation = false;
	bool m_isPlayingUpAnimation = false;
	void showBattleSceneUI();
	void checkBattleStartAnimationComplete();
	void checkShowBattleSceneUIComplete();

	// battle end
	bool m_isPlayingDownAnimation = false;
	void hideBattleSceneUI();
	void checkHideBattleSceneUIComplete();
	void onBattleEnd(cocos2d::EventCustom* event);
	void displayBattleResult(float dt);

	// multi hit
	cocostudio::timeline::ActionTimeline* m_multiHitAction;
	void initMultiHit();
	void updateMultiHit(float dt);
	void setCurrentHit(int num);
	void onMultiHit(cocos2d::EventCustom* event);
	int m_currentAnimationStep = 0;
	bool m_isMultiHitPlaying = false;
	float m_showMultiHitElapse = 0.0;

	// skill description
	void initSkillDescription();
	void updateSkillDescription();
	cocostudio::timeline::ActionTimeline* m_skillDescAction = nullptr;

	// map movement
	cocos2d::Vec2 m_mapStartPos;
	cocos2d::Node* m_map = nullptr;
	cocos2d::ui::Scale9Sprite* m_mapBg = nullptr;
	void initMapPanel();
	void switchMapPanel(bool hide);
	bool m_isMapHide = false;

	// count down
	void initCountDown();
	void updateCountDown();

	// camera
	void moveCameraToRole(int id, bool follow = false);

	// control battle
	bool m_isHeroAttack = false;
	bool m_isSoldierAttack = false;
	bool m_showStrategyPanel = false;//��ǰ�Ƿ�չʾ���ܰ�ť
	float m_strategyStartX;
	void initControlPanel();
	void closeStrategyPanel(cocos2d::EventCustom* event);
	void switchStrategyPanel(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void switchHeroAttack(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void switchSoldierAttack(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void switchSoldierStrategy(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void startDelegate(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void stopDelegate(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	// hero info
	HeroInfo m_leftHeroInfo;
	HeroInfo m_rightHeroInfo;
	bool m_hpLessThan20Percent = false;
	void initHeroInfoPanel();
	void bindHeroInfoPanelWithHeros();
	void updateHeroInfo(int roleId);
	void updateHpBar(int roleId);
	void onHeroClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	// pause panel
	void battleComplete(float dt);
	void exitBattle(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void resumeBattle(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void showPausePanel(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void enableEffect(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void disableEffect(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void enableMusic(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void disableMusic(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateSoundButtonStatus();
	void hidePausePanel();
	void initPausePanel();
	void updatePausePanel();
	cocostudio::timeline::ActionTimeline* m_battlePausePanelTimeline;
	PausePanelStatus m_pausePanelStatus = CLOSE;
	Node* m_battlePausePanel;

	// skill related
	// 0 ���ܼ���δ��� // 0-37 ���ܷŹ�Ȧ��Ӧ���Ǽ������ˣ�// 37-105 ���ܾ���״̬
	void initSkillButtons();
	void bindSkillButtonsWithSkills();
	void updateSkillButtons();
	void onSkillButtonClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	std::vector<int> m_positiveSkills;
	std::vector<ButtonAnimationStatus> m_skillButtonAnimationStatus;//����״̬
	std::vector<cocostudio::timeline::ActionTimeline*> m_skillButtonAnimation;//���ܶ���Ч��
	std::vector<cocos2d::ui::LoadingBar*> m_skillButtonLoadingBar;//���ܼ��ؽ���
	std::vector<Node*> m_skillButtons;//���ܰ�ť
	float m_skillButtonInitY;
	float m_skillButtonIconWidth;
};


