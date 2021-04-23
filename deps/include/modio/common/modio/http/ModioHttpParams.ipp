#ifdef MODIO_SEPARATE_COMPILATION
	#include "ModioHttpParams.h"
#endif

#include "modio/detail/ModioSDKSessionData.h"

namespace Modio
{
	namespace Detail
	{
		Modio::Detail::HttpRequestParams& HttpRequestParams::SetGameID(Modio::GameID ID)
		{
			this->GameID = ID;
			return *this;
		}

		Modio::Detail::HttpRequestParams HttpRequestParams::SetGameID(Modio::GameID ID) const
		{
			auto NewParamsInstance = HttpRequestParams(*this);
			NewParamsInstance.GameID = ID;
			return NewParamsInstance;
		}

		Modio::Detail::HttpRequestParams& HttpRequestParams::SetModID(std::uint32_t ID)
		{
			ModID = ID;
			return *this;
		}

		Modio::Detail::HttpRequestParams HttpRequestParams::SetModID(std::uint32_t ID) const
		{
			auto NewParamsInstance = HttpRequestParams(*this);
			NewParamsInstance.ModID = ID;
			return NewParamsInstance;
		}

		Modio::Detail::HttpRequestParams& HttpRequestParams::SetFilterString(const std::string& InFilterString)
		{
			// If there was a provided filter string, append on & to ensure that we can append on the API key
			if (InFilterString.size())
			{
				// @todo: Move away this & as it makes it hard to follow the logic of how the URL is built
				FilterString = InFilterString + "&";
			}

			return *this;
		}

		Modio::Detail::HttpRequestParams HttpRequestParams::SetFilterString(const std::string& InFilterString) const
		{
			auto NewParamsInstance = HttpRequestParams(*this);
			NewParamsInstance.SetFilterString(InFilterString);
			return NewParamsInstance;
		}

		Modio::Detail::HttpRequestParams HttpRequestParams::AppendPayloadValue(std::string Key, std::string Value) const
		{
			auto NewParamsInstance = HttpRequestParams(*this);
			if (NewParamsInstance.Payload.has_value())
			{
				*NewParamsInstance.Payload += Key + "=" + Value;
			}
			else
			{
				NewParamsInstance.Payload = Key + "=" + Value;
			}
			return NewParamsInstance;
		}

		Modio::Detail::HttpRequestParams& HttpRequestParams::SetAuthTokenOverride(const std::string& AuthToken)
		{
			AuthTokenOverride = AuthToken;
			return *this;
		}

		Modio::Detail::HttpRequestParams HttpRequestParams::SetAuthTokenOverride(const std::string& AuthToken) const
		{
			auto NewParamsInstance = HttpRequestParams(*this);
			NewParamsInstance.AuthTokenOverride = AuthToken;
			return NewParamsInstance;
		}

		Modio::Detail::HttpRequestParams& HttpRequestParams::SetRange(Modio::FileOffset Start,
																	  Modio::Optional<Modio::FileOffset> End)
		{
			StartOffset = Start;
			EndOffset = End;
			return *this;
		}

		Modio::Detail::HttpRequestParams HttpRequestParams::SetRange(Modio::FileOffset Start,
																	 Modio::Optional<Modio::FileOffset> End) const
		{
			auto NewParamsInstance = HttpRequestParams(*this);
			NewParamsInstance.StartOffset = Start;
			NewParamsInstance.EndOffset = End;
			return NewParamsInstance;
		}

		std::string HttpRequestParams::GetServerAddress() const
		{
			if (bFileDownload)
			{
				return FileDownloadServer;
			}
			else
			{
				// @todo: Break this out so that we don't need to use this class in conjunction with the SessionData
				switch (Modio::Detail::SDKSessionData::GetEnvironment())
				{
					case Environment::Live:
						return "api.mod.io";
					case Environment::Test:
						return "api.test.mod.io";
				}
				return "";
			}
		}

		std::string HttpRequestParams::GetFormattedResourcePath() const
		{
			if (bFileDownload)
			{
				return ResourcePath;
			}
			else
			{
				// @todo: We want to clean up this, this is getting messy
				std::string Filter = FilterString.value_or("");
				return std::string(GetAPIVersionString() + GetResolvedResourcePath() + "?" + Filter +
								   GetAPIKeyString());
			}
		}

		std::string HttpRequestParams::GetVerb() const
		{
			switch (CurrentOperationType)
			{
				case Verb::GET:
					return "GET";
				case Verb::HEAD:
					return "HEAD";
				case Verb::POST:
					return "POST";
				case Verb::PUT:
					return "PUT";
				case Verb::DELETE:
					return "DELETE";
			}
			return "";
		}

		Modio::Detail::Verb HttpRequestParams::GetTypedVerb() const
		{
			return CurrentOperationType;
		}

		const Modio::Optional<std::string>& HttpRequestParams::GetPayload() const
		{
			return Payload;
		}

		Modio::Detail::HttpRequestParams::HeaderList HttpRequestParams::GetHeaders() const
		{
			// Default headers
			HeaderList Headers = {{"User-Agent", "Modio-SDKv2-" MODIO_COMMIT_HASH},
								  {"x-modio-platform", MODIO_TARGET_PLATFORM_ID}};

			if (Payload || CurrentOperationType == Modio::Detail::Verb::POST ||
				CurrentOperationType == Modio::Detail::Verb::DELETE)
			{
				Headers.push_back({"Content-Type", "application/x-www-form-urlencoded"});
			}

			const Modio::Optional<std::string>& AuthToken = GetAuthToken();
			if (AuthToken)
			{
				Headers.push_back({"Authorization", fmt::format("Bearer {}", *AuthToken)});
			}
			if (StartOffset || EndOffset)
			{
				Headers.push_back({"Range", fmt::format("bytes={}-{}", StartOffset ? StartOffset.value() : 0,
														EndOffset ? fmt::format("{}", EndOffset.value()) : "")});
			}
			// @todo: Set Content-Type: multipart/form-data for binary payload
			return Headers;
		}

		HttpRequestParams::HttpRequestParams(std::string Server, std::string ResourcePath)
			: bFileDownload(true),
			  FileDownloadServer(Server),
			  ResourcePath(ResourcePath),
			  GameID(0),
			  ModID(0),
			  CurrentOperationType(Modio::Detail::Verb::GET),
			  CurrentAPIVersion(Modio::Detail::APIVersion::V1)
		{}

		

		Modio::Optional<Modio::Detail::HttpRequestParams> HttpRequestParams::FileDownload(std::string URL)
		{
			std::regex URLPattern("(https:\\/\\/)?(.*\\.io)(.+)$", std::regex::icase);
			std::smatch MatchInfo;
			if (std::regex_search(URL, MatchInfo, URLPattern))
			{
				if (MatchInfo.size() == 4)
				{
					return HttpRequestParams(MatchInfo[2].str(), MatchInfo[3].str());
				}
			}
			return {};
		}

		std::string HttpRequestParams::GetAPIVersionString() const
		{
			switch (CurrentAPIVersion)
			{
				case APIVersion::V1:
					return "/v1";
			}
			return "";
		}

		std::string HttpRequestParams::GetAPIKeyString() const
		{
			using namespace Modio::Detail;

			if (Modio::Detail::SDKSessionData::GetAuthenticationToken())
			{
				return std::string("");
			}

			return std::string("api_key=") + SDKSessionData::CurrentAPIKey().InternalApiKey;
		}

		const Modio::Optional<std::string> HttpRequestParams::GetAuthToken() const
		{
			using namespace Modio::Detail;

			return AuthTokenOverride.has_value() ? AuthTokenOverride.value()
												 : Modio::Detail::SDKSessionData::GetAuthenticationToken().and_then(
													   &Modio::Detail::OAuthToken::GetToken);
		}

		std::string HttpRequestParams::GetResolvedResourcePath() const
		{
			using namespace Modio::Detail;

			// @todo: This does allocation, would be nice if we could avoid doing allocation or "lock-in" the
			// parameters after we have done this call first time so that we don't need to do allocations if we call
			// it again. But maybe we won't reuse a resource path, so it won't be a problem anyway
			std::string TempResourcePath = ResourcePath;
			String::ReplaceAll(TempResourcePath, "{game-id}", std::to_string(GameID));
			String::ReplaceAll(TempResourcePath, "{mod-id}", std::to_string(ModID));

			return TempResourcePath;
		}

	} // namespace Detail
} // namespace Modio
