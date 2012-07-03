#include "4DPluginAPI.h"
#include "4DPlugin.h"

#include "zlib/zlib.h"
#include "zlib/unzip.h"
#include "zlib/zip.h"

void Unzip(sLONG_PTR *pResult, PackagePtr pParams);
void Zip(sLONG_PTR *pResult, PackagePtr pParams);

#include <fstream>
#include <time.h>

#define BUFFER_SIZE 8192

#if VERSIONMAC	
#define zstring std::string
#define zchar char
#define folder_separator '/'
#else
#include <Shlobj.h>
#include <tchar.h>
#define PATH_MAX _MAX_PATH
#define zstring std::wstring
#define zchar wchar_t
#define folder_separator L'\\'
#endif