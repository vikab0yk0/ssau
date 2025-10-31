function [x_min, f_min, calls, iters] = dichotomy_md(f, x1, x2, eps, max_iters)
% DICHOTOMY_MD Поиск минимума многомерной функции методом дихотомии вдоль направления
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
    
    direction = x2 - x1;
    direction = eps * direction / norm(direction);
    
    iters = 0;
    calls = 0;
    
    while iters < max_iters
        iters = iters+1;
        
        mid = (x2 + x1) ./ 2;
        
        f1 = f(mid - direction);
        f2 = f(mid + direction);

        calls = calls + 2;
        
        if f1<f2
            x2=mid;
        else
            x1=mid;
        end

        if norm(x2 - x1) <= 2 * eps
            break;
        end

        % fprintf('x1 = [');
        % fprintf(' %.5f ', x1);
        % fprintf('] \t x2 = [');
        % fprintf(' %.5f ', x2);
        % fprintf(']\n');
    end
    
    x_min = (x2 + x1) ./ 2;
    f_min = f(x_min);
    calls = calls + 1;

end