//
//  CCMyShader.h
//  Game
//
//  Created by fu.chenhao on 3/23/15.
//
//

#pragma once

class CCMyShader
{
public:
    
	static void initSharder();
    
private:
	static void initSharder_normal();
    
	static void initSharder_alpha();
    
	static void initSharder_lighten();
};

