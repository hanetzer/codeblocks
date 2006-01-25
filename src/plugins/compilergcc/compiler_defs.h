#ifndef COMPILER_DEFS_H
#define COMPILER_DEFS_H

#include <wx/string.h>

class cbProject;
class ProjectBuildTarget;

struct CompilerCommand
{
    CompilerCommand(const wxString& cmd, const wxString& msg, cbProject* prj, ProjectBuildTarget* tgt, bool is_run = false)
        : command(cmd), message(msg), project(prj), target(tgt), isRun(is_run), mustWait(false)
    {}
    CompilerCommand(const CompilerCommand& rhs)
        : command(rhs.command), message(rhs.message), project(rhs.project), target(rhs.target), isRun(rhs.isRun), mustWait(rhs.mustWait)
    {}
    wxString command;
    wxString message;
    wxString dir;
    cbProject* project;
    ProjectBuildTarget* target;
    bool isRun; ///< if it's a command to run the target.
    bool mustWait; ///< wait for all previous commands to finish (for parallel builds).
};
WX_DECLARE_LIST(CompilerCommand, CompilerCommands);

class CompilerQueue
{
    public:
        CompilerQueue();
        ~CompilerQueue();

        /// Clear the queue.
        void Clear();
        /// Get the commands count.
        size_t GetCount() const;
        bool LastCommandWasRun() const;
        /// Queue a command.
        void Add(CompilerCommand* cmd);
        /// Queue all commands from another CompilerQueue.
        void Add(CompilerQueue* queue);
        /** Get the next command in queue.
          * If @c from is NULL, returns the top command.
          * If @c from is not NULL, the command following @c from is
          * returned.
          * if @c remove is true (default), the command is removed from the list.
          * This means that the caller must delete it.
          */
        CompilerCommand* Next();
        CompilerCommand* Peek();
    protected:
        CompilerCommands m_Commands;
        bool m_LastWasRun;
};

#endif // COMPILER_DEFS_H
