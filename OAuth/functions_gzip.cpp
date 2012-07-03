#include "functions_gzip.h"

typedef std::vector <zstring> zstrings;

void _copyPath(C_TEXT *t, zstring *p){
#if VERSIONMAC	
	NSString *str = t->copyUTF16String();
	NSURL *u = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)str, kCFURLHFSPathStyle, false);
	
	if(u){
		NSString *path = (NSString *)CFURLCopyFileSystemPath((CFURLRef)u, kCFURLPOSIXPathStyle);
		CFIndex size = CFStringGetMaximumSizeOfFileSystemRepresentation((CFStringRef)path);
		std::vector<uint8_t> buf(size);		
		[path getFileSystemRepresentation:(char *)&buf[0] maxLength:size];
		*p = std::string((char *)&buf[0]);
		[path release];
		[u release];
	}	
	
	[str release];
	
#else
	*p = std::wstring((wchar_t *)t->getUTF16StringPtr());
	
	if(p->at(p->size() - 1) == L'\\')
		*p = p->substr(0, p->size() - 1);
#endif	
}

bool _createFolder(zstring &absolutePathName){
	
	bool success = false;
	
#if VERSIONMAC
	NSString *path = (NSString *)CFStringCreateWithFileSystemRepresentation(kCFAllocatorDefault, absolutePathName.c_str());
	NSFileManager *fm = [[NSFileManager alloc]init];
	success = [fm createDirectoryAtPath:path 
			withIntermediateDirectories:YES
							 attributes:nil
								  error:NULL];	
	[path release];					
	[fm release];			
#else
	success = SHCreateDirectory(NULL, (PCWSTR)absolutePathName.c_str());				
#endif	
	
	return success;
}

void _createParentFolder(zstring *p){
#if VERSIONMAC
	NSString *filePath = (NSString *)CFStringCreateWithFileSystemRepresentation(kCFAllocatorDefault, p->c_str());
	zstring folderPath = zstring([[filePath stringByDeletingLastPathComponent]fileSystemRepresentation]);
	_createFolder(folderPath);
	[filePath release];
#else	
	wchar_t	fDrive[_MAX_DRIVE],
	fDir[_MAX_DIR],
	fName[_MAX_FNAME],
	fExt[_MAX_EXT];
	_wsplitpath_s(p->c_str(), fDrive, fDir, fName, fExt);	
	std::wstring folderPath = fDrive;
	folderPath += fDir;
	_createFolder(folderPath);
#endif	
}

void _getSubpaths(zstring &absolutePathName, zstrings &subpaths, size_t absolutePathOffset = 0){
	
	if(!absolutePathOffset)	
		subpaths.clear();
	
#if VERSIONMAC
	NSString *path = (NSString *)CFStringCreateWithFileSystemRepresentation(kCFAllocatorDefault, absolutePathName.c_str());	
	NSFileManager *fm = [[NSFileManager alloc]init];
	
	BOOL isDirectory;
	if([fm fileExistsAtPath:path isDirectory:&isDirectory]){
		
		if(isDirectory){
			
			NSArray *paths = [fm subpathsOfDirectoryAtPath:path error:NULL];
			
			for(NSUInteger i = 0; i < [paths count]; i++){
				
				NSString *itemPath = [paths objectAtIndex:i];
				
				if([fm fileExistsAtPath:[path stringByAppendingPathComponent:itemPath] isDirectory:&isDirectory]){
					
					if(isDirectory)
						itemPath = [itemPath stringByAppendingString:@"/"];
					
					subpaths.push_back([itemPath UTF8String]);				
				}				
				
			}
			
		}else{	
			subpaths.push_back(std::string([[path lastPathComponent]UTF8String]));
		}
		
	}
	
	[path release];	
	[fm release];	
	
#else
	
	WIN32_FIND_DATA find;	
	
	HANDLE h = FindFirstFile(absolutePathName.c_str(), &find);
	
	if(h != INVALID_HANDLE_VALUE){
		
		do {
			
			std::wstring relativePath = find.cFileName;	
			
			if((!wcscmp(relativePath.c_str(), L"..")) || (!wcscmp(relativePath.c_str(), L".")))
				continue;		
			
			if(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				
				if(!absolutePathOffset){
					
					absolutePathOffset = absolutePathName.size() - 1;
					_getSubpaths(absolutePathName + L"\\*", subpaths, absolutePathOffset);
					
				}else{
					
					subpaths.push_back(absolutePathName.substr(0, absolutePathName.size() - 1).substr(absolutePathOffset + 2) + relativePath + folder_separator);
					_getSubpaths(absolutePathName.substr(0, absolutePathName.size() - 1)  + relativePath + L"\\*", subpaths, absolutePathOffset);		
					
				}
				
			}else{
				
				if(!absolutePathOffset){
					subpaths.push_back(relativePath);
				}else{
					subpaths.push_back(absolutePathName.substr(0, absolutePathName.size() - 1).substr(absolutePathOffset + 2)  + relativePath);
				}
				
			}		
			
		} while (FindNextFile(h, &find));
		
		FindClose(h);
		
	}
	
#endif	
}

void Zip(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	zstring srcPath;
	zstring dstPath;
	
	_copyPath(&Param1, &srcPath);
	_copyPath(&Param2, &dstPath);
	
	zipFile hZip = zipOpen64(dstPath.c_str(), APPEND_STATUS_CREATE);
	
	if(hZip){
		
		returnValue.setIntValue(1);
		
		zstrings subpaths;
		
		_getSubpaths(srcPath, subpaths);
		
		zip_fileinfo zi;
		
		time_t currentTime;
		time(&currentTime);
		
		struct tm *tm;	
		tm=localtime(&currentTime);
		
		zi.tmz_date.tm_sec=tm->tm_sec;
		zi.tmz_date.tm_min=tm->tm_min;
		zi.tmz_date.tm_hour=tm->tm_hour;
		zi.tmz_date.tm_mday=tm->tm_mday;
		zi.tmz_date.tm_mon=tm->tm_mon;
		zi.tmz_date.tm_year=tm->tm_year;
		zi.external_fa = 0;
		zi.internal_fa = 0;		
		zi.dosDate = 0;
		
		zi.internal_fa = zi.external_fa = 0;
		
		for (unsigned int i = 0; i < subpaths.size(); i++) {
			
#if VERSIONWIN
			zstring relativePath = subpaths.at(i);
			zstring escapePath = relativePath;			
			for (unsigned int j = 0; j < escapePath.size(); ++j)
				if (escapePath.at(j) == folder_separator)
					escapePath.at(j) = L'/';
			
			C_TEXT t;
			t.setUTF16String((const PA_Unichar *)escapePath.c_str(), escapePath.size());
			CUTF8String u;
			t.copyUTF8String(&u);
			
			std::string fileName = std::string((const char *)u.c_str());
#else
			zstring fileName = subpaths.at(i);		
#endif				
			
			if(zipOpenNewFileInZip64(hZip,
									 fileName.c_str(),
									 &zi,
									 NULL, 0,
									 NULL, 0,
									 NULL,
									 Z_DEFLATED,
									 Z_DEFAULT_COMPRESSION,
									 0) != UNZ_OK){
				returnValue.setIntValue(0);
				break;			
			}
			
			zstring fullPath;
			
			if(subpaths.size() == 1){
				
				fullPath = srcPath;				
				
			}else{
				
#if VERSIONMAC						
				fullPath = srcPath + folder_separator + fileName;
#else
				fullPath = srcPath + folder_separator + relativePath;					
#endif	
			}			
			
#if VERSIONMAC			
			NSString *path = [[NSString alloc]initWithUTF8String:fullPath.c_str()];
			fullPath = [path fileSystemRepresentation];
			[path release];
#endif
			
			std::ifstream ifs(fullPath.c_str(), std::ios::in|std::ios::binary);
			
			if(ifs.is_open()){
				
				std::vector<uint8_t> buf(BUFFER_SIZE);	
				
				while(ifs.good()){
					
					ifs.read((char *)&buf[0], BUFFER_SIZE);
					zipWriteInFileInZip(hZip, (char *)&buf[0], ifs.gcount());				
					
				}
				
				ifs.close();
				
			}
			
			zipCloseFileInZip(hZip);			
			
		}
		
		zipClose(hZip, NULL);	
		
	}
	
	returnValue.setReturn(pResult);
}

void Unzip(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	zstring srcPath;
	zstring dstPath;
	
	_copyPath(&Param1, &srcPath);
	_copyPath(&Param2, &dstPath);
	
	unzFile hUnzip = unzOpen64(srcPath.c_str());	
	
	if (hUnzip){
		
		returnValue.setIntValue(1);
		
		unz_file_info64 fileInfo;
		
		std::vector<uint8_t> szConFilename(PATH_MAX);
		
		zstring relativePathName;
		zstring absolutePathName;
		
		do {
			
			if (unzGetCurrentFileInfo64(hUnzip, &fileInfo, (char *)&szConFilename[0], PATH_MAX, NULL, 0, NULL, 0) != UNZ_OK){
				returnValue.setIntValue(0);
				break;
			}
			
#if VERSIONWIN
			std::string path;
			path = std::string((const char *)&szConFilename[0]);
			
			for (unsigned int i = 0; i < path.size(); ++i)
				if (path.at(i) == '/')
					path.at(i) = '\\';
			
			C_TEXT t;
			t.setUTF8String((const uint8_t *)path.c_str(), path.size());
			relativePathName = zstring((const wchar_t *)t.getUTF16StringPtr());
#else
			relativePathName = std::string((const char *)&szConFilename[0]);			
#endif				
			
			absolutePathName = dstPath + folder_separator + relativePathName;
			
			_createParentFolder(&absolutePathName);
			
			if(relativePathName.size() > 1){
				
				if(relativePathName.at(relativePathName.size() - 1) == folder_separator)
					_createFolder(absolutePathName);
				
				if(unzOpenCurrentFile(hUnzip) != UNZ_OK){
					returnValue.setIntValue(0);
					break;				
				}			
				
				std::ofstream ofs(absolutePathName.c_str(), std::ios::out|std::ios::binary);
				
				if(ofs.is_open()){
					
					std::vector<uint8_t> buf(BUFFER_SIZE);
					std::streamsize size;
					
					while ((size = unzReadCurrentFile(hUnzip, &buf[0], BUFFER_SIZE)) > 0)
						ofs.write((const char *)&buf[0], size);
					
					ofs.close();
					
				}
				
			}
			
			unzCloseCurrentFile(hUnzip);
			
		} while (unzGoToNextFile(hUnzip) != UNZ_END_OF_LIST_OF_FILE);
		
		unzClose(hUnzip);	
	} 
	
	returnValue.setReturn(pResult);
}

