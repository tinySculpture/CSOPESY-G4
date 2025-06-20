#pragma once
#include <unordered_map>

class ICommand
{
public:
	enum CommandType
	{
		PRINT,
		DECLARE,
		ADD,
		SUBTRACT,
		SLEEP,
		FOR
	};

	ICommand(int pid, CommandType commandType);
	CommandType getCommandType() const;
	virtual void execute() = 0;

protected:
	int pid;
	CommandType commandType;
};