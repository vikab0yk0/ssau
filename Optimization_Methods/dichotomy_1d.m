function [x_min, f_min, calls, iters] = dichotomy_1d(f, a, b, eps, tol, max_iters)
% DICHOTOMY_1D Поиск минимума функции одной переменной методом дихотомии
%
% Входные параметры:
%   f   - функция, минимум которой ищем
%   a,b - границы интервала поиска (a < b)
%   eps - половина длины конечного интервала (точность по аргументу)
%   tol - точность по значению функции (критерий остановки)
%
% Выходные параметры:
%   x_min    - найденная точка минимума
%   f_min    - значение функции в точке минимума
%   calls    - количество вызовов функции
%   iters    - количество итераций

    iters = 0;
    calls = 0;

    while iters < max_iters
        iters = iters+1;
        
        mid = (a+b)/2;
        x1=mid-eps;
        x2=mid+eps;

        % fprintf('Итерация %d: [%.6f, %.6f], длина = %.6f\n', iters, a, b, b - a);

        f1 = f(x1);
        f2 = f(x2);

        if f1<f2
            b=x2;
        else
            a=x1;
        end
        calls = calls +2;

        if abs(f1 - f2) < tol
            break;
        end
    end
    
    x_min = (a+b)/2;
    f_min = f(x_min);
    calls = calls+1;
end