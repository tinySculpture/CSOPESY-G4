#include "ICommand.h"

ICommand::ICommand(int pid, CommandType commandType) 
	: pid(pid), commandType(commandType) 
{}

ICommand::CommandType ICommand::getCommandType() const {
	return commandType;
}