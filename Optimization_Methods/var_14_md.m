%% вариант 14: f(x) = -sum(1 ./ (x_i^4 + 2*x_i^2 + 1))
clear; clc; close all;

f_md = @(x) -sum(1 ./ (x.^4 + 2*x.^2 + 1)); % просто сумма по координатам

x1 = [0;0;0];  
x2 = [10;-7;5];  
eps = 1e-3;
max_iters = 1000;

fprintf('eps: %.6f\n\n', 2*eps);

fprintf('Метод Дихотомии\n');
[x_min, f_min, calls, iters] = dichotomy_md(f_md, x1, x2, eps, max_iters);
fprintf('Минимум: x = [ ');
fprintf('%.5f ', x_min);
fprintf(']; f(x) = %.5f\n', f_min);
fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);

fprintf('\nМетод золотого сечения\n');
[x_min, f_min, calls, iters] = golden_ratio_md(f_md, x1, x2, 2*eps, max_iters);
fprintf('Минимум: x = [ ');
fprintf('%.5f ', x_min);
fprintf(']; f(x) = %.5f\n', f_min);
fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);

fprintf('\nМетод Фибоначчи\n');
[x_min, f_min, calls, iters] = fibonacci_md(f_md, x1, x2, calls);
fprintf('Минимум: x = [ ');
fprintf('%.5f ', x_min);
fprintf(']; f(x) = %.5f\n', f_min);
fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);

% fprintf('\nМетод Координатного спуска\n');
% [x_min, f_min, calls, iters] = coordinate_descent(f_md, x1, eps, max_iters);
% fprintf('Минимум: x = [ ');
% fprintf('%.5f ', x_min);
% fprintf(']; f(x) = %.5f\n', f_min);
% fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);