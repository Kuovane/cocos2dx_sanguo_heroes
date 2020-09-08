//
//  BattleComponent
//
//  Created by fuchenhao on 3/1/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//


#pragma once

#include "cocos2d.h"
//#include <vector>

#include "entityx.h"
#include "CCAsset.h"
//#include "BattleConfig.h"
#include "animation/BattleAnimation.h"
#include "animation/BattleAnimationGroup.h"
#include "ModeGroup.h"
#include "StrategyControl.h"
#include "StateMachine.h"
#include "TweenQueue.h"

class SkillObject;

class BattleComponent
{
public:
    
    struct Battle : entityx::Component<Battle>
    {
        float totalTime;
        
		explicit Battle(float totalTime) : totalTime(totalTime) {}
    };
    
    struct Animation : entityx::Component<Animation>
    {
        BattleAnimation* battleAnimation;
        TweenQueue m_tweenQueue;
        std::string assetName;
        
		explicit Animation(AdvancedAnimation *animation)
        {
            battleAnimation = new BattleAnimation(animation);
        }
        
        ~Animation()
        {
            delete battleAnimation;
        }
    };
    
    struct AnimationGroup : entityx::Component<AnimationGroup>
    {
        BattleAnimationGroup* battleAnimation;
        TweenQueue m_tweenQueue;
        bool randomStart;
        
		explicit AnimationGroup(std::vector<BattleAnimation*> *animations, bool randomStart = true) : randomStart(randomStart)
        {
            battleAnimation = new BattleAnimationGroup(animations);
        }
        
        ~AnimationGroup()
        {
            delete battleAnimation;
        }
    };
    
    struct EntityAnimation : entityx::Component<EntityAnimation>
    {
        BattleAnimationGroup* battleAnimation;
        TweenQueue m_tweenQueue;
        bool loop;
        float lifeTime;
        std::string tag;
        
		explicit EntityAnimation(std::vector<BattleAnimation*> *animations, bool loop = true, float lifeTime = -1, std::string* pTag = nullptr) : loop(loop), lifeTime(lifeTime)
        {
            battleAnimation = new BattleAnimationGroup(animations);
            battleAnimation->playCurrent(loop);
            if (pTag != nullptr) tag = *pTag;
        }
        
        ~EntityAnimation()
        {
            delete battleAnimation;
        }
    };
    
    struct BuffAnimation : entityx::Component<BuffAnimation>
    {
        BattleAnimationGroup* battleAnimation;
        bool loop;
        float lifeTime;
        
		explicit  BuffAnimation(std::vector<BattleAnimation*> *animations, bool loop = true, float lifeTime = -1) : loop(loop), lifeTime(lifeTime)
        {
            battleAnimation = new BattleAnimationGroup(animations);
            battleAnimation->playCurrent(loop);
        }
        
        ~BuffAnimation()
        {
            delete battleAnimation;
        }
    };
    
    struct ActiveEntity : entityx::Component<ActiveEntity>
    {
    };
    
	struct Position : entityx::Component<Position>
	{
		explicit Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
	    float x, y, z;
        
        void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
        void add(float x, float y, float z) { this->x += x; this->y += y; this->z += z; }
        void sub(float x, float y, float z) { this->x -= x; this->y -= y; this->z -= z; }
	};

	struct Direction : entityx::Component<Direction>
	{
		explicit Direction(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
	    float x, y, z;
        
        void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	};
    
    struct Target : entityx::Component<Target>
	{
		explicit  Target(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
	    float x, y, z;
        
        void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	};
    
    struct TargetEntity : entityx::Component<TargetEntity>
    {
		explicit TargetEntity(const entityx::Entity& targetEntity) : targetEntity(targetEntity) {}
        entityx::Entity targetEntity;
    };
    
    struct Identify : entityx::Component<Identify>
	{
        int id;
        BattleConfig::EntityType type;
        BattleConfig::Side side;
        BattleConfig::FaceTo faceTo;
        
		explicit Identify(int id, BattleConfig::EntityType type, BattleConfig::Side side, BattleConfig::FaceTo faceTo) : id(id), type(type), side(side), faceTo(faceTo) {}
    };
    
    struct Property : entityx::Component<Property>
    {
    private:
        std::array<bool, BattleConfig::SIZE_OF_PROPERTY_NAME> m_lock;
        std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME> m_current;
        std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME> m_original;
        std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME> m_min;
        std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME> m_max;
        
    public:
		explicit Property(const std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME>& data,
                  const std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME>& dataMin,
				  const std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME>& dataMax);
		void setDataMin(std::array<float, BattleConfig::SIZE_OF_PROPERTY_NAME>& dataMin) { m_min = dataMin; }
        float get(BattleConfig::Property name);
        void set(BattleConfig::Property name, float value);
        float getOriginal(BattleConfig::Property name);
        bool isLocked(BattleConfig::Property name);
        void setLock(BattleConfig::Property name, bool lock);

    };

    
    struct Attack : entityx::Component<Attack>
    {
		explicit Attack(const BattleConfig::AttackData& attackData) : attackData(attackData) {}
        
        BattleConfig::AttackData attackData;
    };
    
    
    struct GasGather:entityx::Component<GasGather>
    {
        std::vector<BattleConfig::SkillData*>& skills;
        bool lock = false;
        
		explicit GasGather(std::vector<BattleConfig::SkillData*>& skills) : skills(skills) {}
    };
    
    struct Buff : entityx::Component<Buff>
    {
        std::vector<BattleConfig::BuffData*> buffList;
    
		explicit Buff() {}
        
        ~Buff()
        {
            for(BattleConfig::BuffData *buffData : buffList)
            {
                if (buffData != NULL) delete buffData;
            }
            buffList.clear();
        }
    };
    
    struct General : entityx::Component<General>
	{
        int target;
        BattleConfig::HeroConfigData config;

		explicit General(const BattleConfig::HeroConfigData& config) : config(config), target(0) {}
        ~General() { CCLOG("general clear"); }
    };
    
	struct Soldier : entityx::Component<Soldier>
	{
        int target;
        BattleConfig::SoldierConfigData config;
        
		explicit Soldier(const BattleConfig::SoldierConfigData& config) : config(config), target(0) {}
        void cleanTarget() { target = 0; }
	};


	struct AnchorArrowData : entityx::Component<AnchorArrowData>
    {
        enum STATE
        {
            WORK,
            DEAD,
        };
        
        entityx::Entity m_firer;
        STATE state = WORK;
        
		explicit AnchorArrowData(entityx::Entity firer): m_firer(firer) {}
    };
    
	struct MiniIcon : entityx::Component<MiniIcon>
    {
        cocos2d::Node* m_pIcon;
		explicit MiniIcon(cocos2d::Node* m_pIcon): m_pIcon(m_pIcon) {}
    };
    
    struct AI : entityx::Component<AI>
    {
        float lastAttackTime = 0;
        int lastResearchTargetTicket = -1;
        int lastResearchBowManTargetTicket = -1;

        std::string strategy;
        std::vector<BattleConfig::AIData*> ai;
        
        bool isTileLosted = false;
        
        ~AI()
        {
            clearAllAI();
        }
        
        void clearAllAI()
        {
            for (auto* pData : ai)
            {
                delete pData;
            }
            ai.clear();
        }
        
        void clearCompletedAI();
        
    };
    
    struct ObjectStateMachine : entityx::Component<ObjectStateMachine>
    {
        bool isDead = false;
        StateMachine stateMachine;

		explicit ObjectStateMachine(entityx::Entity& entity):
        stateMachine(entity)
        {
        }
        virtual ~ObjectStateMachine()
        {
        }
        
        void changeState(IState* pState, bool acceptDelegate = true) { stateMachine.changeState(pState, acceptDelegate); }
        IState* getCurrentState() { return stateMachine.getCurrentState(); }
        int getCurrentStateCode() { return stateMachine.getCurrentStateCode(); }
        int update(float dt) { return stateMachine.update(dt); }
    };
    

    
    //TO-DO check play current
    struct Effect : entityx::Component<Effect>
	{
        float delay, duration;
        bool follow;
        int fromId;
        
        float elapse = 0;
        bool playing = false;
        
        entityx::Entity followTarget; //follow mode
        
		explicit Effect(float delay = 0, float duration = 0, bool follow = false, int fromId = 0) :
               delay(delay), duration(duration), follow(follow), fromId(fromId) {}
    };
    
    enum SkillArrowTrackState
    {
        ARROW_FLYING,
        ARROW_TRACKING
    };
    
    struct SkillArrow : entityx::Component<SkillArrow>
    {
        std::vector<int> targets;//移动过程中遇到的敌人对象
        BattleConfig::SkillArrowData arrowData;
        int startX;//init when add
        int startZ;//init when add
        SkillArrowTrackState track = ARROW_FLYING;
        bool hitHero;
        bool destroyAfterHitHero;
        
        float phase;
        
        int m_audioId;
        
		explicit SkillArrow(const BattleConfig::SkillArrowData& arrowData, int startX, int startZ, bool destroyAfterHitHero = true);
        ~SkillArrow();
    };
    
	struct FlyTextEffect : entityx::Component<FlyTextEffect>
    {
        std::vector<std::vector<BattleAnimation*>*> animations;
        float delay;
        float interval;
        
		explicit FlyTextEffect(float interval, float delay = 0) : interval(interval), delay(delay) {}
        
        ~FlyTextEffect();
    };
    
	struct ShakeCom : entityx::Component<ShakeCom>
    {
        float delay, duration, elapse, amplitude;
        int mode, direction, interval, index;
        //float orgX, orgY, orgZ;
        bool damping;
        
		explicit ShakeCom(float duration = 0.0f, float amplitude = 10.0f, int mode = 0, int interval = 2, float delay = 0.0f, bool damping = true) :
                 duration(duration), amplitude(amplitude), mode(mode), interval(interval), delay(delay)
        {
            elapse = 0.0f;
            direction = 1;
            index = 0;
        }
        
        ~ShakeCom()
        {
        }
    };
    
    struct CameraFollowData
    {
        bool following;
        float toY;
        float toZ;
        float r3D;
        float r2D;
        entityx::Entity followTarget; //follow mode
    };

	struct Camera : entityx::Component<Camera>
	{
//        int cameraMode;
        int cameraLastMode;
        CameraFollowData followData;
        std::vector<BattleConfig::CameraAction*> cameraQueue;
        
        int cameraModeX;
        std::vector<BattleConfig::CameraActionData*> cameraXQueue;
        int cameraModeY;
        std::vector<BattleConfig::CameraActionData*> cameraYQueue;
        float backYDelay;
        int cameraModeZ;
        std::vector<BattleConfig::CameraActionData*> cameraZQueue;
        float backZDelay;
        int cameraModeR3d;
        std::vector<BattleConfig::CameraActionData*> cameraR3dQueue;
        float backR3dDelay;
        int cameraModeR2d;
        std::vector<BattleConfig::CameraActionData*> cameraR2dQueue;
        float backR2dDelay;
        int cameraModeRY;
        std::vector<BattleConfig::CameraActionData*> cameraRYQueue;
        float backRYDelay;
        
        int64_t causeId;
	};
    
	struct Skill : entityx::Component<Skill>
	{
        SkillObject* skill = nullptr;
        float elapse;
        int fromId;
        std::vector<int> m_playingSoundPool;
	    
		explicit Skill(SkillObject* skill, int fromId) : skill(skill), elapse(0), fromId(fromId) {}
        ~Skill();
	};
    
	struct HeroStrategy : entityx::Component<HeroStrategy>
	{
        StrategyControl::HERO_CONTROL_TYPE type;
        
		explicit HeroStrategy(StrategyControl::HERO_CONTROL_TYPE type = StrategyControl::HERO_STOP) : type(type) {}
    };

	struct ActorMode : entityx::Component<ActorMode>
    {
        ModeGroup m_modeGroup;
        
		explicit ActorMode(entityx::Entity& entity):
        m_modeGroup(entity)
        {

        }
        ~ActorMode()
        {

        }
    };
    
	struct Combo : entityx::Component<Combo>
    {
        int m_comboTimes;
        float m_lastComboTime;

        Combo()
        {
        }
        ~Combo()
        {
        }
        
    };
    
	struct SkillTileCenter : entityx::Component<SkillTileCenter>
    {
        int tileZ, tileX;
        
		explicit SkillTileCenter(int tileX, int tileZ) : tileX(tileX), tileZ(tileZ) {}
    };
    
	struct SkillTileTarget : entityx::Component<SkillTileTarget>
    {
        int tileZ, tileX;
        
		explicit SkillTileTarget(int tileX, int tileZ) : tileX(tileX), tileZ(tileZ) {}
    };
    
	struct SkillTileResults : entityx::Component<SkillTileResults>
    {
        BattleConfig::TileResults results;
        
		explicit SkillTileResults(const BattleConfig::TileResults& fromResults)
        {
            for (auto* pTile : fromResults.tiles)
            {
                results.add(pTile->tileZ, pTile->tileX);
            }
        }
    };
    
	struct SkillTargetResults : entityx::Component<SkillTargetResults>
    {
        std::vector<int> targets;
        
		explicit SkillTargetResults(const std::vector<int>& targets) : targets(targets) {}
    };
    
	struct SkillDuration : entityx::Component<SkillDuration>
    {
        float duration;
        
		explicit SkillDuration(float duration) : duration(duration) {}
    };
    
	struct SkillReduceEnemyMp : entityx::Component<SkillReduceEnemyMp>
    {
        float actualReduceValue;
        
		explicit SkillReduceEnemyMp(float actualReduceValue) : actualReduceValue(actualReduceValue) {}
    };
};