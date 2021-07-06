#pragma once

namespace Modio
{
	namespace Detail
	{
		/// <summary>
		/// calling subscribe would extend this
		/// </summary>
		class WaitForCollectionRefreshOp
		{
			template<typename CoroType>
			void operator()(Corotype& Self, Modio::ErrorCode ec = {})
			{}
		};
	} // namespace Detail
} // namespace Modio