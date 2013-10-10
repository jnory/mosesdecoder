#pragma once

#include <unistd.h>

namespace Moses
{
class InputPath;

class Parser
{
public:
	virtual void Init(const InputPath &path) = 0;
	virtual void Extend(const InputPath &path) = 0;
};

} // namespace



