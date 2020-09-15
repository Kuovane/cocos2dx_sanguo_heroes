#pragma once

#include "cocos2d.h"
#include "entityx.h"
#include "SimpleTween.h"
#include "CCAdvancedAnimation.h"
#include "BattleConfig.h"
#include "BattleComponent.h"
#include "SGCamera.h"
//#include "CameraManager.h"
//#include "BattleSceneUI.h"
//#include "BattleDialogueUI.h"

class BattleSceneUI;
class BattleDialogueUI;

class BattleStage
{
public:
    BattleStage(entityx::EntityX& entityX);
    ~BattleStage();
    
    inline float getBattleTime() { return m_battleTime; }
    inline void updateBattleTime(float dt) { m_battleTime += dt; }
    
    inline BattleConfig::BattleType getBattleType() { return m_battleType; }
    
    inline void startFight() { m_startFight = true; };
    inline void stopFight() { m_startFight = false; };
    
    void changeTimeRate(float timeRate);
    
    inline cocos2d::Node* getStage() { return m_stage; };
    inline cocos2d::Node* getBackground() { return m_bg; };
    inline cocos2d::Node* getRole() { return m_role; };
     cocos2d::Node* getMap();
    inline BattleSceneUI* getBattleSceneUI() { return m_battleSceneUI; };
    inline BattleDialogueUI* getBattleDialogueUI() { return m_battleDialogueUI; };
   
    virtual void update(float dt);
    
    void pause();
    void resume();
    
    void showBackgroundMask(float duration = 0, float color = 45);
    void hideBackgroundMask(float duration = 0);
    void pauseBattle();
    void resumeBattle();
    
    void showBGColor(float duration);
    void showBGColor(float duration, unsigned char red, unsigned char green, unsigned char blue);
    void stopBGColor();
    
private:
    entityx::EntityX& m_entityx;
    
    float m_battleTime = 0;
    BattleConfig::BattleType m_battleType;
    float m_timeRate = 1.0f;
    
    bool m_pauseBattle = false;
    bool m_startFight = false;
    
    bool m_isMoving;
    cocos2d::Vec2 m_startPos;
    float m_dragV;
    bool m_isRebound;
    bool m_isMoving2Point;
    float m_dragDt = 0.0f;
    
    cocos2d::Node* m_stage = nullptr;
    cocos2d::Node* m_bg = nullptr;
    cocos2d::Node* m_role = nullptr;

    BattleSceneUI* m_battleSceneUI = nullptr;
    BattleDialogueUI* m_battleDialogueUI = nullptr;
    
    SGCamera* m_roleCamera;
    SGCamera* m_bgCamera;
    
    cocos2d::Scheduler* m_scheduler;
    
    int m_showBackgroundRefCount;
    int m_pauseBattleRefCount;
    SimpleTween_quad m_backgroundColorTween = SimpleTween_quad(0, 255, 255);
    float m_backgroundCurrentColor = 255;
    
    void onTouchBegin(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchMove(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchEnd(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchCancel(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event);
    
    void updataCameraMove(float dt);
    void setBackgroundMaskColor(float value);
    
    void switchViewMode();
};
