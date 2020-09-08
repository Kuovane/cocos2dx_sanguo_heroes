//
//  AddBuff.cpp
//  Game
//
//  Created by fuchenhao on 3/4/15.
//
//

#include "AddBuff.h"
#include "BattleWorld.h"
#include "EnumParser.h"

//using namespace std;

void AddBuff::fireAction(entityx::EntityManager &es)
{
    SkillAction::fireAction(es);

    TargetFinder::TargetResults targetResults;
    std::vector<int>& targets = targetResults.targets;
    if (m_skillActionData->hasMember("SkillTarget"))
    {
        auto skillTarget = EnumParserSkillTarget().getValue(*m_skillActionData->config["SkillTarget"]->s);
        _TARGET_FINDER.findSkillTargets(m_fromId, skillTarget, targetResults);
    }
    else
    {
        auto skillTargetResults = es.get(m_entityId).component<BattleComponent::SkillTargetResults>();
        if (skillTargetResults.valid()) targets = skillTargetResults->targets;
    }

    for (int targetId : targets)
    {
        addBuffToTarget(targetId);
    }

    remove();
}
