#include <iostream>
#include <filesystem>
#include <fstream>

#include "scanner.hpp"

namespace fs = std::filesystem;
using namespace pretrade;
int main(int argc, char** argv)
{
    const auto programPath{ fs::path(argv[0])};
    if(argc < 2)
    {
        std::cerr << "Usage: " << programPath.filename() << " <exchange-file>\n";
        return 1;
    }
    const auto filePath{ fs::path(argv[1]) };
    std::cout << "Parsing: " << filePath.filename();
    Scanner scanner(filePath);
    scanner.scanSecurityReferenceData();
    scanner.scanSecurityAddRecords();
    const fs::path destPath{ scanner.getDestPath() };
    std::ofstream dest{ destPath };
    if(dest.is_open())
    {
        scanner.printHeader(dest);
        for (auto& [id, security] : scanner)
        {
            scanner.finalizeSecurity(security);
            scanner.printSecurity(dest, security);
        }
    }
    else
    {
        std::cerr << "Unable to open destination file path: " << destPath << '\n';
    }

    for(const auto& err : scanner.getErrors().geterrors())
    {
        std::cerr << err << '\n';
    }

    return 0;
}
