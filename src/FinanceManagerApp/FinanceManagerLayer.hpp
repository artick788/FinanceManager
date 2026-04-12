#pragma once

#include <VelyraAppFramework/Application.hpp>

#include "Store.hpp"

namespace FinMgr {
    
    class FinanceManagerLayer : public App::Layer {
    public:
        explicit FinanceManagerLayer(App::AppData& app_data);

        void onAttach(const UP<Core::Window>& window, const UP<Core::Context>& context) override;

        void onDetach(const UP<Core::Window> &, const UP<Core::Context> &) override;

        void onImGui(const UP<Core::Window>& window, const UP<Core::Context>& context) override;

    private:
        void setLayout() const;

    private:
        Store m_Store;
    };

}
