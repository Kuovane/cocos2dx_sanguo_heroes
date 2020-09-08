//
//  AssetDefine
//
//  Created by fuchenhao on 3/1/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

/******************************
swf

fps
tile sheet data
animation
ui
 *******************************/

/******************************
 animations
 
 animation count
 [animations]
    name
    label count
    [labels]
        frame index
        name
    frame count
    [frames]
        assets count
        [assets]
            transform
 *******************************/

#pragma once

#include "cocos2d.h"
#include <string>
#include <vector>
#include <map>



class  AssetDefine : public cocos2d::Ref
{
public:
    struct TilesheetData
    {
        bool isRotated;
        float centerX;
        float centerY;
        float x;
        float y;
        float width;
        float height;
    };
    
    struct QuadData
    {
        int index;
        float x;
        float y;
        float a;
        float b;
        float c;
        float d;
		cocos2d::Color4B color;
        char alpha;
    };
    
    struct LabelData
    {
        std::string *name;
        int frame;
        
        ~LabelData()
        {
            delete name;
        }
    };
    
    struct AnimationData
    {
		cocos2d::TextureAtlas *atlas;
        std::string *name;
        int frameCount;
        int labelCount;
        int assetTotal;
        int *frames;
        int *addedFrames;
        bool *binded;
        std::vector<LabelData*> *labels;
        std::vector<QuadData*> *quads;
       
        ~AnimationData()
        {
            CC_SAFE_RELEASE(atlas);
            
            delete name;
            delete[] frames;
            delete[] addedFrames;
            delete[] binded;
            
            for (std::vector<LabelData*>::iterator it = labels->begin(); it != labels->end(); it++)
            if (NULL != *it)
            {
                delete *it;
                *it = NULL;
            }
            labels->clear();
            delete labels;
            
            for (std::vector<QuadData*>::iterator it = quads->begin(); it != quads->end(); it++)
                if (NULL != *it)
                {
                    delete *it;
                    *it = NULL;
                }
            quads->clear();
            delete quads;
        }
    };
    
    struct UIData
    {
        
    };
    
    static AssetDefine* create(const std::string &image, const std::string &define);
    
    AssetDefine::AnimationData* getAnimation(const std::string &name);
    inline AssetDefine::TilesheetData* getTilesheetData(int index){return (AssetDefine::TilesheetData*)m_tilesheet->at(index);};
    inline unsigned char getFPS(){return this->m_fps;};
    int getAssetIndex(const std::string& name);
    cocos2d::Sprite* createSpriteFrame(const std::string& name);
    
    AssetDefine();
    virtual ~AssetDefine();
    
    bool initWithFile(const std::string &image, const std::string &define);
    
private:
    unsigned char m_fps;
    int m_width;
    int m_height;
    int m_tilesheetCount;
    std::vector<std::string*> *m_assets;
    std::vector<std::string*> *m_imported;
    std::vector<TilesheetData*> *m_tilesheet;
    std::map<std::string, AnimationData*> *m_animations;
    std::map<std::string, UIData*> *m_ui;
	cocos2d::Texture2D *m_texture;
};


