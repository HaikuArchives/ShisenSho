//
// TPreferences
// Using BMessages to save user settings.
//
// Eric Shepherd
//
/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#ifndef __TPREFS_H__
#define __TPREFS_H__

#include <Path.h>
#include <Message.h>

class _EXPORT TPreferences : public BMessage {
	public:
					TPreferences(char *filename);
					~TPreferences();
	status_t		InitCheck(void);
	
	status_t		SetInt32(const char *name, int32 i);
	status_t		SetFloat(const char *name, float f);
	status_t		SetString(const char *name, const char *string);
	
	int32	GetInt32(const char *name, int32 standard);
	float	GetFloat(const char *name, float standard);
	
	private:
	
	BPath			path;
	status_t		status;
};

inline status_t TPreferences::InitCheck(void) {
	return status;
}

#endif
