#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/ModioErrorCode.h"

#include "ModioErrorConditionLibrary.generated.h"


UCLASS()
class UModioErrorConditionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	public:
		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesModioServiceError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ModioServiceError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesNetworkError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::NetworkError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesConfigurationError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ConfigurationError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesInvalidArgsError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::InvalidArgsError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesFilesystemError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::FilesystemError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesInternalError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::InternalError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesApiErrorRefSuccess(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ApiErrorRefSuccess);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesModInstallRetryableError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ModInstallRetryableError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesModInstallDeferredError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ModInstallDeferredError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesModDeleteDeferredError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ModDeleteDeferredError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesModInstallUnrecoverableError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::ModInstallUnrecoverableError);
		}

		UFUNCTION(BlueprintCallable, Category = "mod.io|Error Handling")
		static bool ErrorCodeMatchesEntityNotFoundError(FModioErrorCode ec)
		{
			return Modio::ErrorCodeMatches(ec.GetRawErrorCode(), Modio::ErrorConditionTypes::EntityNotFoundError);
		}

};