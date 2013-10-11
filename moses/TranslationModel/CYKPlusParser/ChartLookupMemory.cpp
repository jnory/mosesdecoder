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
ChartLookupMemory::ChartLookupMemory(const ChartCellCollection &chart
							, const PhraseDictionaryMemory &pt
							, size_t inputSize)
:m_chart(chart)
,m_pt(pt)
{

}

void ChartLookupMemory::Init(const InputPath &path)
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
	}
}

void ChartLookupMemory::Extend(const InputPath &path)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();

	const InputPath *prefixPath = path.GetPrefixPath();
	assert(prefixPath);

	// lookup non term which covers previous range.
	// It wouldn't have been done the last time round
	ExtendNonTermsWithPath(*prefixPath);

	const WordsRange &prefixRange = prefixPath->GetWordsRange();

	size_t prevEndPos = prefixRange.GetEndPos();
	size_t endPos = range.GetEndPos();

	// terminal
	const ActiveChart *prevActiveChart = prefixPath->GetActiveChart(m_pt);
	if (prevActiveChart) {
		for (size_t i = 0; i < prevActiveChart->size(); ++i) {
			const ActiveChartItem &item = (*prevActiveChart)[i];
			const PhraseDictionaryNodeMemory *node = (const PhraseDictionaryNodeMemory*) item.second;

			if (node) {
				const PhraseDictionaryNodeMemory *child = node->GetChild(word);
				if (child) {
					const TargetPhraseCollection &tpColl = child->GetTargetPhraseCollection();
					path.SetTargetPhrasesChart(m_pt, &tpColl, child);
				}
			}
		}
	}

	// non-term
	ExtendNonTerms(path);


}

void ChartLookupMemory::ExtendNonTerms(const InputPath &path)
{
	const std::vector<const InputPath*> &postfixPaths = path.GetPostfixOf();
	for (size_t i = 0; i < postfixPaths.size(); ++i) {
		const InputPath &postfixPath = *postfixPaths[i];
		ExtendNonTermsWithPath(postfixPath);
	}
}

void ChartLookupMemory::ExtendNonTermsWithPath(const InputPath &path)
{
	const InputPath *prefixPath = path.GetPrefixPath();
	if (prefixPath) {
		// not the 1st path. There must be an entry for this
		const ActiveChart *activeChart = prefixPath->GetActiveChart(m_pt);

		if (activeChart) {
			for (size_t i = 0; i < activeChart->size(); ++i) {
				const ActiveChartItem &item = (*activeChart)[i];
				const PhraseDictionaryNodeMemory *prevNode
						= (const PhraseDictionaryNodeMemory *) item.second;
				if (prevNode) {
					ExtendNonTermsWithPath(path, *prevNode);
				}
			}
		}
	}
}

void ChartLookupMemory::ExtendNonTermsWithPath(const InputPath &path
						, const PhraseDictionaryNodeMemory &prevNode)
{
	const WordsRange &range = path.GetWordsRange();
	cerr << "range=" << range << endl;

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

			const PhraseDictionaryNodeMemory *node = prevNode.GetChild(sourceNonTerm, targetLabel);
			if (node) {
				const TargetPhraseCollection *tpColl = &node->GetTargetPhraseCollection();
				path.SetTargetPhrasesChart(m_pt, tpColl, node);
			}
		}

		// do lookup
	}

}


} // namespace

