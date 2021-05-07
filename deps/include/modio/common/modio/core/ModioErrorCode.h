#pragma once

#include <system_error>

#include "modio/core/ModioStdTypes.h" //For ErrorCode type alias

namespace Modio
{
	enum class HttpError
	{
		CannotOpenConnection = 1,
		DownloadNotPermitted = 2,
		HttpAlreadyInitialized = 3,
		HttpNotInitialized = 4,
		InsufficientPermissions = 5,
		OperationCanceled = 6,
		RateLimited = 7,
		RequestError = 8,
		ResourceNotAvailable = 9,
		SecurityConfigurationInvalid = 10,
		ServerClosedConnection = 11,
		ServerUnavailable = 12,
		ServersOverloaded = 13
	};

	struct HttpErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::HttpError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::HttpError>(ErrorValue))
			{
				case HttpError::CannotOpenConnection:
						return "Unable to connect to server";
					break;
				case HttpError::DownloadNotPermitted:
						return "Trying to download file from outside of mod.io domain";
					break;
				case HttpError::HttpAlreadyInitialized:
						return "HTTP service already initialized";
					break;
				case HttpError::HttpNotInitialized:
						return "HTTP service not initialized";
					break;
				case HttpError::InsufficientPermissions:
						return "Insufficient permissions";
					break;
				case HttpError::OperationCanceled:
						return "HTTP operation cancelled";
					break;
				case HttpError::RateLimited:
						return "Too many requests made to the mod.io API within the rate-limiting window. Please wait and try again";
					break;
				case HttpError::RequestError:
						return "An error occurred making a HTTP request";
					break;
				case HttpError::ResourceNotAvailable:
						return "Invalid endpoint path";
					break;
				case HttpError::SecurityConfigurationInvalid:
						return "Invalid platform HTTP security configuration";
					break;
				case HttpError::ServerClosedConnection:
						return "Server closed connection unexpectedly";
					break;
				case HttpError::ServerUnavailable:
						return "Unable to connect to server";
					break;
				case HttpError::ServersOverloaded:
						return "The mod.io servers are overloaded. Please wait a bit before trying again";
					break;
				default:
					return "Unknown HttpError error";
			}
		}
	};

	inline const std::error_category& HttpErrorCategory()
	{
		static HttpErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(HttpError e)
	{
		return { static_cast<int>(e), HttpErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, HttpError B)
	{
		return A.category() == HttpErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, HttpError B)
	{
		return ! (A == B);
	}

	enum class FilesystemError
	{
		FileLocked = 1,
		FileNotFound = 2,
		InsufficientSpace = 3,
		NoPermission = 4,
		ReadError = 5,
		UnableToCreateFile = 6,
		UnableToCreateFolder = 7
	};

	struct FilesystemErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::FilesystemError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::FilesystemError>(ErrorValue))
			{
				case FilesystemError::FileLocked:
						return "File locked (already in use?)";
					break;
				case FilesystemError::FileNotFound:
						return "File not found";
					break;
				case FilesystemError::InsufficientSpace:
						return "Insufficient space for file";
					break;
				case FilesystemError::NoPermission:
						return "Insufficient permission for filesystem operation";
					break;
				case FilesystemError::ReadError:
						return "Error reading file";
					break;
				case FilesystemError::UnableToCreateFile:
						return "Could not create file";
					break;
				case FilesystemError::UnableToCreateFolder:
						return "Could not create folder";
					break;
				default:
					return "Unknown FilesystemError error";
			}
		}
	};

	inline const std::error_category& FilesystemErrorCategory()
	{
		static FilesystemErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(FilesystemError e)
	{
		return { static_cast<int>(e), FilesystemErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, FilesystemError B)
	{
		return A.category() == FilesystemErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, FilesystemError B)
	{
		return ! (A == B);
	}

	enum class UserAuthError
	{
		AlreadyAuthenticated = 1,
		NoAuthToken = 2,
		StatusAuthTokenInvalid = 3,
		StatusAuthTokenMissing = 4,
		UnableToInitStorage = 5
	};

	struct UserAuthErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::UserAuthError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::UserAuthError>(ErrorValue))
			{
				case UserAuthError::AlreadyAuthenticated:
						return "User is already authenticated. To use a new user and OAuth token, call ClearUserDataAsync";
					break;
				case UserAuthError::NoAuthToken:
						return "No Auth token available";
					break;
				case UserAuthError::StatusAuthTokenInvalid:
						return "The user's OAuth token was invalid";
					break;
				case UserAuthError::StatusAuthTokenMissing:
						return "OAuth token was missing";
					break;
				case UserAuthError::UnableToInitStorage:
						return "Could not initialize user storage";
					break;
				default:
					return "Unknown UserAuthError error";
			}
		}
	};

	inline const std::error_category& UserAuthErrorCategory()
	{
		static UserAuthErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(UserAuthError e)
	{
		return { static_cast<int>(e), UserAuthErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, UserAuthError B)
	{
		return A.category() == UserAuthErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, UserAuthError B)
	{
		return ! (A == B);
	}

	enum class UserDataError
	{
		BlobMissing = 1,
		InvalidUser = 2
	};

	struct UserDataErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::UserDataError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::UserDataError>(ErrorValue))
			{
				case UserDataError::BlobMissing:
						return "Some or all of the user data was missing from storage";
					break;
				case UserDataError::InvalidUser:
						return "Invalid user";
					break;
				default:
					return "Unknown UserDataError error";
			}
		}
	};

	inline const std::error_category& UserDataErrorCategory()
	{
		static UserDataErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(UserDataError e)
	{
		return { static_cast<int>(e), UserDataErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, UserDataError B)
	{
		return A.category() == UserDataErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, UserDataError B)
	{
		return ! (A == B);
	}

	enum class ArchiveError
	{
		InvalidHeader = 1
	};

	struct ArchiveErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::ArchiveError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::ArchiveError>(ErrorValue))
			{
				case ArchiveError::InvalidHeader:
						return "File did not have a valid archive header";
					break;
				default:
					return "Unknown ArchiveError error";
			}
		}
	};

	inline const std::error_category& ArchiveErrorCategory()
	{
		static ArchiveErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(ArchiveError e)
	{
		return { static_cast<int>(e), ArchiveErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, ArchiveError B)
	{
		return A.category() == ArchiveErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, ArchiveError B)
	{
		return ! (A == B);
	}

	enum class GenericError
	{
		BadParameter = 1,
		CouldNotCreateHandle = 2,
		EndOfFile = 3,
		IndexOutOfRange = 4,
		NoDataAvailable = 5,
		OperationCanceled = 6,
		QueueClosed = 7,
		SDKAlreadyInitialized = 8,
		SDKNotInitialized = 9
	};

	struct GenericErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::GenericError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::GenericError>(ErrorValue))
			{
				case GenericError::BadParameter:
						return "Bad parameter supplied";
					break;
				case GenericError::CouldNotCreateHandle:
						return "Operating system could not create the requested handle";
					break;
				case GenericError::EndOfFile:
						return "End of file";
					break;
				case GenericError::IndexOutOfRange:
						return "Index out of range";
					break;
				case GenericError::NoDataAvailable:
						return "No data available";
					break;
				case GenericError::OperationCanceled:
						return "The asynchronous operation was cancelled before it completed";
					break;
				case GenericError::QueueClosed:
						return "Operation could not be started as the service queue was missing or destroyed";
					break;
				case GenericError::SDKAlreadyInitialized:
						return "mod.io SDK was already initialized";
					break;
				case GenericError::SDKNotInitialized:
						return "mod.io SDK was not initialized";
					break;
				default:
					return "Unknown GenericError error";
			}
		}
	};

	inline const std::error_category& GenericErrorCategory()
	{
		static GenericErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(GenericError e)
	{
		return { static_cast<int>(e), GenericErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, GenericError B)
	{
		return A.category() == GenericErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, GenericError B)
	{
		return ! (A == B);
	}

	enum class ZlibError
	{
		EndOfStream = 1,
		IncompleteLengthSet = 2,
		InvalidBitLengthRepeat = 3,
		InvalidBlockType = 4,
		InvalidCodeLengths = 5,
		InvalidDistance = 6,
		InvalidDistanceCode = 7,
		InvalidLiteralLength = 8,
		InvalidStoredLength = 9,
		MissingEOB = 10,
		NeedBuffers = 11,
		OverSubscribedLength = 12,
		StreamError = 13,
		TooManySymbols = 14
	};

	struct ZlibErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::ZlibError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::ZlibError>(ErrorValue))
			{
				case ZlibError::EndOfStream:
						return "End of deflate stream";
					break;
				case ZlibError::IncompleteLengthSet:
						return "Incomplete length set";
					break;
				case ZlibError::InvalidBitLengthRepeat:
						return "Invalid bit length repeat";
					break;
				case ZlibError::InvalidBlockType:
						return "Invalid block type";
					break;
				case ZlibError::InvalidCodeLengths:
						return "Invalid code lengths";
					break;
				case ZlibError::InvalidDistance:
						return "Invalid distance";
					break;
				case ZlibError::InvalidDistanceCode:
						return "Invalid distance code";
					break;
				case ZlibError::InvalidLiteralLength:
						return "Invalid literal length";
					break;
				case ZlibError::InvalidStoredLength:
						return "Invalid store block length";
					break;
				case ZlibError::MissingEOB:
						return "Missing end-of-block marker";
					break;
				case ZlibError::NeedBuffers:
						return "Need more input data";
					break;
				case ZlibError::OverSubscribedLength:
						return "Over-subscribed length";
					break;
				case ZlibError::StreamError:
						return "Stream error";
					break;
				case ZlibError::TooManySymbols:
						return "Too many symbols";
					break;
				default:
					return "Unknown ZlibError error";
			}
		}
	};

	inline const std::error_category& ZlibErrorCategory()
	{
		static ZlibErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(ZlibError e)
	{
		return { static_cast<int>(e), ZlibErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, ZlibError B)
	{
		return A.category() == ZlibErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, ZlibError B)
	{
		return ! (A == B);
	}

	enum class ModManagementError
	{
		AlreadySubscribed = 1,
		InstallOrUpdateCancelled = 2,
		ModManagementDisabled = 3,
		NoPendingWork = 4
	};

	struct ModManagementErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::ModManagementError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::ModManagementError>(ErrorValue))
			{
				case ModManagementError::AlreadySubscribed:
						return "The user is already subscribed to the specified mod";
					break;
				case ModManagementError::InstallOrUpdateCancelled:
						return "The current mod installation or update was cancelled";
					break;
				case ModManagementError::ModManagementDisabled:
						return "Could not perform operation: Mod management is disabled and mod collection is locked";
					break;
				case ModManagementError::NoPendingWork:
						return "Internal: No mods require processing for this iteration";
					break;
				default:
					return "Unknown ModManagementError error";
			}
		}
	};

	inline const std::error_category& ModManagementErrorCategory()
	{
		static ModManagementErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(ModManagementError e)
	{
		return { static_cast<int>(e), ModManagementErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, ModManagementError B)
	{
		return A.category() == ModManagementErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, ModManagementError B)
	{
		return ! (A == B);
	}

	enum class ApiError
	{
		AuthenticatedAccountHasBeenDeleted = 11006,
		BannedUserAccount = 11007,
		BinaryFileCorrupted = 13001,
		BinaryFileUnreadable = 13002,
		CrossOriginForbidden = 10001,
		ExpiredOrRevokedAccessToken = 11005,
		FailedToCompleteTheRequest = 10002,
		InvalidAPIKey = 11002,
		InvalidApiVersion = 10003,
		InvalidJSON = 13004,
		MalformedAPIKey = 11001,
		MissingAPIKey = 11000,
		MissingContentTypeHeader = 13005,
		MissingReadPermission = 11004,
		MissingWritePermission = 11003,
		ModioOutage = 10000,
		Ratelimited = 11008,
		ReqestedModNotFound = 15022,
		RequestedCommentNotFound = 15026,
		RequestedGameDeleted = 14006,
		RequestedGameNotFound = 14001,
		RequestedInvalidResponseFormat = 13007,
		RequestedModDeleted = 15023,
		RequestedModfileNotFound = 15010,
		RequestedResourceNotFound = 14000,
		RequestedUserNotFound = 21000,
		UnsupportedContentTypeHeader = 13006,
		ValidationErrors = 13009
	};

	struct ApiErrorCategoryImpl : std::error_category
	{
		inline const char* name() const noexcept override { return "Modio::ApiError"; }
		inline std::string message(int ErrorValue) const override
		{
			switch (static_cast<Modio::ApiError>(ErrorValue))
			{
				case ApiError::AuthenticatedAccountHasBeenDeleted:
						return "Authenticated user account has been deleted.";
					break;
				case ApiError::BannedUserAccount:
						return "Authenticated user account has been banned by mod.io admins.";
					break;
				case ApiError::BinaryFileCorrupted:
						return "The submitted binary file is corrupted.";
					break;
				case ApiError::BinaryFileUnreadable:
						return "The submitted binary file is unreadable.";
					break;
				case ApiError::CrossOriginForbidden:
						return "Cross-origin request forbidden.";
					break;
				case ApiError::ExpiredOrRevokedAccessToken:
						return "Access token is expired, or has been revoked.";
					break;
				case ApiError::FailedToCompleteTheRequest:
						return "mod.io failed to complete the request, please try again. (rare)";
					break;
				case ApiError::InvalidAPIKey:
						return "api_key supplied is invalid.";
					break;
				case ApiError::InvalidApiVersion:
						return "API version supplied is invalid.";
					break;
				case ApiError::InvalidJSON:
						return "You have used the input_json parameter with semantically incorrect JSON.";
					break;
				case ApiError::MalformedAPIKey:
						return "api_key supplied is malformed.";
					break;
				case ApiError::MissingAPIKey:
						return "api_key is missing from your request.";
					break;
				case ApiError::MissingContentTypeHeader:
						return "The Content-Type header is missing from your request.";
					break;
				case ApiError::MissingReadPermission:
						return "Access token is missing the read scope to perform the request.";
					break;
				case ApiError::MissingWritePermission:
						return "Access token is missing the write scope to perform the request.";
					break;
				case ApiError::ModioOutage:
						return "mod.io is currently experiencing an outage. (rare)";
					break;
				case ApiError::Ratelimited:
						return "You have been ratelimited for making too many requests. See Rate Limiting.";
					break;
				case ApiError::ReqestedModNotFound:
						return "The requested mod could not be found.";
					break;
				case ApiError::RequestedCommentNotFound:
						return "The requested comment could not be found.";
					break;
				case ApiError::RequestedGameDeleted:
						return "The requested game has been deleted.";
					break;
				case ApiError::RequestedGameNotFound:
						return "The requested game could not be found.";
					break;
				case ApiError::RequestedInvalidResponseFormat:
						return "You have requested a response format that is not supported (JSON only).";
					break;
				case ApiError::RequestedModDeleted:
						return "The requested mod has been deleted.";
					break;
				case ApiError::RequestedModfileNotFound:
						return "The requested modfile could not be found.";
					break;
				case ApiError::RequestedResourceNotFound:
						return "The requested resource does not exist.";
					break;
				case ApiError::RequestedUserNotFound:
						return "The requested user could not be found.";
					break;
				case ApiError::UnsupportedContentTypeHeader:
						return "The Content-Type header is not supported for this endpoint.";
					break;
				case ApiError::ValidationErrors:
						return "The request contains validation errors for the data supplied. See the attached errors field within the Error Object to determine which input failed.";
					break;
				default:
					return "Unknown ApiError error";
			}
		}
	};

	inline const std::error_category& ApiErrorCategory()
	{
		static ApiErrorCategoryImpl CategoryInstance;
		return CategoryInstance;
	}
	
	inline std::error_code make_error_code(ApiError e)
	{
		return { static_cast<int>(e), ApiErrorCategory() };
	}

	inline bool operator==(Modio::ErrorCode A, ApiError B)
	{
		return A.category() == ApiErrorCategory() && A.value() == static_cast<int>(B);
	}

	inline bool operator!=(std::error_code A, ApiError B)
	{
		return ! (A == B);
	}

}
