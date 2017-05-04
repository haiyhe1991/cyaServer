/*=============================================================================
AES.h: Handling of Advanced Encryption Standard
Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#ifndef __AES_H__
#define __AES_H__

typedef unsigned long AES_DWORD;
typedef unsigned char AES_BYTE;
typedef char ANSICHAR;	// An ANSI character. normally a signed type.
typedef AES_BYTE SBYTE;		// 8-bit  signed.
typedef int AES_INT;

#define AES_BLOCK_SIZE				16

// If the AES key is set to this value, a runtime error will be thrown

#define INVALID_AES_KEY	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define AES_KEY			"Wxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
/**
* AES key for decrypting coalesced ini files
* 32 arbitrary bytes to make a 256 bit key
*
* Licensees should set this to their own value, IT SHOULD BE 32 RANDOM CHARACTERS!!!!
*/
#ifndef AES_KEY
#define AES_KEY	INVALID_AES_KEY
#endif

void appEncryptData(AES_BYTE *Contents, AES_DWORD FileSize);
void appDecryptData(AES_BYTE *Contents, AES_DWORD FileSize);

void appEncryptDataWithKey(AES_BYTE *Contents, AES_DWORD FileSize, ANSICHAR* Key);
void appDecryptDataWithKey(AES_BYTE *Contents, AES_DWORD FileSize, ANSICHAR* Key);

#endif
