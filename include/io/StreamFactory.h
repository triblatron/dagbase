//
// Created by tony on 02/08/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <string>


namespace dagbase
{
    class BackingStore;
    class InputStream;
    class OutputStream;

    extern InputStream DAGBASE_API* createInputStream(const std::string& className, BackingStore& store, const char* filename);
    extern OutputStream DAGBASE_API* createOutputStream(const std::string& className, BackingStore& store, const char* filename);
    extern BackingStore DAGBASE_API* createBackingStore(const std::string& className);
}
