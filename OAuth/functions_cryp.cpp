#include "functions_cryp.h"

#include "openssl/ssl.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/buffer.h"
#include "openssl/hmac.h"

#ifndef _CC_COMMON_HMAC_H_
void CC_EVP(const EVP_MD *evp, uint32_t mlen, const void *data, uint32_t len, unsigned char *md)
{
	unsigned int mdlen = mlen;
	EVP_MD_CTX* c = EVP_MD_CTX_create();
	EVP_MD_CTX_init(c);
	EVP_DigestInit(c, evp);
	EVP_DigestUpdate(c, data, len);
	EVP_DigestFinal(c, md, &mdlen);	
	EVP_MD_CTX_destroy(c);			
}

void CC_SHA256(const void *data, uint32_t len, unsigned char *md)
{
	CC_EVP(EVP_sha256(), 32, data, len, md);
}

void CC_SHA1(const void *data, uint32_t len, unsigned char *md)
{
	CC_EVP(EVP_sha1(), 20, data, len, md);		
}

void CC_MD5(const void *data, uint32_t len, unsigned char *md)
{
	CC_EVP(EVP_md5(), 16, data, len, md);		
}

void CC_SHA512(const void *data, uint32_t len, unsigned char *md)
{
	CC_EVP(EVP_sha512(), 64, data, len, md);			
}

void CC_SHA384(const void *data, uint32_t len, unsigned char *md)
{
	CC_EVP(EVP_sha384(), 48, data, len, md);		
}
#endif

// ----------------------------------- Crypto ----------------------------------


void SHA256(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	uint8_t *buf = (uint8_t *)calloc(32, sizeof(uint8_t)); 
	
	CC_SHA256((unsigned char *)Param1.getBytesPtr(), Param1.getBytesLength(), buf);	
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 32);
	
	switch (Param2.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void SHA1(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	uint8_t *buf = (uint8_t *)calloc(20, sizeof(uint8_t)); 
	
	CC_SHA1((unsigned char *)Param1.getBytesPtr(), Param1.getBytesLength(), buf);	
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 20);
	
	switch (Param2.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void MD5(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	uint8_t *buf = (uint8_t *)calloc(16, sizeof(uint8_t)); 
	
	CC_MD5((unsigned char *)Param1.getBytesPtr(), Param1.getBytesLength(), buf);	
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 16);
	
	switch (Param2.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void SHA512(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	uint8_t *buf = (uint8_t *)calloc(64, sizeof(uint8_t)); 
	
	CC_SHA512((unsigned char *)Param1.getBytesPtr(), Param1.getBytesLength(), buf);	
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 64);
	
	switch (Param2.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void SHA384(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	uint8_t *buf = (uint8_t *)calloc(48, sizeof(uint8_t)); 
	
	CC_SHA384((unsigned char *)Param1.getBytesPtr(), Param1.getBytesLength(), buf);	
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 48);
	
	switch (Param2.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void HMACSHA512(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_BLOB Param2;
	C_LONGINT Param3;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	
	uint8_t *buf = (uint8_t *)calloc(64, sizeof(uint8_t)); 
	uint32_t mdlen = 64;
	
	HMAC(EVP_sha512(), (const void *)Param1.getBytesPtr(), (int)Param1.getBytesLength(), (const unsigned char *)Param2.getBytesPtr(), (int)Param2.getBytesLength(), buf, &mdlen);		
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 64);
	
	switch (Param3.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void HMACSHA384(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_BLOB Param2;
	C_LONGINT Param3;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	
	uint8_t *buf = (uint8_t *)calloc(48, sizeof(uint8_t)); 
	uint32_t mdlen = 48;
	
	HMAC(EVP_sha384(), (const void *)Param1.getBytesPtr(), (int)Param1.getBytesLength(), (const unsigned char *)Param2.getBytesPtr(), (int)Param2.getBytesLength(), buf, &mdlen);		
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 48);
	
	switch (Param3.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void HMACSHA256(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_BLOB Param2;
	C_LONGINT Param3;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	
	uint8_t *buf = (uint8_t *)calloc(32, sizeof(uint8_t)); 
	uint32_t mdlen = 32;
	
	HMAC(EVP_sha256(), (const void *)Param1.getBytesPtr(), (int)Param1.getBytesLength(), (const unsigned char *)Param2.getBytesPtr(), (int)Param2.getBytesLength(), buf, &mdlen);		
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 32);
	
	switch (Param3.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void HMACSHA1(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_BLOB Param2;
	C_LONGINT Param3;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	
	uint8_t *buf = (uint8_t *)calloc(20, sizeof(uint8_t)); 
	uint32_t mdlen = 20;
	
	HMAC(EVP_sha1(), (const void *)Param1.getBytesPtr(), (int)Param1.getBytesLength(), (const unsigned char *)Param2.getBytesPtr(), (int)Param2.getBytesLength(), buf, &mdlen);		
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 20);
	
	switch (Param3.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	returnValue.setReturn(pResult);
}

void HMACMD5(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_BLOB Param2;
	C_LONGINT Param3;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	
	uint8_t *buf = (uint8_t *)calloc(16, sizeof(uint8_t)); 
	uint32_t mdlen = 16;
	
	HMAC(EVP_md5(), (const void *)Param1.getBytesPtr(), (int)Param1.getBytesLength(), (const unsigned char *)Param2.getBytesPtr(), (int)Param2.getBytesLength(), buf, &mdlen);		
	
	C_BLOB temp;
	temp.setBytes((const uint8_t *)buf, 16);
	
	switch (Param3.getIntValue()) 
	{
		case 1:
			temp.toB64Text(&returnValue);	
			break;
		default:
			temp.toHexText(&returnValue);	
			break;
	}
	
	free(buf);
	
	
	returnValue.setReturn(pResult);
}

