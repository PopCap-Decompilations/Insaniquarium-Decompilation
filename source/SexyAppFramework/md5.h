#ifndef __MD5_H__
#define __MD5_H__

// Colin Plumb's public-domain MD5, the version linked into the original:
// MD5Init 0x4A4B10, MD5Update 0x4A4B40, MD5Final 0x4A4C00, MD5Transform 0x4A4C90.

typedef unsigned int uint32;

struct MD5Context
{
	uint32			buf[4];		// +0x00
	uint32			bits[2];	// +0x10
	unsigned char	in[64];		// +0x18  (sizeof 0x58)
};

void MD5Init(MD5Context* ctx);
void MD5Update(MD5Context* ctx, const unsigned char* buf, unsigned len);
void MD5Final(unsigned char digest[16], MD5Context* ctx);
void MD5Transform(uint32 buf[4], const uint32 in[16]);

#endif //__MD5_H__
