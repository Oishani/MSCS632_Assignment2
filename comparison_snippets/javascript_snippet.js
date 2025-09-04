// JavaScript: Dynamic typing, coercion, and closures
function calculateAndMultiplier(arr) {
    let total = 0;

    // Closure captures 'total'
    return function(multiplier) {
        for (let num of arr) {
            total += num;
        }
        return total * multiplier;
    };
}

let numbers = [1, 2, 3, 4, 5];
let closureFunc = calculateAndMultiplier(numbers);

console.log("Sum * 2:", closureFunc(2));     // Works fine → 30
console.log("Sum * 'x':", closureFunc("x")); // NaN due to coercion
