//
//  CCPAnimationCache.h
//  CCPAnimation
//
//  Created by fuchenhao on 3/1/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//


#pragma once

#include "CCAssetDefine.h"


class AssetDefineCache : public cocos2d::Ref
{
public:
    AssetDefineCache();
    ~AssetDefineCache();

    static AssetDefineCache* getInstance();
    static void destroyInstance();
    
    void addAsset(AssetDefine *define, const std::string& name);
    void removeAllAsset();
    void removeAsset(const std::string& name);
    
    AssetDefine* getAsset(const std::string& name);
    bool hasAsset(const std::string& name);
    void addAssetWithFile(const std::string& image, const std::string& define, const std::string& name);
    
    //add library
    
    //? removeUsedAssets();
    
private:
    std::map<std::string, AssetDefine*> *m_assets;
	static AssetDefineCache* s_sharedAssetDefineCache;
};


