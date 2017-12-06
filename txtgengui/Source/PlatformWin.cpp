//
//  Platform.cpp
//  txtgen - App
//
//  Created by joerg piringer on 10.10.17.
//

#include "Platform.hpp"

#include <windows.h>
#include <cstdio>
#include "resource.h"

void LoadFileInResource(int name, int type, DWORD &size, const char *&data) {
    HMODULE handle = ::GetModuleHandle(nullptr);
    HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name),
                              MAKEINTRESOURCE(type));
    HGLOBAL rcData = ::LoadResource(handle, rc);
    size = ::SizeofResource(handle, rc);
    data = static_cast<const char*>(::LockResource(rcData));
}

std::wstring loadResourceFileWithID(int _id) {
    DWORD size = 0;
    const char *data = nullptr;
    LoadFileInResource(_id, TEXTFILE, size, data);
    /* Access bytes in data - here's a simple example involving text output*/
    // The text stored in the resource might not be NULL terminated.
    char *buffer = new char[size+1];
    ::memcpy(buffer, data, size);
    buffer[size] = 0; // NULL terminator
    std::wstring str = fromUTF8(buffer);
    delete[] buffer;
    
    return str;
}
