#include "4DPluginAPI.h"
#include "4DPlugin.h"

#if VERSIONMAC
#include "iconv.h"
#if PA_SMALLENDIAN
#define UTF32NATIVE "UTF-32LE"
#else
#define UTF32NATIVE "UTF-32BE"
#endif
#endif

void JSON_Strip_white_space(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Parse_text(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Export_to_text(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_CLOSE(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_New(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_GET_BOOL_ARRAY(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_GET_CHILD_NODES(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_GET_LONG_ARRAY(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_child_by_name(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_child_by_position(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_comment(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_GET_TEXT_ARRAY(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_GET_REAL_ARRAY(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_bool(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_real(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_long(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_type(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_name(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Get_text(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_DELETE_ITEM_BY_NAME(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_DELETE_ITEM_BY_POSITION(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_TYPE(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_NULL(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_COMMENT(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_NAME(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_TEXT(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_LONG(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_REAL(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_bool_array(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_real_array(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_long_array(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_text_array(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_text(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_long(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_real(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_bool(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_node(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_SET_BOOL(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_CLEAR(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_array(sLONG_PTR *pResult, PackagePtr pParams);
void JSON_Append_array_element(sLONG_PTR *pResult, PackagePtr pParams);