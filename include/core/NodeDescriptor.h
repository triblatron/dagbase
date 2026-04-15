#pragma once

#include "config/DagBaseExport.h"

#include "NodeCategory.h"
#include "MetaPort.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dagbase
{
	struct DAGBASE_API NodeDescriptor
	{
		std::int64_t id;
		std::string name;
		NodeCategory::Category category;
        using MetaPortArray = std::vector<MetaPort>;
	    MetaPortArray ports;

		NodeDescriptor()
			:
		id(-1),
		category(NodeCategory::CAT_UNKNOWN)
		{
			// Do nothing.
		}
	};
}