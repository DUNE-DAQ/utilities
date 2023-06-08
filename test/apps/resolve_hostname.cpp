#include "utilities/Resolver.hpp"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Invalid number of arguments:\nresolve hostname\n";
        exit(1);
    }

    auto results = dunedaq::utilities::get_ips_from_hostname(argv[1]);
    for (auto const& r: results)
        std::cout << r << "\n";

    if (results.size() > 1)
        return 0;
    else
        return 1;
}