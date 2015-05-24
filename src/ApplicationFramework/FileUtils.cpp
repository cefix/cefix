/*
 *  FileUtils.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 29.05.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */
 
#include <cefix/Export.h>
 
#ifdef CEFIX_FOR_IPHONE
	
	#include <sys/stat.h>
	#include <unistd.h>
	#include <utime.h>
	#include <time.h>

#else
    #ifdef __APPLE__
        #include <Cocoa/Cocoa.h>
		#include <sys/stat.h>
		#include <unistd.h>
		#include <utime.h>
		#include <time.h>
    #else
        #define _WIN32_WINNT 0x0500
        #include "windows.h"
    #endif
#endif

#include <fstream>
#include <iostream>
#include "FileUtils.h"
#include <cefix/StringUtils.h>
#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

namespace cefix {

#ifdef CEFIX_FOR_IPHONE

    bool showOpenFileDialog(std::string& result, const std::string& path, const std::string& filetypes)
    {
        osg::notify(osg::WARN) << "showOpenFileDialog not implemented for Iphone" << std::endl;
        return false;
    }
    
    bool showSaveFileDialog(std::string& result, const std::string& suggestedFileName, const std::string& path)
    {
        osg::notify(osg::WARN) << "showSaveFileDialog not implemented for Iphone" << std::endl;
        return false;
    }
    
#else
    #ifdef __APPLE__
        #define FILE_UTILS_COCOA_IMPLEMENTATION 1
    #endif
#endif

#ifdef FILE_UTILS_CARBON_IMPLEMENTATION

struct DialogData {
	std::string defaultLocation;
	std::set<std::string> allowedExtensions;
};

static pascal void NavEventProc(
    NavEventCallbackMessage inSelector,
    NavCBRecPtr ioParams,
    NavCallBackUserData ioUserData );


static NavEventUPP sStandardNavEventFilter = NewNavEventUPP(NavEventProc);


// ----------------------------------------------------------------------------------------------------------
// NavEventProc
// ----------------------------------------------------------------------------------------------------------

static pascal void NavEventProc(
    NavEventCallbackMessage inSelector,
    NavCBRecPtr ioParams,
    NavCallBackUserData ioUserData )
{
    DialogData* data = (DialogData*)(ioUserData);
	if (inSelector == kNavCBEvent)
    {
    }
    else if ( inSelector == kNavCBStart )
    {
        if (data && !(data->defaultLocation.empty()))
        {
            // Set default location for the modern Navigation APIs
            // Apple Technical Q&A 1151
            
			FSRef theFile;
			OSErr result = noErr;
			result = FSPathMakeRef( (UInt8*)(data->defaultLocation.c_str()), &theFile, false);
			
			AEDesc theLocation = { typeNull, NULL };
            if (noErr == ::AECreateDesc(typeFSRef, &theFile, sizeof(FSRef), &theLocation))
                ::NavCustomControl(ioParams->context, kNavCtlSetLocation, (void *) &theLocation);
			
        }

	}
}


// ----------------------------------------------------------------------------------------------------------
// CrossPlatformFilterCallback
// ----------------------------------------------------------------------------------------------------------

pascal Boolean CrossPlatformFilterCallback (
    AEDesc *theItem,
    void *info,
    void *callBackUD,
    NavFilterModes filterMode
)
{
    bool display = true;
    DialogData* data = (DialogData*)(callBackUD);
	
    if (filterMode == kNavFilteringBrowserList)
    {
        NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*) info ;
        if ( !theInfo->isFolder )
        {
            /*
			if (theItem->descriptorType == typeFSS )
            {
                FSSpec    spec;
                memcpy( &spec , *theItem->dataHandle , sizeof(FSSpec) ) ;
				std::cout << "unhandled" << std::endl;
			}
            else 
			*/
			if ( theItem->descriptorType == typeFSRef )
            {
                FSRef fsref ;
                memcpy( &fsref , *theItem->dataHandle , sizeof(FSRef) ) ;

                CFURLRef fullURLRef;
                fullURLRef = ::CFURLCreateFromFSRef(NULL, &fsref);
                CFURLPathStyle pathstyle = kCFURLPOSIXPathStyle;
                CFStringRef cfString = CFURLCopyFileSystemPath(fullURLRef, pathstyle);
                ::CFRelease( fullURLRef ) ;
				char buffer[2048];
				
				if (CFStringGetCString(cfString, buffer, 2048, kCFStringEncodingUTF8)) {
					std::string filestr(buffer);
					std::string ext(osgDB::getFileExtension(filestr));
					display = (data->allowedExtensions.find(ext) != data->allowedExtensions.end());
					
				}
				::CFRelease( cfString );
				
			}
        }
    }

    return display;
}

// ----------------------------------------------------------------------------------------------------------
// showOpenFileDialog
// ----------------------------------------------------------------------------------------------------------
	
bool showOpenFileDialog(std::string& result, const std::string& path, const std::string& filetypes) {
	
	class Error : public std::exception {
	public:
		Error(OSStatus status) : std::exception(), _status(status) {}
		OSStatus getStatus() { return _status; }
		virtual const char* what() { return "Error while showing OpenFileDialog"; }
		
		static void checkForError(OSStatus status) { if (status != noErr) throw Error(status); }
		
		virtual ~Error() throw () {}
	private:
		OSStatus _status;
	};

	OSStatus status;
	NavDialogRef dlg(NULL);
	NavObjectFilterUPP navFilterUPP = NULL;
	NavDialogCreationOptions myDialogOptions;
	NavReplyRecord        navReply;
	bool success(false);
	try {
		status = NavGetDefaultDialogCreationOptions (&myDialogOptions);
		myDialogOptions.modality = kWindowModalityAppModal;
		
		DialogData data;
		data.defaultLocation = (path.empty()) ? "" : osgDB::findDataFile(path);
		if (!filetypes.empty()) {
			navFilterUPP = NewNavObjectFilterUPP(CrossPlatformFilterCallback);
			std::vector<std::string> exts;
			cefix::strTokenize(filetypes,exts,",");
			data.allowedExtensions.insert(exts.begin(), exts.end());
		}
		status = NavCreateChooseFileDialog(&myDialogOptions, NULL, sStandardNavEventFilter, NULL, navFilterUPP, &data, &dlg);
		Error::checkForError(status);
		
		status = NavDialogRun (dlg);
		Error::checkForError(status);
		
		NavUserAction userAction = NavDialogGetUserAction(dlg);
		if(userAction == kNavUserActionChoose) 
		{
			const unsigned int max_path = 2048;
			FSRef fsRef;
			
			status  = NavDialogGetReply( dlg, &navReply );
			Error::checkForError(status);
			status = AEGetNthPtr(&(navReply.selection), 1, typeFSRef, NULL, NULL, &fsRef, sizeof(FSRef), NULL);
			
			char	 path[max_path];
			status	= FSRefMakePath ( &fsRef, (UInt8*)path, max_path );
			
			NavDisposeReply ( &navReply );
			result = std::string(path);
			success = true;
		}
			
	}
	catch (Error e) {
		log::error("showOpenFileDialog") << "failed with " << e.getStatus() << std::endl;
		success = false;
	}
	if (navFilterUPP)
		::DisposeNavObjectFilterUPP(navFilterUPP);
	if (dlg)
		NavDialogDispose (dlg);
	
	return success;
	
}
	
	
// ----------------------------------------------------------------------------------------------------------
// showSaveFileDialog
// ----------------------------------------------------------------------------------------------------------

bool showSaveFileDialog(std::string& result, const std::string& suggestedFileName, const std::string& path) {
	
		class Error : public std::exception {
	public:
		Error(OSStatus status) : std::exception(), _status(status) {}
		OSStatus getStatus() { return _status; }
		virtual const char* what() { return "Error while showing OpenFileDialog"; }
		
		static void checkForError(OSStatus status) { if (status != noErr) throw Error(status); }
		
		virtual ~Error() throw () {}
	private:
		OSStatus _status;
	};

	OSStatus status;
	NavDialogRef dlg(NULL);
	NavObjectFilterUPP navFilterUPP = NULL;
	NavDialogCreationOptions myDialogOptions;
	NavReplyRecord        navReply;
	bool success(false);
	try {
		status = NavGetDefaultDialogCreationOptions (&myDialogOptions);
		myDialogOptions.modality = kWindowModalityAppModal;
		
		DialogData data;
		data.defaultLocation = (path.empty()) ? "" : osgDB::findDataFile(path);
		
		myDialogOptions.saveFileName = CFStringCreateWithCString(kCFAllocatorDefault, suggestedFileName.c_str(), kCFStringEncodingUTF8);
		
		status = NavCreatePutFileDialog(&myDialogOptions, '????','????',sStandardNavEventFilter, &data, &dlg);
		Error::checkForError(status);
		
		status = NavDialogRun (dlg);
		Error::checkForError(status);
		
		NavUserAction userAction = NavDialogGetUserAction(dlg);
		if(userAction == kNavUserActionSaveAs) 
		{
			const unsigned int max_path = 2048;
			FSRef fsRef;
			
			status  = NavDialogGetReply( dlg, &navReply );
			Error::checkForError(status);
			status = AEGetNthPtr(&(navReply.selection), 1, typeFSRef, NULL, NULL, &fsRef, sizeof(FSRef), NULL);
			
			char	 path[max_path];
			status	= FSRefMakePath ( &fsRef, (UInt8*)path, max_path );
			result = std::string(path);
			
			if (CFStringGetCString(navReply.saveFileName, path, max_path, kCFStringEncodingUTF8)) 
			{
				success = true;
				result += "/" + std::string(path);
			}
			
			NavDisposeReply ( &navReply );
			
			
		}
			
	}
	catch (Error e) {
		log::error("showOpenFileDialog") <<"failed with " << e.getStatus() << std::endl;
		success = false;
	}
	if (navFilterUPP)
		::DisposeNavObjectFilterUPP(navFilterUPP);
	if (dlg)
		NavDialogDispose (dlg);
	
	return success;	
}

#endif

#ifdef FILE_UTILS_COCOA_IMPLEMENTATION


bool showOpenFileDialog(std::string& result, const std::string& path, const std::string& filetypes) 
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSWindow* keywindow = [NSApp keyWindow];
	bool success(false);
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	[openDlg setCanChooseFiles:YES];
	[openDlg retain];
	NSMutableArray* file_types_nsarray(nil);
	std::vector<std::string> file_types_vector;
	cefix::strTokenize(filetypes, file_types_vector, ",");
	if(file_types_vector.size() > 0) {
		file_types_nsarray = [[NSMutableArray alloc] init];
		for(std::vector<std::string>::iterator i = file_types_vector.begin(); i != file_types_vector.end(); ++i) {
			NSString* s = [NSString stringWithUTF8String: (*i).c_str()];
			[file_types_nsarray addObject: s];
		}
	}
	NSString* nspath = [NSString stringWithUTF8String: path.c_str()];
	if ([openDlg runModalForDirectory: nspath file:nil types: file_types_nsarray] == NSOKButton) 
	{
		NSArray* files = [openDlg filenames];
		if (files) {
			NSString* filename = [files objectAtIndex: 0];
			result = std::string([filename UTF8String]);
			success = true;
		}
	}
	[openDlg release];
	// fenster wieder aktiv machen
	if (keywindow)
		[keywindow makeKeyAndOrderFront: nil];
	
	[pool release];
	
	return success;
}


	
// ----------------------------------------------------------------------------------------------------------
// showSaveFileDialog
// ----------------------------------------------------------------------------------------------------------

bool showSaveFileDialog(std::string& result, const std::string& suggestedFileName, const std::string& path) 
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSWindow* keywindow = [NSApp keyWindow];
	bool success(false);
	NSSavePanel* saveDlg = [NSSavePanel savePanel];
	[saveDlg retain];
	
	NSString* nspath = [NSString stringWithUTF8String: path.c_str()];
	NSString* ns_suggested_filename = [NSString stringWithUTF8String: suggestedFileName.c_str()];
	if ([saveDlg runModalForDirectory: nspath file:ns_suggested_filename] == NSOKButton) 
	{
		NSString* filename = [saveDlg filename];
		result = std::string([filename UTF8String]);
		success = true;
	}
	[saveDlg release];
	// fenster wieder aktiv machen
	if (keywindow)
		[keywindow makeKeyAndOrderFront: nil];
	
	[pool release];
	
	return success;

}


#endif

#ifdef WIN32 
bool showOpenFileDialog(std::string& result, const std::string& path, const std::string& filetypes) {
	

    OPENFILENAME ofn; 
	char *FilterSpec ="All Files(.)\0*.*\0"; 
	char *Title ="Open...."; 
	char szFileName[MAX_PATH]; 
	char szFileTitle[MAX_PATH]; 
	int Result;

    *szFileName = 0; 
	*szFileTitle = 0;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

    /* fill in non-variant fields of OPENFILENAME struct. */
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = GetFocus();
    ofn.lpstrFilter       = FilterSpec;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = szFileName;
    ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir   = ".";
	if (!path.empty())
		ofn.lpstrInitialDir = path.c_str(); // Initial directory.
    ofn.lpstrFileTitle    = szFileTitle;
    ofn.nMaxFileTitle     = MAX_PATH;
    ofn.lpstrTitle        = Title;
    //ofn.lpstrDefExt   =		default_extension;

    ofn.Flags             = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;

    if (GetOpenFileName ((LPOPENFILENAME)&ofn))
    {
		result = std::string(szFileName);
		return true;
    }

	return false;
}

bool showSaveFileDialog(std::string& result, const std::string& suggestedFileName, const std::string& path) {
	

    OPENFILENAME ofn; 
	char *FilterSpec ="All Files(.)\0*.*\0"; 
	char *Title ="Open...."; 
	char szFileName[MAX_PATH]; 
	char szFileTitle[MAX_PATH]; 
	int Result;

    *szFileName = 0; 
	*szFileTitle = 0;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

    /* fill in non-variant fields of OPENFILENAME struct. */
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = GetFocus();
    ofn.lpstrFilter       = FilterSpec;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = szFileName;
    ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir   = ".";
	if (!path.empty())
		ofn.lpstrInitialDir = path.c_str(); // Initial directory.
    ofn.lpstrFileTitle    = szFileTitle;
    ofn.nMaxFileTitle     = MAX_PATH;
    ofn.lpstrTitle        = Title;
    //ofn.lpstrDefExt   =		default_extension;

    ofn.Flags             = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName ((LPOPENFILENAME)&ofn))
    {
		result = std::string(szFileName);
		return true;
    }

	return false;
}
#endif



void readFileIntoString(const std::string& filename, std::string& result) 
{	
	std::ifstream ifs(filename.c_str());
	result.append((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}

std::string readFileIntoString(const std::string& filename)
{
	std::string result; 
	readFileIntoString(filename, result);
	return result;
}

/** reads a file into a std::vector */
void  readFileIntoVector(const std::string& filename, std::vector<std::string>& result)
{
	std::ifstream ifs(filename.c_str());
	std::string line;
	while (std::getline(ifs, line, '\n'))
	{
		result.push_back(line);
	}
}

std::vector<std::string> readFileIntoVector(const std::string& filename)
{
	std::vector<std::string> result; 
	readFileIntoVector(filename, result);
	return result;
}

#ifdef CEFIX_FOR_IPHONE
    bool openFileExternally(const std::string& filename) 
    {
        return false;
    }
    
    bool revealFileExternally(const std::string& filename) 
    {
        return false;
    }

#else

	
    bool openFileExternally(const std::string& filename) 
    {
    #ifdef __APPLE__
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        NSString * path    = [NSString stringWithCString:filename.c_str() encoding:NSUTF8StringEncoding];
        [path autorelease];
        NSURL    * fileURL = [NSURL fileURLWithPath: path];
        //[fileURL autorelease];
        NSWorkspace * ws = [NSWorkspace sharedWorkspace];
        BOOL result = [ws openURL: fileURL];
        [pool release];
        return  (result == YES);
    #else
        std::cout << "openFileExternally not implemented yet!" << std::endl;
        return false;
    #endif
    }
        
    bool revealFileExternally(const std::string& filename) 
    {
    #ifdef __APPLE__
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        NSString * path    = [NSString stringWithCString:filename.c_str() encoding:NSUTF8StringEncoding];
        [path autorelease];
        NSWorkspace * ws = [NSWorkspace sharedWorkspace];
        BOOL result = [ws selectFile: path inFileViewerRootedAtPath: @"" ];
        [pool release];
        return  (result == YES);
    #else
        std::cout << "revealFileExternally not implemented yet!" << std::endl;
        return false;
    #endif
    }
#endif
}

#ifdef __APPLE__

bool cefix::getModifiedTimeStamp(const std::string& filename, time_t& t)
{
	
	if (!osgDB::fileExists(filename))
		return false;
		struct stat attrib;			// create a file attribute structure
        
	stat(filename.c_str(), &attrib);
	
	t = attrib.st_mtime;
	return true;
}


bool cefix::setModifiedTimeStamp(const std::string& filename, time_t t)
{
	if (!osgDB::fileExists(filename))
		return false;
	
	struct utimbuf tb;
	tb.actime  = t;
	tb.modtime = t;
	return (utime(filename.c_str(), &tb) == 0);
}

#else

time_t fromFileTimeNP(unsigned int fileTimeLow, unsigned int fileTimeHigh)
{
	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
	epoch.LowPart  = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;
	
	ULARGE_INTEGER ts;
	ts.LowPart  = fileTimeLow;
	ts.HighPart = fileTimeHigh;
	ts.QuadPart -= epoch.QuadPart;

	return ts.QuadPart/(10 * 1000000);
}


void toFileTimeNP(time_t t, unsigned int& fileTimeLow, unsigned int& fileTimeHigh) 
{
	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
	epoch.LowPart  = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;

	ULARGE_INTEGER ts;
	ts.QuadPart  = t*10 * 1000000;
	ts.QuadPart += epoch.QuadPart;
	fileTimeLow  = ts.LowPart;
	fileTimeHigh = ts.HighPart;
}


class FileHandle
{
public:
	FileHandle(const std::string& path, DWORD access, DWORD share, DWORD disp)
	{
		_h = CreateFileA(path.c_str(), access, share, 0, disp, 0, 0);
		if (_h == INVALID_HANDLE_VALUE)
		{
		}
	}
	
	~FileHandle()
	{
		if (_h != INVALID_HANDLE_VALUE) CloseHandle(_h);
	}
	
	HANDLE get() const
	{
		return _h;
	}
	
private:
	HANDLE _h;
};

bool cefix::getModifiedTimeStamp(const std::string& filename, time_t& t)
{
	if (!osgDB::fileExists(filename))
		return false;
	
	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (GetFileAttributesEx(osgDB::convertFileNameToNativeStyle(filename).c_str(), GetFileExInfoStandard, &fad)) {
		t = fromFileTimeNP(fad.ftLastWriteTime.dwLowDateTime, fad.ftLastWriteTime.dwHighDateTime);
		return true;
	}
	
	return false;
}


bool cefix::setModifiedTimeStamp(const std::string& filename, time_t t)
{
	if (!osgDB::fileExists(filename))
		return false;
	
	unsigned int low;
	unsigned int high;
	toFileTimeNP(t, low, high);
	FILETIME ft;
	ft.dwLowDateTime  = low;
	ft.dwHighDateTime = high;
	FileHandle fh(osgDB::convertFileNameToNativeStyle(filename), FILE_ALL_ACCESS, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);
	if (SetFileTime(fh.get(), 0, &ft, &ft) == 0)
		return false;
	
	return true;

}

#endif


