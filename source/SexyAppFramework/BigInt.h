#ifndef __BIGINT_H__
#define __BIGINT_H__

// Arbitrary-precision integer used by the registration and signature checks
// (SexyApp::Validate, SexyApp::CheckSignature). Global namespace, vftable 0x5C1430 (one slot).
// The value is stored as 16-bit little-endian words.

#include <string>

class BigInt
{
public:
	bool					mNegative;		// +0x04
	unsigned short*			mData;			// +0x08
	int						mLength;		// +0x0C  words in use
	int						mCapacity;		// +0x10  words allocated

public:
	void					Reserve(int theCapacity);						// 0x4A3540
	void					Grow();											// 0x4A3590
	void					Normalize();									// 0x4A35B0
	void					Clear();										// 0x4A35E0
	unsigned short			GetTopWord() const;								// 0x4A3660
	unsigned short			GetWord(int theIndex) const;					// 0x4A3680
	void					SetWord(int theIndex, unsigned short theWord);	// 0x4A36A0
	BigInt&					ShiftLeft(int theBits);							// 0x4A3950 (in place)
	BigInt&					ShiftRight(int theBits);						// 0x4A3A40 (in place)
	static void				DivMod(const BigInt& theDividend, const BigInt& theDivisor, BigInt& theQuotient, BigInt& theRemainder); // 0x4A3710

public:
	BigInt();														// 0x4A3380
	BigInt(int theValue);											// 0x4A33C0
	BigInt(const BigInt& theBigInt);								// 0x4A3410
	BigInt(const std::string& theHexString);						// 0x4A3430
	virtual ~BigInt();												// 0x4A3520 (scalar deleting destructor 0x4A33A0)

	int						GetLength() const;						// 0x49BDF0 (a 4-byte getter shared by identical-code folding)
	bool					IsOdd() const;							// 0x4A35F0
	bool					IsNegative() const;						// 0x4A3610
	int						GetNumBits() const;						// 0x4A3620

	BigInt&					operator=(const BigInt& theBigInt);			// 0x4A3C40
	bool					operator==(const BigInt& theBigInt) const;	// 0x4A3AF0 (ignores the sign)
	bool					operator<(const BigInt& theBigInt) const;	// 0x4A3B30
	bool					operator>(const BigInt& theBigInt) const;	// 0x4A3BE0
	bool					operator>=(const BigInt& theBigInt) const;	// 0x4A3C10
	BigInt					operator-() const;							// 0x4A3C20
	BigInt					operator+(const BigInt& theBigInt) const;	// 0x4A3EC0
	BigInt					operator-(const BigInt& theBigInt) const;	// 0x4A4010
	BigInt					operator*(const BigInt& theBigInt) const;	// 0x4A41B0
	BigInt					operator%(const BigInt& theBigInt) const;	// 0x4A4300
	BigInt					operator|(const BigInt& theBigInt) const;	// 0x4A4460
	BigInt					operator<<(int theBits) const;				// 0x4A4380
	BigInt					operator>>(int theBits) const;				// 0x4A43F0 (LTCG folded theBits = 1)
	BigInt&					operator*=(const BigInt& theBigInt);		// 0x4A3CA0
	BigInt&					operator+=(const BigInt& theBigInt);		// 0x4A3D10
	BigInt&					operator|=(const BigInt& theBigInt);		// 0x4A3E50
	BigInt&					operator<<=(int theBits);					// 0x4A3D80
	BigInt&					operator>>=(int theBits);					// 0x4A3DF0 (LTCG folded theBits = 1)

	BigInt					ModPow(const BigInt& theExponent, const BigInt& theModulus) const; // 0x4A4520
};

BigInt						HashData(const char* theData, int theLength, int theHashBits);	// 0x4A46D0 (LTCG folded theHashBits = 94)
BigInt						HashString(const std::string& theString, int theHashBits);		// 0x4A47E0
BigInt						KeyToInt(const std::string& theKey);							// 0x4A4830

#endif //__BIGINT_H__
