#include "System.h"
#include <windows.h>
#include <shellapi.h>
#include <cstring>

// ===== helpers =====

bool IsDirectory(const char* path)
{
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool HasExtension(const char* path, const char* ext)
{
    const char* dot = strrchr(path, '.');
    if (!dot) return false;
    return _stricmp(dot, ext) == 0;
}

// ===== Drop Target =====

struct SimpleDropTarget : public IDropTarget
{
    ULONG ref = 1;

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == IID_IUnknown || riid == IID_IDropTarget)
        {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG __stdcall AddRef() override
    {
        return ++ref;
    }

    ULONG __stdcall Release() override
    {
        if (--ref == 0)
            delete this;
        return ref;
    }

    HRESULT __stdcall DragEnter(IDataObject*, DWORD, POINTL, DWORD* effect) override
    {
        g_IsDraggingFile = true;
        *effect = DROPEFFECT_COPY;
        return S_OK;
    }

    HRESULT __stdcall DragOver(DWORD, POINTL, DWORD* effect) override
    {
        g_IsDraggingFile = true;
        *effect = DROPEFFECT_COPY;
        return S_OK;
    }

    HRESULT __stdcall DragLeave() override
    {
        g_IsDraggingFile = false;
        return S_OK;
    }

    HRESULT __stdcall Drop(IDataObject* data, DWORD, POINTL, DWORD* effect) override
    {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg{};

        if (SUCCEEDED(data->GetData(&fmt, &stg)))
        {
            HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
            char path[MAX_PATH]{};

            if (DragQueryFileA(hDrop, 0, path, MAX_PATH))
            {
                g_FilePath = path;
                g_FileDropped = true;

                
                if (!IsDirectory(path))
                {
                    
                    if (HasExtension(path, ".xxs"))
                    {
                        Button_Menu = 1;
                        File_List::Get()->Add(path);
                    }
                    else
                    {
                        Button_Menu = 2;
                        Button_Menu_Extract = true;
                       
                       
                    }
                }
            }

            GlobalUnlock(stg.hGlobal);
            ReleaseStgMedium(&stg);
        }

        g_IsDraggingFile = false;
        *effect = DROPEFFECT_COPY;
        return S_OK;
    }
};

// ===== factory =====

IDropTarget* CreateDropTarget()
{
    return new SimpleDropTarget();
}
