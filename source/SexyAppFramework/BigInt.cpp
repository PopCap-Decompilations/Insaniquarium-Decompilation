#include "BigInt.h"
#include "md5.h"

#include <string.h>
#include <ctype.h>

// The 28 registration-key characters (no 0, 1, 5, 8, B, I, O or S), 0x5C1408
static const char gKeyChars[] = "234679ACDEFGHJKLMNPQRTUVWXYZ";

BigInt::BigInt()
{
	mNegative = false;
	mData = NULL;
	mLength = 0;
	mCapacity = 0;
}

BigInt::BigInt(int theValue)
{
	mData = new unsigned short[8];
	mCapacity = 8;
	mLength = 2;
	if (theValue < 0)
	{
		mNegative = true;
		theValue = -theValue;
	}
	else
	{
		mNegative = false;
	}
	mData[0] = (unsigned short)theValue;
	mData[1] = (unsigned short)(theValue >> 16);
	Normalize();
}

BigInt::BigInt(const BigInt& theBigInt)
{
	mData = NULL;
	mLength = 0;
	mCapacity = 0;
	*this = theBigInt;
}

BigInt::BigInt(const std::string& theHexString)
{
	// The original leaves mNegative unset here; the first |= below replaces it with false anyway
	mNegative = false;
	mData = NULL;
	mLength = 0;
	mCapacity = 0;

	for (int i = 0; i < (int)theHexString.length(); i++)
	{
		char aChar = theHexString[i];
		int aDigit = 0;
		if ((aChar >= '0') && (aChar <= '9'))
			aDigit = aChar - '0';
		else if ((aChar >= 'a') && (aChar <= 'f'))
			aDigit = aChar - 'a' + 10;
		if ((aChar >= 'A') && (aChar <= 'F'))
			aDigit = aChar - 'A' + 10;

		*this <<= 4;
		*this |= BigInt(aDigit);
	}
}

BigInt::~BigInt()
{
	delete [] mData;
}

void BigInt::Reserve(int theCapacity)
{
	mCapacity = theCapacity;
	unsigned short* aNewData = new unsigned short[theCapacity];
	if (mLength > 0)
		memcpy(aNewData, mData, mLength * sizeof(unsigned short));
	delete [] mData;
	mData = aNewData;
}

void BigInt::Grow()
{
	if (mCapacity == 0)
		Reserve(8);
	else
		Reserve(mCapacity * 2);
}

void BigInt::Normalize()
{
	while ((mLength >= 1) && (mData[mLength - 1] == 0))
		mLength--;
}

void BigInt::Clear()
{
	mNegative = false;
	mLength = 0;
}

int BigInt::GetLength() const
{
	return mLength;
}

bool BigInt::IsOdd() const
{
	if (mLength == 0)
		return false;
	return (mData[0] & 1) != 0;
}

bool BigInt::IsNegative() const
{
	return mNegative;
}

int BigInt::GetNumBits() const
{
	unsigned short aTopWord = GetTopWord();
	for (int aBit = 15; aBit >= 0; aBit--)
	{
		if ((aTopWord & (1 << aBit)) != 0)
			return mLength * 16 + aBit - 15;
	}
	return 0;
}

unsigned short BigInt::GetTopWord() const
{
	if (mLength == 0)
		return 0;
	return mData[mLength - 1];
}

unsigned short BigInt::GetWord(int theIndex) const
{
	if (theIndex < mLength)
		return mData[theIndex];
	return 0;
}

void BigInt::SetWord(int theIndex, unsigned short theWord)
{
	while (theIndex >= mLength + 1)
	{
		if (mLength >= mCapacity)
			Grow();
		mData[mLength] = 0;
		mLength++;
	}

	if (theIndex < mLength)
	{
		mData[theIndex] = theWord;
	}
	else
	{
		if (mLength >= mCapacity)
			Grow();
		mData[mLength] = theWord;
		mLength++;
	}
}

BigInt& BigInt::ShiftLeft(int theBits)
{
	int aLength = GetLength();
	int aWordShift = theBits / 16;
	if (aWordShift > 0)
	{
		while (mLength + aWordShift - 1 >= mCapacity)
			Grow();
		// The original calls memcpy on these overlapping buffers; the VS2005 memcpy copies
		// overlapping memory correctly, so memmove keeps that result
		memmove(mData + aWordShift, mData, mLength * sizeof(unsigned short));
		for (int i = 0; i < aWordShift; i++)
			mData[i] = 0;
		mLength += aWordShift;
	}

	int aCarry = 0;
	if (aLength > 0)
	{
		int aBitShift = theBits % 16;
		int anIndex = aWordShift;
		for (int aCount = aLength; aCount > 0; aCount--)
		{
			int aValue = (GetWord(anIndex) << aBitShift) + aCarry;
			aCarry = aValue >> 16;
			SetWord(anIndex, (unsigned short)aValue);
			anIndex++;
		}
		if (aCarry != 0)
			SetWord(aWordShift + aLength, (unsigned short)aCarry);
	}
	return *this;
}

BigInt& BigInt::ShiftRight(int theBits)
{
	int aWordShift = theBits / 16;
	unsigned short aCarry = 0;
	if (aWordShift > 0)
	{
		if (aWordShift >= mLength)
		{
			mLength = 0;
			mNegative = false;
			return *this;
		}
		mLength -= aWordShift;
		memmove(mData, mData + aWordShift, mLength * sizeof(unsigned short));
	}

	int aBitShift = theBits % 16;
	for (int i = GetLength() - 1; i >= 0; i--)
	{
		unsigned short aWord = GetWord(i);
		int aNewWord = ((int)aWord >> aBitShift) + aCarry;
		aCarry = (unsigned short)((unsigned int)aWord << (16 - aBitShift));
		SetWord(i, (unsigned short)aNewWord);
	}
	Normalize();
	return *this;
}

void BigInt::DivMod(const BigInt& theDividend, const BigInt& theDivisor, BigInt& theQuotient, BigInt& theRemainder)
{
	theQuotient.Clear();
	if (theDivisor > theDividend)
	{
		theRemainder = theDividend;
		return;
	}

	int aShift = 1 - theDivisor.GetNumBits() + theDividend.GetNumBits();
	theRemainder = theDividend;
	if (theRemainder.mCapacity == 0)
		theRemainder.Grow();
	theRemainder.ShiftRight(aShift);

	for (int aBit = aShift - 1; aBit >= 0; aBit--)
	{
		// theRemainder *= 2, in place
		if (theRemainder.mLength > 0)
		{
			int aCarry = 0;
			for (int i = 0; i < theRemainder.mLength; i++)
			{
				int aValue = aCarry + theRemainder.mData[i] * 2;
				aCarry = aValue >> 16;
				theRemainder.mData[i] = (unsigned short)aValue;
			}
			if (aCarry != 0)
			{
				if (theRemainder.mLength == theRemainder.mCapacity)
					theRemainder.Grow();
				theRemainder.mData[theRemainder.mLength++] = (unsigned short)aCarry;
			}
		}

		int aWordIdx = aBit / 16;
		int aBitIdx = aBit % 16;
		if ((theDividend.mData[aWordIdx] & (1 << aBitIdx)) != 0)
		{
			if (theRemainder.mLength == 0)
			{
				theRemainder.mLength = 1;
				theRemainder.mData[0] = 1;
			}
			else
			{
				theRemainder.mData[0] |= 1;
			}
		}

		if (theRemainder >= theDivisor)
		{
			if (aWordIdx < theQuotient.mLength)
			{
				theQuotient.mData[aWordIdx] |= (unsigned short)(1 << aBitIdx);
			}
			else
			{
				while (aWordIdx >= theQuotient.mCapacity)
					theQuotient.Grow();
				while (aWordIdx > theQuotient.mLength)
					theQuotient.mData[theQuotient.mLength++] = 0;
				theQuotient.mData[theQuotient.mLength++] = (unsigned short)(1 << aBitIdx);
			}

			if (theDivisor.mLength > 0)
			{
				int aBorrow = 0;
				int i;
				for (i = 0; i < theDivisor.mLength; i++)
				{
					int aValue = theRemainder.mData[i] - theDivisor.mData[i] - aBorrow;
					if (aValue < 0)
					{
						aBorrow = 1;
						aValue += 0x10000;
					}
					else
					{
						aBorrow = 0;
					}
					theRemainder.mData[i] = (unsigned short)aValue;
				}

				// As in the original, the borrow loop never advances i. That is harmless here:
				// theRemainder < 2 * theDivisor, so at most one extra word (holding 1) is touched.
				while (aBorrow > 0)
				{
					int aValue = theRemainder.mData[i] - aBorrow;
					if (aValue < 0)
					{
						aBorrow = 1;
						aValue += 0x10000;
					}
					else
					{
						aBorrow = 0;
					}
					theRemainder.mData[i] = (unsigned short)aValue;
				}
			}

			while ((theRemainder.mLength > 0) && (theRemainder.mData[theRemainder.mLength - 1] == 0))
				theRemainder.mLength--;
		}
	}

	theQuotient.mNegative = theDivisor.IsNegative() ^ theDividend.IsNegative();
}

BigInt& BigInt::operator=(const BigInt& theBigInt)
{
	// No self-assignment check, as in the original
	unsigned short* anOldData = mData;
	mLength = theBigInt.mLength;
	mCapacity = theBigInt.mCapacity;
	delete [] anOldData;
	mData = new unsigned short[mCapacity];
	if (mLength > 0)
		memcpy(mData, theBigInt.mData, mLength * sizeof(unsigned short));
	mNegative = theBigInt.mNegative;
	return *this;
}

bool BigInt::operator==(const BigInt& theBigInt) const
{
	if (mLength != theBigInt.mLength)
		return false;
	for (int i = 0; i < mLength; i++)
	{
		if (mData[i] != theBigInt.mData[i])
			return false;
	}
	return true;
}

bool BigInt::operator<(const BigInt& theBigInt) const
{
	if (IsNegative())
	{
		if (!theBigInt.IsNegative())
			return true;
		if (mLength > theBigInt.mLength)
			return true;
		if (mLength < theBigInt.mLength)
			return false;
		for (int i = mLength - 1; i >= 0; i--)
		{
			if (mData[i] < theBigInt.mData[i])
				return false;
			if (mData[i] > theBigInt.mData[i])
				return true;
		}
		return false;
	}

	if (theBigInt.IsNegative())
		return false;
	if (mLength > theBigInt.mLength)
		return false;
	if (mLength < theBigInt.mLength)
		return true;
	for (int i = mLength - 1; i >= 0; i--)
	{
		if (mData[i] < theBigInt.mData[i])
			return true;
		if (mData[i] > theBigInt.mData[i])
			return false;
	}
	return false;
}

bool BigInt::operator>(const BigInt& theBigInt) const
{
	return !(*this == theBigInt) && !(*this < theBigInt);
}

bool BigInt::operator>=(const BigInt& theBigInt) const
{
	return !(*this < theBigInt);
}

BigInt BigInt::operator-() const
{
	BigInt aResult(*this);
	aResult.mNegative = !aResult.mNegative;
	return aResult;
}

BigInt BigInt::operator+(const BigInt& theBigInt) const
{
	if (theBigInt.IsNegative())
		return *this - (-theBigInt);
	if (IsNegative())
		return theBigInt - (-*this);

	BigInt aResult;
	int aLength = (GetLength() > theBigInt.GetLength()) ? GetLength() : theBigInt.GetLength();
	int aCarry = 0;
	for (int i = 0; i < aLength; i++)
	{
		int aValue = theBigInt.GetWord(i) + aCarry + GetWord(i);
		aResult.SetWord(i, (unsigned short)aValue);
		aCarry = aValue >> 16;
	}
	if (aCarry > 0)
		aResult.SetWord(aLength, (unsigned short)aCarry);
	return aResult;
}

BigInt BigInt::operator-(const BigInt& theBigInt) const
{
	if (theBigInt.IsNegative())
		return *this + (-theBigInt);
	if (IsNegative())
		return -((-*this) + theBigInt);
	if (theBigInt > *this)
		return -(theBigInt - *this);

	BigInt aResult;
	int aLength = (GetLength() > theBigInt.GetLength()) ? GetLength() : theBigInt.GetLength();
	int aBorrow = 0;
	for (int i = 0; i < aLength; i++)
	{
		int aValue = GetWord(i) - aBorrow - theBigInt.GetWord(i);
		if (aValue < 0)
		{
			aBorrow = 1;
			aValue += 0x10000;
		}
		else
		{
			aBorrow = 0;
		}
		aResult.SetWord(i, (unsigned short)aValue);
	}
	aResult.Normalize();
	return aResult;
}

BigInt BigInt::operator*(const BigInt& theBigInt) const
{
	BigInt aResult;
	for (int j = 0; j < theBigInt.GetLength(); j++)
	{
		unsigned short aMultiplier = theBigInt.GetWord(j);
		BigInt aPartial;
		unsigned int aCarry = 0;
		for (int i = 0; i < GetLength(); i++)
		{
			unsigned int aValue = (unsigned int)GetWord(i) * aMultiplier + aCarry;
			aPartial.SetWord(i + j, (unsigned short)aValue);
			aCarry = aValue >> 16;
		}
		if (aCarry != 0)
			aPartial.SetWord(GetLength() + j, (unsigned short)aCarry);
		aResult += aPartial;
	}
	aResult.mNegative = theBigInt.IsNegative() ^ IsNegative();
	return aResult;
}

BigInt BigInt::operator%(const BigInt& theBigInt) const
{
	BigInt aQuotient;
	BigInt aRemainder;
	DivMod(*this, theBigInt, aQuotient, aRemainder);
	return aRemainder;
}

BigInt BigInt::operator|(const BigInt& theBigInt) const
{
	// No Normalize, and the sign stays false
	BigInt aResult;
	int aLength = (GetLength() > theBigInt.GetLength()) ? GetLength() : theBigInt.GetLength();
	for (int i = 0; i < aLength; i++)
		aResult.SetWord(i, theBigInt.GetWord(i) | GetWord(i));
	return aResult;
}

BigInt BigInt::operator<<(int theBits) const
{
	BigInt aResult(*this);
	aResult.ShiftLeft(theBits);
	return aResult;
}

BigInt BigInt::operator>>(int theBits) const
{
	BigInt aResult(*this);
	aResult.ShiftRight(theBits);
	return aResult;
}

BigInt& BigInt::operator*=(const BigInt& theBigInt)
{
	*this = *this * theBigInt;
	return *this;
}

BigInt& BigInt::operator+=(const BigInt& theBigInt)
{
	*this = *this + theBigInt;
	return *this;
}

BigInt& BigInt::operator|=(const BigInt& theBigInt)
{
	*this = *this | theBigInt;
	return *this;
}

BigInt& BigInt::operator<<=(int theBits)
{
	*this = *this << theBits;
	return *this;
}

BigInt& BigInt::operator>>=(int theBits)
{
	*this = *this >> theBits;
	return *this;
}

BigInt BigInt::ModPow(const BigInt& theExponent, const BigInt& theModulus) const
{
	BigInt aResult(1);
	BigInt aBase(*this);
	BigInt anExponent(theExponent);
	while (anExponent > BigInt(0))
	{
		if (anExponent.IsOdd())
			aResult = (aResult * aBase) % theModulus;
		anExponent >>= 1;
		aBase = (aBase * aBase) % theModulus;
	}
	return aResult;
}

// The top theHashBits bits of the MD5 digest, read as one big-endian number
BigInt HashData(const char* theData, int theLength, int theHashBits)
{
	MD5Context aContext;
	unsigned char aDigest[16];
	MD5Init(&aContext);
	MD5Update(&aContext, (const unsigned char*)theData, theLength);
	MD5Final(aDigest, &aContext);

	BigInt aResult;
	for (int i = 0; i < 16; i++)
	{
		aResult <<= 8;
		aResult += BigInt(aDigest[i]);
	}
	aResult.ShiftRight(aResult.GetNumBits() - theHashBits);
	return aResult;
}

BigInt HashString(const std::string& theString, int theHashBits)
{
	return HashData(theString.c_str(), theString.length(), theHashBits);
}

// Turns a "XXXXX-XXXXX-..." registration key into a base-28 number; any invalid character gives 0
BigInt KeyToInt(const std::string& theKey)
{
	// toupper gets unsigned char values: the original's VS2005 C-locale toupper left bytes above
	// 0x7F unchanged, while the debug UCRT asserts on negative arguments
	std::string aKey;
	for (int i = 0; i < (int)theKey.length(); i++)
	{
		if (theKey[i] != ' ')
		{
			if (theKey[i] == '1')
				aKey += "L";
			else if ((theKey[i] == '0') || (theKey[i] == 'o') || (theKey[i] == 'O'))
				aKey += 'Q';
			else
				aKey += toupper((unsigned char)theKey[i]);
		}
	}

	BigInt aResult;
	for (int i = 0; i < (int)aKey.length(); i++)
	{
		// sic: the original reads theKey here, not the cleaned aKey (asm 0x4A49A2-0x4A49B5), so the
		// 1->L and 0/o/O->Q substitutions and the space removal only change the length
		char aChar = toupper((unsigned char)theKey[i]);	// the original keeps only the low byte (0x4A49BA) and compares bytes (0x4A49D2, 0x4A4A10)
		if ((aKey.length() - i) % 6 == 0)
		{
			if (aChar != '-')
				return BigInt(0);
		}
		else
		{
			int aValue = -1;
			for (int j = 0; j < 28; j++)
			{
				if (gKeyChars[j] == aChar)
					aValue = j;
			}
			if (aValue == -1)
				return BigInt(0);

			aResult *= BigInt(28);
			aResult += BigInt(aValue);
		}
	}
	return aResult;
}
