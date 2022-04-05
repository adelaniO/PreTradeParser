#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
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
    const auto& v1 = scanner.find(4749);
    const auto& v2 = scanner.find(4491);
    CHECK(v1 != scanner.end());
    CHECK(v2 != scanner.end());
}

TEST_CASE( "Test secRefWithBuys.txt")
{
    Scanner scanner("testfiles/secRefWithBuys.txt");
    scanner.scanSecurityReferenceData();
    CHECK(scanner.getErrors().empty());
    CHECK(scanner.size() == 1);
    auto v1 = scanner.find(4749);
    REQUIRE(v1 != scanner.end());
    REQUIRE(v1->first == 4749);
    auto& [id, security] = *v1;
    scanner.scanSecurityAddRecords();
    CHECK(security.countSell == 0);
    CHECK(security.quantitySell == 0);

    CHECK(security.countBuy == 4);
    CHECK(security.quantityBuy == 11);
    CHECK(security.maxBuy == 50000000);

    const auto average = ((1*50000000)+(2*10000000)+(4*20000000)+(4*10000000)) / security.quantityBuy;
    CHECK(security.averageBuy == average);
}

//TEST_CASE("Benchmark")
//{
//    BENCHMARK("Scan")
//    {
//        Scanner scanner("testfiles/benchmark.txt");
//        scanner.scanSecurityReferenceData();
//        scanner.scanSecurityAddRecords();
//        for (auto& [id, security] : scanner)
//            scanner.finalizeSecurity(security);
//        CHECK(scanner.size() == 1695);
//        return scanner.size();
//    };
//}