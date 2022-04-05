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
    const fs::path destPath{ scanner.getDestPath() };
    std::ofstream dest{ destPath };
    if(dest.is_open())
    {
        scanner.printHeader(dest);
        while (scanner.size())
        {
            auto [id, security] = scanner.pop().value();
            scanner.scanSecurityAddRecord(id, security);
            scanner.printSecurity(dest, security);
        }
    }
    else
    {
        std::cerr << "Unable to open destination file path: " << destPath << '\n';
    }

    return 0;
}
