#pragma once

#include "modio/core/ModioCoreTypes.h"
#include "modio/file/ModioFileService.h"

namespace Modio
{
	/// @docpublic
	/// @brief Structure with all initialization parameters needed to start the API, all need to be filled in for the
	/// SDK to properly initialize
	struct InitializeOptions
	{
		/// @brief Initializes all variables to a invalid state if possible and minimal functionality
		MODIO_IMPL InitializeOptions();
		MODIO_IMPL explicit InitializeOptions(Modio::GameID GameID, Modio::ApiKey APIKey,
											  Modio::Environment GameEnvironment, Modio::UserHandleType User);

		/// @brief The Mod.io-provided ID for the game
		Modio::GameID GameID = Modio::GameID::InvalidGameID();
		/// @brief The Mod.io-provided API key for your application or game
		Modio::ApiKey APIKey = Modio::ApiKey::InvalidAPIKey();
		/// @brief The user handle for the platform you are targeting
		Modio::UserHandleType User;
		/// @brief The mod.io environment you want to run the SDK on
		Modio::Environment GameEnvironment = Modio::Environment::Test;
	};
} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
	#include "ModioInitializeOptions.ipp"
#endif
