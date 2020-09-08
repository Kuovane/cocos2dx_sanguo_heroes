//
//  PlaySound.h
//  sanguoClient
//
//  Created by fuchenhao on 6/3/15.
//
//

#ifndef __sanguoClient__PlaySound__
#define __sanguoClient__PlaySound__

#include "SkillAction.h"

class SLGPlaySound : public SkillAction
{
private:
    void addPlayingSoundToPool(int audioId);
    
public:
    ~SLGPlaySound();
    
    void fireAction(entityx::EntityManager &es) override;
};

#endif /* defined(__sanguoClient__PlaySound__) */
