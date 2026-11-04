#pragma once

#include <VelyraUtils/Types/Types.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

#include <algorithm>
#include <vector>

namespace FinMgr {

    using namespace Velyra;

    /**
     * @brief Erases the element the pointer refers to from the vector.
     * @return false if the pointer does not refer to an element of the vector
     */
    template<typename T>
    bool eraseElement(std::vector<T>& elements, const T* element) {
        const auto it = std::ranges::find_if(elements, [element](const T& e) { return &e == element; });
        if (it == elements.end()) {
            return false;
        }
        elements.erase(it);
        return true;
    }

}
