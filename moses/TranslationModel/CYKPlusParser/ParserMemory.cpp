#include "ParserMemory.h"
#include "moses/Util.h"
#include "moses/WordsRange.h"
#include "moses/Word.h"
#include "moses/InputPath.h"
#include "moses/NonTerminal.h"
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

void ParserMemory::Init(const InputPath &path)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();
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

void ParserMemory::Extend(const InputPath &path
		, const WordsRange &prevRange)
{
	const Word &word = path.GetLastWord();
	const WordsRange &range = path.GetWordsRange();

	const InputPath *prevPath = path.GetPrevPath();
	assert(prevPath);
	//const WordsRange &prevRange

	size_t prevEndPos = prevRange.GetEndPos();
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
	ExtendNonTermsRecursive(path);


}

void ParserMemory::ExtendNonTermsRecursive(const InputPath &path)
{
	// lookup non term which covers this range, and
	ExtendNonTerms(path);

	const std::vector<const InputPath*> &postfixPaths = path.GetPostfixOf();
	for (size_t i = 0; i < postfixPaths.size(); ++i) {
		const InputPath &postfixPath = *postfixPaths[i];
		ExtendNonTerms(postfixPath);
	}
}

void ParserMemory::ExtendNonTerms(const InputPath &path)
{
	// loop thru all non-terms
	const NonTerminalSet &sourceNonTerms = path.GetNonTerminalSet();

	NonTerminalSet::const_iterator iter;
	for (iter = sourceNonTerms.begin(); iter != sourceNonTerms.end(); ++iter) {
		const Word &sourceNonTerm = *iter;

		// do lookup
	}

}

} // namespace

