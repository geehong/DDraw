#ifndef COMMAND_H
#define COMMAND_H

class ICommand
{
public:
    virtual ~ICommand() {}
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Redo() { Execute(); }
};

#endif // COMMAND_H
