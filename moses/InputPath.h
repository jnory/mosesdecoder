#pragma once

#include <map>
#include <iostream>
#include <vector>
#include "Phrase.h"
#include "WordsRange.h"
#include "NonTerminal.h"

namespace Moses
{

class PhraseDictionary;
class TargetPhraseCollection;
class ScoreComponentCollection;
class TargetPhrase;
class InputPath;
struct ScorePair;

class ActiveChartItem
{
public:
	ActiveChartItem(const TargetPhraseCollection *tpColl, const void *ptNode)
	:m_tpColl(tpColl)
	,m_ptNode(ptNode)
	{}

	const TargetPhraseCollection *GetTargetPhraseCollection() const
	{ return m_tpColl; }
	const void *GetPtNode() const
	{ return m_ptNode; }
protected:
	const TargetPhraseCollection *m_tpColl;
	const void *m_ptNode;
};

typedef std::vector<InputPath*> InputPathList;
typedef std::vector<ActiveChartItem> ActiveChart;
typedef std::pair<const InputPath*, const InputPath*> InputPathSegmentation;


/** Each node contains
1. substring used to searching the phrase table
2. the source range it covers
3. a list of InputPath that it is a prefix of
This is for both sentence input, and confusion network/lattices
*/
class InputPath
{
  friend std::ostream& operator<<(std::ostream& out, const InputPath &obj);

protected:
  const InputPath *m_prefixPath;
  Phrase m_phrase;
  WordsRange m_range;
  const ScorePair *m_inputScore;
  size_t m_nextNode; // distance to next node. For lattices

  // for phrase-based model only
  std::map<const PhraseDictionary*, ActiveChartItem*> m_targetPhrases;

  // for syntax model only
  // phrases which fits this source path, but are the acutal source side of the rule
  // can contain non-terms & terms
  // used but placeholder code
  mutable std::vector<std::vector<const Word*> > m_ruleSourceFromInputPath;
  const NonTerminalSet m_sourceNonTerms;
  std::vector<InputPathSegmentation> m_postfixOf;

  mutable std::map<const PhraseDictionary*, ActiveChart> m_activeChart;

public:
  explicit InputPath()
    : m_prefixPath(NULL)
    , m_range(NOT_FOUND, NOT_FOUND)
    , m_inputScore(NULL)
    , m_nextNode(NOT_FOUND)
  {}

  InputPath(const Phrase &phrase, const NonTerminalSet &sourceNonTerms, const WordsRange &range, const InputPath *prevNode
            ,const ScorePair *inputScore);
  ~InputPath();

  const Phrase &GetPhrase() const {
    return m_phrase;
  }
  const NonTerminalSet &GetNonTerminalSet() const {
    return m_sourceNonTerms;
  }
  const WordsRange &GetWordsRange() const {
    return m_range;
  }
  const Word &GetLastWord() const;

  const InputPath *GetPrefixPath() const {
    return m_prefixPath;
  }

  //! distance to next node in input lattice. For sentences and confusion networks, this should be 1 (default)
  size_t GetNextNode() const {
    return m_nextNode;
  }

  void SetNextNode(size_t nextNode) {
    m_nextNode = nextNode;
  }

  void SetTargetPhrases(const PhraseDictionary &phraseDictionary
                        , const TargetPhraseCollection *targetPhrases
                        , const void *ptNode);

  void SetTargetPhrasesChart(const PhraseDictionary &phraseDictionary
                        , const TargetPhraseCollection *targetPhrases
                        , const void *ptNode) const;

  const TargetPhraseCollection *GetTargetPhrases(const PhraseDictionary &phraseDictionary) const;

  const ActiveChart *GetActiveChart(const PhraseDictionary &phraseDictionary) const;

  // pointer to internal node in phrase-table. Since this is implementation dependent, this is a void*
  const void *GetPtNode(const PhraseDictionary &phraseDictionary) const;
  const ScorePair *GetInputScore() const {
    return m_inputScore;
  }

  std::vector<const Word*> &AddRuleSourceFromInputPath() const {
    m_ruleSourceFromInputPath.push_back(std::vector<const Word*>());
    return m_ruleSourceFromInputPath.back();
  }

  void AddPostfixOf(const InputPath *begin, const InputPath *end)
  {
	  m_postfixOf.push_back(InputPathSegmentation(begin, end));
  }

  const std::vector<InputPathSegmentation> &GetPostfixOf() const
  { return m_postfixOf; }

};

};

