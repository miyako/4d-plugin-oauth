#include "functions_curl.h"

std::map<uint32_t, uint32_t> _curl_write_method_ids;
std::map<uint32_t, uint32_t> _curl_read_method_ids;

void cURL_Get_version(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	CUTF8String version = (const uint8_t *)curl_version();
	returnValue.setUTF8String(&version);
	
	returnValue.setReturn(pResult);
}

void cURL_Unescape_url(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CURL *curl = curl_easy_init();
	
	if(curl){
		
		CUTF8String srcUrl;
		Param1.copyUTF8String(&srcUrl);
		
		int outLength;
		char *escapedUrl = curl_easy_unescape(curl, (char *)srcUrl.c_str(), srcUrl.size(), &outLength);
		
		CUTF8String dstUrl;		
		dstUrl = CUTF8String((const uint8_t *)escapedUrl, outLength);
		returnValue.setUTF8String(&dstUrl);
		
		curl_free(escapedUrl);
		curl_easy_cleanup(curl);	
	}
	
	returnValue.setReturn(pResult);
}

void cURL_Get_date(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_LONGINT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CUTF8String datestring;
	Param1.copyUTF8String(&datestring);
	
	time_t date = curl_getdate((char *)datestring.c_str(), NULL);
	
	switch (date) {
		case -1:
			returnValue.setIntValue(0);
			break;
		case  0x7FFFFFFF:
			returnValue.setIntValue(0);
			break;			
		default:
			Param2.setIntValue(date);			
			returnValue.setIntValue(1);			
			break;
	}
	
	Param2.toParamAtIndex(pParams, 2);
	returnValue.setReturn(pResult);
}

void cURL_Escape_url(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CURL *curl = curl_easy_init();
	
	if(curl){
		
		CUTF8String srcUrl;
		Param1.copyUTF8String(&srcUrl);
		
		char *escapedUrl = curl_easy_escape(curl, (char *)srcUrl.c_str(), srcUrl.size());
		
		if(escapedUrl){
			CUTF8String dstUrl;		
			dstUrl = CUTF8String((const uint8_t *)escapedUrl);
			returnValue.setUTF8String(&dstUrl);		
		}
		
		curl_free(escapedUrl);
		curl_easy_cleanup(curl);	
	}
	
	returnValue.setReturn(pResult);
}

size_t _cURL_write_data(void *buffer, size_t size, size_t nmemb, C_BLOB &outData){
    
	PA_YieldAbsolute();
    
	size_t len = size * nmemb;
	
	outData.addBytes((const uint8_t *)buffer, len);
	
    uint32_t processId = PA_GetCurrentProcessNumber();
    uint32_t _curl_write_method_id = 0;
    
    std::map<uint32_t, uint32_t>::iterator pos = _curl_write_method_ids.find(processId);
	
	if(pos != _curl_write_method_ids.end()) {
		_curl_write_method_id = pos->second;
        if(_curl_write_method_id){
			if(!PA_IsProcessDying()){
                PA_Variable	params[2];
                params[0] = PA_CreateVariable(eVK_Longint);
                params[1] = PA_CreateVariable(eVK_Longint);
                PA_SetLongintVariable(&params[0], len);
                PA_SetLongintVariable(&params[1], outData.getBytesLength());
                PA_Variable statusCode = PA_ExecuteMethodByID(_curl_write_method_id, params, 2);
                if(PA_GetVariableKind(statusCode) == eVK_Boolean)
                    if(PA_GetBooleanVariable(statusCode)){
                        if(len){len=0;}else{len=1;}
                    }
			}
        }
	}
    
	return len;
}

size_t _cURL_read_data(void *buffer, size_t size, size_t nmemb, C_BLOB &inData){
	
	PA_YieldAbsolute();
	
	size_t len = size * nmemb;
    
	const uint8_t *ptr = inData.getBytesPtrForSize((uint32_t *)&len);
	if(ptr) memcpy(buffer, ptr, len);
    
    uint32_t processId = PA_GetCurrentProcessNumber();
    uint32_t _curl_read_method_id = 0;
    
    std::map<uint32_t, uint32_t>::iterator pos = _curl_read_method_ids.find(processId);
    
	if(pos != _curl_read_method_ids.end()) {
		_curl_read_method_id = pos->second;
        if(_curl_read_method_id){
			if(!PA_IsProcessDying()){
                PA_Variable	params[2];
                params[0] = PA_CreateVariable(eVK_Longint);
                params[1] = PA_CreateVariable(eVK_Longint);
                PA_SetLongintVariable(&params[0], len);
                PA_SetLongintVariable(&params[1], inData.getBytesLength());
                PA_Variable statusCode = PA_ExecuteMethodByID(_curl_read_method_id, params, 2);
                if(PA_GetVariableKind(statusCode) == eVK_Boolean)
                    if(PA_GetBooleanVariable(statusCode)){
                        if(len){len=0;}else{len=1;}
                    }
			}
        }
    }
    
	return len;
}

void _cURL(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1Url;
	ARRAY_LONGINT Param2Keys;
	ARRAY_TEXT Param3Values;
	C_BLOB Param4InData;
	C_BLOB Param5OutData;
	C_LONGINT returnValue;
	ARRAY_TEXT Param6DataNames;
	ARRAY_TEXT Param7DataTypes;
	ARRAY_TEXT Param8DataFileNames;
    PA_Variable Param9DataObjects;
    
	Param1Url.fromParamAtIndex(pParams, 1);
	Param2Keys.fromParamAtIndex(pParams, 2);
	Param3Values.fromParamAtIndex(pParams, 3);
	Param4InData.fromParamAtIndex(pParams, 4);
    
	Param6DataNames.fromParamAtIndex(pParams, 6);
	Param7DataTypes.fromParamAtIndex(pParams, 7);
	Param8DataFileNames.fromParamAtIndex(pParams, 8);
	Param9DataObjects = *((PA_Variable*) pParams[8]);
	
    unsigned int size6, size7, size8, size9;
    size6 = Param6DataNames.getSize();
    size7 = Param7DataTypes.getSize();
    size8 = Param8DataFileNames.getSize();
    size9 = (Param9DataObjects.uValue.fArray.fNbElements) + 1;
    
	uint32_t processId = (uint32_t)PA_GetCurrentProcessNumber();
	
	CURL *curl = curl_easy_init();
	CURLcode error = CURLE_OK;
	
	if(curl){
		
		CUTF8String url;
		Param1Url.copyUTF8String(&url);
        
		error = curl_easy_setopt(curl, CURLOPT_READFUNCTION, _cURL_read_data);
		error = curl_easy_setopt(curl, CURLOPT_READDATA, &Param4InData);
		
		error = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _cURL_write_data);
		error = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Param5OutData);
        
		struct curl_slist *values_CURLOPT_MAIL_RCPT = NULL;
        
		struct curl_slist *values_CURLOPT_QUOTE = NULL;
		struct curl_slist *values_CURLOPT_POSTQUOTE = NULL;
		struct curl_slist *values_CURLOPT_PREQUOTE = NULL;
		struct curl_slist *values_CURLOPT_RESOLVE = NULL;
		struct curl_slist *values_CURLOPT_HTTP200ALIASES = NULL;
        
		struct curl_slist *values_CURLOPT_HTTPHEADER = NULL;
        
		struct curl_httppost *post_CURLOPT_HTTPPOST = NULL;
		struct curl_httppost *last_CURLOPT_HTTPPOST = NULL;
		
		if(Param2Keys.getSize() == Param3Values.getSize()){
            
			CURLoption option;
			CUTF8String value;
			CUTF16String methodName;
			uint32_t _curl_write_method_id;
            uint32_t _curl_read_method_id;
            
			for(unsigned int i = 0; i < Param2Keys.getSize(); ++i){
				
				option = (CURLoption)Param2Keys.getIntValueAtIndex(i);
				Param3Values.copyUTF8StringAtIndex(&value, i);
				
				switch (option) {
					case 0://typically element #0 of array
                    case CURLOPT_URL:
                    case CURLOPT_WRITEDATA:
                    case CURLOPT_READDATA:
                        break;
                    case 11://CURLOPT_WRITEFUNCTION
                        Param3Values.copyUTF16StringAtIndex(&methodName, i);
                        _curl_write_method_id = PA_GetMethodID((PA_Unichar *)methodName.c_str());
                        _curl_write_method_ids.insert(std::map<uint32_t, uint32_t>::value_type(processId, _curl_write_method_id));
                        break;
                    case 12://CURLOPT_READFUNCTION
                        Param3Values.copyUTF16StringAtIndex(&methodName, i);
                        _curl_read_method_id = PA_GetMethodID((PA_Unichar *)methodName.c_str());
                        _curl_read_method_ids.insert(std::map<uint32_t, uint32_t>::value_type(processId, _curl_read_method_id));
                        break;
					case 24://CURLOPT_HTTPPOST
                        if((Param9DataObjects.fType == eVK_ArrayPicture)
                           && (size6 == size7)
                           && (size7 == size8)
                           && (size8 == size9)){
                            for(unsigned int i = 0; i < size8; ++i){
                                
                                CUTF8String formName;
                                Param6DataNames.copyUTF8StringAtIndex(&formName, i);
                                
                                CUTF8String formType;
                                Param7DataTypes.copyUTF8StringAtIndex(&formType, i);
                                
                                CUTF8String formFileName;
                                Param8DataFileNames.copyUTF8StringAtIndex(&formFileName, i);
                                
                                C_TEXT codec;
                                codec.setUTF8String(&formType);
                                PA_Unistring u = PA_CreateUnistring((PA_Unichar *)codec.getUTF16StringPtr());
                                
                                PA_Variable params[3];
                                params[0] = PA_CreateVariable(eVK_Picture);
                                params[1] = PA_CreateVariable(eVK_Blob);
                                params[2] = PA_CreateVariable(eVK_Unistring);
                                
                                PA_Picture picture = PA_GetPictureInArray(Param9DataObjects, i);
                                PA_SetPictureVariable(&params[0], picture);
                                PA_SetBlobVariable(&params[1], 0, 0);
                                PA_SetStringVariable(&params[2], &u);
                                
                                PA_ExecuteCommandByID(692, params, 3);
                                
                                PA_Handle h = PA_GetBlobHandleVariable(params[1]);
                                
                                if(PA_GetHandleSize(h)){
                                    
                                    curl_formadd(&post_CURLOPT_HTTPPOST,
                                                 &last_CURLOPT_HTTPPOST,
                                                 CURLFORM_COPYNAME, formName.c_str(),
                                                 CURLFORM_NAMELENGTH, formName.size(),
                                                 CURLFORM_COPYCONTENTS, PA_LockHandle(h),
                                                 CURLFORM_CONTENTSLENGTH, PA_GetHandleSize(h),
                                                 CURLFORM_CONTENTTYPE, formType.c_str(),
                                                 CURLFORM_END);
                                    
                                    PA_UnlockHandle(h);
                                    
                                }
                                
                                PA_DisposeUnistring(&u);
                            }
                            
                        }
                        
                        if(post_CURLOPT_HTTPPOST){
                            //disable the HTTP 1.1 Expect header for multipart post
                            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post_CURLOPT_HTTPPOST);
                            values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER,
                                                                          (const char *)"Expect:");
                        }
                        
						break;

					case 15://CURLOPT_HTTP_METHOD_OVERRIDE
						
						values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER,
                                                                      (const char *)(CUTF8String((const uint8_t *)"X-HTTP-Method-Override: ") + value).c_str());
                        
						break;
                        
					case 2://CURLOPT_AUTHORIZATION
						
						values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER,
                                                                      (const char *)(CUTF8String((const uint8_t *)"Authorization: ") + value).c_str());
                        
						break;
						
					case 8://CURLOPT_HTTP_CONTENT_TYPE
						
						values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER,
                                                                      (const char *)(CUTF8String((const uint8_t *)"Content-Type: ") + value).c_str());
                        
						break;
						
					case 9://CURLOPT_HTTP_ACCEPT
						
						values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER,
                                                                      (const char *)(CUTF8String((const uint8_t *)"Accept: ") + value).c_str());
                        
						break;
                        
						//integer options
					case 42://CURLOPT_HEADER:
						curl_easy_setopt(curl, CURLOPT_HEADER, atoi((const char *)value.c_str()));
						break;
					case 99://CURLOPT_NOSIGNAL:
						curl_easy_setopt(curl, CURLOPT_NOSIGNAL, atoi((const char *)value.c_str()));
						break;
					case 34://CURLOPT_TIMEVALUE:
						curl_easy_setopt(curl, CURLOPT_TIMEVALUE, atoi((const char *)value.c_str()));
						break;
					case 33://CURLOPT_TIMECONDITION:
						curl_easy_setopt(curl, CURLOPT_TIMECONDITION, atoi((const char *)value.c_str()));
						break;
					case 117://CURLOPT_MAXFILESIZE_LARGE:
						curl_easy_setopt(curl, CURLOPT_MAXFILESIZE_LARGE, atoi((const char *)value.c_str()));
						break;
					case 114://CURLOPT_MAXFILESIZE:
						curl_easy_setopt(curl, CURLOPT_MAXFILESIZE, atoi((const char *)value.c_str()));
						break;
					case 115://CURLOPT_INFILESIZE_LARGE:
						curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, atoi((const char *)value.c_str()));
						break;
					case 14://CURLOPT_INFILESIZE:
						curl_easy_setopt(curl, CURLOPT_INFILESIZE, atoi((const char *)value.c_str()));
						break;
					case 44://CURLOPT_NOBODY:
						curl_easy_setopt(curl, CURLOPT_NOBODY, atoi((const char *)value.c_str()));
						break;
					case 69://CURLOPT_FILETIME:
						curl_easy_setopt(curl, CURLOPT_FILETIME, atoi((const char *)value.c_str()));
						break;
					case 116://CURLOPT_RESUME_FROM_LARGE:
						curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, atoi((const char *)value.c_str()));
						break;
					case 21://CURLOPT_RESUME_FROM:
						curl_easy_setopt(curl, CURLOPT_RESUME_FROM, atoi((const char *)value.c_str()));
						break;
					case 27://CURLOPT_CRLF:
						curl_easy_setopt(curl, CURLOPT_CRLF, atoi((const char *)value.c_str()));
						break;
					case 166://CURLOPT_PROXY_TRANSFER_MODE:
						curl_easy_setopt(curl, CURLOPT_PROXY_TRANSFER_MODE, atoi((const char *)value.c_str()));
						break;
					case 53://CURLOPT_TRANSFERTEXT:
						curl_easy_setopt(curl, CURLOPT_TRANSFERTEXT, atoi((const char *)value.c_str()));
						break;
					case 160://CURLOPT_NEW_DIRECTORY_PERMS:
						curl_easy_setopt(curl, CURLOPT_NEW_DIRECTORY_PERMS, atoi((const char *)value.c_str()));
						break;
					case 159://CURLOPT_NEW_FILE_PERMS:
						curl_easy_setopt(curl, CURLOPT_NEW_FILE_PERMS, atoi((const char *)value.c_str()));
						break;
					case 151://CURLOPT_SSH_AUTH_TYPES:
						curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, atoi((const char *)value.c_str()));
						break;
					case 210://CURLOPT_GSSAPI_DELEGATION:
						curl_easy_setopt(curl, CURLOPT_GSSAPI_DELEGATION, atoi((const char *)value.c_str()));
						break;
					case 216://CURLOPT_SSL_OPTIONS:
						curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, atoi((const char *)value.c_str()));
						break;
					case 150://CURLOPT_SSL_SESSIONID_CACHE:
						curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, atoi((const char *)value.c_str()));
						break;
					case 172://CURLOPT_CERTINFO:
						curl_easy_setopt(curl, CURLOPT_CERTINFO, atoi((const char *)value.c_str()));
						break;
					case 81://CURLOPT_SSL_VERIFYHOST:
						curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, atoi((const char *)value.c_str()));
						break;
					case 64://CURLOPT_SSL_VERIFYPEER:
						curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, atoi((const char *)value.c_str()));
						break;
					case 32://CURLOPT_SSLVERSION:
						curl_easy_setopt(curl, CURLOPT_SSLVERSION, atoi((const char *)value.c_str()));
						break;
					case 90://CURLOPT_SSLENGINE_DEFAULT:
						curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, atoi((const char *)value.c_str()));
						break;
					case 212://CURLOPT_ACCEPTTIMEOUT_MS:
						curl_easy_setopt(curl, CURLOPT_ACCEPTTIMEOUT_MS, atoi((const char *)value.c_str()));
						break;
					case 119://CURLOPT_USE_SSL:
						curl_easy_setopt(curl, CURLOPT_USE_SSL, atoi((const char *)value.c_str()));
						break;
					case 141://CURLOPT_CONNECT_ONLY:
						curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, atoi((const char *)value.c_str()));
						break;
					case 113://CURLOPT_IPRESOLVE:
						curl_easy_setopt(curl, CURLOPT_IPRESOLVE, atoi((const char *)value.c_str()));
						break;
					case 156://CURLOPT_CONNECTTIMEOUT_MS:
						curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, atoi((const char *)value.c_str()));
						break;
					case 78://CURLOPT_CONNECTTIMEOUT:
						curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, atoi((const char *)value.c_str()));
						break;
					case 75://CURLOPT_FORBID_REUSE:
						curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, atoi((const char *)value.c_str()));
						break;
					case 74://CURLOPT_FRESH_CONNECT:
						curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, atoi((const char *)value.c_str()));
						break;
					case 71://CURLOPT_MAXCONNECTS:
						curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, atoi((const char *)value.c_str()));
						break;
					case 146://CURLOPT_MAX_RECV_SPEED_LARGE:
						curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, atoi((const char *)value.c_str()));
						break;
					case 145://CURLOPT_MAX_SEND_SPEED_LARGE:
						curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, atoi((const char *)value.c_str()));
						break;
					case 20://CURLOPT_LOW_SPEED_TIME:
						curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, atoi((const char *)value.c_str()));
						break;
					case 19://CURLOPT_LOW_SPEED_LIMIT:
						curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, atoi((const char *)value.c_str()));
						break;
					case 155://CURLOPT_TIMEOUT_MS:
						curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, atoi((const char *)value.c_str()));
						break;
					case 13://CURLOPT_TIMEOUT:
						curl_easy_setopt(curl, CURLOPT_TIMEOUT, atoi((const char *)value.c_str()));
						break;
					case 189://CURLOPT_RTSP_REQUEST:
						curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, atoi((const char *)value.c_str()));
						break;
					case 193://CURLOPT_RTSP_CLIENT_CSEQ:
						curl_easy_setopt(curl, CURLOPT_RTSP_CLIENT_CSEQ, atoi((const char *)value.c_str()));
						break;
					case 194://CURLOPT_RTSP_SERVER_CSEQ:
						curl_easy_setopt(curl, CURLOPT_RTSP_SERVER_CSEQ, atoi((const char *)value.c_str()));
						break;
					case 178://CURLOPT_TFTP_BLKSIZE:
						curl_easy_setopt(curl, CURLOPT_TFTP_BLKSIZE, atoi((const char *)value.c_str()));
						break;
					case 48://CURLOPT_DIRLISTONLY:
						curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, atoi((const char *)value.c_str()));
						break;
					case 50://CURLOPT_APPEND:
						curl_easy_setopt(curl, CURLOPT_APPEND, atoi((const char *)value.c_str()));
						break;
					case 106://CURLOPT_FTP_USE_EPRT:
						curl_easy_setopt(curl, CURLOPT_FTP_USE_EPRT, atoi((const char *)value.c_str()));
						break;
					case 85://CURLOPT_FTP_USE_EPSV:
						curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, atoi((const char *)value.c_str()));
						break;
					case 188://CURLOPT_FTP_USE_PRET:
						curl_easy_setopt(curl, CURLOPT_FTP_USE_PRET, atoi((const char *)value.c_str()));
						break;
					case 112://CURLOPT_FTP_RESPONSE_TIMEOUT:
						curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, atoi((const char *)value.c_str()));
						break;
					case 137://CURLOPT_FTP_SKIP_PASV_IP:
						curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, atoi((const char *)value.c_str()));
						break;
					case 129://CURLOPT_FTPSSLAUTH:
						curl_easy_setopt(curl, CURLOPT_FTPSSLAUTH, atoi((const char *)value.c_str()));
						break;
					case 154://CURLOPT_FTP_SSL_CCC:
						curl_easy_setopt(curl, CURLOPT_FTP_SSL_CCC, atoi((const char *)value.c_str()));
						break;
					case 134://CURLOPT_FTP_ACCOUNT:
						curl_easy_setopt(curl, CURLOPT_FTP_ACCOUNT, atoi((const char *)value.c_str()));
						break;
					case 138://CURLOPT_FTP_FILEMETHOD:
						curl_easy_setopt(curl, CURLOPT_FTP_FILEMETHOD, atoi((const char *)value.c_str()));
						break;
					case 110://CURLOPT_FTP_CREATE_MISSING_DIRS:
						curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, atoi((const char *)value.c_str()));
						break;
					case 58://CURLOPT_AUTOREFERER:
						curl_easy_setopt(curl, CURLOPT_AUTOREFERER, atoi((const char *)value.c_str()));
						break;
					case 52://CURLOPT_FOLLOWLOCATION:
						curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, atoi((const char *)value.c_str()));
						break;
					case 105://CURLOPT_UNRESTRICTED_AUTH:
						curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, atoi((const char *)value.c_str()));
						break;
					case 68://CURLOPT_MAXREDIRS:
						curl_easy_setopt(curl, CURLOPT_MAXREDIRS, atoi((const char *)value.c_str()));
						break;
					case 161://CURLOPT_POSTREDIR:
						curl_easy_setopt(curl, CURLOPT_POSTREDIR, atoi((const char *)value.c_str()));
						break;
					case 60://CURLOPT_POSTFIELDSIZE:
 						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, atoi((const char *)value.c_str()));
						break;
					case 120://CURLOPT_POSTFIELDSIZE_LARGE:
 						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, atoi((const char *)value.c_str()));
						break;
					case 96://CURLOPT_COOKIESESSION:
 						curl_easy_setopt(curl, CURLOPT_COOKIESESSION, atoi((const char *)value.c_str()));
						break;
					case 80://CURLOPT_HTTPGET:
 						curl_easy_setopt(curl, CURLOPT_HTTPGET, atoi((const char *)value.c_str()));
						break;
					case 84://CURLOPT_HTTP_VERSION:
 						curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, atoi((const char *)value.c_str()));
						break;
					case 158://CURLOPT_HTTP_CONTENT_DECODING:
 						curl_easy_setopt(curl, CURLOPT_HTTP_CONTENT_DECODING, atoi((const char *)value.c_str()));
						break;
					case 157://CURLOPT_HTTP_TRANSFER_DECODING:
 						curl_easy_setopt(curl, CURLOPT_HTTP_TRANSFER_DECODING, atoi((const char *)value.c_str()));
						break;
					case 136://CURLOPT_IGNORE_CONTENT_LENGTH:
 						curl_easy_setopt(curl, CURLOPT_IGNORE_CONTENT_LENGTH, atoi((const char *)value.c_str()));
						break;
					case 111://CURLOPT_PROXYAUTH:
 						curl_easy_setopt(curl, CURLOPT_PROXYAUTH, atoi((const char *)value.c_str()));
						break;
					case 206://CURLOPT_TLSAUTH_TYPE:
 						curl_easy_setopt(curl, CURLOPT_TLSAUTH_TYPE, atoi((const char *)value.c_str()));
						break;
					case 107://CURLOPT_HTTPAUTH:
 						curl_easy_setopt(curl, CURLOPT_HTTPAUTH, atoi((const char *)value.c_str()));
						break;
					case 51://CURLOPT_NETRC:
 						curl_easy_setopt(curl, CURLOPT_NETRC, atoi((const char *)value.c_str()));
						break;
					case 215://CURLOPT_TCP_KEEPINTVL:
 						curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, atoi((const char *)value.c_str()));
						break;
					case 214://CURLOPT_TCP_KEEPIDLE:
 						curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, atoi((const char *)value.c_str()));
						break;
					case 213://CURLOPT_TCP_KEEPALIVE:
 						curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, atoi((const char *)value.c_str()));
						break;
					case 171://CURLOPT_ADDRESS_SCOPE:
 						curl_easy_setopt(curl, CURLOPT_ADDRESS_SCOPE, atoi((const char *)value.c_str()));
						break;
					case 121://CURLOPT_TCP_NODELAY:
 						curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, atoi((const char *)value.c_str()));
						break;
					case 3://CURLOPT_PORT:
 						curl_easy_setopt(curl, CURLOPT_PORT, atoi((const char *)value.c_str()));
						break;
					case 98://CURLOPT_BUFFERSIZE:
 						curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, atoi((const char *)value.c_str()));
						break;
					case 92://CURLOPT_DNS_CACHE_TIMEOUT:
 						curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, atoi((const char *)value.c_str()));
						break;
					case 140://CURLOPT_LOCALPORTRANGE:
 						curl_easy_setopt(curl, CURLOPT_LOCALPORTRANGE, atoi((const char *)value.c_str()));
						break;
					case 139://CURLOPT_LOCALPORT:
 						curl_easy_setopt(curl, CURLOPT_LOCALPORT, atoi((const char *)value.c_str()));
						break;
					case 180://CURLOPT_SOCKS5_GSSAPI_NEC:
 						curl_easy_setopt(curl, CURLOPT_SOCKS5_GSSAPI_NEC, atoi((const char *)value.c_str()));
						break;
					case 61://CURLOPT_HTTPPROXYTUNNEL:
 						curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, atoi((const char *)value.c_str()));
						break;
					case 101://CURLOPT_PROXYTYPE:
 						curl_easy_setopt(curl, CURLOPT_PROXYTYPE, atoi((const char *)value.c_str()));
						break;
					case 59://CURLOPT_PROXYPORT:
 						curl_easy_setopt(curl, CURLOPT_PROXYPORT, atoi((const char *)value.c_str()));
						break;
					case 182://CURLOPT_REDIR_PROTOCOLS:
 						curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, atoi((const char *)value.c_str()));
						break;
					case 181://CURLOPT_PROTOCOLS:
						curl_easy_setopt(curl, CURLOPT_PROTOCOLS, atoi((const char *)value.c_str()));
						break;
						//set the body size here, for security
					case 54://CURLOPT_PUT:
						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, Param4InData.getBytesLength());
						curl_easy_setopt(curl, CURLOPT_PUT, atoi((const char *)value.c_str()));
						break;
					case 47://CURLOPT_POST:
						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, Param4InData.getBytesLength());
						curl_easy_setopt(curl, CURLOPT_POST, atoi((const char *)value.c_str()));
						break;
					case 46://CURLOPT_UPLOAD:
						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, Param4InData.getBytesLength());
						curl_easy_setopt(curl, CURLOPT_UPLOAD, atoi((const char *)value.c_str()));
						break;
					case 187://CURLOPT_MAIL_RCPT
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - p);
							values_CURLOPT_MAIL_RCPT = curl_slist_append(values_CURLOPT_MAIL_RCPT, (const char *)v.c_str());
                            
						}
						
						if(values_CURLOPT_MAIL_RCPT) curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, values_CURLOPT_MAIL_RCPT);
                        
						break;
					case 28://CURLOPT_QUOTE
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - p);
							values_CURLOPT_QUOTE = curl_slist_append(values_CURLOPT_QUOTE, (const char *)v.c_str());
                            
						}
						
						if(values_CURLOPT_QUOTE) curl_easy_setopt(curl, CURLOPT_QUOTE, values_CURLOPT_QUOTE);
                        
						break;
					case 39://CURLOPT_POSTQUOTE
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - p);
							values_CURLOPT_POSTQUOTE = curl_slist_append(values_CURLOPT_POSTQUOTE, (const char *)v.c_str());
                            
						}
						
						if(values_CURLOPT_POSTQUOTE) curl_easy_setopt(curl, CURLOPT_POSTQUOTE, values_CURLOPT_POSTQUOTE);
                        
						break;
					case 93://CURLOPT_PREQUOTE
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - p);
							values_CURLOPT_PREQUOTE = curl_slist_append(values_CURLOPT_PREQUOTE, (const char *)v.c_str());
                            
						}
						
						if(values_CURLOPT_PREQUOTE) curl_easy_setopt(curl, CURLOPT_PREQUOTE, values_CURLOPT_PREQUOTE);
                        
						break;
					case 203://CURLOPT_RESOLVE
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - p);
							values_CURLOPT_RESOLVE = curl_slist_append(values_CURLOPT_RESOLVE, (const char *)v.c_str());
                            
						}
						
						if(values_CURLOPT_RESOLVE) curl_easy_setopt(curl, CURLOPT_RESOLVE, values_CURLOPT_RESOLVE);
                        
						break;
					case 104://CURLOPT_HTTP200ALIASES
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - p);
							values_CURLOPT_HTTP200ALIASES = curl_slist_append(values_CURLOPT_HTTP200ALIASES, (const char *)v.c_str());
                            
						}
						
						if(values_CURLOPT_HTTP200ALIASES) curl_easy_setopt(curl, CURLOPT_HTTP200ALIASES, values_CURLOPT_HTTP200ALIASES);
                        
						break;
					case 36://CURLOPT_CUSTOMREQUEST:
						curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, value.c_str());
						break;
					case 102://CURLOPT_ACCEPT_ENCODING:
						curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, value.c_str());
						break;
					case 118://CURLOPT_NETRC_FILE:
						curl_easy_setopt(curl, CURLOPT_NETRC_FILE, value.c_str());
						break;
					case 135://CURLOPT_COOKIELIST:
						curl_easy_setopt(curl, CURLOPT_COOKIELIST, value.c_str());
						break;
					case 147://CURLOPT_FTP_ALTERNATIVE_TO_USE:
						curl_easy_setopt(curl, CURLOPT_FTP_ALTERNATIVE_TO_USER, value.c_str());
						break;
					case 152://CURLOPT_SSH_PUBLIC_KEYFILE:
						curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, value.c_str());
						break;
					case 153://CURLOPT_SSH_PRIVATE_KEYFILE:
						curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, value.c_str());
						break;
					case 16://CURLOPT_REFERER:
						curl_easy_setopt(curl, CURLOPT_REFERER, value.c_str());
						break;
					case 162://CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
						curl_easy_setopt(curl, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, value.c_str());
						break;
					case 169://CURLOPT_CRLFILE:
						curl_easy_setopt(curl, CURLOPT_CRLFILE, value.c_str());
						break;
					case 17://CURLOPT_FTPPORT:
						curl_easy_setopt(curl, CURLOPT_FTPPORT, value.c_str());
						break;
					case 170://CURLOPT_ISSUERCERT:
						curl_easy_setopt(curl, CURLOPT_ISSUERCERT, value.c_str());
						break;
					case 173://CURLOPT_USERNAME:
						curl_easy_setopt(curl, CURLOPT_USERNAME, value.c_str());
						break;
					case 174://CURLOPT_PASSWORD:
						curl_easy_setopt(curl, CURLOPT_PASSWORD, value.c_str());
						break;
					case 175://CURLOPT_PROXYUSERNAME:
						curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, value.c_str());
						break;
					case 176://CURLOPT_PROXYPASSWORD:
						curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, value.c_str());
						break;
					case 177://CURLOPT_NOPROXY:
						curl_easy_setopt(curl, CURLOPT_NOPROXY, value.c_str());
						break;
					case 179://CURLOPT_SOCKS5_GSSAPI_SERVICE:
						curl_easy_setopt(curl, CURLOPT_SOCKS5_GSSAPI_SERVICE, value.c_str());
						break;
					case 18://CURLOPT_USERAGENT:
						curl_easy_setopt(curl, CURLOPT_USERAGENT, value.c_str());
						break;
					case 183://CURLOPT_SSH_KNOWNHOSTS:
						curl_easy_setopt(curl, CURLOPT_SSH_KNOWNHOSTS, value.c_str());
						break;
					case 186://CURLOPT_MAIL_FROM:
						curl_easy_setopt(curl, CURLOPT_MAIL_FROM, value.c_str());
						break;
					case 190://CURLOPT_RTSP_SESSION_ID:
						curl_easy_setopt(curl, CURLOPT_RTSP_SESSION_ID, value.c_str());
						break;
					case 191://CURLOPT_RTSP_STREAM_URI:
						curl_easy_setopt(curl, CURLOPT_RTSP_STREAM_URI, value.c_str());
						break;
					case 192://CURLOPT_RTSP_TRANSPORT:
						curl_easy_setopt(curl, CURLOPT_RTSP_TRANSPORT, value.c_str());
						break;
					case 197://CURLOPT_WILDCARDMATCH:
						curl_easy_setopt(curl, CURLOPT_WILDCARDMATCH, value.c_str());
						break;
					case 204://CURLOPT_TLSAUTH_USERNAME:
						curl_easy_setopt(curl, CURLOPT_TLSAUTH_USERNAME, value.c_str());
						break;
					case 205://CURLOPT_TLSAUTH_PASSWORD:
						curl_easy_setopt(curl, CURLOPT_TLSAUTH_PASSWORD, value.c_str());
						break;
					case 207://CURLOPT_TRANSFER_ENCODING:
						curl_easy_setopt(curl, CURLOPT_TRANSFER_ENCODING, value.c_str());
						break;
					case 217://CURLOPT_MAIL_AUTH:
						curl_easy_setopt(curl, CURLOPT_MAIL_AUTH, value.c_str());
						break;
					case 25://CURLOPT_SSLCERT:
						curl_easy_setopt(curl, CURLOPT_SSLCERT, value.c_str());
						break;
					case 26://CURLOPT_KEYPASSWD:
						curl_easy_setopt(curl, CURLOPT_KEYPASSWD, value.c_str());
						break;
					case 31://CURLOPT_COOKIEFILE:
						curl_easy_setopt(curl, CURLOPT_COOKIEFILE, value.c_str());
						break;
					case 4://CURLOPT_PROXY:
						curl_easy_setopt(curl, CURLOPT_PROXY, value.c_str());
						break;
					case 5://CURLOPT_USERPWD:
						curl_easy_setopt(curl, CURLOPT_USERPWD, value.c_str());
						break;
					case 6://CURLOPT_PROXYUSERPWD:
						curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, value.c_str());
						break;
					case 62://CURLOPT_INTERFACE:
						curl_easy_setopt(curl, CURLOPT_INTERFACE, value.c_str());
						break;
					case 63://CURLOPT_KRBLEVEL:
						curl_easy_setopt(curl, CURLOPT_KRBLEVEL, value.c_str());
						break;
					case 65://CURLOPT_CAINFO:
						curl_easy_setopt(curl, CURLOPT_CAINFO, value.c_str());
						break;
					case 7://CURLOPT_RANGE:
						curl_easy_setopt(curl, CURLOPT_RANGE, value.c_str());
						break;
					case 76://CURLOPT_RANDOM_FILE:
						curl_easy_setopt(curl, CURLOPT_RANDOM_FILE, value.c_str());
						break;
					case 77://CURLOPT_EGDSOCKET:
						curl_easy_setopt(curl, CURLOPT_EGDSOCKET, value.c_str());
						break;
					case 82://CURLOPT_COOKIEJAR:
						curl_easy_setopt(curl, CURLOPT_COOKIEJAR, value.c_str());
						break;
					case 83://CURLOPT_SSL_CIPHER_LIST:
						curl_easy_setopt(curl, CURLOPT_SSL_CIPHER_LIST, value.c_str());
						break;
					case 86://CURLOPT_SSLCERTTYPE:
						curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, value.c_str());
						break;
					case 87://CURLOPT_SSLKEY:
						curl_easy_setopt(curl, CURLOPT_SSLKEY, value.c_str());
						break;
					case 88://CURLOPT_SSLKEYTYPE:
						curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, value.c_str());
						break;
					case 89://CURLOPT_SSLENGINE:
						curl_easy_setopt(curl, CURLOPT_SSLENGINE, value.c_str());
						break;
					default:
						curl_easy_setopt(curl, option, value.c_str());
						break;
				}
				
			}
            
		}
		
        if(values_CURLOPT_HTTPHEADER) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, values_CURLOPT_HTTPHEADER);
        
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        error = curl_easy_perform(curl);
        
        if(values_CURLOPT_MAIL_RCPT) curl_slist_free_all(values_CURLOPT_MAIL_RCPT);
        
        if(values_CURLOPT_QUOTE) curl_slist_free_all(values_CURLOPT_QUOTE);
        if(values_CURLOPT_POSTQUOTE) curl_slist_free_all(values_CURLOPT_POSTQUOTE);
        if(values_CURLOPT_PREQUOTE) curl_slist_free_all(values_CURLOPT_PREQUOTE);
        if(values_CURLOPT_RESOLVE) curl_slist_free_all(values_CURLOPT_RESOLVE);
        if(values_CURLOPT_HTTP200ALIASES) curl_slist_free_all(values_CURLOPT_HTTP200ALIASES);
        
        if(values_CURLOPT_HTTPHEADER) curl_slist_free_all(values_CURLOPT_HTTPHEADER);
        if(post_CURLOPT_HTTPPOST) curl_formfree(post_CURLOPT_HTTPPOST);
		
        curl_easy_cleanup(curl);
        
        std::map<uint32_t, uint32_t>::iterator pos;
        
        pos = _curl_write_method_ids.find(processId);
        if(pos != _curl_write_method_ids.end()) _curl_write_method_ids.erase(pos);
        
        pos = _curl_read_method_ids.find(processId);
        if(pos != _curl_read_method_ids.end()) _curl_read_method_ids.erase(pos);
        
	}
	
	if(error != CURLE_OK) returnValue.setIntValue(error);
	
	Param5OutData.toParamAtIndex(pParams, 5);
	returnValue.setReturn(pResult);
    
}