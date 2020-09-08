#pragma once

#include "cocos2d.h"
#include "BattleEvent.h"

class BattleLoadingUI;
 
class BattleSG : public cocos2d::Layer, public entityx::Receiver<BattleSG>
{
public:
	static cocos2d::Scene* createScene();
	BattleSG();
	virtual ~BattleSG();
	void receive(const BattleEvent::StartBattleProgress& e);
	void receive(const BattleEvent::StartBattleComplete& e);

	CREATE_FUNC(BattleSG);

private:
    void createBattleWorld(float dt);

	BattleLoadingUI* m_battleLoadingUI = nullptr;
};


#ifdef __cplusplus
extern "C"
{
#endif

extern  int createBattle(lua_State *L);
#ifdef __cplusplus
}
#endif
