function results = compare_1d()
    clear; close all; clc;
% COMPARE Выводит результаты методов оптимизации для всех функций

    functions_data = all_functions();
    
    eps = 1e-5;
    max_iters = 1000;
    results = [];
    
    for ii = 1:length(functions_data)
        fprintf('\n%s\n', repmat('=', 1, 60));
        fprintf('ФУНКЦИЯ %d/%d: %s\n', ii, length(functions_data), functions_data(ii).name);
        fprintf('%s\n', repmat('=', 1, 60));
        
        f = functions_data(ii).f;
        a = functions_data(ii).a;
        b = functions_data(ii).b;
        
        tol = calculate_tol(f, a, b, eps);
        % tol = 1e-5;

        fprintf('Интервал: [%.2f, %.2f], Точность: %.2e\n', a, b, tol);
        
        methods = {'Дихотомия', 'Золотое сечение', 'Фибоначчи'};
        
        for method_idx = 1:3
            
            switch method_idx
                case 1
                    [x_min, f_min, calls, iters] = dichotomy_1d(f, a, b, eps, tol, max_iters);
                case 2
                    [x_min, f_min, calls, iters] = golden_ratio_1d(f, a, b, tol, max_iters);
                case 3
                    [x_min, f_min, calls, iters] = fibonacci_1d(f, a, b, tol, max_iters);
            end

            fprintf('%s:\n', methods{method_idx});
            fprintf('  x_min = %.8f, f_min = %.8f\n', x_min, f_min);
            fprintf('  Итераций: %d, Вызовов: %d\n', iters, calls);
        end
        
        % построение и сохранение графика для каждой функции
        plot_function_result(f, a, b, x_min, f_min, functions_data(ii).name, ii);
    end
end


function plot_function_result(f, a, b, x_min, f_min, title_str, number)
    figure;
    x_plot = linspace(a, b, 1000);
    y_plot = f(x_plot);
    
    plot(x_plot, y_plot, 'LineWidth', 2);
    hold on;
    plot(x_min, f_min, 'go', 'MarkerSize', 8, 'MarkerFaceColor', 'g');

    xlabel('x');
    ylabel('f(x)');
    title(['Минимум функции: ', title_str]);
    grid on;
    legend('Функция', 'Найденный минимум', 'Location', 'best');
    
    saveas(gcf, sprintf('plots/function_%02d.png', number));
    close;
end