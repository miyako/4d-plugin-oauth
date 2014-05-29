#include "4DPluginAPI.h"
#include "4DPlugin.h"

void RSASHA256(sLONG_PTR *pResult, PackagePtr pParams);
void HMACMD5(sLONG_PTR *pResult, PackagePtr pParams);
void HMACSHA1(sLONG_PTR *pResult, PackagePtr pParams);
void HMACSHA256(sLONG_PTR *pResult, PackagePtr pParams);
void HMACSHA384(sLONG_PTR *pResult, PackagePtr pParams);
void HMACSHA512(sLONG_PTR *pResult, PackagePtr pParams);
void SHA384(sLONG_PTR *pResult, PackagePtr pParams);
void SHA512(sLONG_PTR *pResult, PackagePtr pParams);
void MD5(sLONG_PTR *pResult, PackagePtr pParams);
void SHA1(sLONG_PTR *pResult, PackagePtr pParams);
void SHA256(sLONG_PTR *pResult, PackagePtr pParams);

// --- PKCS
void PEM_From_P12(sLONG_PTR *pResult, PackagePtr pParams);

// --- Common Crypto II
void RSASHA1(sLONG_PTR *pResult, PackagePtr pParams);