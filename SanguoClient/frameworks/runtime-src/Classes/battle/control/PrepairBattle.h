//
//  PrepairBattle.h
//  Game
//
//  Created by fuchenhao on 3/6/15.
//
//

#ifndef __PREPAIR_BATTLE_H__
#define __PREPAIR_BATTLE_H__

#include "BattleConfig.h"

class PrepairBattle
{
public:
    static void prepair();
    void update(float dt);
    
protected:
    
    int m_currentStep;
    int m_currentAsset;
    
    const std::string m_pathSolder = "png/soldier/";
    const std::string m_pathHero = "png/hero/";
    const std::string m_pathSkill = "png/skill/";

	std::vector<std::string> m_assetSoldiers;
	std::vector<std::string> m_assetHeros;
	std::vector<std::string> m_assetSkills;

    void start();
    void collectHeroAsset(std::vector<BattleConfig::HeroConfig*>& heros, const std::string& soldierStyle);
    void collectSoldierAsset(BattleConfig::SoldierConfig& soldierConfig, const std::string& soldierStyle);
    void prepairEntity();
    void prepairBackground();
    
    void addSoldier(BattleConfig::Side side);
    void addHero(int heroId, BattleConfig::HeroConfig& heroConfig, bool asBackup);
};

#endif /* defined(__PREPAIR_BATTLE_H__) */
