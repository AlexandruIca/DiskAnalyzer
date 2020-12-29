#include "filesystem.hpp"

#include <iostream>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    try {
        da::fs_tree tree{};

        tree.add(da::to_path("/user/abcxyz/devel"));
        tree.add(da::to_path("/user/abcxyz/devel/"));
        tree.add(da::to_path("/user/abcxyz/devel/A/B/C"));
        tree.add(da::to_path("/user/D/E/F"));
        tree.add(da::to_path("/X/Y/Z"));
        tree.add(da::to_path("X/Y/Z"));

        tree.pretty_print_to(std::cout);

        std::cout << std::endl;

        std::cout << tree.at(da::to_path("/user/abcxyz")).task_id << std::endl;
    }
    catch(std::exception const& e) {
        std::cout << e.what() << std::endl;
    }
}

