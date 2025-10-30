function [x_min, f_min, calls, iters] = fibonacci_1d_naive(f, a, b, N)
% FIBONACCI_1D_NAIVE Поиск минимума функции одной переменной методом Фибоначчи
%
% Входные параметры:
%   f - функция для минимизации
%   a, b - границы интервала поиска (a < b)
%   N - количество шагов алгоритма
%
% Выходные параметры:
%   x_min - найденная точка минимума
%   f_min - значение функции в точке минимума
%   calls - количество вычислений функции
%   iters - количество выполненных итераций

    fib = generate_fib_sequence(N+2);
    
    iters = 0;
    calls = 0;

    x1 = a + (fib(N) / fib(N + 2)) * (b - a);
    x2 = a + (fib(N + 1) / fib(N + 2)) * (b - a);
    
    f1 = f(x1);
    f2 = f(x2);
    calls = calls + 2;

    for k = 1:N-3
        iters = iters + 1;
        
        if f1 < f2
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = a + (fib(N - k + 1) / fib(N - k + 3)) * (b - a);
            f1 = f(x1);
            calls = calls + 1;
        else
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + (fib(N - k + 2) / fib(N - k + 3)) * (b - a);
            f2 = f(x2);
            calls = calls + 1;
        end

        % fprintf('[%.6f, %.6f], Итерация %d: [%.6f, %.6f], длина = %.6f\n', x1, x2, iters, a, b, b - a);

    end

    x_min = (a + b) / 2;
    f_min = f(x_min);
    calls = calls + 1;
end


function fib_sequence = generate_fib_sequence(n)
    fib_sequence = zeros(1, n);
    fib_sequence(1) = 0;
    fib_sequence(2) = 1;
    
    for ii = 3:n
        fib_sequence(ii) = fib_sequence(ii-1) + fib_sequence(ii-2);
    end
end