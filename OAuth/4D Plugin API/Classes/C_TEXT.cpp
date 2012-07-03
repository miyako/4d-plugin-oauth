/*
 *  C_TEXT.cpp
 *  4D Plugin
 *
 *  Created by miyako on 11/01/03.
 *
 */

#include "C_TEXT.h"

void C_TEXT::fromParamAtIndex(PackagePtr pParams, uint16_t index)
{
	if(index)
	{
		PA_Unistring *str = (PA_Unistring *)(pParams[index - 1]);
		*this->_CUTF16String = CUTF16String(str->fString, str->fLength);
	}
}

void C_TEXT::toParamAtIndex(PackagePtr pParams, uint16_t index)
{	
	if(index)
	{
		PA_SetUnistring(
						(PA_Unistring *)(pParams[index - 1]),
						(PA_Unichar *)this->_CUTF16String->c_str()
						);
	}	
}

void C_TEXT::setReturn(sLONG_PTR* pResult)
{	
	PA_SetUnistring(
					*(PA_Unistring **) pResult,
					(PA_Unichar *)this->_CUTF16String->c_str()
					);		
}

void C_TEXT::setUTF16String(CUTF16String* pString)
{
	*this->_CUTF16String = CUTF16String(pString->c_str(), pString->length());		
}

void C_TEXT::setUTF16String(const PA_Unichar* pString, uint32_t len)
{
	*this->_CUTF16String = CUTF16String(pString, (size_t)len);	
}

void C_TEXT::setUTF16String(const PA_Unistring* pString)
{
	*this->_CUTF16String = CUTF16String(pString->fString, pString->fLength);	
}

#if VERSIONMAC
#ifdef __OBJC__	
void C_TEXT::setUTF16String(NSString* pString)
{
	uint32_t len = [pString length];
	uint32_t size = (len * sizeof(PA_Unichar)) + sizeof(PA_Unichar);
	std::vector<uint8_t> buf(size);	
	
	if([pString getCString:(char *)&buf[0] maxLength:size encoding:NSUnicodeStringEncoding])
		this->setUTF16String((const PA_Unichar *)&buf[0], len);	
}

NSMutableString* C_TEXT::copyUTF16MutableString()
{
	return [[NSMutableString alloc]initWithCharacters:this->_CUTF16String->c_str() length:this->_CUTF16String->length()];
}

NSString* C_TEXT::copyUTF16String()
{
	return [[NSString alloc]initWithCharacters:this->_CUTF16String->c_str() length:this->_CUTF16String->length()];
}

bool C_TEXT::convertPathSystemToPOSIX()
{	
	bool success = false;
	
	NSString* str = this->copyUTF16String();
	
	NSURL * url = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)str, kCFURLHFSPathStyle, false);	
	
	if(url)
	{
		NSString *path = (NSString *)CFURLCopyFileSystemPath((CFURLRef)url, kCFURLPOSIXPathStyle);
		this->setUTF16String(path);
		[path release];
		[url release];	
		success = true;
	}
	
	[str release];
	
	return success;
}

bool C_TEXT::convertPathPOSIXToSystem()
{
	bool success = false;
	
	NSString* str = this->copyUTF16String();
	
	NSURL * url = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)str, kCFURLPOSIXPathStyle, false);	
	
	if(url)
	{
		NSString *path = (NSString *)CFURLCopyFileSystemPath((CFURLRef)url, kCFURLHFSPathStyle);
		this->setUTF16String(path);
		[path release];
		[url release];	
		success = true;
	}
	
	[str release];
	
	return success;	
}
#endif
#endif

void C_TEXT::convertFromUTF8(const CUTF8String* fromString, CUTF16String* toString)	
{
	uint32_t size = ((uint32_t)fromString->length() * sizeof(PA_Unichar)) + sizeof(PA_Unichar);
	std::vector<uint8_t> buf(size);
	
	uint32_t len = PA_ConvertCharsetToCharset(
											  (char *)fromString->c_str(),
											  (uint32_t)fromString->length(),
											  eVTC_UTF_8,
											  (char *)&buf[0],
											  size,
											  eVTC_UTF_16
											  );
	
//	*toString = CUTF16String((const PA_Unichar *)&buf[0], len);	
	*toString = CUTF16String((const PA_Unichar *)&buf[0], len/2);		
}

void C_TEXT::convertToUTF8(const CUTF16String* fromString, CUTF8String* toString)
{
	uint32_t size = ((uint32_t)fromString->length() * 4) + sizeof(uint8_t);
	std::vector<uint8_t> buf(size);
	
	uint32_t len = PA_ConvertCharsetToCharset(
											  (char *)fromString->c_str(),
											  (uint32_t)fromString->length() * sizeof(PA_Unichar),
											  eVTC_UTF_16,
											  (char *)&buf[0],
											  size,
											  eVTC_UTF_8
											  );
	
	*toString = CUTF8String((const uint8_t *)&buf[0], len);
}

void C_TEXT::setUTF8String(CUTF8String* pString)
{
	convertFromUTF8(pString, this->_CUTF16String);
}

void C_TEXT::setUTF8String(const uint8_t *pString, uint32_t len)
{
	CUTF8String u = CUTF8String(pString, len);
	convertFromUTF8(&u, this->_CUTF16String);	
}

const PA_Unichar *C_TEXT::getUTF16StringPtr()
{
	return this->_CUTF16String->c_str();
}

uint32_t C_TEXT::getUTF16Length()
{
	return (uint32_t)this->_CUTF16String->length();	
}

void C_TEXT::copyUTF16String(CUTF16String* pString)
{
	*pString = CUTF16String(this->_CUTF16String->c_str(), this->_CUTF16String->length());
}

void C_TEXT::copyUTF8String(CUTF8String* pString)
{
	convertToUTF8(this->_CUTF16String, pString);	
}

C_TEXT::C_TEXT() : _CUTF16String(new CUTF16String)
{
	
}

C_TEXT::~C_TEXT()
{ 
	delete _CUTF16String; 
}

void CUTF8StringReplaceString(CUTF8String *src, const uint8_t *f, const uint8_t *t)
{
	CUTF8String from = CUTF8String(f);
	CUTF8String to = CUTF8String(t);
	
	CUTF8String::size_type pos = 0;
	
    while(pos = src->find(from, pos), pos != CUTF8String::npos) 
	{
        src->replace(pos, from.length(), to);
        pos += to.length();
    }
}