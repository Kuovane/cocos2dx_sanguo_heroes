//
//  TweenQueue.h
//  Game
//
//  Created by fu.chenhao on 3/19/15.
//
//


#pragma once

#include <stdio.h>
#include "SimpleTween.h"
#include <vector>
struct TweenQueueResult;

class TweenQueue
{
public:
	enum OPERATION
	{
		IN_T,
		OUT_T,
		INOUT_T
	};
    enum TARGET
    {
		R,
		G,
		B,
		ALPHA,
		X,
		Y,
		Z,
    };



    TweenQueue();
    ~TweenQueue();
    
    void pushTween(TARGET type, OPERATION operation, SimpleTween* pTween, float m_delay = 0);
    void clearTween();

    std::vector<TweenQueueResult>& update(float dt);
//    TARGET getLastTarget() {return m_lastTarget;}
//    OPERATION getLastOperation() {return m_lastOperation;}
//    float getLastValue() {return m_lastValue;}
    
private:
    struct Segment
    {
        float m_delay;
        TARGET m_target;
        OPERATION m_operation;
        SimpleTween* m_pTween;
        
        Segment(TARGET m_target, OPERATION operation, SimpleTween* m_pTween, float m_delay):
        m_target(m_target), m_operation(operation), m_pTween(m_pTween), m_delay(m_delay)
        {
        }
        ~Segment()
        {
            CC_SAFE_DELETE(m_pTween);
        }
    };
    
    std::vector<Segment*> m_queue;
    std::vector<TweenQueueResult> m_result;

//    TARGET m_lastTarget;
//    OPERATION m_lastOperation;
//    float m_lastValue;
    
};

struct TweenQueueResult
{
    TweenQueue::TARGET m_lastTarget;
    TweenQueue::OPERATION m_lastOperation;
    float m_lastValue;
    
};


