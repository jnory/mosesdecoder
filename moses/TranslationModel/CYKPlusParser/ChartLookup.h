#pragma once

#include <unistd.h>
#include "moses/StackVec.h"

namespace Moses
{
class InputPath;
class ChartParserCallback;

class ChartLookup
{
public:
	virtual void Init(const InputPath &path, ChartParserCallback &to) = 0;
	virtual void Extend(const InputPath &path, ChartParserCallback &to) = 0;

protected:
	StackVec m_stackVec;
};

} // namespace



