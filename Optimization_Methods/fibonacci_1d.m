function [x_min, f_min, calls, iters] = fibonacci_1d(f, a, b, tol, max_iters)
% FIBONACCI Поиск минимума функции одной переменной методом Фибоначчи
%
% Входные параметры:
%   f     - функция для минимизации
%   a, b  - границы интервала поиска (a < b)
%   tol   - желаемая точность
%
% Выходные параметры:
%   x_min - найденная точка минимума
%   f_min - значение функции в точке минимума
%   calls - количество вычислений функции
%   iters - количество итераций

    fib = generate_fib(a, b, tol);
    n = length(fib)-1;

    iters = 0;

    x1 = a + (fib(n-1) / fib(n+1)) * (b-a);
    x2 = a + (fib(n) / fib(n+1)) * (b-a);

    f1 = f(x1);
    f2 = f(x2);
    calls = 2;

    for k = 1:n-1
        if iters > max_iters
            break;
        end
        iters = iters+1;

        % fprintf('Итерация %d: [%.6f, %.6f], длина = %.6f\n', iters, a, b, b - a);

        if f1<f2
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = a + (fib(n-k) / fib(n-k+2)) * (b-a);
            f1 = f(x1);
        else
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + (fib(n-k+1) / fib(n-k+2)) * (b-a);
            f2 = f(x2);
        end
        calls = calls+1;
    end

    x_min = (a+b)/2;
    f_min = f(x_min);
    calls = calls+1;
end


function fib_sequence = generate_fib(a, b, tol)
    L0 = b-a;
    fib_sequence = [0, 1];
    
    if tol < 1e-8
        tol = 1e-8;
    end

    n=1;
    while fib_sequence(end) < L0/tol
        fib_sequence(end+1) = fib_sequence(end) + fib_sequence(end-1);
        n = n+1;
    end
end