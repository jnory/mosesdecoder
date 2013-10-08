#include "ParserMemory.h"
#include "moses/Util.h"
#include "moses/WordsRange.h"
#include "moses/Word.h"
#include "moses/TranslationModel/PhraseDictionaryMemory.h"


namespace Moses
{
ActiveChartItem::ActiveChartItem(const WordsRange &range,
					const PhraseDictionaryNodeMemory &node)
:m_range(range)
,m_node(node)
{

}

ActiveChart::~ActiveChart()
{
	RemoveAllInColl(m_coll);
}

ParserMemory::ParserMemory(const PhraseDictionaryMemory &pt, size_t inputSize)
:m_pt(pt)
,m_activeCharts(inputSize)
{

}

void ParserMemory::Init(const Word &word
		, const WordsRange &range)
{
	const PhraseDictionaryNodeMemory &root = m_pt.GetRootNode();

	// terminal
	const PhraseDictionaryNodeMemory *child = root.GetChild(word);
	if (child) {
		ActiveChartItem *item = new ActiveChartItem(range, *child);

		ActiveChart &activeChart = m_activeCharts[range.GetEndPos()];
		activeChart.Add(item);
	}

	// non-terminal

}

void ParserMemory::Extend(const Word &word
		, const WordsRange &prevRange
		, size_t nextPos)
{

}

} // namespace

