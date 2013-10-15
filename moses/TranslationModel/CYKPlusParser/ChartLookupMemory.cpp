#include "ChartLookupMemory.h"
#include "moses/Util.h"
#include "moses/WordsRange.h"
#include "moses/Word.h"
#include "moses/InputPath.h"
#include "moses/NonTerminal.h"
#include "moses/TranslationModel/PhraseDictionaryMemory.h"
#include "moses/ChartCell.h"
#include "moses/ChartCellCollection.h"
#include "moses/ChartParserCallback.h"

using namespace std;

namespace Moses
{
ChartLookupMemory::ChartLookupMemory(const ChartCellCollection &chart
							, const PhraseDictionaryMemory &pt
							, size_t inputSize)
:m_chart(chart)
,m_pt(pt)
{

}

void ChartLookupMemory::Init(const InputPath &path, ChartParserCallback &to)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();
	const PhraseDictionaryNodeMemory &root = m_pt.GetRootNode();

	size_t endPos = range.GetEndPos();

	// just terminal
	const PhraseDictionaryNodeMemory *child = root.GetChild(word);
	if (child) {
		const TargetPhraseCollection &tpColl = child->GetTargetPhraseCollection();
		path.SetTargetPhrasesChart(m_pt, &tpColl, child);
		to.Add(tpColl, m_stackVec, range);
	}
}

void ChartLookupMemory::Extend(const InputPath &path, ChartParserCallback &to)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();
	cerr << "range=" << range << endl;

	const InputPath *prefixPath = path.GetPrefixPath();
	assert(prefixPath);
	cerr << "prefixPath=" << *prefixPath << endl;

	// lookup non term which covers previous range.
	// It wouldn't have been done the last time round
	ExtendNonTermsWithPath(*prefixPath, m_pt.GetRootNode(), to);

	const WordsRange &prefixRange = prefixPath->GetWordsRange();

	size_t prevEndPos = prefixRange.GetEndPos();
	size_t endPos = range.GetEndPos();

	// terminal
	const ActiveChart *prevActiveChart = prefixPath->GetActiveChart(m_pt);
	if (prevActiveChart) {
		for (size_t i = 0; i < prevActiveChart->size(); ++i) {
			const ActiveChartItem &item = (*prevActiveChart)[i];
			const PhraseDictionaryNodeMemory *node = (const PhraseDictionaryNodeMemory*) item.GetPtNode();

			if (node) {
				const PhraseDictionaryNodeMemory *child = node->GetChild(word);
				if (child) {
					const TargetPhraseCollection &tpColl = child->GetTargetPhraseCollection();
					path.SetTargetPhrasesChart(m_pt, &tpColl, child);
					to.Add(tpColl, m_stackVec, range);
				}
			}
		}
	}

	// non-term
	ExtendNonTerms(path, to);


}

void ChartLookupMemory::ExtendNonTerms(const InputPath &path, ChartParserCallback &to)
{
	const std::vector<InputPathSegmentation> &segmentations = path.GetPostfixOf();
	for (size_t i = 0; i < segmentations.size(); ++i) {
		// lookup thru all possible division of the path.
		// eg. if path=[3-5], than loop thru [3-3][4-5], [3-4][5-5]
		const InputPathSegmentation &segmentation = segmentations[i];
		ExtendNonTermsWithPath(segmentation, to);
	}
}

void ChartLookupMemory::ExtendNonTermsWithPath(const InputPathSegmentation &segmentation, ChartParserCallback &to)
{
	// lookup all non-terms that start somewhere in the middle of the path
	const InputPath *beginPath = segmentation.first;
	const InputPath *endPath = segmentation.second;
	cerr << "beginPath=" << *beginPath << " endPath=" << *endPath << endl;

	const ActiveChart *activeChart = beginPath->GetActiveChart(m_pt);

	if (activeChart) {
		for (size_t i = 0; i < activeChart->size(); ++i) {
			// loop thru each item in active chart from the 1st half of the path
			const ActiveChartItem &item = (*activeChart)[i];
			const PhraseDictionaryNodeMemory *prevNode
					= (const PhraseDictionaryNodeMemory *) item.GetPtNode();
			if (prevNode) {
				ExtendNonTermsWithPath(*endPath, *prevNode, to);
			}
		}
	}
}

void ChartLookupMemory::ExtendNonTermsWithPath(const InputPath &path,
						const PhraseDictionaryNodeMemory &prevNode,
						ChartParserCallback &to)
{
	// lookup all non-terms that can space this path
	const WordsRange &range = path.GetWordsRange();
	const ChartCell &cell = m_chart.Get(range);
	const ChartCellLabelSet &targetLabels = cell.GetTargetLabelSet();

	// loop thru all non-terms
	const NonTerminalSet &sourceNonTerms = path.GetNonTerminalSet();

	NonTerminalSet::const_iterator iterSourceNonTerms;
	for (iterSourceNonTerms = sourceNonTerms.begin(); iterSourceNonTerms != sourceNonTerms.end(); ++iterSourceNonTerms) {
		const Word &sourceNonTerm = *iterSourceNonTerms;

		ChartCellLabelSet::const_iterator iterTargetLabels;
		for (iterTargetLabels = targetLabels.begin(); iterTargetLabels != targetLabels.end(); ++iterTargetLabels) {
			const Word &targetLabel = iterTargetLabels->first;

			const PhraseDictionaryNodeMemory *node = prevNode.GetChild(sourceNonTerm, targetLabel);
			if (node) {
				cerr << "range=" << range << " non-terms=" << sourceNonTerm << targetLabel << endl;

				const TargetPhraseCollection &tpColl = node->GetTargetPhraseCollection();
				path.SetTargetPhrasesChart(m_pt, &tpColl, node);
				to.Add(tpColl, m_stackVec, range);
			}
		}
	}
}


} // namespace

