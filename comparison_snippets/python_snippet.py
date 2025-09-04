# Python: Dynamic typing and closures
def calculate_and_multiplier(arr):
    total = 0

    # Closure captures 'total'
    def add_and_multiply(multiplier):
        nonlocal total
        for num in arr:
            total += num
        return total * multiplier

    return add_and_multiply

numbers = [1, 2, 3, 4, 5]
closure_func = calculate_and_multiplier(numbers)

print("Sum * 2:", closure_func(2))      # Works fine → 30
print("Sum * 'x':", closure_func("x"))  # Repeats 'x' 15 times
