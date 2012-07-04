#include "functions_curl.h"

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
	
	return len;
	
}

size_t _cURL_read_data(void *buffer, size_t size, size_t nmemb, C_BLOB &inData){
	
	PA_YieldAbsolute();
	
	size_t len = size * nmemb;
	
	const uint8_t *ptr = inData.getBytesPtrForSize((uint32_t *)&len);
	
	if(ptr) memcpy(buffer, ptr, len);
	
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
        
		struct curl_slist *values_CURLOPT_HTTPHEADER = NULL;
        
		struct curl_httppost *post_CURLOPT_HTTPPOST = NULL;
		struct curl_httppost *last_CURLOPT_HTTPPOST = NULL;
		
		if(Param2Keys.getSize() == Param3Values.getSize()){
            
			CURLoption option;
			CUTF8String value;			
			
			for(unsigned int i = 0; i < Param2Keys.getSize(); ++i){
				
				option = (CURLoption)Param2Keys.getIntValueAtIndex(i);
				Param3Values.copyUTF8StringAtIndex(&value, i);
				
				switch (option) {
					case 0://typically element #0 of array
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
                            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post_CURLOPT_HTTPPOST);
                            values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER, 
                                                                          (const char *)"Expect:");                             
                        } 
                        
						break;
                        
					case 2://CURLOPT_AUTHORIZATION
						
						values_CURLOPT_HTTPHEADER = curl_slist_append(values_CURLOPT_HTTPHEADER, 
                                                                      (const char *)(CUTF8String((const uint8_t *)"Authorization: ") + value).c_str());  
                        
						break;	
                        
						//no action;dedicated methods exist 						
                        //					case CURLOPT_URL:
                        //					case CURLOPT_WRITEFUNCTION:	
                        //					case CURLOPT_WRITEDATA:	
                        //					case CURLOPT_READFUNCTION:							
                        //					case CURLOPT_READDATA:							
                        //						break;
                        
						//integer options
					case 42://CURLOPT_HEADER:							
					case 99://CURLOPT_NOSIGNAL:						
					case 34://CURLOPT_TIMEVALUE:	
					case 33://CURLOPT_TIMECONDITION:	
					case 117://CURLOPT_MAXFILESIZE_LARGE:	
					case 114://CURLOPT_MAXFILESIZE:	
					case 115://CURLOPT_INFILESIZE_LARGE:	
					case 14://CURLOPT_INFILESIZE:	
					case 44://CURLOPT_NOBODY:	
					case 69://CURLOPT_FILETIME:	
					case 116://CURLOPT_RESUME_FROM_LARGE:	
					case 21://CURLOPT_RESUME_FROM:	
					case 27://CURLOPT_CRLF:	
					case 166://CURLOPT_PROXY_TRANSFER_MODE:	
					case 53://CURLOPT_TRANSFERTEXT:		
					case 160://CURLOPT_NEW_DIRECTORY_PERMS:					
					case 159://CURLOPT_NEW_FILE_PERMS:						
					case 151://CURLOPT_SSH_AUTH_TYPES:							
					case 210://CURLOPT_GSSAPI_DELEGATION:							
					case 216://CURLOPT_SSL_OPTIONS:								
					case 150://CURLOPT_SSL_SESSIONID_CACHE:								
					case 172://CURLOPT_CERTINFO:							
					case 81://CURLOPT_SSL_VERIFYHOST:	
					case 64://CURLOPT_SSL_VERIFYPEER:							
					case 32://CURLOPT_SSLVERSION:	
					case 90://CURLOPT_SSLENGINE_DEFAULT:							
					case 212://CURLOPT_ACCEPTTIMEOUT_MS:							
					case 119://CURLOPT_USE_SSL:						
					case 141://CURLOPT_CONNECT_ONLY:						
					case 113://CURLOPT_IPRESOLVE:	
					case 156://CURLOPT_CONNECTTIMEOUT_MS:							
					case 78://CURLOPT_CONNECTTIMEOUT:	
					case 75://CURLOPT_FORBID_REUSE:	
					case 74://CURLOPT_FRESH_CONNECT:	
					case 71://CURLOPT_MAXCONNECTS:	
					case 146://CURLOPT_MAX_RECV_SPEED_LARGE:							
					case 145://CURLOPT_MAX_SEND_SPEED_LARGE:						
					case 20://CURLOPT_LOW_SPEED_TIME:	
					case 19://CURLOPT_LOW_SPEED_LIMIT:	
					case 155://CURLOPT_TIMEOUT_MS:							
					case 13://CURLOPT_TIMEOUT:														
					case 189://CURLOPT_RTSP_REQUEST:						
					case 193://CURLOPT_RTSP_CLIENT_CSEQ:								
					case 194://CURLOPT_RTSP_SERVER_CSEQ:						
					case 178://CURLOPT_TFTP_BLKSIZE:					
					case 48://CURLOPT_DIRLISTONLY:							
					case 50://CURLOPT_APPEND:	
					case 106://CURLOPT_FTP_USE_EPRT:	
					case 85://CURLOPT_FTP_USE_EPSV:	
					case 188://CURLOPT_FTP_USE_PRET:							
					case 112://CURLOPT_FTP_RESPONSE_TIMEOUT:	
					case 137://CURLOPT_FTP_SKIP_PASV_IP:							
					case 129://CURLOPT_FTPSSLAUTH:	
					case 154://CURLOPT_FTP_SSL_CCC:						
					case 134://CURLOPT_FTP_ACCOUNT:							
					case 138://CURLOPT_FTP_FILEMETHOD:						
					case 110://CURLOPT_FTP_CREATE_MISSING_DIRS:	
					case 58://CURLOPT_AUTOREFERER:
					case 52://CURLOPT_FOLLOWLOCATION:
					case 105://CURLOPT_UNRESTRICTED_AUTH:
					case 68://CURLOPT_MAXREDIRS:
					case 161://CURLOPT_POSTREDIR:							
					case 60://CURLOPT_POSTFIELDSIZE:
					case 120://CURLOPT_POSTFIELDSIZE_LARGE:
					case 96://CURLOPT_COOKIESESSION:
					case 80://CURLOPT_HTTPGET:
					case 84://CURLOPT_HTTP_VERSION:
					case 158://CURLOPT_HTTP_CONTENT_DECODING:					
					case 157://CURLOPT_HTTP_TRANSFER_DECODING:							
					case 136://CURLOPT_IGNORE_CONTENT_LENGTH:
					case 111://CURLOPT_PROXYAUTH:
					case 206://CURLOPT_TLSAUTH_TYPE:							
					case 107://CURLOPT_HTTPAUTH:
					case 51://CURLOPT_NETRC:
					case 215://CURLOPT_TCP_KEEPINTVL:						
					case 214://CURLOPT_TCP_KEEPIDLE:						
					case 213://CURLOPT_TCP_KEEPALIVE:
					case 171://CURLOPT_ADDRESS_SCOPE:						
					case 121://CURLOPT_TCP_NODELAY:
					case 3://CURLOPT_PORT:
					case 98://CURLOPT_BUFFERSIZE:
					case 92://CURLOPT_DNS_CACHE_TIMEOUT:
					case 140://CURLOPT_LOCALPORTRANGE:							
					case 139://CURLOPT_LOCALPORT:						
					case 180://CURLOPT_SOCKS5_GSSAPI_NEC:							
					case 61://CURLOPT_HTTPPROXYTUNNEL:						
					case 101://CURLOPT_PROXYTYPE:						
					case 59://CURLOPT_PROXYPORT:						
					case 182://CURLOPT_REDIR_PROTOCOLS:						
					case 181://CURLOPT_PROTOCOLS:
						
						curl_easy_setopt(curl, option, atoi((const char *)value.c_str()));	
						break;
						//set the body size here, for security
					case 54://CURLOPT_PUT:						
					case 47://CURLOPT_POST:
					case 46://CURLOPT_UPLOAD:						
						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, Param4InData.getBytesLength());
						curl_easy_setopt(curl, option, atoi((const char *)value.c_str()));	
						break;						
						
					case 187://CURLOPT_MAIL_RCPT							
                        
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - i);
							values_CURLOPT_MAIL_RCPT = curl_slist_append(values_CURLOPT_MAIL_RCPT, (const char *)v.c_str());
                            
						}	
						
						if(values_CURLOPT_MAIL_RCPT) curl_easy_setopt(curl, option, values_CURLOPT_MAIL_RCPT);
                        
						break;	
                        
						//comma to linked list; not implemented                        
                        //                    case CURLOPT_QUOTE:		
                        //                    case CURLOPT_POSTQUOTE:						
                        //                    case CURLOPT_PREQUOTE:	
                        //                    case CURLOPT_RESOLVE:						
                        //                    case CURLOPT_HTTP200ALIASES:
                        //						break;
                        
					default:
						curl_easy_setopt(curl, option, value.c_str());							
						break;
				}
				
			}
            
		}
		
        if(values_CURLOPT_HTTPHEADER) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, values_CURLOPT_HTTPHEADER);
        
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
		error = curl_easy_perform(curl);
        
		if(values_CURLOPT_MAIL_RCPT) curl_slist_free_all(values_CURLOPT_MAIL_RCPT);
		if(values_CURLOPT_HTTPHEADER) curl_slist_free_all(values_CURLOPT_HTTPHEADER);
		if(post_CURLOPT_HTTPPOST) curl_formfree(post_CURLOPT_HTTPPOST);
		
		curl_easy_cleanup(curl);		
        
	}	
	
	if(error != CURLE_OK) returnValue.setIntValue(error);	
	
	Param5OutData.toParamAtIndex(pParams, 5);
	returnValue.setReturn(pResult);
}