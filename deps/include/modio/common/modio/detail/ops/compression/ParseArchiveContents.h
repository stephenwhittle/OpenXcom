#pragma once

#include "modio/core/ModioStdTypes.h"
#include "modio/detail/compression/zip/ArchiveFileImplementation.h"
#include "modio/file/ModioFile.h"

#include <asio/yield.hpp>
namespace Modio
{
	namespace Detail
	{
		class ParseArchiveContents
		{
			Modio::StableStorage<Modio::Detail::File> ArchiveFileOnDisk;
			Modio::StableStorage<ArchiveFileImplementation> ArchiveState;
			asio::coroutine CoroutineState;
			std::uintmax_t CurrentSearchOffset = 0;
			bool bEndOfFileReached = false;
			std::uintmax_t CurrentRecordOffset = 0;
			std::vector<ArchiveFileImplementation::ArchiveEntry> PendingEntries;
			std::uintmax_t CurrentFixupEntryIndex = 0;

		public:
			ParseArchiveContents(std::shared_ptr<ArchiveFileImplementation> ArchiveState) : ArchiveState(ArchiveState)
			{
				ArchiveFileOnDisk = std::make_shared<Modio::Detail::File>(ArchiveState->FilePath, false);
				std::size_t FileSize = ArchiveFileOnDisk->GetFileSize();
				CurrentSearchOffset = FileSize - (std::min((uint64_t)FileSize, (uint64_t)(64 * 1024)));
			};

			template<typename CoroType>
			void operator()(CoroType& Self, Modio::ErrorCode ec = {},
							Modio::Optional<Modio::Detail::Buffer> FileChunk = {})
			{
				reenter(CoroutineState)
				{
					while (ArchiveState->ZipMagicOffset == 0)
					{
						yield ArchiveFileOnDisk->async_ReadSomeAt(CurrentSearchOffset, 64 * 1024, std::move(Self));
						if (ec && ec != Modio::GenericError::EndOfFile)
						{
							Self.complete(ec);
							return;
						}
						else
						{
							if (FileChunk.has_value())
							{
								Modio::Detail::Buffer& Chunk = FileChunk.value();
								if (Chunk.GetSize() >= 4)
								{
									std::uintmax_t LocalPosition = Chunk.GetSize() - 4;
									for (; LocalPosition > 0; LocalPosition--)
									{
										uint32_t Value =
											Modio::Detail::TypedBufferRead<std::uint32_t>(Chunk, LocalPosition);
										if (Value == 0x06054b50)
										{
											ArchiveState->ZipMagicOffset = CurrentSearchOffset + LocalPosition;
											break;
										}
									}
								}
								else
								{
									Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::File,
																"Read less than 4 bytes from archive file.");
								}
							}
							else
							{
								Self.complete(Modio::make_error_code(Modio::FilesystemError::ReadError));
								return;
							}
						}

						// If we haven't found the signature and we're at the end of the file, bail
						if (ArchiveState->ZipMagicOffset == 0 && CurrentSearchOffset == 0)
						{
							Self.complete(Modio::make_error_code(Modio::ArchiveError::InvalidHeader));
							return;
						}
						if (CurrentSearchOffset >= 64 * 1024)
						{
							CurrentSearchOffset -= 64 * 1024;
						}
						else
						{
							CurrentSearchOffset = 0;
						}
					}
					// Now we've located the central directory metadata, do a direct read on that region (in case a
					// chunk boundary from the earlier search splits it)
					yield ArchiveFileOnDisk->async_ReadSomeAt(ArchiveState->ZipMagicOffset, 20, std::move(Self));
					// If we got EOF whilst trying to read the 20 bytes containing the directory information, inform the
					// consumer
					if (ec == Modio::GenericError::EndOfFile)
					{
						Self.complete(Modio::make_error_code(Modio::ArchiveError::InvalidHeader));
						return;
					}

					if (FileChunk.has_value())
					{
						ArchiveState->NumberOfRecords =
							Modio::Detail::TypedBufferRead<std::uint16_t>(FileChunk.value(), 10);
						ArchiveState->CentralDirectorySize =
							Modio::Detail::TypedBufferRead<std::uint32_t>(FileChunk.value(), 12);
						ArchiveState->CentralDirectoryOffset =
							Modio::Detail::TypedBufferRead<std::uint32_t>(FileChunk.value(), 16);
						yield ArchiveFileOnDisk->async_ReadSomeAt(ArchiveState->CentralDirectoryOffset,
																  ArchiveState->CentralDirectorySize, std::move(Self));
						if (FileChunk.has_value() && FileChunk->GetSize() != ArchiveState->CentralDirectorySize)
						{
							// Could not read full central directory
							Self.complete(Modio::make_error_code(Modio::FilesystemError::ReadError));
							return;
						}

						CurrentRecordOffset = 0;

						for (std::uint16_t RecordIndex = 0; RecordIndex < ArchiveState->NumberOfRecords; RecordIndex++)
						{
							std::uint16_t CompressionMethod = Modio::Detail::TypedBufferRead<std::uint16_t>(
								FileChunk.value(), CurrentRecordOffset + 10);

							std::uint32_t CompressedSize = Modio::Detail::TypedBufferRead<std::uint32_t>(
								FileChunk.value(), CurrentRecordOffset + 20);
							std::uint32_t UncompressedSize = Modio::Detail::TypedBufferRead<std::uint32_t>(
								FileChunk.value(), CurrentRecordOffset + 24);
							std::uint16_t FileNameLength = Modio::Detail::TypedBufferRead<std::uint16_t>(
								FileChunk.value(), CurrentRecordOffset + 28);

							std::uint16_t ExtraFieldLength = Modio::Detail::TypedBufferRead<std::uint16_t>(
								FileChunk.value(), CurrentRecordOffset + 30);

							std::uint16_t CommentLength = Modio::Detail::TypedBufferRead<std::uint16_t>(
								FileChunk.value(), CurrentRecordOffset + 32);

							std::uint32_t LocalHeaderOffset = Modio::Detail::TypedBufferRead<std::uint32_t>(
								FileChunk.value(), CurrentRecordOffset + 42);

							ArchiveState->TotalExtractedSize += Modio::FileSize(UncompressedSize);

							PendingEntries.push_back(ArchiveFileImplementation::ArchiveEntry {
								static_cast<ArchiveFileImplementation::CompressionMethod>(CompressionMethod),
								std::string((const char*) FileChunk->Data() + CurrentRecordOffset + 46, FileNameLength),
								LocalHeaderOffset, CompressedSize});

							CurrentRecordOffset += (46 + FileNameLength + ExtraFieldLength + CommentLength);
						}
						// Now we have the information about the local headers for all the files, we need to read those
						// parts of the zip locate the actual file offsets
						for (CurrentFixupEntryIndex = 0; CurrentFixupEntryIndex < PendingEntries.size();
							 CurrentFixupEntryIndex++)
						{
							yield ArchiveFileOnDisk->async_ReadSomeAt(PendingEntries[CurrentFixupEntryIndex].FileOffset,
																	  30, std::move(Self));

							if (ec && ec != Modio::GenericError::EndOfFile)
							{
								Self.complete(ec);
								return;
							}

							std::uint16_t LocalFileLength =
								Modio::Detail::TypedBufferRead<std::uint16_t>(FileChunk.value(), 26);
							std::uint16_t LocalExtraFieldLength =
								Modio::Detail::TypedBufferRead<std::uint16_t>(FileChunk.value(), 28);
							PendingEntries[CurrentFixupEntryIndex].FileOffset +=
								30 + LocalFileLength + LocalExtraFieldLength;
							ArchiveState->AddEntry(PendingEntries[CurrentFixupEntryIndex]);
						}

						Self.complete(Modio::ErrorCode {});
					}
					else
					{
						Self.complete(Modio::make_error_code(Modio::FilesystemError::ReadError));
						return;
					}
				}
			}
		};

	} // namespace Detail
} // namespace Modio
#include <asio/unyield.hpp>
