//
//  BattleLoadingUI.h
//  sanguoGame
//
//  Created by fuchenhao on 7/20/15.
//
//
#pragma once
#include "cocos2d.h"
//#include <vector>
//#include "entityx.h"
//#include "BattleEvent.h"
//#include "BattleConfig.h"

class BattleLoadingUI : public cocos2d::Layer
{
public:
    BattleLoadingUI();
    
    void updateProgress(float percent);
    
    CREATE_FUNC(BattleLoadingUI);
};


