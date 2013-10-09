#pragma once

#include <unistd.h>
#include <vector>
#include "Parser.h"

namespace Moses
{
class InputPath;
class Word;
class WordsRange;
class PhraseDictionaryMemory;
class PhraseDictionaryNodeMemory;
class ChartCellCollection;

class ActiveChartItem
{
public:
	ActiveChartItem(const WordsRange &range,
					const PhraseDictionaryNodeMemory &node);

	const WordsRange &GetRange() const
	{ return m_range; }

	const PhraseDictionaryNodeMemory &GetNode() const
	{ return m_node; }

protected:
	const WordsRange &m_range;
	const PhraseDictionaryNodeMemory &m_node;
};

class ActiveChart
{
public:
	typedef std::vector<const ActiveChartItem*> Coll;

	ActiveChart()
	{}
	virtual ~ActiveChart();

	void Add(const ActiveChartItem *item)
	{
		m_coll.push_back(item);
	}

	const Coll &GetColl() const
	{ return m_coll; }

protected:
	Coll m_coll;

};

class ParserMemory : public Parser
{
public:
	ParserMemory(const ChartCellCollection &chart
			, const PhraseDictionaryMemory &pt
			, size_t inputSize);

	void Init(const InputPath &path);

	void Extend(const InputPath &path
			, const WordsRange &prevRange);

protected:
	const ChartCellCollection &m_chart;
	const PhraseDictionaryMemory &m_pt;
	std::vector<ActiveChart> m_activeCharts;


	void ExtendNonTerms(const InputPath &path);
	void ExtendNonTermsWithPostFixPath(const InputPath &path);

};

}

