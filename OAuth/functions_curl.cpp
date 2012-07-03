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
	
	Param1Url.fromParamAtIndex(pParams, 1);
	Param2Keys.fromParamAtIndex(pParams, 2);
	Param3Values.fromParamAtIndex(pParams, 3);
	Param4InData.fromParamAtIndex(pParams, 4);
	
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
		struct curl_slist *values_CURLOPT_AUTHORIZATION = NULL;
		
		if(Param2Keys.getSize() == Param3Values.getSize()){
			
			CURLoption option;
			CUTF8String value;			
			
			for(unsigned int i = 0; i < Param2Keys.getSize(); ++i){
				
				option = (CURLoption)Param2Keys.getIntValueAtIndex(i);
				Param3Values.copyUTF8StringAtIndex(&value, i);
				
				switch (option) {
						//no action;require linked list						
					case CURLOPT_HTTPPOST:							
					case CURLOPT_HTTPHEADER:							
						break;	
						
					case 2://CURLOPT_AUTHORIZATION
						
						values_CURLOPT_AUTHORIZATION = curl_slist_append(values_CURLOPT_AUTHORIZATION, (const char *)(CUTF8String((const uint8_t *)"Authorization: ") + value).c_str());  
						curl_easy_setopt(curl, CURLOPT_HTTPHEADER, values_CURLOPT_AUTHORIZATION);
						
						break;		
						//no action;dedicated methods exist 						
					case CURLOPT_URL:
					case CURLOPT_WRITEFUNCTION:	
					case CURLOPT_WRITEDATA:	
					case CURLOPT_READFUNCTION:							
					case CURLOPT_READDATA:							
						break;
						//integer options
					case CURLOPT_HEADER:							
					case CURLOPT_NOSIGNAL:						
					case CURLOPT_TIMEVALUE:	
					case CURLOPT_TIMECONDITION:	
					case CURLOPT_MAXFILESIZE_LARGE:	
					case CURLOPT_MAXFILESIZE:	
					case CURLOPT_INFILESIZE_LARGE:	
					case CURLOPT_INFILESIZE:	
					case CURLOPT_NOBODY:	
					case CURLOPT_FILETIME:	
					case CURLOPT_RESUME_FROM_LARGE:	
					case CURLOPT_RESUME_FROM:	
					case CURLOPT_CRLF:	
					case CURLOPT_PROXY_TRANSFER_MODE:	
					case CURLOPT_TRANSFERTEXT:		
					case CURLOPT_NEW_DIRECTORY_PERMS:					
					case CURLOPT_NEW_FILE_PERMS:						
					case CURLOPT_SSH_AUTH_TYPES:							
					case CURLOPT_GSSAPI_DELEGATION:							
					case CURLOPT_SSL_OPTIONS:								
					case CURLOPT_SSL_SESSIONID_CACHE:								
					case CURLOPT_CERTINFO:							
					case CURLOPT_SSL_VERIFYHOST:	
					case CURLOPT_SSL_VERIFYPEER:							
					case CURLOPT_SSLVERSION:	
					case CURLOPT_SSLENGINE_DEFAULT:							
					case CURLOPT_ACCEPTTIMEOUT_MS:							
					case CURLOPT_USE_SSL:						
					case CURLOPT_CONNECT_ONLY:						
					case CURLOPT_IPRESOLVE:	
					case CURLOPT_CONNECTTIMEOUT_MS:							
					case CURLOPT_CONNECTTIMEOUT:	
					case CURLOPT_FORBID_REUSE:	
					case CURLOPT_FRESH_CONNECT:	
					case CURLOPT_MAXCONNECTS:	
					case CURLOPT_MAX_RECV_SPEED_LARGE:	
					case 146:							
					case CURLOPT_MAX_SEND_SPEED_LARGE:
					case 145:						
					case CURLOPT_LOW_SPEED_TIME:	
					case CURLOPT_LOW_SPEED_LIMIT:	
					case CURLOPT_TIMEOUT_MS:							
					case CURLOPT_TIMEOUT:														
					case CURLOPT_RTSP_REQUEST:						
					case CURLOPT_RTSP_CLIENT_CSEQ:								
					case CURLOPT_RTSP_SERVER_CSEQ:						
					case CURLOPT_TFTP_BLKSIZE:					
					case CURLOPT_DIRLISTONLY:							
					case CURLOPT_APPEND:	
					case CURLOPT_FTP_USE_EPRT:	
					case CURLOPT_FTP_USE_EPSV:	
					case CURLOPT_FTP_USE_PRET:							
					case CURLOPT_FTP_RESPONSE_TIMEOUT:	
					case CURLOPT_FTP_SKIP_PASV_IP:							
					case CURLOPT_FTPSSLAUTH:	
					case CURLOPT_FTP_SSL_CCC:						
					case CURLOPT_FTP_ACCOUNT:							
					case CURLOPT_FTP_FILEMETHOD:						
					case CURLOPT_FTP_CREATE_MISSING_DIRS:	
					case CURLOPT_AUTOREFERER:
					case CURLOPT_FOLLOWLOCATION:
					case CURLOPT_UNRESTRICTED_AUTH:
					case CURLOPT_MAXREDIRS:
					case CURLOPT_POSTREDIR:							
					case CURLOPT_POSTFIELDSIZE:
					case CURLOPT_POSTFIELDSIZE_LARGE:
					case CURLOPT_COOKIESESSION:
					case CURLOPT_HTTPGET:
					case CURLOPT_HTTP_VERSION:
					case CURLOPT_HTTP_CONTENT_DECODING:					
					case CURLOPT_HTTP_TRANSFER_DECODING:							
					case CURLOPT_IGNORE_CONTENT_LENGTH:
					case CURLOPT_PROXYAUTH:
					case CURLOPT_TLSAUTH_TYPE:							
					case CURLOPT_HTTPAUTH:
					case CURLOPT_NETRC:
					case CURLOPT_TCP_KEEPINTVL:						
					case CURLOPT_TCP_KEEPIDLE:						
					case CURLOPT_TCP_KEEPALIVE:
					case CURLOPT_ADDRESS_SCOPE:						
					case CURLOPT_TCP_NODELAY:
					case CURLOPT_PORT:
					case CURLOPT_BUFFERSIZE:
					case CURLOPT_DNS_CACHE_TIMEOUT:
					case CURLOPT_LOCALPORTRANGE:							
					case CURLOPT_LOCALPORT:						
					case CURLOPT_SOCKS5_GSSAPI_NEC:							
					case CURLOPT_HTTPPROXYTUNNEL:						
					case CURLOPT_PROXYTYPE:						
					case CURLOPT_PROXYPORT:						
					case CURLOPT_REDIR_PROTOCOLS:						
					case CURLOPT_PROTOCOLS:
						
						curl_easy_setopt(curl, option, atoi((const char *)value.c_str()));	
						break;
						//set the body size here, for security
					case CURLOPT_PUT:						
					case CURLOPT_POST:
					case CURLOPT_UPLOAD:						
						curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, Param4InData.getBytesLength());
						curl_easy_setopt(curl, option, atoi((const char *)value.c_str()));	
						break;						
						//comma to linked list; to be implemented						
					case CURLOPT_MAIL_RCPT:		
					case 187:							
						//		case CURLOPT_QUOTE:		
						//		case CURLOPT_POSTQUOTE:						
						//		case CURLOPT_PREQUOTE:	
						//		case CURLOPT_RESOLVE:						
						//		case CURLOPT_HTTP200ALIASES:	
						
						for(unsigned int p = 0, n; p <= value.length(); p = n + 1){
							
							n = value.find_first_of((const uint8_t *)",", p);
							if(n == CUTF8String::npos) n = value.length();
							CUTF8String v = value.substr(p, n - i);
							values_CURLOPT_MAIL_RCPT = curl_slist_append(values_CURLOPT_MAIL_RCPT, (const char *)v.c_str());
							
						}	
						
						if(values_CURLOPT_MAIL_RCPT) curl_easy_setopt(curl, option, values_CURLOPT_MAIL_RCPT);
						
						break;		
						
					default:
						curl_easy_setopt(curl, option, value.c_str());							
						break;
				}
				
			}
			
		}
		
		error = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		
		error = curl_easy_perform(curl);
		
		if(values_CURLOPT_MAIL_RCPT) curl_slist_free_all(values_CURLOPT_MAIL_RCPT);
		if(values_CURLOPT_AUTHORIZATION) curl_slist_free_all(values_CURLOPT_AUTHORIZATION);
		
		curl_easy_cleanup(curl);		
		
	}	
	
	if(error != CURLE_OK) returnValue.setIntValue(error);	
	
	Param5OutData.toParamAtIndex(pParams, 5);
	returnValue.setReturn(pResult);
}

