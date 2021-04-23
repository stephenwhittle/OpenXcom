#pragma once

#include "modio/core/ModioLogger.h"

namespace Modio
{
	namespace Detail
	{
#if MODIO_TRACK_OPS
		template<typename Base>
		BaseOperation<Base>::BaseOperation()
		{
			++BaseOperation<Base>::Count;
			
			char output[512];
			sprintf(output, "%s %d\n", __FUNCTION__, Count);
			OutputDebugStringA(output);
		}

		template<typename Base>
		BaseOperation<Base>::BaseOperation(BaseOperation<Base>&& Other)
		{			
			Other.bMovedFrom = true;
		}

		template<typename Base>
		BaseOperation<Base>::~BaseOperation()
		{
			if (bMovedFrom)
			{
				return;
			}

			--BaseOperation<Base>::Count;
			
			char output[512];
			sprintf(output, "%s %d\n", __FUNCTION__, Count);
			OutputDebugStringA(output);
		}
#endif
	} // namespace Detail
} // namespace Modio
