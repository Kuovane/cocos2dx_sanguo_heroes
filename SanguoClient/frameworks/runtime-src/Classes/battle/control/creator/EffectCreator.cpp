//
//  EffectCreator.cpp
//  Game
//
//  Created by fuchenhao on 3/19/15.
//
//

#include "EffectCreator.h"
#include "BattleComponent.h"
#include "BattleWorld.h"
#include "TweenQueue.h"

//using namespace std;

EffectCreator::EffectCreator()
{
}

EffectCreator::~EffectCreator()
{
}

void EffectCreator::createNormalAttackEffect(entityx::Entity& fromEntity, const std::string& animationName)
{
	auto identify = fromEntity.component<BattleComponent::Identify>();
	auto animation = fromEntity.component<BattleComponent::Animation>();

	auto* pDefine = AssetDefineCache::getInstance()->getAsset(animation->assetName);
	if (pDefine && pDefine->getAnimation(animationName))
	{
		bool reverse = (identify->faceTo == BattleConfig::FaceTo::FACE_TO_RIGHT);
		auto* pEffectGroup = _BATTLE_ANIMATION_CREATOR.convertAnimationToEffectGroup(animation->assetName, animationName, AdvancedAnimation::BlendMode::BLEND_ADD, 1);
		_ENTITY_EVENT.emit<BattleEvent::AddEffectToEntity>(pEffectGroup, reverse, fromEntity, 0, 0, identify->id);
		CC_SAFE_DELETE(pEffectGroup);
	}
}

void EffectCreator::createHitEffect(entityx::Entity& fromEntity, entityx::Entity& targetEntity, const BattleConfig::AttackData& attackData)
{
	auto targetIdentify = targetEntity.component<BattleComponent::Identify>();
	bool isFromHero = fromEntity.component<BattleComponent::Identify>()->type == BattleConfig::ENTITY_HERO;
	bool isTargetHero = targetIdentify->type == BattleConfig::ENTITY_HERO;
	auto targetPos = targetEntity.component<BattleComponent::Position>();
	auto fromPos = fromEntity.component<BattleComponent::Position>();
	cocos2d::Vec3 rotation(0, 0, cocos2d::random<int>(0, 360));
	float x = 0;
	float y = 0;
	float z = 0;

	if (attackData.getAttackType() == BattleConfig::AttackType::SKILL)
	{
		if (attackData.damageData.showDefaultSkillHitEffect)
		{
			if (isTargetHero)
			{
				x += targetPos->x + (targetPos->x > fromPos->x ? -HALF_GRID_WIDTH : HALF_GRID_WIDTH) + (cocos2d::rand_minus1_1() > 0 ? 1 : -1) * HERO_HEIGHT / 10;
				y += targetPos->y + HERO_HEIGHT / 2 + (cocos2d::rand_minus1_1() > 0 ? 1 : -1) * HERO_HEIGHT / 10;
				z += targetPos->z;

				auto animations = _BATTLE_ANIMATION_CREATOR.create("attack", "animation_hit_hero_hit_hero_layer1", x, y, z, false, rotation);
				_ENTITY_EVENT.emit<BattleEvent::AddEffectSimple>(animations, x, y, z);
			}
			else
			{
				x += targetPos->x;
				y += targetPos->y + SOLDIER_MELEE_HEIGHT;
				z += targetPos->z;

				auto animations = _BATTLE_ANIMATION_CREATOR.create("attack", "animation_hit_hero_hit_soldier_layer1", x, y, z, false, rotation);
				_ENTITY_EVENT.emit<BattleEvent::AddEffectSimple>(animations, x, y, z);
			}
		}
	}
	else if (isTargetHero)
	{
		x += targetPos->x + (targetPos->x > fromPos->x ? -HALF_GRID_WIDTH : HALF_GRID_WIDTH);
		y += targetPos->y + HERO_HEIGHT / 2;
		z += targetPos->z;

		const std::string& aniName = (isFromHero ? "animation_hit_hero_hit_hero_layer1" : "animation_hit_soldier_hit_hero_layer1");
		auto animations = _BATTLE_ANIMATION_CREATOR.create("attack", aniName, x, y, z, false, rotation);
		_ENTITY_EVENT.emit<BattleEvent::AddEffectSimple>(animations, x, y, z);
	}
	else if (isFromHero)
	{
		// ����ʿ��
		//TODO, lrui, ����λ��̫�ߣ���Ҫ����
		x += targetPos->x;
		y += targetPos->y + SOLDIER_MELEE_HEIGHT;//TODO, ֧�ֲ�ͬ�ı�������
		z += targetPos->z;

		auto animations = _BATTLE_ANIMATION_CREATOR.create("attack", "animation_hit_hero_hit_soldier_layer1", x, y, z, false, rotation);
		_ENTITY_EVENT.emit<BattleEvent::AddEffectSimple>(animations, x, y, z);
	}
	else
	{
		// ʿ����ʿ��������Ҫ�����ܻ���Ч
		auto targetProperty = targetEntity.component<BattleComponent::Property>();
		if (targetProperty->get(BattleConfig::Property::HP) <= 0)
		{
			x += targetPos->x;
			y += targetPos->y + SOLDIER_MELEE_HEIGHT / 2;//TODO, ֧�ֲ�ͬ�ı�������
			z += targetPos->z;

			auto animations = _BATTLE_ANIMATION_CREATOR.create("attack", "animation_hit_hero_hit_soldier_layer1", x, y, z, false, rotation);
			_ENTITY_EVENT.emit<BattleEvent::AddEffectSimple>(animations, x, y, z);
		}
		// ʿ����ʿ��������Ч
		else
		{
			_ENTITY_EVENT.emit<BattleEvent::KnockBackSoldier>(targetEntity, fromEntity, GRID_WIDTH / 4);
		}
	}

	if (!isTargetHero)
	{
		createTurnRedEffect(targetEntity);
	}
}

void EffectCreator::createTurnRedEffect(entityx::Entity& targetEntity)
{
	auto animation = targetEntity.component<BattleComponent::Animation>();

	//    auto orgG = animation->battleAnimation->getColorG();
	//    auto orgB = animation->battleAnimation->getColorB();

	float orgR = 1.0f;
	float orgG = 1.0f;
	float orgB = 1.0f;

	animation->m_tweenQueue.clearTween();
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::R, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 1.0, 1.0), 0.0);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::G, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 0.4, 0.4), 0.0);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::B, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 0.4, 0.4), 0.0);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::R, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 0.5, 0.5), 0.1);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::G, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 0.5, 0.5), 0.1);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::B, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 1.0, 1.0), 0.1);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::R, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 1.0, 1.0), 0.2);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::G, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 0.4, 0.4), 0.2);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::B, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, 0.4, 0.4), 0.2);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::R, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, orgR, orgR), 0.3);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::G, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, orgG, orgG), 0.3);
	animation->m_tweenQueue.pushTween(TweenQueue::TARGET::B, TweenQueue::OPERATION::IN_T, new SimpleTween_linear(0.0, orgB, orgB), 0.3);
}

void EffectCreator::createMissEffect(entityx::Entity& targetEntity)
{
	std::map<std::string, cocos2d::Node*> replaces;

	char strName[10] = {};
	sprintf(strName, "δ����");

	auto labelMiss = cocos2d::Label::createWithBMFont("fonts/Texts.fnt", strName);
	labelMiss->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
	replaces.insert(std::pair<std::string, cocos2d::Node*>("asset_placeholder", labelMiss));

	auto& appearPosition = getFlyTextAppearPosition(targetEntity);
	auto animations = _BATTLE_ANIMATION_CREATOR.create("damage", "animation_hitNum", replaces, appearPosition.x, appearPosition.y, appearPosition.z);
	_ENTITY_EVENT.emit<BattleEvent::AddEffectFlyText>(targetEntity, animations, true);
}

void EffectCreator::createHPDecreaseEffect(entityx::Entity& fromEntity, entityx::Entity& targetEntity, int value, bool isCritical)
{
	bool isFromSoldier = fromEntity.component<BattleComponent::Identify>()->type == BattleConfig::ENTITY_SOLDIER;
	bool isTargetSoldier = targetEntity.component<BattleComponent::Identify>()->type == BattleConfig::ENTITY_SOLDIER;

	// ������������˺�����
	if (isFromSoldier && isTargetSoldier)
	{
		return;
	}

	std::map<std::string, cocos2d::Node*> replaces;
	char strName[50] = {};
	if (isCritical)
	{
		sprintf(strName, "����");
		auto labelCritical = cocos2d::Label::createWithBMFont("fonts/Texts.fnt", strName);
		labelCritical->setAnchorPoint(cocos2d::Vec2(1.0f, 0.5f));
		replaces.insert(std::pair<std::string, cocos2d::Node*>("asset_placeholderR", labelCritical));

		memset(strName, 0, 50);
		sprintf(strName, "%d", value);
		auto labelDamage = cocos2d::Label::createWithBMFont("fonts/RedSmall.fnt", strName);
		labelDamage->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
		replaces.insert(std::pair<std::string, cocos2d::Node*>("asset_placeholderL", labelDamage));
	}
	else
	{
		memset(strName, 0, 50);
		sprintf(strName, "%d", value);
		auto labelDamage = cocos2d::Label::createWithBMFont("fonts/RedSmall.fnt", strName);
		labelDamage->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
		replaces.insert(std::pair<std::string, cocos2d::Node*>("asset_placeholder", labelDamage));
	}

	auto& appearPosition = getFlyTextAppearPosition(targetEntity);
	auto animations = _BATTLE_ANIMATION_CREATOR.create("damage", (isCritical ? "animation_hitNumText" : "animation_hitNum"), replaces, appearPosition.x, appearPosition.y, appearPosition.z);
	bool fireNow = isCritical;
	_ENTITY_EVENT.emit<BattleEvent::AddEffectFlyText>(targetEntity, animations, fireNow);
}

void EffectCreator::createHPIncreaseEffect(entityx::Entity& fromEntity, entityx::Entity& targetEntity, int value, bool fireNow)
{
	std::map<std::string, cocos2d::Node*> replaces;
	char strName[50] = {};
	sprintf(strName, "%d", value);
	auto labelAdd = cocos2d::Label::createWithBMFont("fonts/GreenSmall.fnt", strName);
	labelAdd->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
	replaces.insert(std::pair<std::string, cocos2d::Node*>("asset_placeholder", labelAdd));

	auto& appearPosition = getFlyTextAppearPosition(targetEntity);
	auto animations = _BATTLE_ANIMATION_CREATOR.create("damage", "animation_hitNum", replaces, appearPosition.x, appearPosition.y, appearPosition.z);
	_ENTITY_EVENT.emit<BattleEvent::AddEffectFlyText>(targetEntity, animations, fireNow);
}

void EffectCreator::createMPChangeEffect(entityx::Entity& fromEntity, entityx::Entity& targetEntity, int value, bool increase, bool fireNow)
{
	std::map<std::string, cocos2d::Node*> replaces;
	char strName[50] = {};
	sprintf(strName, "%s%d", (increase ? "+" : "-"), abs(value));
	auto labelValue = cocos2d::Label::createWithBMFont("fonts/BlueSmall.fnt", strName);
	labelValue->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
	replaces.insert(std::pair<std::string, cocos2d::Node*>("asset_placeholder", labelValue));

	auto& appearPosition = getFlyTextAppearPosition(targetEntity);
	auto animations = _BATTLE_ANIMATION_CREATOR.create("damage", "animation_hitNum", replaces, appearPosition.x, appearPosition.y, appearPosition.z);
	_ENTITY_EVENT.emit<BattleEvent::AddEffectFlyText>(targetEntity, animations, fireNow);
}

const cocos2d::Vec3& EffectCreator::getFlyTextAppearPosition(entityx::Entity& targetEntity)
{
	auto targetPos = targetEntity.component<BattleComponent::Position>();
	auto targetIdentify = targetEntity.component<BattleComponent::Identify>();
	bool isTargetSoldier = targetIdentify->type == BattleConfig::ENTITY_SOLDIER;

	float x = targetPos->x;
	float y = targetPos->y;
	float z = targetPos->z;

	if (isTargetSoldier)//TODO, lrui, ֧�ֲ�ͬ�ı�������
	{
		y += SOLDIER_MELEE_HEIGHT;
	}
	else
	{
		y += HERO_HEIGHT;
	}
	m_tempPosition.set(x, y, z);
	return m_tempPosition;
}
