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
        // Git sync is configured when the settings are created, so it is read from the settings file up front
        desc.settings = createSettingsDesc(readGitSyncConfig(getSettingsFilePath()));

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
