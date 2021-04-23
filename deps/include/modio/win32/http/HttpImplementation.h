#pragma once
#include "ModioGeneratedVariables.h"
#include "common/GDKHttpSharedState.h"
#include "common/detail/ops/http/InitializeHttp.h"
#include "common/http/HttpImplementation.h"
#include "modio/core/ModioErrorCode.h"
#include "modio/core/ModioServices.h"
#include "modio/detail/http/IHttpServiceImplementation.h"
#include "modio/http/ModioHttpParams.h"
#include <asio.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <winhttp.h>
namespace Modio
{
	namespace Detail
	{
		//TODO:@modio-win32 This should be able to use the common implementation of HTTPSharedState directly perhaps
		class HttpImplementation : public Modio::Detail::HttpImplementationBase<HttpImplementation, HttpSharedStateBase>
		{
		public:
			using HttpImplementationBase::HttpImplementationBase;

			auto MakeInitializeHttpOp(std::wstring UserString, std::shared_ptr<HttpSharedStateBase> State)
			{
				return InitializeHttp(UserString, State);
			}
		};
	} // namespace Detail
} // namespace Modio
