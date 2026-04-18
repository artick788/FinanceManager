#include "Utils.hpp"

namespace FinMgr {

    OLSLinearRegression performOLSLinearRegression(const std::vector<double>& x, const std::vector<double>& y) {
        OLSLinearRegression result;
        if (x.size() != y.size() || x.empty()) {
            return result; // Return default if sizes don't match or are empty
        }
        double sumX = 0.0;
        double sumY = 0.0;
        double sumXX = 0.0;
        double sumXY = 0.0;
        const Size n = x.size();

        for (Size i = 0; i < n; ++i) {
            sumX += x[i];
            sumY += y[i];
            sumXX += x[i] * x[i];
            sumXY += x[i] * y[i];
        }
        const double denominator = n * sumXX - sumX * sumX;
        if (denominator == 0.0) {
            return result; // Avoid division by zero
        }
        result.slope = (n * sumXY - sumX * sumY) / denominator;
        result.intercept = (sumY - result.slope * sumX) / n;
        // Calculate R-squared
        double ssTotal = 0.0;
        double ssResidual = 0.0;
        const double meanY = sumY / n;
        for (Size i = 0; i < n; ++i) {
            double predictedY = result.slope * x[i] + result.intercept;
            ssTotal += (y[i] - meanY) * (y[i] - meanY);
            ssResidual += (y[i] - predictedY) * (y[i] - predictedY);
        }
        result.rSquared = (ssTotal > 0.0) ? (1.0 - ssResidual / ssTotal) : 0.0;
        return result;
    }

}
