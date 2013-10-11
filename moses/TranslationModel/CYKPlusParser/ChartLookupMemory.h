#pragma once

#include <unistd.h>
#include <vector>
#include "ChartLookup.h"
#include "moses/InputPath.h"

namespace Moses
{
class InputPath;
class Word;
class WordsRange;
class PhraseDictionaryMemory;
class PhraseDictionaryNodeMemory;
class ChartCellCollection;

class ChartLookupMemory : public ChartLookup
{
public:
	ChartLookupMemory(const ChartCellCollection &chart
			, const PhraseDictionaryMemory &pt
			, size_t inputSize);

	void Init(const InputPath &path);

	void Extend(const InputPath &path);

protected:
	const ChartCellCollection &m_chart;
	const PhraseDictionaryMemory &m_pt;


	void ExtendNonTerms(const InputPath &path);
	void ExtendNonTermsWithPath(const InputPathSegmentation &segmentation);
	void ExtendNonTermsWithPath(const InputPath &path
							, const PhraseDictionaryNodeMemory &prevNode);

};

}

