#include "errorlog.hpp"

namespace pretrade
{
    std::ostream &operator<<(std::ostream &out, const ErrorLog &errorLog)
    {
        for(const auto& errorMsg : errorLog.m_errors)
            out << errorMsg << '\n';
        return out;
    }
    void ErrorLog::clear()
    {
        m_errors.clear();
        m_status = Status::OK;
    }

    Status ErrorLog::status() const
    {
        return m_status;
    }

    void ErrorLog::add(std::string&& errorMsg)
    {
        m_errors.push_back(errorMsg);
        m_status = Status::ERR;
    }
} // namespace parser
