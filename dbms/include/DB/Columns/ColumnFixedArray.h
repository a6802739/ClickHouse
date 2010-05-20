#ifndef DBMS_CORE_COLUMN_FIXED_ARRAY_H
#define DBMS_CORE_COLUMN_FIXED_ARRAY_H

#include <Poco/SharedPtr.h>

#include <DB/Core/Exception.h>
#include <DB/Core/ErrorCodes.h>

#include <DB/Columns/IColumn.h>


namespace DB
{

using Poco::SharedPtr;

/** Cтолбeц значений типа "массив фиксированного размера".
  */
class ColumnFixedArray : public IColumn
{
public:
	/** Создать пустой столбец массивов фиксированного размера n, со типом значений, как в столбце nested_column */
	ColumnVector(SharedPtr<IColumn> nested_column, size_t n_)
		: data(nested_column), n(n_)
	{
		data.clear();
	}
	
	size_t size() const
	{
		return data->size() / n;
	}
	
	Field operator[](size_t index) const
	{
		Array res;
		for (size_t i = n * index; i < n * (index + 1); ++i)
			res[i] = (*data)[n * index + i];
		return res;
	}

	void cut(size_t start, size_t length)
	{
		data->cut(n * start, n * length);
	}

	void insert(const Field & x)
	{
		const Array & array = boost::get<Array &>(x);
		if (n != array.size())
			throw Exception("Size of array doesn't match size of FixedArray column",
				ErrorCodes::SIZE_OF_ARRAY_DOESNT_MATCH_SIZE_OF_FIXEDARRAY_COLUMN);

		for (size_t i = 0; i < n; ++i)
			data->insert(array[i]);
	}

	void insertDefault()
	{
		for (size_t i = 0; i < n; ++i)
			data->insertDefault();
	}

	void clear()
	{
		data.clear();
	}

	/** Более эффективные методы манипуляции */
	IColumn & getData()
	{
		return *data;
	}

	const IColumn & getData() const
	{
		return *data;
	}

private:
	SharedPtr<IColumn> data;
	const size_t n;
};


}

#endif
