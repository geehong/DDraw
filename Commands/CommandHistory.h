#ifndef COMMAND_HISTORY_H
#define COMMAND_HISTORY_H

#include "ICommand.h"
#include <vector>
#include <memory>
#include <cstddef>

class CommandHistory
{
private:
    std::vector<std::unique_ptr<ICommand>> m_history;
    size_t m_currentIndex;  // Stack current index
    size_t m_savedIndex;     // Saved position index
    bool m_bSavedIndexValid; // Stack truncation flag
    size_t m_maxCapacity;

public:
    CommandHistory(size_t maxCapacity = 100)
        : m_currentIndex(0), m_savedIndex(0), m_bSavedIndexValid(true), m_maxCapacity(maxCapacity) {}

    ~CommandHistory() {}

    void ExecuteCommand(std::unique_ptr<ICommand> cmd)
    {
        if (!cmd) return;

        if (m_currentIndex < m_history.size())
        {
            m_history.erase(m_history.begin() + m_currentIndex, m_history.end());
            if (m_savedIndex > m_currentIndex)
            {
                m_bSavedIndexValid = false;
            }
        }

        cmd->Execute();
        m_history.push_back(std::move(cmd));
        m_currentIndex++;

        if (m_history.size() > m_maxCapacity)
        {
            m_history.erase(m_history.begin());
            m_currentIndex--;

            if (m_savedIndex == 0)
            {
                m_bSavedIndexValid = false;
            }
            else
            {
                m_savedIndex--;
            }
        }
    }

    bool CanUndo() const { return m_currentIndex > 0; }
    bool CanRedo() const { return m_currentIndex < m_history.size(); }

    void Undo()
    {
        if (CanUndo())
        {
            m_currentIndex--;
            m_history[m_currentIndex]->Undo();
        }
    }

    void Redo()
    {
        if (CanRedo())
        {
            m_history[m_currentIndex]->Redo();
            m_currentIndex++;
        }
    }

    void Reset()
    {
        m_history.clear();
        m_currentIndex = 0;
        m_savedIndex = 0;
        m_bSavedIndexValid = true;
    }

    void MarkSaved()
    {
        m_savedIndex = m_currentIndex;
        m_bSavedIndexValid = true;
    }

    bool IsDirty() const
    {
        if (!m_bSavedIndexValid) return true;
        return m_currentIndex != m_savedIndex;
    }

    size_t GetCurrentIndex() const { return m_currentIndex; }
    size_t GetHistorySize() const { return m_history.size(); }
};

#endif // COMMAND_HISTORY_H
