#pragma once

namespace Modio
{
	/// <summary>
	/// Declared with List<std::vector, std::string>
	/// </summary>
	/// <typeparam name="ContainerType"></typeparam>
	/// <typeparam name="ValueType"></typeparam>
	template<template<typename> typename ContainerType, typename ValueType>
	class List
	{
	protected:
		using ListType = ContainerType<ValueType>;
		ListType InternalList;

	public:
		using iterator = typename ListType::iterator;
		using const_iterator = typename ListType::const_iterator;

		/// <summary>
		/// Fastest lookup of a index, will crash if the index is out of range
		/// </summary>
		/// <param name="Index"></param>
		/// <returns></returns>
		const ValueType& operator[](int Index) const
		{
			assert(Index >= 0);
			assert(Index < InternalList.size());

			return InternalList[Index];
		}

		/// <summary>
		/// Fastest lookup of a index, will crash if the index is out of range
		/// </summary>
		/// <param name="Index"></param>
		/// <returns></returns>
		ValueType& operator[](int Index)
		{
			assert(Index >= 0);
			assert(Index < InternalList.size());

			return InternalList[Index];
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="Index"></param>
		/// <returns>a nullptr if the value is out of range, else the element of the kind in the list</returns>
		const ValueType* At(int Index) const
		{
			if (Index < 0 && Index >= InternalList.size())
			{
				return &InternalList[Index];
			}
			return nullptr;
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="Index"></param>
		/// <returns>a nullptr if the value is out of range, else the element of the kind in the list</returns>
		ValueType* At(int Index)
		{
			if (Index < 0 && Index >= InternalList.size())
			{
				return &InternalList[Index];
			}
			return nullptr;
		}

		const ListType& GetRawList() const
		{
			return InternalList;
		}

		ListType& GetRawList()
		{
			return InternalList;
		}

		std::size_t Size() const
		{
			return InternalList.size();
		}

		iterator begin()
		{
			return InternalList.begin();
		}

		const_iterator begin() const
		{
			return InternalList.begin();
		}

		iterator end()
		{
			return InternalList.end();
		}

		const_iterator end() const
		{
			return InternalList.end();
		}
	};
} // namespace Modio
