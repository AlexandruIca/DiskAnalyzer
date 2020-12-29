#include "filesystem.hpp"
#include "task_manager.hpp"

#include <iostream>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    try {
        da::tid_t task_id = da::no_task;

        da::task_manager mng{};
        da::fs_tree tree{};

        std::string input{};

        while(std::getline(std::cin, input)) {
            if(input == "q") {
                break;
            }

            if(input[0] == 'l') {
                tree.pretty_print_to(std::cout);
            }
            else if(input[0] == 'i') {
                std::string tmp{ input.begin() + 1, input.end() };
                auto const path = da::to_path(tmp);

                if(!tree.exists(path)) {
                    std::cout << tmp << "  does not exist!" << std::endl;
                }
                else {
                    std::cout << tree.is_already_being_processed(da::to_path(tmp)) << std::endl;
                }
            }
            else {
                auto priority =
                    (input[0] == '1' ? da::task_priority::low
                                     : (input[0] == '2' ? da::task_priority::medium : da::task_priority::high));

                std::string tmp{ input.begin() + 1, input.end() };
                tree.add(da::to_path(tmp));
                mng.add_task(priority, ++task_id);
            }
        }
    }
    catch(std::exception const& e) {
        std::cout << "Exception!!!\n\t" << e.what() << std::endl;
    }
}
