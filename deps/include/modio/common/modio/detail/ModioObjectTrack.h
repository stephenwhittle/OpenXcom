#pragma once
namespace Modio
{
	namespace Detail
	{
		struct BaseOperationCommonImpl
		{
			inline static bool RequiresShutdown = false;
		};

		template<typename Base>
		struct BaseOperation : public BaseOperationCommonImpl
		{
#if MODIO_TRACK_OPS
			inline static int32_t Count = 0;
			bool bMovedFrom = false;
			BaseOperation();
			BaseOperation(BaseOperation&& Other);
			~BaseOperation();
#endif
		};
	} // namespace Detail
} // namespace Modio

#include "modio/impl/detail/ModioObjectTrack.ipp"
