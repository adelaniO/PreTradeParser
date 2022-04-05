#pragma once

#include <vector>
#include <ostream>

namespace pretrade
{
    enum class Status
    {
        OK = 0,
        ERR
    };

    class ErrorLog
    {
    public:
        void clear();
        bool empty() const { return m_errors.empty(); }
        Status status() const;
        void add(std::string&& message);
        friend std::ostream& operator<< (std::ostream& out, const ErrorLog& errorLog);
        const auto& geterrors() const { return m_errors; }

    private:
        std::vector<std::string> m_errors;
        Status m_status{ Status::OK };
    };
}
