#include "task_manager.hpp"

#include <chrono>
#include <iostream>
#include <thread>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    try {
        da::task_manager mng{};

        da::tid_t id = da::no_task;
        da::task_priority p = da::task_priority::medium;
        da::task_priority p2 = da::task_priority::high;
        da::task_priority p3 = da::task_priority::low;

        mng.add_task(p, ++id);
        mng.add_task(p, ++id);
        mng.add_task(p, ++id);
        mng.add_task(p, ++id);
        mng.add_task(p, ++id);
        mng.add_task(p, ++id);
        mng.add_task(p, ++id);

        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(500ms);
        }

        mng.stop_task(p, 2);
        mng.pause_task(p, 3);
        mng.resume_task(p, 3);
        mng.add_task(p, 99);
        mng.add_task(p2, 88);
        mng.stop_task(p2, 88);
        mng.add_task(p3, 77);
    }
    catch(std::exception const& e) {
        std::cout << "Exception:\n\t" << e.what() << std::endl;
    }
}
