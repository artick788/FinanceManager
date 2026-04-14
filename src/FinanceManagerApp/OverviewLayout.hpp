#pragma once

#include "Store.hpp"
#include <VelyraAppFramework/Application.hpp>

namespace FinMgr {

    class OverviewLayout {
    public:
        OverviewLayout(App::AppData& appData, Store& store);

        ~OverviewLayout() = default;

        SP<App::Node> createLayout() const;

        void draw();

    private:
        App::AppData& m_AppData;
        Store& m_Store;
    };
}
