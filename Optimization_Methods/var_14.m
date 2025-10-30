%% вариант 14: -1 / (x^4 + 2*x^2 + 1) [-1, 2]
clear; clc; close all;
f = @(x) -1 ./ (x.^4 + 2*x.^2 +1);
[a, b] = deal(-1, 2);

tol = 1e-3;

fprintf('tol: %.6f\n\n', 2*tol);

fprintf('Метод Дихотомии\n');
[x_min, f_min, calls, iters] = dichotomy_1d(f, a, b, tol, 1000);
fprintf('Минимум: x = %.5f; f(x) = %.5f\n', x_min, f_min);
fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);

fprintf('\nМетод золотого сечения\n');
[x_min, f_min, calls, iters] = golden_ratio_1d(f, a, b, 2*tol, 1000);
fprintf('Минимум: x = %.5f; f(x) = %.5f\n', x_min, f_min);
fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);

fprintf('\nМетод Фибоначчи\n');
[x_min, f_min, calls, iters] = fib_method(f, a, b, calls);
fprintf('Минимум: x = %.5f; f(x) = %.5f\n', x_min, f_min);
fprintf('Количество итераций: %d; вызовов: %d\n', iters, calls);

% график
% x_plot = linspace(a, b, 1000);
% y_plot = f(x_plot);
% figure;
% plot(x_plot, y_plot, 'LineWidth', 2);
% hold on;
% plot(x_min, f_min, 'go', 'MarkerSize', 8, 'MarkerFaceColor', 'g');
% xlabel('x');
% ylabel('f(x)');
% title('График функции и найденный минимум');
% grid on;
% legend('f(x) = -1/(x^4 + 2x^2 + 1)', 'Найденный минимум');