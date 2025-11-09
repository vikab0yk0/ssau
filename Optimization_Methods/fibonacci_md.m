function [x_min, f_min, calls, iters] = fibonacci_md(f, x1, x2, N)
% FIBONACCI_MD Поиск минимума многомерной функции методом Фибоначчи вдоль направления
%
% Входные параметры:
%   f        - многомерная функция, минимум которой ищем (принимает вектор)
%   x1, x2   - две точки, задающие направление поиска (векторы)
%   N - количество шагов алгоритма
%
% Выходные параметры:
%   x_min - найденная точка минимума
%   f_min - значение функции в точке минимума
%   calls - количество вычислений функции
%   iters - количество выполненных итераций

    fib_sequence = generate_fib_sequence(N + 1);

    iters = 0;
    calls = 0;
    
    direction = x2 - x1;
    L_total = norm(direction);
    
    direction = direction / L_total;
    
    eps = L_total / fib_sequence(N + 1);
    L_n = L_total / fib_sequence(N + 1) + (fib_sequence(N - 1) / fib_sequence(N + 1)) * eps;
    
    L2 = fib_sequence(N) * L_n - fib_sequence(N - 2) * eps;
    x_left = x2 - L2 * direction;
    x_right = x1 + L2 * direction;
    
    f_left = f(x_left);
    f_right = f(x_right);
    calls = calls + 2;
    
    x_start = x1;
    x_end = x2;
    
    for k = 1:N - 3
        iters = iters + 1;
        
        if f_left < f_right
            x_end = x_right;
            x_right = x_left;
            f_right = f_left;
            
            L_i = norm(x_right - x_start);
            x_left = x_end - L_i * direction;
            f_left = f(x_left);
            calls = calls + 1;
        else
            x_start = x_left;
            x_left = x_right;
            f_left = f_right;
            
            L_i = norm(x_end - x_left);
            x_right = x_start + L_i * direction;
            f_right = f(x_right);
            calls = calls + 1;
        end

        % fprintf('x1 = [');
        % fprintf(' %.5f ', x_start);
        % fprintf('] \t x2 = [');
        % fprintf(' %.5f ', x_end);
        % fprintf(']\n');
    end

    if f_left < f_right
        x_end = x_right;
    else
        x_start = x_left;
    end
    
    x_min = (x_start + x_end) / 2;
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