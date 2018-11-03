//
// TPreferences
//
// Class for saving and loading preference information via BMessages.
//
// Eric Shepherd
//

/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#include <Message.h>
#include <Messenger.h>
#include <File.h>
#include <FindDirectory.h>
#include "TPreferences.h"

TPreferences::TPreferences(char *filename) : BMessage('pref') {
	BFile file;
	
	status = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	if (status != B_OK) {
		return;
	}
	
	path.Append(filename);
	status = file.SetTo(path.Path(), B_READ_ONLY);
	if (status == B_OK) {
		status = Unflatten(&file);
	}
}

TPreferences::~TPreferences() {
	BFile file;
	
	if (file.SetTo(path.Path(), B_WRITE_ONLY | B_CREATE_FILE) == B_OK) {
		Flatten(&file);
	}
}

status_t TPreferences::SetInt32(const char *name, int32 i) {
	if (HasInt32(name)) {
		return ReplaceInt32(name, 0, i);
	}
	return AddInt32(name, i);
}

status_t TPreferences::SetFloat(const char *name, float f) {
	if (HasFloat(name)) {
		return ReplaceFloat(name, 0, f);
	}
	return AddFloat(name, f);
}

status_t TPreferences::SetString(const char *name, const char *s) {
	if (HasString(name)) {
		return ReplaceString(name, 0, s);
	}
	return AddString(name, s);
}

int32 TPreferences::GetInt32(const char *name, int32 standard) {
  int32 nr;
	if (FindInt32(name, &nr) != B_OK) {
		nr = standard;
	}
  return nr;
}

float TPreferences::GetFloat(const char *name, float standard) {
  float nr;
	if (FindFloat(name, &nr) != B_OK) {
		nr = standard;
	}
  return nr;
}
	
