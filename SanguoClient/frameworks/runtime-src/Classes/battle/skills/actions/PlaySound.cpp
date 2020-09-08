//
//  SLGPlaySound.cpp
//  sanguoClient
//
//  Created by fuchenhao on 6/3/15.
//
//

#include "PlaySound.h"
#include "BattleStage.h"
#include "BattleWorld.h"
#include "AudioManager.h"

SLGPlaySound::~SLGPlaySound()
{
}

void SLGPlaySound::addPlayingSoundToPool(int audioId)
{
    auto skillEntity = _ENTITY_MANAGER.get(m_entityId);
    if(skillEntity && skillEntity.valid())
    {
        auto skillHandler = skillEntity.component<BattleComponent::Skill>();
        skillHandler->m_playingSoundPool.push_back(audioId);
    }
    
}

void SLGPlaySound::fireAction(entityx::EntityManager &es)
{
    SkillAction::fireAction(es);
    
    //TODO, add implementation here
    
    std::string eventName = *this->m_skillActionData->config["event"]->s;
    std::string triggerName = *this->m_skillActionData->config["trigger"]->s;
    BattleEvent::BattleSound event(eventName, triggerName, entityx::Entity());
    
    _ENTITY_EVENT.emit(event);

    int audioId = *event.pSoundId;
    addPlayingSoundToPool(audioId);
    
    remove();
}
