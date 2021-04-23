#pragma once
#include "common/detail/ops/file/InitializeFileSystem.h"
#include "common/file/FileSystemImplementation.h"

namespace Modio
{
	namespace Detail
	{
		class FileSystemImplementation
			: public Modio::Detail::FileSystemImplementationBase<FileSystemImplementation, std::string>
		{
		public:
			using FileSystemImplementationBase::FileSystemImplementationBase;
			
			auto MakeInitializeStorageOp(std::string NewUserHandle, Modio::GameID GameID,
										 Modio::filesystem::path& CommonPath, Modio::filesystem::path& UserPath, Modio::filesystem::path& TempPath)
			{
				return Modio::Detail::InitializeFileSystem(NewUserHandle, GameID, CommonPath, UserPath, TempPath);
			}
		};
	} // namespace Detail
} // namespace Modio
