#include "curl/curl.h"
#include "4DPluginAPI.h"
#include "4DPlugin.h"

void _cURL(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Escape_url(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Unescape_url(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Get_version(sLONG_PTR *pResult, PackagePtr pParams);
void cURL_Get_date(sLONG_PTR *pResult, PackagePtr pParams);