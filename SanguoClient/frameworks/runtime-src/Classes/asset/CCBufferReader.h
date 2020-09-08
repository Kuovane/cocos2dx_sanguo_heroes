//
//  BufferReader
//
//  Created by user on 12-5-12.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#pragma once

//#include "CCBufferReader.h"
#include "cocos2d.h"
#include <string>

enum Endian
{
    BIG_ENDIAN_MODE,
    LITTLE_ENDIAN_MODE
};

class BufferReader
{ 
public:
    
    unsigned char readByte();
	short readShort();
	int readInt();
    float readFloat();
    std::string* readString();
    
    BufferReader(unsigned char* buffer, int size, Endian endian);
    ~BufferReader();

private:
    int read();
private:
    static const int ERROR_OF_EOF = -1;
    Endian m_endian;
    unsigned char* m_buffer;
    int m_size;
    int m_currentPosition;
};





