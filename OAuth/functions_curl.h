#if VERSIONMAC
#include "curl/curl.h"
#else
#include "windows/curl/curl.h"
#endif
#include "4DPluginAPI.h"
#include "4DPlugin.h"

void _cURL(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Escape_url(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Unescape_url(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Get_version(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Get_date(sLONG_PTR *pResult, PackagePtr pParams);