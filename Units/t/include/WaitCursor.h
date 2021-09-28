//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2009-11-26
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#pragma once

enum TWAIT_CURSOR { WAIT = 0, BACKGROUND = 1 };

class T_CLASS TWaitCursor
{
protected:
	TWAIT_CURSOR m_enWaitType;

public:
	TWaitCursor(TWAIT_CURSOR enWaitType = WAIT);
	~TWaitCursor();

// Operations
public:
	void Restore();
};
