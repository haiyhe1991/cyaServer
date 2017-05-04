/* string_hash.h
* 经典的字符串hash函数
* "ELF Hash Function" 较常用 */

#ifndef __STRING_HASH_H__
#define __STRING_HASH_H__

#if defined(_MSC_VER) &&  _MSC_VER > 1000
	#pragma once
#endif

/* RS Hash Function */
unsigned long RSHash(const char* str);
/* JS Hash Function */
unsigned long JSHash(const char* str);
/* P. J. Weinberger Hash Function */
unsigned long PJWHash(const char* str);
/* ELF Hash Function */
unsigned long ELFHash(const char* str);
/* BKDR Hash Function */
unsigned long BKDRHash(const char* str);
/* SDBM Hash Function */
unsigned long SDBMHash(const char* str);
/* DJB Hash Function */
unsigned long DJBHash(const char* str);
/* AP Hash Function */
unsigned long APHash(const char* str);

unsigned long PHPPJWHash(const char* str, int len);
unsigned long MySqlHash(const char* str, int len);
unsigned long NollVoHash(const char* str, int len);
unsigned long ExpHash(const char* str);


#endif