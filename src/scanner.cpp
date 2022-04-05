#include <fstream>
#include <string>
#include <array>

#include "scanner.hpp"
#include "errorlog.hpp"
#include <charconv>


namespace pretrade
{
    Scanner::Scanner(const fs::path &source)
        : m_source{source}, m_dest{source}
    {
        m_dest.replace_extension("tsv");
    }

    void Scanner::scanSecurityReferenceData()
    {
        std::ifstream source{m_source};

        if(!source.is_open())
        {
            m_errors.add("Unable to Open File: " + m_source.string());
            return;
        }

        std::string line;
        while(source)
        {
            std::getline(source, line);
            if(line._Starts_with("0 "))
            {
                if(auto type{getIntValue(TokenType::MSG_TYPE, line)})
                {
                    if(type != 8) continue;
                    
                    if (auto id{ getIntValue(TokenType::SECURITY_ID, line) })
                    {
                        Security security;
                        const auto isin{ getStringValue(TokenType::ISIN, line) };
                        if (!isin || isin.value().size() != 12)
                            m_errors.add("Invalid ISIN for Security Id: " + std::to_string(id.value()));
                        else
                            isin->copy(security.isin.data(), 12);

                        const auto currency{ getStringValue(TokenType::CURRENCY, line) };
                        if (!currency || currency.value().size() != 3)
                            m_errors.add("Invalid Currency for Security Id: " + std::to_string(id.value()));
                        else
                            currency->copy(security.currency.data(), 3);
                        const auto& [iter, success] = m_securities.emplace(id.value(), security);
                        if(success)
                            m_errors.add("Duplicate Reference Found for Security Id: " + std::to_string(id.value()));
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    void Scanner::scanSecurityAddRecords()
    {
        std::ifstream source{m_source};

        if(!source.is_open())
        {
            m_errors.add("Unable to Open File: " + m_source.string());
            return;
        }

        std::string line;
        while(source)
        {
            std::getline(source, line);
            if(line._Starts_with("0 "))
            {
                if(auto type{getIntValue(TokenType::MSG_TYPE, line)}; type.value() == 12)
                {
                    if (auto idSearch{ getIntValue(TokenType::SECURITY_ID, line) })
                    {
                        auto search{ m_securities.find(*idSearch) };
                        if(search == m_securities.end())
                            continue;
                        auto& [id, security] = *search;
                        const auto quantity = getIntValue(TokenType::QUANTITY, line);
                        const auto price = getIntValue(TokenType::PRICE, line);
                        const auto side{ getStringValue(TokenType::SIDE, line)};
                        if(!price || !quantity || !side)
                            m_errors.add("Invalid Pricing information for Security Id: " + std::to_string(id));
                        else if(side == "BUY,")
                        {
                            security.countBuy++;
                            security.quantityBuy += *quantity;
                            security.maxBuy = std::max(security.maxBuy, *price);
                            security.averageBuy += (*price) * (*quantity);
                        }
                        else
                        {
                            security.countSell++;
                            security.quantitySell += *quantity;
                            security.minSell = std::min(security.minSell, *price);
                            security.averageSell += (*price) * (*quantity);
                        }
                    }
                }
            }
        }
    }

    void Scanner::printHeader(std::ostream& dest)
    {
        dest << "ISIN\tCurrency\tTotal Buy Count\tTotal Sell Count\tTotal Buy Quantity\tTotal Sell Quantity\tWeighted Average Buy Price\tWeighted Average Sell Price\tMax Buy Price\tMin Sell Price\n";
    }

    void Scanner::finalizeSecurity(Security& security)
    {
        security.averageBuy /= security.quantityBuy ? security.quantityBuy : 1;
        security.averageSell /= security.quantitySell ? security.quantitySell : 1;
    }

    void Scanner::printSecurity(std::ostream& dest, const Security& security)
    {
        dest << std::string(security.isin.begin(), security.isin.end()) << '\t';
        dest << std::string(security.currency.begin(), security.currency.end()) << '\t';
        dest << security.countBuy << '\t';
        dest << security.countSell << '\t';
        dest << security.quantityBuy << '\t';
        dest << security.quantitySell << '\t';
        dest << security.averageBuy << '\t';
        dest << security.averageSell << '\t';
        dest << security.maxBuy << '\t';
        dest << security.minSell << '\n';
    }

    size_t Scanner::size() const
    {
        return m_securities.size();
    }

    const ErrorLog& Scanner::getErrors() const
    {
        return m_errors;
    }

    char const* puctuatorSkipSet { "\",:{}" };
    char const* numberSkipSet { "0123456789" };

    std::optional<int64_t> getIntValue(TokenType token, std::string_view line)
    {
        const auto& key = keywords.at(token);
        auto pos{line.find(key)};
        bool found{};
        size_t start{}, end{};
        for(pos += key.size(); pos < line.size(); ++pos)
        {
            if (line[pos] == ',' || (found && !std::isdigit(line[pos])))
            {
                end = pos;
                break;
            }
            if(!found && std::isdigit(line[pos]))
            {
                found = true;
                start = pos;
            }
        }
        int64_t result;
        auto [ptr, ec] { std::from_chars(line.data() + start, line.data() + end, result) };
        if(ec == std::errc())
            return {result};
        return std::nullopt;
    }

    std::optional<std::string_view> getStringValue(TokenType token, std::string_view line)
    {
        const auto& key{ keywords.at(token) };
        const auto pos{ line.find(key) + key.size() };
        if(pos < line.size())
        {
            const auto start{ line.find_first_not_of(puctuatorSkipSet, pos) };
            const auto end{ start < line.size() ? line.find_first_of('/"', start) : start};
            if(end < line.size())
                return { line.substr(start, end - start) };
        }
        return std::nullopt;
    }
}