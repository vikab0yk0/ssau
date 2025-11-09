function [x_min, f_min, calls, iters] = golden_ratio_1d(f, a, b, tol, max_iters)
% GOLDEN_RATIO_1D Поиск минимума функции одной переменной методом золотого сечения
%
% Входные параметры:
%   f   - функция, минимум которой ищем
%   a,b - границы интервала поиска (a < b)
%   tol - точность по значению функции (критерий остановки)
%   max_iters - максимальное количество итераций
%
% Выходные параметры:
%   x_min    - найденная точка минимума
%   f_min    - значение функции в точке минимума
%   calls    - количество вызовов функции
%   iters    - количество итераций

    iters = 0;
    
    r = 0.618;
    x1 = b-r*(b-a);
    x2 = a+r*(b-a);
    
    f1 = f(x1);
    f2 = f(x2);
    calls = 2;
    
    while iters < max_iters
        iters = iters+1;
        
        if f1 < f2
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = b-r*(b-a);
            f1 = f(x1);
        else
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + r*(b - a);
            f2 = f(x2);
        end
        calls = calls+1;

        % fprintf('[%.6f, %.6f], Итерация %d: [%.6f, %.6f], длина = %.6f\n', x1, x2, iters, a, b, b - a);

        if b - a < tol            
            break;
        end
    end
    
    x_min = (a+b)/2;
    f_min = f(x_min);
    calls = calls+1;
end