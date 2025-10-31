function [x_min, f_min, calls, iters] = golden_ratio_md(f, x1, x2, eps, max_iters)
% GOLDEN_RATIO_MD Поиск минимума многомерной функции методом золотого сечения вдоль направления
%
% Входные параметры:
%   f        - многомерная функция, минимум которой ищем (принимает вектор)
%   x1, x2   - две точки, задающие направление поиска (векторы)
%   eps      - точность поиска
%   max_iters - максимальное количество итераций
%
% Выходные параметры:
%   x_min    - найденная точка минимума
%   f_min    - значение функции в точке минимума
%   calls    - количество вызовов функции
%   iters    - количество итераций

    iters = 0;
    calls = 0;

    direction = x2 - x1;
    L_total = norm(direction);
    direction = direction / L_total;
    
    r = 0.618;
    
    x_left = x2 - L_total * r * direction;
    x_right = x1 + L_total * r * direction;
    
    f_left = f(x_left);
    f_right = f(x_right);
    calls = calls + 2;

    x_start = x1;
    x_end = x2;

    while iters < max_iters
        iters = iters + 1;
        
        if f_left > f_right
            x_start = x_left;
            x_left = x_right;
            f_left = f_right;
            
            L_i = norm(x_end - x_start);
            x_right = x_start + L_i * r * direction;
            f_right = f(x_right);
            calls = calls + 1;
        else
            x_end = x_right;
            x_right = x_left;
            f_right = f_left;
            
            L_i = norm(x_end - x_start);
            x_left = x_end - L_i * r * direction;
            f_left = f(x_left);
            calls = calls + 1;
        end

        if norm(x_end - x_start) <= eps
            break;
        end
    end

    x_min = (x_start + x_end) / 2;
    f_min = f(x_min);
    calls = calls + 1;
end