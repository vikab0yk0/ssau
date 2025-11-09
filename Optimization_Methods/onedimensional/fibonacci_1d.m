function [x_min, f_min, calls, iters] = fibonacci_1d(f, a, b, N)
% FIBONACCI_1D Поиск минимума функции одной переменной методом Фибоначчи
%
%  ---------> НЕ НАИВНЫЙ <---------
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
%   L_n - теоретическая длина интервала неопределенности
%   iters - количество выполненных итераций

    fib_sequence = generate_fib_sequence(N + 1);
    
    iters = 0;
    calls = 0;

    L_1 = b - a;
    eps = L_1 / fib_sequence(N + 1);
    L_n = L_1 / fib_sequence(N + 1) + (fib_sequence(N - 1) / fib_sequence(N + 1)) * eps;

    L2 = fib_sequence(N) * L_n - fib_sequence(N - 2) * eps;
    x1 = b - L2;
    x2 = a + L2;

    f1 = f(x1);
    f2 = f(x2);
    calls = calls + 2;
    
    for k = 1:N - 3
        iters = iters + 1;
        
        if f1 < f2
            b = x2;
            x2 = x1;
            f2 = f1;
            L_i = x2 - a;
            x1 = b - L_i;
            f1 = f(x1);
            calls = calls + 1;
        else
            a = x1;
            x1 = x2;
            f1 = f2;
            L_i = b - x1;
            x2 = a + L_i;
            f2 = f(x2);
            calls = calls + 1;
        end
        
        % fprintf('[%.6f, %.6f], Итерация %d: [%.6f, %.6f], длина = %.6f\n', x1, x2, iters, a, b, b - a);
    end

    if f1 < f2
        b = x2;
    else
        a = x1;
    end

    x_min = (a + b) / 2;
    f_min = f(x_min);
    calls = calls + 1;
    
    % fprintf('\n\nМой в начале Ln: %.6f, Дикий Ln: %.6f\n\n', L_n, L_i);
end

function fib_sequence = generate_fib_sequence(n)
    fib_sequence = zeros(1, n);
    fib_sequence(1) = 0;
    fib_sequence(2) = 1;
    
    for ii = 3:n
        fib_sequence(ii) = fib_sequence(ii-1) + fib_sequence(ii-2);
    end
end
