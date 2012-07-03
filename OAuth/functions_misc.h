#include "4DPluginAPI.h"
#include "4DPlugin.h"

void PATH_From_user_selection(sLONG_PTR *pResult, PackagePtr pParams);
void PATH_Get_known_folder(sLONG_PTR *pResult, PackagePtr pParams);
void VOLUME_Is_ejectable(sLONG_PTR *pResult, PackagePtr pParams);
void VOLUME_Is_remote(sLONG_PTR *pResult, PackagePtr pParams);
void STRUCTURE_Import_definition(sLONG_PTR *pResult, PackagePtr pParams);
void SYSTEM_Get_unixtime(sLONG_PTR *pResult, PackagePtr pParams);
void SYSTEM_Generate_UUID(sLONG_PTR *pResult, PackagePtr pParams);
void SYSTEM_Get_timestring(sLONG_PTR *pResult, PackagePtr pParams);
void SYSTEM_Get_timestamp(sLONG_PTR *pResult, PackagePtr pParams);
void PROCESS_Get_id(sLONG_PTR *pResult, PackagePtr pParams);
void PROCESS_GET_LIST(sLONG_PTR *pResult, PackagePtr pParams);
void STACK_Get_available_size(sLONG_PTR *pResult, PackagePtr pParams);
void PICTURE_GET_RAW_DATA(sLONG_PTR *pResult, PackagePtr pParams);