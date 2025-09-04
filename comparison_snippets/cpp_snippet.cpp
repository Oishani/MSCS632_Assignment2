// C++: Static typing, block scope, and closures
#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

auto calculateAndMultiplier(const vector<int>& arr) {
    int total = 0;

    // Lambda closure capturing 'total' by reference.
    // Uses accumulate so the sum is deterministic and easy to verify.
    auto addAndMultiply = [&](int multiplier) {
        total = accumulate(arr.begin(), arr.end(), 0);
        return total * multiplier;
    };

    return addAndMultiply;
}

int main() {
    vector<int> numbers = {1, 2, 3, 4, 5};
    auto closureFunc = calculateAndMultiplier(numbers);

    cout << "DEBUG sum: " << accumulate(numbers.begin(), numbers.end(), 0) << "\n"; // 15
    cout << "Sum * 2: " << closureFunc(2) << endl; // 30
    // cout << "Sum * 'x': " << closureFunc("x") << endl; // Compile-time error: no viable conversion
    return 0;
}
