#include "src/FinanceManagerApp/FinanceManagerLayer.hpp"

#include <VelyraAppFramework/Application.hpp>

#include <iostream>

int main(const int argc, const char* argv[]) {
    using namespace FinMgr;

    const App::ProgramArgs args(argv, argv + argc);
    try {
        App::ApplicationDesc desc;
        desc.applicationName = "FinanceManager";
        desc.useImPlot = true;
        desc.settings.enableSave = true;
        desc.settings.settingsFile = getSettingsFilePath();
        App::Application app(desc, args);
        app.createAppLayer<FinanceManagerLayer>();
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
