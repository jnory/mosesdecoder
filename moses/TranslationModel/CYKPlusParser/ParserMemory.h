#pragma once

#include <unistd.h>
#include <vector>
#include "Parser.h"

namespace Moses
{

class Word;
class WordsRange;
class PhraseDictionaryMemory;
class PhraseDictionaryNodeMemory;

class ActiveChartItem
{
public:
	ActiveChartItem(const WordsRange &range,
					const PhraseDictionaryNodeMemory &node);
protected:
	const WordsRange &m_range;
	const PhraseDictionaryNodeMemory &m_node;
};

class ActiveChart
{
public:
	ActiveChart()
	{}
	virtual ~ActiveChart();

	void Add(const ActiveChartItem *item)
	{
		m_coll.push_back(item);
	}
protected:
	std::vector<const ActiveChartItem*> m_coll;

};

class ParserMemory : public Parser
{
public:
	ParserMemory(const PhraseDictionaryMemory &pt, size_t inputSize);

	void Init(const Word &word
			, const WordsRange &range);

	void Extend(const Word &word
			, const WordsRange &prevRange
			, size_t nextPos);

protected:
	const PhraseDictionaryMemory &m_pt;

	std::vector<ActiveChart> m_activeCharts;
};

}

