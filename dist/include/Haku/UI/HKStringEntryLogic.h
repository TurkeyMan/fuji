#pragma once
#if !defined(_HKSTRING_ENTRY_LOGIC)
#define _HKSTRING_ENTRY_LOGIC

#include "Middleware/FastDelegate.h"

class HKStringEntryLogic
{
public:
	typedef fastdelegate::FastDelegate1<const char *> StringChangeCallback;

	enum StringType
	{
		ST_Unknown = -1,

		ST_Regular,
		ST_MultiLine,
		ST_Numeric,
		ST_Password
	};

	HKStringEntryLogic();
	~HKStringEntryLogic() { }

	void Update();
	void Draw() {}

	MFString GetString() const { return buffer; }
	MFString GetRenderString() const;
	void SetString(MFString string);

	void SetMaxLength(int maxLength) { maxLen = maxLength; }
	void SetType(StringType type) { this->type = type; }

	void SetChangeCallback(StringChangeCallback callback) { changeCallback = callback; }

	int GetCursorPos() const { return cursorPos; }
	void SetCursorPos(int position, bool bUpdateSelection = false);

	void GetSelection(int *pSelStart, int *pSelEnd) const;
	void SetSelection(int start, int end);

	void SetAcceptableCharacters(const char *pCharList) { include = pCharList; }
	void SetExcludedCharacters(const char *pCharList) { exclude = pCharList; }

	static void SetRepeatParams(float repeatDelay, float repeatRate) { gRepeatDelay = repeatDelay; gRepeatRate = repeatRate; }

private:
	void StringCopyOverlap(char *pDest, const char *pSrc);
	void ClearSelection();

	MFString buffer;
	MFString include;
	MFString exclude;

	int maxLen;
	int cursorPos;
	int selectionStart, selectionEnd;
	int holdKey;
	float repeatDelay;

	StringType type;

	StringChangeCallback changeCallback;

	static float gRepeatDelay;
	static float gRepeatRate;
};

#endif
