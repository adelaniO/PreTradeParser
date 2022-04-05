#pragma once

#include "errorlog.hpp"

#include <queue>
#include <variant>
#include <filesystem>
#include <optional>
#include <map>
#include <unordered_map>
#include <array>

namespace pretrade
{
    namespace fs = std::filesystem;

    enum class TokenType
    {
        // Keywords
        ISIN = 0, CURRENCY, MSG_TYPE, SECURITY_ID,
        SIDE, QUANTITY, PRICE
    };

    struct Token
    {
        TokenType type;
        int numValue;
        std::array<char, 12> stringValue;
    };

    const std::unordered_map<TokenType, std::string> keywords{
        { TokenType::MSG_TYPE   ,  "msgType_"   },
        { TokenType::SECURITY_ID,  "securityId_"},
        { TokenType::ISIN       ,  "isin_"      },
        { TokenType::CURRENCY   ,  "currency_"  },
        { TokenType::SIDE       ,  "side_"      },
        { TokenType::QUANTITY   ,  "quantity_"  },
        { TokenType::PRICE      ,  "price_"     },
    };

    struct Security
    {
        std::array<char, 12> isin;
        std::array<char, 3> currency;
        int64_t countBuy{}, countSell{};
        int64_t quantityBuy{}, quantitySell{};
        int64_t averageBuy{}, averageSell{};
        int64_t maxBuy{}, minSell{};
    };

    std::optional<int64_t> getIntValue(TokenType token, std::string_view line);
    std::optional<std::string_view> getStringValue(TokenType token, std::string_view line);

    class Scanner
    {
    public:
        Scanner(const fs::path& source);
        void scanSecurityReferenceData();
        void scanSecurityAddRecords();
        void printHeader(std::ostream& dest);
        void finalizeSecurity(Security& security); // For testing
        void printSecurity(std::ostream& dest, const Security& security);
        const ErrorLog& getErrors() const;
        size_t size() const;
        const fs::path& getDestPath() { return m_dest; }

        // convenience functions
        auto find(int64_t id) { return m_securities.find(id); }
        const auto find(int64_t id) const { return m_securities.find(id); }
        auto begin() { return m_securities.begin(); }
        const auto begin() const { return m_securities.cbegin(); }
        auto end() { return m_securities.end(); }
        const auto end() const { return m_securities.cend(); }
    private:
        int m_msgCount{};
        fs::path m_source, m_dest;
        std::unordered_map<int64_t, Security> m_securities;
        ErrorLog m_errors;
    };

}