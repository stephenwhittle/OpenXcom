#pragma once
#include "modio/detail/ModioDefines.h"
#include "ModioGeneratedVariables.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/detail/AsioWrapper.h"
#include <memory>
#include <mutex>

namespace Modio
{
	namespace Detail
	{
		/// @internal
		/// @brief Alignable moveable fixed-size buffer class. Aligned storage is more wasteful but will allow us to
		/// swap to unbuffered IO on Windows/ERA platforms if we need additional performance
		class Buffer
		{
			std::unique_ptr<unsigned char[]> InternalData;
			std::size_t Alignment;
			std::size_t AlignmentOffset;
			std::size_t Size = 0;

		public:
			using iterator = unsigned char*;
			using const_iterator = const unsigned char*;

			MODIO_IMPL std::size_t GetAlignment() const;

			MODIO_IMPL Buffer(std::size_t Size, std::size_t Alignment = 1);
			MODIO_IMPL ~Buffer();
			Buffer(Buffer&& Source) noexcept
				: InternalData(std::move(Source.InternalData)),
				  Alignment(std::move(Source.Alignment)),
				  AlignmentOffset(Source.AlignmentOffset),
				  Size(Source.Size) {};
			MODIO_IMPL Buffer& operator=(Buffer&& Source) noexcept;

			Buffer(const Buffer&) = delete;
			Buffer& operator=(const Buffer&) = delete;
			MODIO_IMPL Buffer CopyRange(std::size_t BeginIndex, std::size_t EndIndex);
			MODIO_IMPL Buffer CopyRange(const_iterator Start, const_iterator End);

			MODIO_IMPL Buffer Clone() const;

			MODIO_IMPL unsigned char* const Data() const;
			MODIO_IMPL unsigned char* const begin() const;
			MODIO_IMPL unsigned char* const end() const;
			MODIO_IMPL unsigned char& operator[](size_t Index) const;
			MODIO_IMPL std::size_t GetSize() const;
		};

		/// @brief Class conforming to ASIO's DynamicBuffer_v2 concept also providing a stable address for the
		/// underlying data, suitable for use in composed asynchronous operations. Copy constructor will only copy the
		/// shared_ptr not the underlying data so that the address of the data itself doesn't change
		class DynamicBuffer
		{
			std::shared_ptr<std::vector<Modio::Detail::Buffer>> InternalBuffers;
			std::size_t Alignment = 1;
			std::shared_ptr<std::mutex> BufferLock;
			class DynamicBufferSequence
			{
				std::vector<Modio::MutableBufferView> BufferViews;

			public:
				MODIO_IMPL DynamicBufferSequence(std::shared_ptr<std::vector<Modio::Detail::Buffer>> BuffersToView,
												 std::size_t ByteOffset,
									  std::size_t NumberOfBytes);

				MODIO_IMPL DynamicBufferSequence(std::shared_ptr<std::vector<Modio::Detail::Buffer>> BuffersToView);

				MODIO_IMPL std::vector<Modio::MutableBufferView>::const_iterator begin() const;
				MODIO_IMPL const std::vector<Modio::MutableBufferView>::const_iterator end() const;

				using value_type = Modio::MutableBufferView;
				using const_iterator = std::vector<Modio::MutableBufferView>::const_iterator;
			};

		public:
			MODIO_IMPL DynamicBuffer(std::size_t Alignment = 1);
			MODIO_IMPL DynamicBuffer(const DynamicBuffer& Other);

			MODIO_IMPL DynamicBuffer Clone() const;

			// @todo: This feels a bit hakky, I would like to rename this
			MODIO_IMPL void CopyBufferConfiguration(const DynamicBuffer& Other);

			MODIO_IMPL std::unique_lock<std::mutex> Lock();

			/// <summary>
			/// Clear out the content of the buffer
			/// @todo: Optimize, go through and ensure that we can reuse the buffer instead of clearing it
			/// </summary>
			MODIO_IMPL void Clear();

			using const_buffers_type = DynamicBufferSequence;

			using mutable_buffers_type = DynamicBufferSequence;

			// More friendly type alias
			using Sequence = DynamicBufferSequence;

			MODIO_IMPL std::size_t size() const;

			MODIO_IMPL std::size_t max_size() const;

			MODIO_IMPL std::size_t capacity() const;

			MODIO_IMPL const_buffers_type data(std::size_t pos, std::size_t n) const;

			MODIO_IMPL mutable_buffers_type data(std::size_t pos, std::size_t n);

			MODIO_IMPL const_buffers_type data() const;

			MODIO_IMPL mutable_buffers_type data();

			MODIO_IMPL void grow(std::size_t n);

			MODIO_IMPL void shrink(std::size_t n);

			MODIO_IMPL void consume(std::size_t n);

			MODIO_IMPL Modio::Optional<Modio::Detail::Buffer> TakeInternalBuffer();

			MODIO_IMPL void AppendBuffer(Modio::Detail::Buffer NewBuffer);

			MODIO_IMPL std::vector<Modio::Detail::Buffer>::iterator begin();
			MODIO_IMPL std::vector<Modio::Detail::Buffer>::iterator end();

			MODIO_IMPL std::vector<Modio::Detail::Buffer>::const_iterator begin() const;
			MODIO_IMPL std::vector<Modio::Detail::Buffer>::const_iterator end() const;

			MODIO_IMPL bool Equals(const Modio::Detail::DynamicBuffer& Other) const;
		};

		template<typename DestinationType>
		DestinationType TypedBufferRead(Modio::Detail::DynamicBuffer& BufferToRead, std::uintmax_t Offset)
		{
			auto DesiredDataRange =
				BufferToRead.data(Offset, sizeof(DestinationType));
			DestinationType Destination;
			Modio::MutableBufferView MBV(&Destination, sizeof(Destination));
			asio::buffer_copy(MBV, DesiredDataRange);
			return Destination;
		};

		template<typename DestinationType>
		DestinationType TypedBufferRead(Modio::Detail::Buffer& BufferToRead, std::uintmax_t Offset)
		{
			DestinationType Destination;
			Modio::ConstBufferView SourceBufferView(BufferToRead.Data() + Offset, sizeof(DestinationType));
			Modio::MutableBufferView DestBufferView(&Destination, sizeof(Destination));
			asio::buffer_copy(DestBufferView, SourceBufferView);
			return Destination;
		}

		/// <summary>
		/// Copies a Modio::Detail::DynamicBuffer into a Modio::Detail::Buffer with no error checking
		/// </summary>
		/// <param name="Destination"></param>
		/// <param name="Source"></param>
		/// <returns>The number of bytes copied</returns>
		inline std::size_t BufferCopy(Modio::Detail::Buffer& Destination, const Modio::Detail::DynamicBuffer Source)
		{
			return asio::buffer_copy(Modio::MutableBufferView(Destination.Data(), Destination.GetSize()),
									 Source.data());
		}

		inline std::size_t BufferCopy(Modio::Detail::DynamicBuffer& Destination, const Modio::Detail::DynamicBuffer Source)
		{
			Modio::Detail::DynamicBuffer::Sequence SourceBufferView = Source.data();
			Modio::Detail::DynamicBuffer::Sequence DestinationBufferView = Destination.data();

			return asio::buffer_copy(DestinationBufferView, SourceBufferView);
		}
	} // namespace Detail
} // namespace Modio

#ifndef MODIO_SEPARATE_COMPILATION
#include "ModioBuffer.ipp"
#endif