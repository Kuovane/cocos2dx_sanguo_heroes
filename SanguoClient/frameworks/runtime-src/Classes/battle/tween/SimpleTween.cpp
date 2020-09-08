//
//  SimpleTween.cpp
//  Game
//
//  Created by fu.chenhao on 3/16/15.
//
//

#include "SimpleTween.h"
#include "cocos2d.h"
// ============

static void sample()
{
	SimpleTween_cubic tween(3, 100, 500);

	float dt = 0.016;
	while (true)
	{
		float currentValue = tween.in(dt);
		if (tween.isFinish())
		{
			CCLOG("value at this frame = %f", currentValue);
			break;
		}
	}
}