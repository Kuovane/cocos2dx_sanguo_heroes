//
//  AttackSystem.cpp
//  Game
//
//  Created by fuchenhao on 2/26/15.
//
//

#include "AttackSystem.h"
#include "BattleWorld.h"
#include "BattleComponent.h"
#include "BattleEvent.h"
#include "AICommonHelper.h"
#include "AudioSystem.h"

using namespace entityx;


AttackSystem::AttackSystem()
{
	m_CSDRFloorCoefficient = BattleConfig::getInstance()->getBattleFieldConfig().CSDRFloorCoefficient;
}

void AttackSystem::configure(entityx::EventManager& events)
{
	events.subscribe<BattleEvent::AddAttack>(*this);
	events.subscribe<BattleEvent::BattlePause>(*this);
	events.subscribe<BattleEvent::BattleResume>(*this);
}

void AttackSystem::receive(const BattleEvent::AddAttack& e)
{
	if (e.attackData.getAttackTargetType() == BattleConfig::ATTACK_POSITION && (!MapHelper::validTile(e.attackData.getTargetTileZ(), e.attackData.getTargetTileX())))
	{
		return;
	}

	entityx::Entity entity = _ENTITY_MANAGER.create();
	entity.assign<BattleComponent::ActiveEntity>();
	entity.assign<BattleComponent::Attack>(e.attackData);
}

void AttackSystem::receive(const BattleEvent::BattlePause& e)
{
	m_isPausing = true;
}

void AttackSystem::receive(const BattleEvent::BattleResume& e)
{
	m_isPausing = false;
}

void AttackSystem::update(EntityManager &es, EventManager &events, double dt)
{
	BattleComponent::Attack::Handle attack;

	for (entityx::Entity attackEntity : es.entities_with_components(attack))
	{
		if ((!m_isPausing) || attackEntity.has_component<BattleComponent::ActiveEntity>())
		{
			if (attack->attackData.delay > 0)
			{
				attack->attackData.delay -= dt;
				if (attack->attackData.delay > 0) continue;
			}

			BattleConfig::AttackData& attackData = attack->attackData;
			Entity* pFromEntity = _BATTLE_ENTITY.getEntity(attackData.fromId);
			Entity* pTargetEntity = NULL;

			if (pFromEntity == NULL)
			{
				attackEntity.destroy();
				continue;
			}

			// ����Ŀ��Ĺ���
			if (attackData.getAttackTargetType() == BattleConfig::ATTACK_TARGET)
			{
				pTargetEntity = _BATTLE_ENTITY.getEntityEvenIfDead(attackData.getTargetId());
			}
			// ����λ�õĹ���
			else if (attackData.getAttackTargetType() == BattleConfig::ATTACK_POSITION)
			{
				int targetId = _BATTLE_ENTITY.getTarget(attackData.getTargetTileZ(), attackData.getTargetTileX());
				if (targetId > 0)
				{
					pTargetEntity = _BATTLE_ENTITY.getEntityEvenIfDead(targetId);
					attackData.setTargetId(targetId);
				}
			}

			if (pTargetEntity == NULL)
			{
				attackEntity.destroy();
				continue;
			}

			// TODO, �Ƿ���Ҫ���targetEntity�Ƿ�����ͬ�����͵ĳ����˺��Ҹ��˺����ܵ���
			// TODO, �Ƿ���Ҫ���targetEntity�Ƿ���fromEntity�Ĺ�����Χ�ڣ�
			doHurt(*pFromEntity, *pTargetEntity, attackData);

			if (attack->attackData.loop > 0)
			{
				attack->attackData.loop--;
				attack->attackData.isFirstHit = false;
				attack->attackData.delay = attack->attackData.interval;
			}

			if (attack->attackData.loop == 0)
			{
				attackEntity.destroy();
			}
		}
	}
}

bool AttackSystem::canNormalAttackTarget(entityx::Entity& fromEntity, entityx::Entity& targetEntity, const BattleConfig::AttackData& attackData)
{
	int fromId = attackData.fromId;

	if (MapHelper::isHero(fromId))
	{
		auto fromState = fromEntity.component<BattleComponent::ObjectStateMachine>();
		if (fromState->getCurrentStateCode() != BattleConfig::GeneralState::GENERAL_ATTACK)
		{
			return false;
		}
	}

	return true;
}

bool AttackSystem::canSkillAttackTarget(entityx::Entity& fromEntity, entityx::Entity& targetEntity, const BattleConfig::AttackData& attackData)
{
	bool correctSide = false;
	bool correctEntity = false;

	int fromId = attackData.fromId;
	int targetId = attackData.getTargetId();

	switch (attackData.damageData.damageSide) {
	case BattleConfig::DamageSide::ENEMY:
		correctSide = (MapHelper::getSide(fromId) != MapHelper::getSide(targetId));
		break;
	case BattleConfig::DamageSide::SELF:
		correctSide = (MapHelper::getSide(fromId) == MapHelper::getSide(targetId));
		break;
	case BattleConfig::DamageSide::BOTH:
		correctSide = true;
		break;
	default:
		break;
	}

	switch (attackData.damageData.damageEntityType) {
	case BattleConfig::DAMAGE_ENTITY_SOLDIER:
		correctEntity = MapHelper::getEntityType(targetId) == BattleConfig::ENTITY_SOLDIER;
		break;
	case BattleConfig::DAMAGE_ENTITY_HERO:
		correctEntity = MapHelper::getEntityType(targetId) == BattleConfig::ENTITY_HERO;
		break;
	case BattleConfig::DAMAGE_ENTITY_ALL:
		correctEntity = true;
		break;
	default:
		break;
	}

	return correctSide && correctEntity;
}

void AttackSystem::doHurt(entityx::Entity& fromEntity, entityx::Entity& targetEntity, BattleConfig::AttackData& attackData)
{
	if (isDead(fromEntity))
	{
		return;
	}

	float hurt = 0;
	float vamp = 0;
	bool isCritical = false;
	auto fromProperty = fromEntity.component<BattleComponent::Property>();
	auto targetProperty = targetEntity.component<BattleComponent::Property>();
	auto actorMode = targetEntity.component<BattleComponent::ActorMode>();

	if (attackData.getAttackType() == BattleConfig::AttackType::SKILL)
	{
		if (!canSkillAttackTarget(fromEntity, targetEntity, attackData))
		{
			return;
		}

		int skillLevel = getSkillLevel(fromEntity, attackData.skillId);

		switch (attackData.damageData.damageType)
		{
		case BattleConfig::DAMAGE_PHYSICAL:
			if (!actorMode.valid() || !actorMode->m_modeGroup.isModeActive(ActorMode::MODE_TYPE::NO_DMG_PHYSICAL))
			{
				hurt = computeSkillPhysicalHurt(fromProperty, targetProperty, attackData.damageData, skillLevel, isCritical);
			}
			break;
		case BattleConfig::DAMAGE_MAGIC:
			if (!actorMode.valid() || !actorMode->m_modeGroup.isModeActive(ActorMode::MODE_TYPE::NO_DMG_MAGIC))
			{
				hurt = computeSkillMagicHurt(fromProperty, targetProperty, attackData.damageData, skillLevel, isCritical);
			}
			break;
		case BattleConfig::DAMAGE_HOLY:
			hurt = computeSkillHolyHurt(attackData.damageData);
			break;
		case BattleConfig::DAMAGE_KILL:
			hurt = computeSkillKillHurt(targetProperty);
			break;
		case BattleConfig::DAMAGE_PERCENT:
			hurt = computeSkillPercentHurt(targetProperty, attackData.damageData);
			break;
		case BattleConfig::DAMAGE_CURE:
			hurt = computeCureHurt(fromProperty, targetProperty, attackData.damageData, skillLevel, isCritical);
			break;
		default:
			break;
		}

		if (targetEntity.component<BattleComponent::Identify>()->type == BattleConfig::EntityType::ENTITY_HERO)
		{
			std::string eventName = AUDIOEVENTS_HERO_SKILL_HURT;
			std::string triggerName = "skill_";
			triggerName += cocos2d::StringUtils::format("%d", attackData.skillId);
			BattleEvent::BattleSound soundEvent(eventName, triggerName, targetEntity);
			_ENTITY_EVENT.emit(soundEvent);
		}
		else
		{
			std::string eventName = AUDIOEVENTS_SOLDIER_SKILL_HURT;
			std::string triggerName = "skill_";
			triggerName += cocos2d::StringUtils::format("%d", attackData.skillId);
			BattleEvent::BattleSound soundEvent(eventName, triggerName, targetEntity);
			_ENTITY_EVENT.emit(soundEvent);
		}
	}
	else
	{
		if (attackData.damageData.damageType == BattleConfig::DAMAGE_KILL)
		{
			hurt = targetProperty->get(BattleConfig::Property::HP);
		}
		else
		{
			if (!canNormalAttackTarget(fromEntity, targetEntity, attackData))
			{
				return;
			}

			bool isDodge = isNormalHurtDodge(fromProperty, targetProperty);
			if (isDodge)
			{
				{
					std::string eventName = AUDIOEVENTS_HERO_DODGE;
					std::string triggerName = "hero";
					BattleEvent::BattleSound soundEvent(eventName, triggerName, targetEntity);
					_ENTITY_EVENT.emit(soundEvent);
				}

				{
					std::string eventName = AUDIOEVENTS_SOLDIER_DODGE;
					std::string triggerName = "soldier";
					BattleEvent::BattleSound soundEvent(eventName, triggerName, targetEntity);
					_ENTITY_EVENT.emit(soundEvent);
				}


				_EFFECT_CREATOR.createMissEffect(targetEntity);
				return;
			}

			isCritical = isNormalHurtCritical(fromProperty, targetProperty);

			BattleConfig::NormalDamageType normalDamageType = getNormalDamageType(fromEntity);
			switch (normalDamageType)
			{
			case BattleConfig::NormalDamageType::PHYSICAL:
				hurt = computeNormalPhysicalHurt(fromProperty, targetProperty, isCritical);
				break;
			case BattleConfig::NormalDamageType::MAGIC:
				hurt = computeNormalMagicHurt(fromProperty, targetProperty, isCritical);
				break;
			default:
				break;
			}
		}
		vamp = computeNormalVamp(fromProperty, hurt);


		if (fromEntity.component<BattleComponent::Identify>()->type == BattleConfig::EntityType::ENTITY_HERO)
		{
			std::string triggerName = getHeroHittingAudioTriggerName(fromEntity);
			if (triggerName != "")
			{
				std::string eventName = AUDIOEVENTS_HERO_HIT;
				BattleEvent::BattleSound soundEvent(eventName, triggerName, fromEntity);
				_ENTITY_EVENT.emit(soundEvent);
			}
		}
		else
		{
			std::string triggerName = getSoldierHittingAudioTriggerName(fromEntity);
			if (triggerName != "")
			{
				std::string eventName = AUDIOEVENTS_SOLDIER_HIT;
				BattleEvent::BattleSound soundEvent(eventName, triggerName, fromEntity);
				_ENTITY_EVENT.emit(soundEvent);
			}
		}


	}

	attackData.finalHpChanged = changeHP(targetEntity, -hurt);

	_ENTITY_EVENT.emit<BattleEvent::HurtByAttack>(fromEntity, targetEntity, attackData);

	if (attackData.damageData.triggerSkillActionOnAttack != nullptr)
	{
		_ENTITY_EVENT.emit<BattleEvent::TriggerSkillAction>(attackData.skillEntityId, *attackData.damageData.triggerSkillActionOnAttack);
	}

	if (hurt > 0)
	{
		if (attackData.damageData.showHpFlyText)
		{
			_EFFECT_CREATOR.createHPDecreaseEffect(fromEntity, targetEntity, hurt, isCritical);
		}

		_EFFECT_CREATOR.createHitEffect(fromEntity, targetEntity, attackData);
	}
	else if (hurt < 0)
	{
		if (attackData.damageData.showHpFlyText)
		{
			_EFFECT_CREATOR.createHPIncreaseEffect(fromEntity, targetEntity, -hurt, false);
		}
	}

	if (targetProperty->get(BattleConfig::Property::HP) <= 0 && !isDead(targetEntity))
	{
		_ENTITY_EVENT.emit<BattleEvent::BattleActorDead>(targetEntity, attackData);

		changeGas(fromEntity, targetEntity);
	}

	if (vamp > 0)
	{
		changeHP(fromEntity, vamp);

		_EFFECT_CREATOR.createHPIncreaseEffect(fromEntity, fromEntity, vamp, true);
	}

	if (targetEntity.component<BattleComponent::Identify>()->type == BattleConfig::EntityType::ENTITY_HERO)
	{
		//        std::string eventName = AUDIO_EVENTS_HERO_PHYSICAL_HURT;
		//        std::string triggerName = getHeroSoldierAudioTriggerName(targetEntity);
		//        BattleEvent::BattleSound soundEvent(eventName, triggerName);
		//        _ENTITY_EVENT.emit(soundEvent);
	}
	else
	{
		//        std::string eventName = AUDIO_EVENTS_SOLDIER_PHYSICAL_HURT;
		//        std::string triggerName = getHeroSoldierAudioTriggerName(targetEntity);
		//        BattleEvent::BattleSound soundEvent(eventName, triggerName);
		//        _ENTITY_EVENT.emit(soundEvent);
	}


}

int AttackSystem::getSkillLevel(entityx::Entity& fromEntity, int skillId)
{
	auto gasGather = fromEntity.component<BattleComponent::GasGather>();
	for (auto* pSkillData : gasGather->skills)
	{
		if (pSkillData->id == skillId)
		{
			return pSkillData->level;
		}
	}
	return 1;
}

BattleConfig::NormalDamageType AttackSystem::getNormalDamageType(entityx::Entity& fromEntity)
{
	auto hero = fromEntity.component<BattleComponent::General>();
	if (hero.valid())
	{
		return hero->config.normalDamageType;
	}
	auto soldier = fromEntity.component<BattleComponent::Soldier>();
	if (soldier.valid())
	{
		return soldier->config.normalDamageType;
	}
	return BattleConfig::NormalDamageType::UNKNOWN;
}

float AttackSystem::changeHP(entityx::Entity& entity, float delta)
{
	auto actorMode = entity.component<BattleComponent::ActorMode>();
	auto property = entity.component<BattleComponent::Property>();

	float maxHp = property->getOriginal(BattleConfig::Property::HP);
	float curHp = property->get(BattleConfig::Property::HP);
	float hp = curHp + delta;
	if (hp > maxHp) hp = maxHp;
	if (hp < 0) hp = 0;
	if (actorMode.valid() && actorMode->m_modeGroup.isModeActive(ActorMode::MODE_TYPE::NO_DEAD) && hp < 1) hp = 1;

	property->set(BattleConfig::Property::HP, hp);

	return (hp - curHp);
}

void AttackSystem::changeGas(entityx::Entity& attackEntity, entityx::Entity& deadEntity)
{
	auto attackIdentify = attackEntity.component<BattleComponent::Identify>();
	auto deadIdentify = deadEntity.component<BattleComponent::Identify>();

	if (deadIdentify->type == BattleConfig::ENTITY_SOLDIER)
	{
		//�佫ɱ���Է�ʿ������
		if (attackIdentify->type == BattleConfig::ENTITY_HERO && attackIdentify->side != deadIdentify->side)
		{
			_ENTITY_EVENT.emit<BattleEvent::ChangeGasEvent>(attackEntity.id(), 0, BattleConfig::GAS_KILLSOLDIER);
		}

		//�Լ�ʿ����ɱ���ؼ�����������
		auto pMainHeroEntity = _BATTLE_ENTITY.getEntity(deadIdentify->side == BattleConfig::SIDE_LEFT ? LEFT_HERO : RIGHT_HERO);
		if (pMainHeroEntity != nullptr)
		{
			_ENTITY_EVENT.emit<BattleEvent::ChangeGasEvent>(pMainHeroEntity->id(), 0, BattleConfig::GAS_SOLDIERKILLED);
		}
	}
}

bool AttackSystem::isNormalHurtDodge(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty)
{
	// ������ = �չ������� - �չ�������
	// ��������������ȷ���Ƿ����С�
	float ahr = fromProperty->get(BattleConfig::Property::ATK_HIT_RATE);
	float adr = targetProperty->get(BattleConfig::Property::ATK_DODGE_RATE);
	float hitRate = (ahr - adr) * 100;
	int diceValue = cocos2d::random<int>(1, 100);
	return diceValue > hitRate;
}

bool AttackSystem::isSkillHurtDodge(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty)
{
	// ������ = ���������� - ����������
	// ��������������ȷ���Ƿ����С�
	float ahr = fromProperty->get(BattleConfig::Property::ABILITY_HIT_RATE);
	float adr = targetProperty->get(BattleConfig::Property::ABILITY_DODGE_RATE);
	float hitRate = (ahr - adr) * 100;
	int diceValue = cocos2d::random<int>(1, 100);
	return diceValue > hitRate;
}

bool AttackSystem::isNormalHurtCritical(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty)
{
	// ������ = �չ������� - �չ�������
	float acsr = fromProperty->get(BattleConfig::Property::ATK_CSR);
	float aacsr = targetProperty->get(BattleConfig::Property::ATK_ANTI_CSR);
	float criticalHitRate = (acsr - aacsr) * 100;
	int diceValue = cocos2d::random<int>(1, 100);
	return diceValue <= criticalHitRate;
}

bool AttackSystem::isSkillHurtCritical(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty)
{
	// ������ = ���ܱ����� - ���ܱ�����
	float acsr = fromProperty->get(BattleConfig::Property::ABILITY_CSR);
	float aacsr = targetProperty->get(BattleConfig::Property::ABILITY_ANTI_CSR);
	float criticalHitRate = (acsr - aacsr) * 100;
	int diceValue = cocos2d::random<int>(1, 100);
	return diceValue <= criticalHitRate;
}

float AttackSystem::computeNormalPhysicalHurt(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty, bool isCritical)
{
	float atk = fromProperty->get(BattleConfig::Property::ATTACK);
	float arpr = fromProperty->get(BattleConfig::Property::ARPR);
	float pdr = targetProperty->get(BattleConfig::Property::PHYSICAL_DR);
	float dr = targetProperty->get(BattleConfig::Property::DR);
	float pcsdr = fromProperty->get(BattleConfig::Property::PHYSICAL_CSDR);
	float pr = targetProperty->get(BattleConfig::Property::PHYSICAL_RESILIENCE);
	float hd = fromProperty->get(BattleConfig::Property::HOLY_DAMAGE);
	float ahd = fromProperty->get(BattleConfig::Property::ATK_HOLY_DAMAGE);

	//(������ֵ * max(0.2, (�����״�͸�� - �����˺�������))) * max(0, (1 - �˺�������)) + ��ʵ�˺�ֵ
	float hurt = atk * fmax(0.2f, (arpr - pdr)) * fmax(0, (1 - dr));

	//(������ֵ * max(0.2, (�����״�͸�� - �����˺�������))) * max(0, (1 - �˺�������)) * max(0, max((�������� - ����������), ��������ϵ��)) + ��ʵ�˺�ֵ
	if (isCritical) hurt *= fmax(0.2f, fmax((pcsdr - pr), m_CSDRFloorCoefficient));

	//���������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::SHIELD);
	//�����������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::PHYSICAL_SHIELD);
	//��ʵ�˺������ӷ���
	hurt += hd + ahd;
	//��������˺���+-10
	hurt = randomizeHurtNumber(hurt);

	return ceil(hurt);
}

float AttackSystem::computeNormalMagicHurt(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty, bool isCritical)
{
	float pow = fromProperty->get(BattleConfig::Property::POWER);
	float srpr = fromProperty->get(BattleConfig::Property::SRPR);
	float sdr = targetProperty->get(BattleConfig::Property::SPELL_DR);
	float dr = targetProperty->get(BattleConfig::Property::DR);
	float scsdr = fromProperty->get(BattleConfig::Property::SPELL_CSDR);
	float sr = targetProperty->get(BattleConfig::Property::SPELL_RESILIENCE);
	float hd = fromProperty->get(BattleConfig::Property::HOLY_DAMAGE);
	float ahd = fromProperty->get(BattleConfig::Property::ABILITY_HOLY_DAMAGE);

	//(��������ֵ * max(0.2, (����������͸�� - �����˺�������)) * max(0, (1 - �˺�������)) + ��ʵ�˺�ֵ
	float hurt = pow * fmax(0.2f, (srpr - sdr)) * fmax(0, (1 - dr));

	//(��������ֵ * max(0.2, (����������͸�� - �����˺�������)) * max(0, (1 - �˺�������)) * max(0, max((���������� - ����������), ��������ϵ��)) + ��ʵ�˺�ֵ
	if (isCritical) hurt *= fmax(0.2f, fmax((scsdr - sr), m_CSDRFloorCoefficient));

	//���������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::SHIELD);
	//�������������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::SPELL_SHIELD);
	//��ʵ�˺������ӷ���
	hurt += hd + ahd;
	//��������˺���+-10
	hurt = randomizeHurtNumber(hurt);

	return ceil(hurt);
}

float AttackSystem::computeSkillPhysicalHurt(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty, const BattleConfig::DamageData& damageData, int sLevel, bool isCritical)
{
	float sBase = damageData.damageBase;
	float sStep = damageData.damageStep;
	float sFactor = damageData.damageFactor;

	float arpr = fromProperty->get(BattleConfig::Property::ARPR);
	float pdr = targetProperty->get(BattleConfig::Property::PHYSICAL_DR);
	float atk = fromProperty->get(BattleConfig::Property::ATTACK);
	float dr = targetProperty->get(BattleConfig::Property::DR);
	float pcsdr = fromProperty->get(BattleConfig::Property::PHYSICAL_CSDR);
	float pr = targetProperty->get(BattleConfig::Property::PHYSICAL_RESILIENCE);
	float hd = fromProperty->get(BattleConfig::Property::HOLY_DAMAGE);
	float ahd = fromProperty->get(BattleConfig::Property::ABILITY_HOLY_DAMAGE);

	//�չ��˺���δ������= (���ܻ����˺� + �����˺����� * ���ܵȼ� + ������ֵ * ���ܱ���) * max(0.3, (�����״�͸�� - �����˺�������)) * max(0, (1 - �˺�������)) + ��ʵ�˺�ֵ
	float hurt = (sBase + sStep * sLevel + sFactor * atk) * fmax(0.3f, (arpr - pdr)) * fmax(0, (1 - dr));
	//�չ��˺���������= (���ܻ����˺� + �����˺����� * ���ܵȼ� + ������ֵ * ���ܱ���) * max(0.3, (�����״�͸�� - �����˺�������)) * max(0, (1 - �˺�������)) * max(0, max((�������� - ����������), ��������ϵ��)) + ��ʵ�˺�ֵ
	if (isCritical) hurt *= fmax(0.3f, fmax((pcsdr - pr), m_CSDRFloorCoefficient));

	//���������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::SHIELD);
	//�����������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::PHYSICAL_SHIELD);
	//��ʵ�˺������ӷ���
	hurt += hd + ahd;
	//��������˺���+-10
	hurt = randomizeHurtNumber(hurt);

	return ceil(hurt);
}

float AttackSystem::computeSkillMagicHurt(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty, const BattleConfig::DamageData& damageData, int sLevel, bool isCritical)
{
	float sBase = damageData.damageBase;
	float sStep = damageData.damageStep;
	float sFactor = damageData.damageFactor;

	float pow = fromProperty->get(BattleConfig::Property::POWER);
	float srpr = fromProperty->get(BattleConfig::Property::SRPR);
	float sdr = targetProperty->get(BattleConfig::Property::SPELL_DR);
	float dr = targetProperty->get(BattleConfig::Property::DR);
	float scsdr = fromProperty->get(BattleConfig::Property::SPELL_CSDR);
	float sr = targetProperty->get(BattleConfig::Property::SPELL_RESILIENCE);
	float hd = fromProperty->get(BattleConfig::Property::HOLY_DAMAGE);
	float ahd = fromProperty->get(BattleConfig::Property::ABILITY_HOLY_DAMAGE);

	//���������˺���δ������= (���ܻ����˺� + �����˺����� * ���ܵȼ� + ��������ֵ * ���ܱ���) * max(0.3, (����������͸�� - �����˺�������)) * max(0, (1 - �˺�������)) + ��ʵ�˺�ֵ
	float hurt = (sBase + sStep * sLevel + sFactor * pow) * fmax(0.3f, (srpr - sdr)) * fmax(0, (1 - dr));
	//���������˺���������= (���ܻ����˺� + �����˺����� * ���ܵȼ� + ��������ֵ * ���ܱ���) * max(0.3, (����������͸�� - �����˺�������)) * max(0, (1 - �˺�������)) * max(0, max((���������� - ����������), ��������ϵ��)) + ��ʵ�˺�ֵ
	if (isCritical) hurt *= fmax(0.3f, fmax((scsdr - sr), m_CSDRFloorCoefficient));

	//���������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::SHIELD);
	//�������������˺�
	hurt = computeShieldAbsorb(hurt, targetProperty, BattleConfig::Property::SPELL_SHIELD);
	//��ʵ�˺������ӷ���
	hurt += hd + ahd;
	//��������˺���+-10
	hurt = randomizeHurtNumber(hurt);

	return ceil(hurt);
}

float AttackSystem::computeShieldAbsorb(float hurt, BattleComponent::Property::Handle& targetProperty, BattleConfig::Property shieldType)
{
	if (hurt <= 0) return hurt;

	float shield = targetProperty->get(shieldType);
	if (shield <= 0) return hurt;

	if (hurt < shield)
	{
		shield -= hurt;
		hurt = 0;
	}
	else
	{
		hurt -= shield;
		shield = 0;
	}

	targetProperty->set(shieldType, shield);
	return hurt;
}

float AttackSystem::computeCureHurt(BattleComponent::Property::Handle& fromProperty, BattleComponent::Property::Handle& targetProperty, const BattleConfig::DamageData& damageData, int sLevel, bool isCritical)
{
	float sBase = damageData.damageBase;
	float sStep = damageData.damageStep;
	float sFactor = damageData.damageFactor;
	float pow = fromProperty->get(BattleConfig::Property::POWER);
	float hb = targetProperty->get(BattleConfig::Property::HEALING_BONUS);
	float scsdr = fromProperty->get(BattleConfig::Property::SPELL_CSDR);
	//���ƻظ�����δ������= (���ܻ����˺� + �����˺����� * ���ܵȼ� + ��������ֵ * ���ܱ���) *��1+����Ч������ʡ�Ŀ�꡿��
	float hurt = (sBase + sStep * sLevel + sFactor * pow) * (1 + hb);
	//���ƻظ�����������= (���ܻ����˺� + �����˺����� * ���ܵȼ� + ��������ֵ * ���ܱ���) *��1+����Ч������ʡ�Ŀ�꡿��* ����������
	if (isCritical) hurt *= scsdr;
	return -ceil(hurt);
}

float AttackSystem::computeSkillKillHurt(BattleComponent::Property::Handle& targetProperty)
{
	return targetProperty->get(BattleConfig::Property::HP);
}

float AttackSystem::computeSkillPercentHurt(BattleComponent::Property::Handle& targetProperty, const BattleConfig::DamageData& damageData)
{
	float hp = targetProperty->get(BattleConfig::Property::HP);
	float hurt = hp * damageData.damagePercent;
	//�ٷֱ��˺�����ʹ����ֵС��1
	if (hp - hurt < 1) hurt = hp - 1;
	return ceil(hurt);
}

float AttackSystem::computeSkillHolyHurt(const BattleConfig::DamageData& damageData)
{
	float hurt = damageData.damageHoly;
	return ceil(hurt);
}

float AttackSystem::computeNormalVamp(BattleComponent::Property::Handle& fromProperty, float hurt)
{
	float av = fromProperty->get(BattleConfig::Property::ATK_VAMP);
	float avr = fromProperty->get(BattleConfig::Property::ATK_VAMP_RATE);
	float vamp = av + (hurt > 0 ? hurt * avr : 0);
	return ceil(vamp);
}

float AttackSystem::randomizeHurtNumber(float hurt)
{
	if (hurt >= 20)
	{
		hurt += cocos2d::random<int>(-10, 10);
	}
	return hurt;
}
