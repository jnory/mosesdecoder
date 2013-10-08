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

	size_t endPos = range.GetEndPos();
	ActiveChart &activeChart = m_activeCharts[endPos];


	// terminal
	const PhraseDictionaryNodeMemory *child = root.GetChild(word);
	if (child) {
		ActiveChartItem *item = new ActiveChartItem(range, *child);
		activeChart.Add(item);
	}

	// non-terminal

}

void ParserMemory::Extend(const Word &word
		, const WordsRange &prevRange
		, const WordsRange &thisRange)
{
	size_t endPos = thisRange.GetEndPos();
	ActiveChart &activeChart = m_activeCharts[endPos];

	// terminal
	for (size_t i = 0; i < activeChart.GetColl().size(); ++i) {
		const ActiveChartItem &item = *activeChart.GetColl()[i];
		const PhraseDictionaryNodeMemory &node = item.GetNode();

		const PhraseDictionaryNodeMemory *child = node.GetChild(word);
		if (child) {
			ActiveChartItem *item = new ActiveChartItem(thisRange, *child);
			activeChart.Add(item);
		}
	}
}

} // namespace

