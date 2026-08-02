//
// Created by tony on 02/08/2026.
//

#include <string>

#include "config/config.h"
#include "io/BinaryOutputStream.h"
#include "io/TextOutputStream.h"
#include "io/BinaryInputStream.h"
#include "io/TextInputStream.h"
#include "io/MemoryBackingStore.h"
#include "io/FileBackingStore.h"

namespace dagbase
{
    BackingStore* createBackingStore(const std::string& className, BackingStore::Mode mode, const char* filename)
    {
        if (className == "MemoryBackingStore")
        {
            auto retval = new MemoryBackingStore();

            return retval;
        }
        if (className == "FileBackingStore")
        {
            auto retval = new FileBackingStore();

            return retval;
        }

        return nullptr;
    }

    OutputStream* createOutputStream(const std::string& className, BackingStore& store, const char* filename)
    {
        store.open(BackingStore::MODE_OUTPUT_BIT, filename);
        if (className == "TextFormat")
        {
            return new TextOutputStream(&store);
        }
        if (className == "BinaryFormat")
        {
            return new BinaryOutputStream(&store);
        }

        return nullptr;
    }

    InputStream* createInputStream(const std::string& className, BackingStore& store, const char* filename)
    {
        store.open(BackingStore::MODE_INPUT_BIT, filename);
        if (className == "TextFormat")
        {
            return new TextInputStream(&store);
        }
        if (className == "BinaryFormat")
        {
            return new BinaryInputStream(&store);
        }

        return nullptr;
    }
}
