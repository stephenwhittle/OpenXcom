#pragma once
#include "ModioGeneratedVariables.h"
#include "fmt/format.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/ModioStringHelpers.h"
#include "modio/detail/http/ModioRequestBodyKVPContainer.h"
#include "modio/core/ModioLogger.h"
#include <regex>
#include <string>
#undef DELETE

namespace Modio
{
	// TODO: @Modio-core implement payload-setting function on the HttpRequestParams for PUT/POST requests
	namespace Detail
	{
		enum class Verb
		{
			GET,
			POST,
			HEAD,
			PUT,
			DELETE
		};
		enum class APIVersion
		{
			V1
		};

		class HttpRequestParams
		{
		public:
			// that mutates the object instead to avoid memory allocations as it will be potentially expensive to copy
			// the object when setting a payload
			MODIO_IMPL HttpRequestParams SetGameID(GameID ID) const;

			MODIO_IMPL HttpRequestParams& SetGameID(GameID ID);

			MODIO_IMPL HttpRequestParams SetModID(std::uint32_t ID) const;

			MODIO_IMPL HttpRequestParams& SetModID(std::uint32_t ID);

			MODIO_IMPL HttpRequestParams SetFilterString(const std::string& InFilterString) const;

			MODIO_IMPL HttpRequestParams& SetFilterString(const std::string& InFilterString);

			// Overload to silently drop nulled keys - doesn't have to be a template strictly, but is one so that it's
			// lower priority than the std::string version in the overload set
			template<typename UnderlyingType>
			HttpRequestParams AppendPayloadValue(std::string Key, Modio::Optional<UnderlyingType> Value) const
			{
				if (Value)
				{
					return AppendPayloadValue(Key, *Value);
				}
				else
				{
					return HttpRequestParams(*this);
				}
			}

			MODIO_IMPL HttpRequestParams AppendPayloadValue(std::string Key, std::string Value) const;

			template<typename T>
			HttpRequestParams SetPayload(T RawPayload) const
			{
				auto NewParamsInstance = HttpRequestParams(*this);
				NewParamsInstance.SetPayload(RawPayload);
				return NewParamsInstance;
			}

			/// <summary>
			/// // @todo: This seems like it can be a bad idea to have a potentially heavy string here
			/// Set the payload (body) of the http request. If the request is a GET request, the call will be ignored
			/// as you can't post any payload with a get request.
			/// </summary>
			/// <typeparam name="T">There needs to exista function with the signature
			/// void ToRequestBody(Modio::Detail::RequestBodyKVPContainer& Container, const T& Payload); that serializes
			/// that calls Container.Add("key", value); for each key so that we know what keys exists in the struct
			/// </typeparam> <param name="RawPayload">The payload in raw format</param> <returns>The modified
			/// request</returns>
			template<typename T>
			HttpRequestParams& SetPayload(T RawPayload)
			{
				// Get requests can't have payloads
				if (GetTypedVerb() == Verb::GET)
				{
					Modio::Detail::Logger().Log(LogLevel::Warning, LogCategory::Http,
												"Can't add any payload to GET request [{}], silently failing",
												ResourcePath);
					return *this;
				}

				Modio::Detail::RequestBodyKVPContainer KVPValues;
				ToRequestBody(KVPValues, RawPayload);

				std::string TempPayload;
				bool bFirstIteration = true;
				for (const auto& Itr : KVPValues)
				{
					if (!bFirstIteration)
					{
						TempPayload += "&";
					}
					else
					{
						bFirstIteration = false;
					}

					TempPayload += fmt::format("{}={}", Itr.first, Modio::Detail::String::URLEncode(Itr.second));
				}
				Payload = std::move(TempPayload);

				return *this;
			}

			MODIO_IMPL HttpRequestParams SetAuthTokenOverride(const std::string& AuthToken) const;

			MODIO_IMPL HttpRequestParams& SetAuthTokenOverride(const std::string& AuthToken);

			MODIO_IMPL HttpRequestParams SetRange(Modio::FileOffset Start,
												  Modio::Optional<Modio::FileOffset> End) const;

			MODIO_IMPL HttpRequestParams& SetRange(Modio::FileOffset Start, Modio::Optional<Modio::FileOffset> End);

			MODIO_IMPL std::string GetServerAddress() const;

			MODIO_IMPL std::string GetFormattedResourcePath() const;

			MODIO_IMPL std::string GetVerb() const;

			MODIO_IMPL Verb GetTypedVerb() const;

			MODIO_IMPL const Optional<std::string>& GetPayload() const;

			using Header = std::pair<std::string, std::string>;
			using HeaderList = std::vector<Header>;

			MODIO_IMPL HeaderList GetHeaders() const;

			HttpRequestParams(Modio::Detail::Verb CurrentOperationType, const char* ResourcePath)
				: ResourcePath(ResourcePath),
				  GameID(0),
				  ModID(0),
				  CurrentOperationType(CurrentOperationType),
				  CurrentAPIVersion(Modio::Detail::APIVersion::V1)
			{}
			
			HttpRequestParams()
				: ResourcePath("NOT_SET"),
				  GameID(0),
				  ModID(0),
				  CurrentOperationType(Modio::Detail::Verb::GET),
				  CurrentAPIVersion(Modio::Detail::APIVersion::V1)
			{}

			MODIO_IMPL static Modio::Optional<HttpRequestParams> FileDownload(std::string URL);

		private:
			MODIO_IMPL HttpRequestParams(std::string Server, std::string ResourcePath);
			MODIO_IMPL std::string GetAPIVersionString() const;
			MODIO_IMPL std::string GetAPIKeyString() const;

			MODIO_IMPL const Modio::Optional<std::string> GetAuthToken() const;

			/// <summary>
			/// Resolves all "template parameters" in a resource path to their actual values
			/// </summary>
			/// <returns>The resolved resource path</returns>
			MODIO_IMPL std::string GetResolvedResourcePath() const;

			bool bFileDownload = false;

			std::string FileDownloadServer;

			std::string ResourcePath;
			std::uint64_t GameID;
			std::uint64_t ModID;

			// @todo: Investigate FilterString/Payload how we could refactor those
			Modio::Optional<std::string> FilterString;
			// This should most likely be a ID into a separate payload store or
			// let it be put as a different parameter
			Modio::Optional<std::string> Payload;
			Modio::Optional<std::string> AuthTokenOverride;

			std::string APIKey;
			// constexpr static const char* APIKey = MODIO_API_KEY;

			// current API key
			// URL
			Verb CurrentOperationType;
			// other headers
			// payload data
			// a lot of this data can be constexpr
			// api version
			APIVersion CurrentAPIVersion;

			Modio::Optional<Modio::FileOffset> StartOffset;
			Modio::Optional<Modio::FileOffset> EndOffset;
		};

		// TODO: @Modio-core implement comparison operator for HttpRequestParams and maybe IsValid()?
		static inline HttpRequestParams InvalidParams {};
	} // namespace Detail
}; // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioHttpParams.ipp"
#endif

#include "modio/core/ModioDefaultRequestParameters.h"