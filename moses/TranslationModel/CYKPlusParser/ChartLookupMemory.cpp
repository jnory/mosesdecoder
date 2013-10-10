#include "ChartLookupMemory.h"
#include "moses/Util.h"
#include "moses/WordsRange.h"
#include "moses/Word.h"
#include "moses/InputPath.h"
#include "moses/NonTerminal.h"
#include "moses/TranslationModel/PhraseDictionaryMemory.h"
#include "moses/ChartCell.h"
#include "moses/ChartCellCollection.h"

using namespace std;

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

ChartLookupMemory::ChartLookupMemory(const ChartCellCollection &chart
							, const PhraseDictionaryMemory &pt
							, size_t inputSize)
:m_chart(chart)
,m_pt(pt)
,m_activeCharts(inputSize)
{

}

void ChartLookupMemory::Init(const InputPath &path)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();
	const PhraseDictionaryNodeMemory &root = m_pt.GetRootNode();

	size_t endPos = range.GetEndPos();
	ActiveChart &activeChart = m_activeCharts[endPos];


	// just terminal
	const PhraseDictionaryNodeMemory *child = root.GetChild(word);
	if (child) {
		ActiveChartItem *item = new ActiveChartItem(range, *child);
		activeChart.Add(item);
	}
}

void ChartLookupMemory::Extend(const InputPath &path)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();

	const InputPath *prefixPath = path.GetPrefixPath();
	assert(prefixPath);
	const WordsRange &prefixRange = prefixPath->GetWordsRange();

	size_t prevEndPos = prefixRange.GetEndPos();
	const ActiveChart &prevChart = m_activeCharts[prevEndPos];

	size_t endPos = range.GetEndPos();
	ActiveChart &activeChart = m_activeCharts[endPos];

	// terminal
	for (size_t i = 0; i < prevChart.GetColl().size(); ++i) {
		const ActiveChartItem &item = *prevChart.GetColl()[i];
		const PhraseDictionaryNodeMemory &node = item.GetNode();

		const PhraseDictionaryNodeMemory *child = node.GetChild(word);
		if (child) {
			ActiveChartItem *item = new ActiveChartItem(range, *child);
			activeChart.Add(item);
		}
	}

	// non-term
	ExtendNonTerms(path);


}

void ChartLookupMemory::ExtendNonTerms(const InputPath &path)
{
	// lookup non term which covers previous range.
	// It wouldn't have been done the last time round
	const InputPath *prefixPath = path.GetPrefixPath();
	CHECK(prefixPath);
	ExtendNonTermsWithPostFixPath(*prefixPath);

	const std::vector<const InputPath*> &postfixPaths = path.GetPostfixOf();
	for (size_t i = 0; i < postfixPaths.size(); ++i) {
		const InputPath &postfixPath = *postfixPaths[i];
		ExtendNonTermsWithPostFixPath(postfixPath);
	}
}

void ChartLookupMemory::ExtendNonTermsWithPostFixPath(const InputPath &path)
{
	const WordsRange &range = path.GetWordsRange();
	const ChartCell &cell = m_chart.Get(range);
	const ChartCellLabelSet &targetLabels = cell.GetTargetLabelSet();

	// loop thru all non-terms
	const NonTerminalSet &sourceNonTerms = path.GetNonTerminalSet();

	NonTerminalSet::const_iterator iterSourceNonTerms;
	for (iterSourceNonTerms = sourceNonTerms.begin(); iterSourceNonTerms != sourceNonTerms.end(); ++iterSourceNonTerms) {
		const Word &sourceNonTerm = *iterSourceNonTerms;
		cerr << "sourceNonTerm=" << sourceNonTerm << endl;

		ChartCellLabelSet::const_iterator iterTargetLabels;
		for (iterTargetLabels = targetLabels.begin(); iterTargetLabels != targetLabels.end(); ++iterTargetLabels) {
			const Word &targetLabel = iterTargetLabels->first;
			cerr << "non-terms=" << sourceNonTerm << targetLabel << endl;
		}


		// do lookup
	}

}

} // namespace

