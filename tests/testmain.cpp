#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <scanner.hpp>
#include <string>

using namespace pretrade;

TEST_CASE( "Test securityReference.txt")
{
    Scanner scanner("testfiles/securityReference.txt");
    scanner.scanSecurityReferenceData();
    CHECK(scanner.getErrors().empty());
    CHECK(scanner.size() == 2);
    const auto v1 = scanner.pop();
    const auto v2 = scanner.pop();
    REQUIRE(v1.has_value());
    REQUIRE(v2.has_value());
    CHECK(scanner.size() == 0);
    CHECK(v1->first == 4749);
    CHECK(v2->first== 4491);
}

TEST_CASE( "Test secRefWithBuys.txt")
{
    Scanner scanner("testfiles/secRefWithBuys.txt");
    scanner.scanSecurityReferenceData();
    CHECK(scanner.getErrors().empty());
    CHECK(scanner.size() == 1);
    auto v1 = scanner.pop();
    REQUIRE(v1.has_value());
    REQUIRE(v1->first == 4749);
    CHECK(scanner.size() == 0);
    auto& [id, security] = *v1;
    scanner.scanSecurityAddRecord(id, security);
    CHECK(security.countSell == 0);
    CHECK(security.quantitySell == 0);

    CHECK(security.countBuy == 4);
    CHECK(security.quantityBuy == 11);
    CHECK(security.maxBuy == 50000000);

    const auto average = ((1*50000000)+(2*10000000)+(4*20000000)+(4*10000000)) / security.quantityBuy;
    CHECK(security.averageBuy == average);
}