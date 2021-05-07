#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/core/ModioInitializeOptions.h"
#endif

namespace Modio
{
	InitializeOptions::InitializeOptions() {}

	InitializeOptions::InitializeOptions(Modio::GameID GameID, Modio::ApiKey APIKey, Modio::Environment GameEnvironment,
										 Modio::UserHandleType User)
		: GameID(GameID),
		  APIKey(APIKey),
		  User(User),
		  GameEnvironment(GameEnvironment)
	{}
} // namespace Modio
