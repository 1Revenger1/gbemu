#include "gb.h"
#include <shobjidl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <knownfolders.h>
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

const COMDLG_FILTERSPEC c_rgSaveTypes[] = {
    {L"Gameboy (*.gb)", L"*.gb"}
};

class OpenDialogEventHandler : public IFileDialogEvents
{
public:
    // IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] = {
            QITABENT(OpenDialogEventHandler, IFileDialogEvents),
            { 0 },
#pragma warning(suppress:4838)
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&_cRef);
        if (!cRef)
            delete this;
        return cRef;
    }

    // IFileDialogEvents methods
    IFACEMETHODIMP OnFileOk(IFileDialog*) { return S_OK; };
    IFACEMETHODIMP OnFolderChange(IFileDialog*) { return S_OK; };
    IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) { return S_OK; };
    IFACEMETHODIMP OnHelp(IFileDialog*) { return S_OK; };
    IFACEMETHODIMP OnSelectionChange(IFileDialog*) { return S_OK; };
    IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) { return S_OK; };
    IFACEMETHODIMP OnTypeChange(IFileDialog* pfd) { return S_OK; };
    IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) { return S_OK; };

    OpenDialogEventHandler() : _cRef(1) { };


private:
    ~OpenDialogEventHandler() { };
    long _cRef;
};

// Instance creation helper
HRESULT OpenDialogEventHandler_CreateInstance(REFIID riid, void** ppv)
{
    *ppv = NULL;
    OpenDialogEventHandler* pDialogEventHandler = new (std::nothrow) OpenDialogEventHandler();
    HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pDialogEventHandler->QueryInterface(riid, ppv);
        pDialogEventHandler->Release();
    }
    return hr;
}

HRESULT openFileMenu(PWSTR* path)
{
    IFileDialog* pfd = NULL;
    IFileDialogEvents* pfde = NULL;
    IShellItem* psiResult;

    DWORD dwCookie = 0;
    DWORD dwFlags;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    
    // Create File Dialog instnace
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (!SUCCEEDED(hr))
        goto exit;

    // Create an event handling object, and hook it up to the dialog.
    hr = OpenDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
    if (!SUCCEEDED(hr))
        goto exit;

    // Hook up the event handler.
    hr = pfd->Advise(pfde, &dwCookie);
    if (!SUCCEEDED(hr))
        goto exit;

    // Read current settings
    hr = pfd->GetOptions(&dwFlags);
    if (!SUCCEEDED(hr))
        goto exit;

    // Add setting to force files to exist in file system
    hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
    if (!SUCCEEDED(hr))
        goto exit;

    // Only allow gameboy roms
    hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
    if (!SUCCEEDED(hr))
        goto exit;

    hr = pfd->Show(NULL);
    if (!SUCCEEDED(hr))
        goto exit;

    // Obtain the result, once the user clicks the 'Open' button.
    // The result is an IShellItem object.
    hr = pfd->GetResult(&psiResult);
    if (!SUCCEEDED(hr))
        goto exit;

    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, path);
    psiResult->Release();

exit:
    if (pfd != nullptr) {
        // Unhook event handler
        pfd->Unadvise(dwCookie);
        pfd->Release();
    }

    if (pfde != nullptr) {
        pfde->Release();
    }

    CoUninitialize();
    return hr;
}