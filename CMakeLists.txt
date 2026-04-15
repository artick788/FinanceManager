cmake_minimum_required(VERSION 3.22)
project(FinanceManager DESCRIPTION "Manage your finances")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

set(VELYRA_APP_FRAMEWORK_PATH)
vl_include_or_fetch(VelyraAppFramework VELYRA_APP_FRAMEWORK_PATH)
message(STATUS "VelyraAppFramework found at ${VELYRA_APP_FRAMEWORK_PATH}")

set(FINANCE_MANAGER_HEADERS
    include/FinanceManager/FinanceManagerInclude.hpp
    include/FinanceManager/Date.hpp
    include/FinanceManager/Wage.hpp
)

set(FINANCE_MANAGER_SRC
    src/FinanceManager/Date.cpp
    src/FinanceManager/Wage.cpp
)

add_library(FinanceManager STATIC ${FINANCE_MANAGER_SRC} ${FINANCE_MANAGER_HEADERS})
vl_target_set_compile_flags(FinanceManager)
target_link_libraries(FinanceManager PUBLIC VelyraUtils)
target_include_directories(FinanceManager PUBLIC include)

set(FINANCE_MANAGER_APP_HEADERS
    src/FinanceManagerApp/FinanceManagerLayer.hpp
    src/FinanceManagerApp/Store.hpp
    src/FinanceManagerApp/ConfigureFinanceLayout.hpp
    src/FinanceManagerApp/OverviewLayout.hpp
    src/FinanceManagerApp/Utils.hpp

    src/FinanceManagerApp/Popups/AddIncomePopup.hpp
    src/FinanceManagerApp/Popups/AddPaycheckPopup.hpp

    src/FinanceManagerApp/Widgets/DateInput.hpp
)

set(FINANCE_MANAGER_APP_SRC
    src/FinanceManagerApp/FinanceManagerLayer.cpp
    src/FinanceManagerApp/Store.cpp
    src/FinanceManagerApp/ConfigureFinanceLayout.cpp
    src/FinanceManagerApp/OverviewLayout.cpp

    src/FinanceManagerApp/Popups/AddIncomePopup.cpp
    src/FinanceManagerApp/Popups/AddPaycheckPopup.cpp

    src/FinanceManagerApp/Widgets/DateInput.cpp
)

add_executable(FinanceManagerApp main.cpp ${FINANCE_MANAGER_APP_SRC} ${FINANCE_MANAGER_APP_HEADERS})
target_link_libraries(FinanceManagerApp PUBLIC FinanceManager VelyraAppFramework)

set(FINANCE_MANAGER_TEST_SRC
    test/TestDate.cpp
)

if (BUILD_TESTING)
    vl_fetch_gtest()

    add_executable(TestFinanceManager ${FINANCE_MANAGER_TEST_SRC})
    target_include_directories(TestFinanceManager PUBLIC include)
    target_link_libraries(TestFinanceManager PUBLIC GTest::gtest GTest::gtest_main FinanceManager)
    add_test(NAME CTestFinanceManager COMMAND ${PROOT} $<TARGET_FILE:TestFinanceManager>)
    set_tests_properties(CTestFinanceManager PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif ()