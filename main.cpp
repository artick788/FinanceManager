#include "src/FinanceManagerApp/FinanceManagerLayer.hpp"
#include <iostream>

int main(const int argc, const char* argv[]) {
    using namespace FinMgr;

    const App::ProgramArgs args(argv, argv + argc);
    try {
        App::ApplicationDesc desc;
        desc.applicationName = "FinanceManager";
        App::Application app(desc, args);
        app.pushLayer<FinanceManagerLayer>();
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
