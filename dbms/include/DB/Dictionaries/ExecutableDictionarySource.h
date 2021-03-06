#pragma once

#include <DB/Dictionaries/IDictionarySource.h>
#include <DB/Dictionaries/DictionaryStructure.h>

namespace DB
{


/// Allows loading dictionaries from executable
class ExecutableDictionarySource final : public IDictionarySource
{
public:
	ExecutableDictionarySource(
		const DictionaryStructure & dict_struct_,
		const Poco::Util::AbstractConfiguration & config,
		const std::string & config_prefix,
		Block & sample_block,
		const Context & context
	);

	ExecutableDictionarySource(const ExecutableDictionarySource & other);

	BlockInputStreamPtr loadAll() override;

	BlockInputStreamPtr loadIds(const std::vector<UInt64> & ids) override;

	BlockInputStreamPtr loadKeys(
		const ConstColumnPlainPtrs & key_columns, const std::vector<std::size_t> & requested_rows) override;

	bool isModified() const override;

	bool supportsSelectiveLoad() const override;

	DictionarySourcePtr clone() const override;

	std::string toString() const override;

private:
	Logger * log = &Logger::get("ExecutableDictionarySource");

	const DictionaryStructure dict_struct;
	const std::string command;
	const std::string format;
	Block sample_block;
	const Context & context;
};

void idsToBuffer(const Context & context, const std::string & format, Block & sample_block, WriteBuffer & out_stream,
	const std::vector<UInt64> & ids);
void columnsToBuffer(const Context & context, const std::string & format, Block & sample_block, WriteBuffer & out_buffer,
	const DictionaryStructure & dict_struct, const ConstColumnPlainPtrs & key_columns, const std::vector<std::size_t> & requested_rows);

}
