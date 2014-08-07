#include "functions_misc.h"

#define kVolumeIsRemote 1
#define kVolumeIsEjectable 2

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#if VERSIONMAC
#include <AppKit/AppKit.h>
typedef struct PANEL
{
	NSSavePanel *panel;
	int result;
	NSString *path;
	NSString *name;
};

void _runModal(PANEL *panel)
{
	panel->result = [panel->panel runModal];
}

void _runModalLegacy(PANEL *panel)
{
	panel->result = [panel->panel runModalForDirectory:panel->path file:panel->name];
}

OSErr _getSystemVersion(SInt32 *SystemVersionMajor, SInt32 *SystemVersionMinor, SInt32 *SystemVersionBugFix)
{
	OSErr err = noErr;
	
	err = Gestalt(gestaltSystemVersionMajor, SystemVersionMajor);
	if(!err)
		err = Gestalt(gestaltSystemVersionMinor, SystemVersionMinor);
	if(!err)	
		err = Gestalt(gestaltSystemVersionBugFix, SystemVersionBugFix);
	
	return err;
}

BOOL _isSnowLeopardOrLater()
{	
	SInt32 SystemVersionMajor, SystemVersionBugFix;
	SInt32 SystemVersionMinor = 0;
	
	_getSystemVersion(&SystemVersionMajor, &SystemVersionMinor, &SystemVersionBugFix);
	
	return ((SystemVersionMinor >= 6) ? TRUE : FALSE);
}

BOOL _isTiger()
{	
	SInt32 SystemVersionMajor, SystemVersionBugFix;
	SInt32 SystemVersionMinor = 0;
	
	_getSystemVersion(&SystemVersionMajor, &SystemVersionMinor, &SystemVersionBugFix);
	
	return ((SystemVersionMinor == 4) ? TRUE : FALSE);
}

int	GetVolParmsInfoExtendedAttributes(const GetVolParmsInfoBuffer *volParms)
{
	return ( (volParms->vMVersion >= 3) ? volParms->vMExtendedAttributes : 0 );
}

OSErr _getVolumeInfo(FSVolumeRefNum fsVolumeRefNum, int key, int *value)
{
	GetVolParmsInfoBuffer volParmsInfo;
	OSErr err = 1;
	
#if MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_4			
	HParamBlockRec pb;
	pb.ioParam.ioNamePtr = NULL;
	pb.ioParam.ioVRefNum = fsVolumeRefNum;
	pb.ioParam.ioBuffer = (Ptr)&volParmsInfo;
	pb.ioParam.ioReqCount = sizeof(GetVolParmsInfoBuffer);
	
	err = PBHGetVolParmsSync(&pb);
#else
	err = FSGetVolumeParms(fsVolumeRefNum, &volParmsInfo, sizeof(GetVolParmsInfoBuffer));
#endif	
	
	if(err == noErr){
		
		switch (key){
			case kVolumeIsRemote :
				*value = volParmsInfo.vMServerAdr;
				break;
			case kVolumeIsEjectable :
				*value = (GetVolParmsInfoExtendedAttributes(&volParmsInfo) & (1L << bIsEjectable));
				break;				
			default:
				break;
		}
	}
	
	return err;
}

OSErr _lookupVolumeInfo(NSString *path, int key, int *value)
{   
	OSErr err = 1;
	
	NSURL *url = [[NSURL alloc]initFileURLWithPath:path];
	
	FSRef fsRef;
	FSCatalogInfo fsCatalogInfo;
	
	if(CFURLGetFSRef((CFURLRef)url, &fsRef)){
		
		if(FSGetCatalogInfo(&fsRef, kFSCatInfoVolume, &fsCatalogInfo, NULL, NULL, NULL) == noErr){
			
			FSVolumeRefNum fsVolumeRefNum = fsCatalogInfo.volume;
			
			int flag = 0; 
			
			if(_getVolumeInfo(fsVolumeRefNum, key, &flag) == noErr){
				*value = flag;
				err = noErr;
			}
			
		}
		
	}
	
	[url release];
	
	return err;
}

#else
typedef wchar_t UniChar;
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#include <Windows.h>
#include <Shlobj.h>
#include <TlHelp32.h>
#pragma comment(lib, "Shell32.lib")
int _lookupDriveType(LPCTSTR lpRootPathName, int key, int *value){
	
	int err = 1;
	
	unsigned int dtype = GetDriveType(lpRootPathName);	
	
	if((dtype != DRIVE_UNKNOWN) 
	   && (dtype != DRIVE_NO_ROOT_DIR) 
	   && (dtype != DRIVE_CDROM)
	   && (dtype != DRIVE_RAMDISK)
	   ){
		
		switch (key) {
			case kVolumeIsRemote:
				*value = (dtype == DRIVE_REMOTE);
				err = 0;
				break;
			case kVolumeIsEjectable:
				*value = (dtype == DRIVE_REMOVABLE);
				err = 0;
				break;			
			default:
				break;
		}
	}
	
	return err;
	
}
#endif

void PICTURE_GET_RAW_DATA(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_PICTURE Param1;
	ARRAY_TEXT Param2;
	PA_Variable Param3;
	
	Param1.fromParamAtIndex(pParams, 1);	
	Param2.setSize(1);
	Param3 = *((PA_Variable*) pParams[2]);
	
	switch (Param3.fType) 
	{
		case eVK_ArrayPicture:
			break;
		case eVK_Undefined:
			PA_ClearVariable(&Param3);				
			Param3 = PA_CreateVariable(eVK_ArrayPicture);
			break;				
		default:
			break;
	}	
	
	if(Param3.fType == eVK_ArrayPicture){
		
		PA_ResizeArray(&Param3, 0);
		
		unsigned int i;
		PA_ErrorCode err = eER_NoErr;
		CUTF8String pictureData;
		i = 0;	
		
		while (err == eER_NoErr){
			err = Param1.getPictureDataTypeAtIndex(++i, &pictureData);
			if(err == eER_NoErr){
				
				PA_Handle h = PA_NewHandle(0);	
				PA_GetPictureData(Param1.getPicture(), i, h);
				err = PA_GetLastError();
				
				if(err == eER_NoErr)
				{
					PA_Picture picture = PA_CreatePicture(PA_LockHandle(h), PA_GetHandleSize(h));
					if(Param3.fType == eVK_ArrayPicture)
						PA_ResizeArray(&Param3, i);
					
					PA_SetPictureInArray(Param3, i, picture);				
					PA_UnlockHandle(h);
					Param2.appendUTF8String(&pictureData);
					
				}
				PA_DisposeHandle(h);
			}			
		}
		
		Param2.toParamAtIndex(pParams, 2);
		
		PA_Variable *param = ((PA_Variable *)pParams[2]);
		
		param->fType = Param3.fType;
		param->fFiller = Param3.fFiller;
		param->uValue.fArray.fCurrent = Param3.uValue.fArray.fCurrent;
		param->uValue.fArray.fNbElements = Param3.uValue.fArray.fNbElements;
		param->uValue.fArray.fData = Param3.uValue.fArray.fData;		
	}
	
}

// ------------------------------------- Stack ------------------------------------

void STACK_Get_available_size(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT returnValue;
	
	returnValue.setIntValue(PA_CheckFreeStack(0));
	returnValue.setReturn(pResult);
}

// ------------------------------------ Process -----------------------------------

void PROCESS_GET_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	ARRAY_TEXT Param1;
	ARRAY_TEXT Param2;
	ARRAY_LONGINT Param3;
	
	Param1.setSize(1);
	Param2.setSize(1);
	Param3.setSize(1);	
	
#if VERSIONWIN
	using namespace std;
	
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if(hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		
		if(Process32First(hProcessSnap, &pe32))
		{
			do
			{
				hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
				wstring process_name = pe32.szExeFile;
				if(hModuleSnap != INVALID_HANDLE_VALUE)
				{
					me32.dwSize = sizeof(MODULEENTRY32);
					
					if(Module32First(hModuleSnap, &me32))
					{
						do
						{
							wstring module_name = me32.szModule;
							
							if(module_name == process_name)
							{
								CUTF16String p1 = (const PA_Unichar*)me32.szModule;
								Param1.appendUTF16String(&p1);
								CUTF16String p2 = (const PA_Unichar*)me32.szExePath;
								Param2.appendUTF16String(&p2);								
								Param3.appendIntValue(me32.th32ProcessID);							
							}
							
						}while(Module32Next(hModuleSnap, &me32));
						CloseHandle(hModuleSnap);
					}
				}
				
			}while(Process32Next(hProcessSnap, &pe32));	
			CloseHandle(hProcessSnap);			 
		}
	}
#else
	NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];	
	NSArray *launchedApplications = [sharedWorkspace launchedApplications];
	
	unsigned int i;
	
	for(i = 0; i < [launchedApplications count]; i++)
	{
		NSString *NSApplicationName = (NSString *)[[launchedApplications objectAtIndex:i] valueForKey:@"NSApplicationName"];
		Param1.appendUTF16String(NSApplicationName);
		
		NSString *NSApplicationPath = (NSString *)[[launchedApplications objectAtIndex:i] valueForKey:@"NSApplicationPath"];
		NSURL *url = [[NSURL alloc]initFileURLWithPath:NSApplicationPath];
		
		if(url)
		{	
			NSString *filePath = (NSString *)CFURLCopyFileSystemPath((CFURLRef)url, kCFURLHFSPathStyle);
			Param2.appendUTF16String(filePath);
			[filePath release];
			[url release];
		}else{
			Param2.appendUTF16String(@"");
		}
		
		NSNumber *NSApplicationProcessIdentifier = (NSNumber *)[[launchedApplications objectAtIndex:i] valueForKey:@"NSApplicationProcessIdentifier"];
		Param3.appendIntValue([NSApplicationProcessIdentifier intValue]);
	}				
#endif
	
	Param1.toParamAtIndex(pParams, 1);
	Param2.toParamAtIndex(pParams, 2);
	Param3.toParamAtIndex(pParams, 3);	
	
}

void PROCESS_Get_id(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT returnValue;
	
#if VERSIONMAC
	ProcessSerialNumber psn;
	pid_t pid = 0;
	
	if(GetCurrentProcess(&psn) == noErr)
		GetProcessPID(&psn, &pid);
	
	returnValue.setIntValue(pid);
#else
	returnValue.setIntValue((unsigned int)GetCurrentProcessId());	
#endif
	
	returnValue.setReturn(pResult);
}

// ------------------------------------ System ------------------------------------

void SYSTEM_Get_timestamp(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	time_t timestamp = time(NULL);
	char ts[256];	
	
#if VERSIONMAC	
	setenv("TZ",(const char *)"GMT", 1);
#else	
	_wputenv_s(L"TZ",L"GMT"); 
#endif
	
	strftime(ts, 256, "%Y-%m-%dT%XZ",  localtime(&timestamp));
	
	returnValue.setUTF8String((const uint8_t*)ts, (uint32_t)strlen(ts));
	returnValue.setReturn(pResult);
}

void SYSTEM_Get_timestring(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	time_t timestamp = time(NULL);
	char ts[256];	
	
#if VERSIONMAC	
	setenv("TZ",(const char *)"GMT", 1);
#else	
	_wputenv_s(L"TZ",L"GMT"); 
#endif
	
	strftime(ts, 256, "%a, %d %b %y %X %Z",  localtime(&timestamp)); 
	
	returnValue.setUTF8String((const uint8_t*)ts, (uint32_t)strlen(ts));
	returnValue.setReturn(pResult);
}

void SYSTEM_Generate_UUID(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
#if VERSIONMAC
	CFUUIDRef uuid = CFUUIDCreate(kCFAllocatorDefault);
	CFStringRef uuid_str = CFUUIDCreateString(kCFAllocatorDefault,uuid);
	
	int size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(uuid_str),kCFStringEncodingUnicode)+2;	
	char *str = (char *)malloc(size);
	
	if(str)
	{
		memset(str,0x00,size);		
		CFStringGetCString(uuid_str,str,size,kCFStringEncodingUnicode);	
		CUTF16String uid = (const PA_Unichar *)str;
		returnValue.setUTF16String(&uid);
		free (str);
	}
	
	CFRelease(uuid);
	CFRelease(uuid_str);
#else
	RPC_WSTR str;
	UUID uuid;
	int len;
	UniChar *buffer;
	
	if(UuidCreate(&uuid)==RPC_S_OK)
	{
		if(UuidToString(&uuid,&str)==RPC_S_OK)
		{
			len = (wcslen((const wchar_t *)str)*2)+2;
			buffer = (UniChar *)malloc(len);
			if(buffer)
			{
				memset(buffer,0x00,len);
				memcpy(buffer,str,len);
				_wcsupr(buffer);
				CUTF16String uid = (const PA_Unichar *)buffer;
				returnValue.setUTF16String(&uid);	
				free(buffer);
			}
			RpcStringFree(&str);
		}
	}
#endif
	
	returnValue.setReturn(pResult);
}

void SYSTEM_Get_unixtime(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	time_t timestamp = time(NULL);
	char ts[256];
	sprintf((char *)ts, "%i", (int)timestamp);
	
	returnValue.setUTF8String((const uint8_t*)ts, (uint32_t)strlen(ts));
	returnValue.setReturn(pResult);
}

// ----------------------------------- Structure ----------------------------------

void STRUCTURE_Import_definition(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT returnValue;
	
	PA_Unistring *xml = (PA_Unistring *)(pParams[0]);
	PA_CreateElementsFromXMLDefinition(xml);	
	
	returnValue.setIntValue(PA_GetLastError());
	returnValue.setReturn(pResult);
}

// ------------------------------------ Volume ------------------------------------

void VOLUME_Is_ejectable(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_LONGINT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
#if VERSIONWIN	
	LPCTSTR lpRootPathName = NULL;	//the root of the current directory.
	
	if(Param1.getUTF16Length()){
		
		CUTF8String rdn;
		Param1.copyUTF8String(&rdn);
		
		if(rdn.find_last_of((uint8_t *)"\\") != rdn.length()){
			rdn.append((uint8_t *)"\\");
			Param1.setUTF8String(&rdn);	
		}
		
		lpRootPathName = (LPCTSTR)Param1.getUTF16StringPtr();
		int flag;
		
		if(_lookupDriveType(lpRootPathName, kVolumeIsEjectable, &flag) == 0){
			Param2.setIntValue(flag);
			returnValue.setIntValue(1);
		}
	}
#else	
	NSString *path = Param1.copyPath();
	
	int flag; 
	
	if(_lookupVolumeInfo(path, kVolumeIsEjectable, &flag) == noErr){
		
		Param2.setIntValue(flag != 0 );
		returnValue.setIntValue(1);	
		
	}
	
	[path release];		
#endif
	
	Param2.toParamAtIndex(pParams, 2);
	returnValue.setReturn(pResult);
}

void VOLUME_Is_remote(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_LONGINT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
#if VERSIONWIN	
	LPCTSTR lpRootPathName = NULL;	//the root of the current directory.
	
	if(Param1.getUTF16Length()){
		
		CUTF8String rdn;
		Param1.copyUTF8String(&rdn);
		
		if(rdn.find_last_of((uint8_t *)"\\") != rdn.length()){
			rdn.append((uint8_t *)"\\");
			Param1.setUTF8String(&rdn);	
		}
		
		lpRootPathName = (LPCTSTR)Param1.getUTF16StringPtr();
		int flag;
		
		if(_lookupDriveType(lpRootPathName, kVolumeIsRemote, &flag) == 0){
			Param2.setIntValue(flag);
			returnValue.setIntValue(1);
		}
	}
#else	
	NSString *path = Param1.copyPath();
	
	int flag; 
	
	if(_lookupVolumeInfo(path, kVolumeIsRemote, &flag) == noErr){
		
		Param2.setIntValue(flag != 0 );
		returnValue.setIntValue(1);	
		
	}
	
	[path release];		
#endif
	
	Param2.toParamAtIndex(pParams, 2);
	returnValue.setReturn(pResult);
}

// ----------------------------------- Get Path -----------------------------------

void PATH_From_user_selection(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT fileName;
	C_TEXT folderPath;
	C_TEXT title;
	C_TEXT returnValue;
	
	fileName.fromParamAtIndex(pParams, 1);
	folderPath.fromParamAtIndex(pParams, 2);
	title.fromParamAtIndex(pParams, 3);
	
#if VERSIONWIN
	DWORD			MAX_PATH_EXTRA = 1024;	
	wchar_t szPath[1024];
	
	wchar_t path[_MAX_PATH] = {0};
	
	wchar_t pathDrive[_MAX_DRIVE] = {0};			
	wchar_t pathDir[_MAX_DIR] = {0};		
	wchar_t pathFile[_MAX_FNAME] = {0};			
	wchar_t pathExtension[_MAX_EXT] = {0};	
	wchar_t xpathDrive[_MAX_DRIVE] = {0};			
	wchar_t xpathDir[_MAX_DIR] = {0};		
	wchar_t xpathFile[_MAX_FNAME] = {0};			
	wchar_t xpathExtension[_MAX_EXT] = {0};	
	
	_wsplitpath_s((const wchar_t *)folderPath.getUTF16StringPtr(), pathDrive, pathDir, xpathFile, xpathExtension);
	_wsplitpath_s((const wchar_t *)fileName.getUTF16StringPtr(), xpathDrive, xpathDir, pathFile, pathExtension);
	_wmakepath_s(szPath, pathDrive, pathDir, pathFile, pathExtension);
	
	OPENFILENAME	ofn;
	memset(&ofn, 0x00, sizeof(OPENFILENAME));
	
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= (HWND)PA_GetHWND(0);
	ofn.lpstrFile		= (LPWSTR)szPath;
	ofn.nMaxFile		= MAX_PATH_EXTRA;
	ofn.lpstrTitle		= (LPCTSTR)title.getUTF16StringPtr();
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;	
	
	if( GetSaveFileName(&ofn))
	{
		CUTF16String path = CUTF16String((PA_Unichar *)szPath);	
		returnValue.setUTF16String(&path);
	}
	
#else
	
	NSString *file = fileName.copyUTF16String();
	NSString *folder = folderPath.copyUTF16String();
	NSString *titleString = title.copyUTF16String();
	
	NSSavePanel *panel = [NSSavePanel savePanel];
	
//	bool isSnowLeopardOrLater = _isSnowLeopardOrLater();
	
	[panel setTitle:titleString];
	
	[panel setFloatingPanel:NO];
	[panel setBecomesKeyOnlyIfNeeded:NO];	
	[panel setWorksWhenModal:YES];	
//	if(isSnowLeopardOrLater) [panel setShowsHiddenFiles:NO];		
	[panel setTreatsFilePackagesAsDirectories:YES];	
	[panel setCanSelectHiddenExtension:YES];
	[panel setCanCreateDirectories:YES];
	[panel setAllowsOtherFileTypes:YES];	
	
	PANEL savePanel;
	savePanel.panel = panel;
	savePanel.result = NSFileHandlingPanelCancelButton;
	savePanel.name = file;
/*	
	if(isSnowLeopardOrLater)
	{		
		[panel setNameFieldStringValue:file];
		NSURL *folderUrl = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)folder, kCFURLHFSPathStyle, true);
		if(folderUrl) 	[panel setDirectoryURL:folderUrl];
		PA_RunInMainProcess((PA_RunInMainProcessProcPtr)_runModal, (void *)&savePanel);
		if(folderUrl)	[folderUrl release];
		
	}else{// use deprecatead api for 10.5 or earlier
*/		
		NSURL *folderUrl = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)folder, kCFURLHFSPathStyle, true);
	
		if(folderUrl)
		{
			NSString *folderName = (NSString *)CFURLCopyFileSystemPath((CFURLRef)folderUrl, kCFURLPOSIXPathStyle);
			savePanel.path = folderName;
			[folderName release];
			[folderUrl release];			
		}else{
			savePanel.path = @"";
		}			
		
		PA_RunInMainProcess((PA_RunInMainProcessProcPtr)_runModalLegacy, (void *)&savePanel);
//	}
	
	if(savePanel.result == NSFileHandlingPanelOKButton)
	{
		NSURL *url = [panel URL];
		if(url)
		{
			NSMutableString *path = [[NSMutableString alloc]initWithString:(NSString *)CFURLCopyFileSystemPath((CFURLRef)url, kCFURLHFSPathStyle)];
			CFStringNormalize((CFMutableStringRef)path, kCFStringNormalizationFormC);
			returnValue.setUTF16String(path);
			[path release];
		}
	}	
	
	[file release];	
	[folder release];
	[titleString release];
#endif	
	returnValue.setReturn(pResult);
}

void PATH_Get_known_folder(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
#if VERSIONWIN
	HWND hwndOwner	= NULL;
	int nFolder	= NULL;
	HANDLE hToken	= NULL;
	DWORD dwFlags	= SHGFP_TYPE_CURRENT;
	wchar_t szPath[MAX_PATH];
	
	switch (Param1.getIntValue()) 
	{
		case 0:	//WIN My Documents
			nFolder = CSIDL_PERSONAL;
			break;
		case 1:	//WIN App Data Local
			nFolder = CSIDL_LOCAL_APPDATA;
			break;		
		case 2:	//WIN App Data Common
			nFolder = CSIDL_COMMON_APPDATA;
			break;
		case 3:	//WIN App Data Roaming
			nFolder = CSIDL_APPDATA;
			break;				
		default:
			nFolder = CSIDL_PERSONAL;			
			break;
	}
	
	if(SHGetFolderPath(hwndOwner,
					   nFolder|CSIDL_FLAG_CREATE,
					   hToken,
					   dwFlags,
					   szPath) == S_OK)
	{
		CUTF16String path = CUTF16String((PA_Unichar *)szPath);
		returnValue.setUTF16String(&path);
	}
	
#else
	
	NSSearchPathDirectory directory = (NSSearchPathDirectory)0;
	NSSearchPathDomainMask mask = (NSSearchPathDomainMask)0;
	
	switch (Param1.getIntValue()) 
	{
		case 0:	//MAC Documents
			directory = NSDocumentDirectory;
			mask = NSUserDomainMask;
			break;
		case 1:	//MAC App Support User
			directory = NSApplicationSupportDirectory;
			mask = NSUserDomainMask;
			break;		
		case 2:	//MAC App Support Local
			directory = NSApplicationSupportDirectory;
			mask = NSLocalDomainMask;
			break;			
		default:
			directory = NSDocumentDirectory;	
			mask = NSUserDomainMask;
			break;
	}	
	
	NSString *path = [NSSearchPathForDirectoriesInDomains(directory, mask, YES) lastObject];
	returnValue.setUTF16String(path);
	
#endif	
	returnValue.setReturn(pResult);
}