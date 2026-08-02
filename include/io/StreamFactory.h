//
// Created by tony on 02/08/2026.
//

#pragma once

#include <string>

#include "config/DagBaseExport.h"

namespace dagbase
{
    class BackingStore;
    class InputStream;
    class OutputStream;

    InputStream DAGBASE_API* createInputStream(const std::string& className, BackingStore& store, const char* filename);
    OutputStream DAGBASE_API* createOutputStream(const std::string& className, BackingStore& store, const char* filename);
    BackingStore* DAGBASE_API createBackingStore(const std::string& className, BackingStore::Mode mode, const char* filename);
}
