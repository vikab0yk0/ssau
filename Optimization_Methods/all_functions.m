function functions_data = all_functions()
% TEST_FUNCTIONS Возвращает массив тестовых функций и интервалов для оптимизации
%
% Структура данных:
%   functions_data(i).name - название функции
%   functions_data(i).f - function handle
%   functions_data(i).a, functions_data(i).b - границы интервала

    % functions_data().name = ' на [, ]';
    % functions_data().f = @(x) ;
    % functions_data().a = ; functions_data().b = ;

    functions_data = [];
    
    functions_data(1).name = '(x-3)^2 + 4 на [0, 10]';
    functions_data(1).f = @(x) (x-3).^2 + 4;
    functions_data(1).a = 0; functions_data(1).b = 10;

    functions_data(2).name = '0.1*exp((x-1)^2) на [-2.5, 4]';
    functions_data(2).f = @(x) 0.1*exp((x-1).^2);
    functions_data(2).a = -2.5; functions_data(2).b = 4;

    functions_data(3).name = 'ch((x+1)^2) на [-2, 0.5]';
    functions_data(3).f = @(x) cosh((x+1).^2);
    functions_data(3).a = -2; functions_data(3).b = 0.5;

    functions_data(4).name = '2-cos(x) на [-pi/4, pi/2]';
    functions_data(4).f = @(x) 2-cos(x);
    functions_data(4).a = -pi/4; functions_data(4).b = pi/2;
    
    functions_data(5).name = 'sh(2x)^2 на [-1, 1.5]';
    functions_data(5).f = @(x) sinh(2*x).^2;
    functions_data(5).a = -1; functions_data(5).b = 1.5;
    
    functions_data(6).name = 'x + 1/x на [0.5, 1.5]';
    functions_data(6).f = @(x) x + 1./x;
    functions_data(6).a = 0.5; functions_data(6).b = 1.5;

    functions_data(7).name = '(x+2)^4 - 1 на [-7, 5]';
    functions_data(7).f = @(x) (x+2).^4 - 1;
    functions_data(7).a = -7; functions_data(7).b = 5;

    functions_data(8).name = 'exp(x - 1 + 1/(x-2)) на [2.5, 4]';
    functions_data(8).f = @(x) exp(x - 1 + 1./(x-2));
    functions_data(8).a = 2.5; functions_data(8).b = 4;

    functions_data(9).name = '|(x-2)^3| на [0, 5]';
    functions_data(9).f = @(x) abs((x-2).^3);
    functions_data(9).a = 0; functions_data(9).b = 5;

    functions_data(10).name = 'sqrt[|(x-2)|^5] на [0, 5]';
    functions_data(10).f = @(x) sqrt(abs(x-2).^5);
    functions_data(10).a = 0; functions_data(10).b = 5;

    functions_data(11).name = 'tg(|x|^1.5) на [-pi/4, pi/3]';
    functions_data(11).f = @(x) tan(abs(x).^1.5);
    functions_data(11).a = -pi/4; functions_data(11).b = pi/3;

    functions_data(12).name = 'ln(x^2 - 4x + 5) на [1, 4]';
    functions_data(12).f = @(x) log(x.^2 - 4*x + 5);
    functions_data(12).a = 1; functions_data(12).b = 4;

    functions_data(13).name = '|arcsin(x/2)| на [-1, 2]';
    functions_data(13).f = @(x) abs(asin(x/2));
    functions_data(13).a = -1; functions_data(13).b = 2;

    functions_data(14).name = '-1 / (x^4 + 2x^2 + 1) на [-1, 2]';
    functions_data(14).f = @(x) -1 ./ (x.^4 + 2*x.^2 +1);
    functions_data(14).a = -1; functions_data(14).b = 2;

    functions_data(15).name = '-2 / [ch(4*x+3) + 3] на [-3, 2]';
    functions_data(15).f = @(x) -2 ./ (cosh(4*x+3)+3);
    functions_data(15).a = -3; functions_data(15).b = 2;

    functions_data(16).name = 'th(|x-2|^3) на [-3, 5]';
    functions_data(16).f = @(x) tanh(abs(x-2).^3);
    functions_data(16).a = -3; functions_data(16).b = 5;

    functions_data(17).name = '1 - exp[-(x-2)^2] на [-3, 5]';
    functions_data(17).f = @(x) 1 - exp(-1*(x-2).^2);
    functions_data(17).a = -3; functions_data(17).b = 5;

    functions_data(18).name = '2 - 1/[10 + sh(x+2)^2] на [-5, 2]';
    functions_data(18).f = @(x) 2 - 1./(10 + sinh(x+2).^2);
    functions_data(18).a = -5; functions_data(18).b = 2;

    functions_data(19).name = 'tg(1 + x^2/4)^1.5 на [-pi/4, pi/3]';
    functions_data(19).f = @(x) tan(1 + x.^2/4).^1.5;
    functions_data(19).a = -pi/4; functions_data(19).b = pi/3;

    functions_data(20).name = 'ln(2 + tg(x/4)^2) на [-3, 5]';
    functions_data(20).f = @(x) log(2 + tan(x/4).^2);
    functions_data(20).a = -3; functions_data(20).b = 5;

    functions_data(21).name = 'x + 1/(x-2.5) на [3, 6]';
    functions_data(21).f = @(x) x + 1./(x-2.5);
    functions_data(21).a = 3; functions_data(21).b = 6;

    functions_data(22).name = '|(x^2 - 2x + 2)^2| на [-2, 5]';
    functions_data(22).f = @(x) abs((x.^2 - 2*x + 2).^2);
    functions_data(22).a = -2; functions_data(22).b = 5;

    functions_data(23).name = 'cos(x)^4 + sin(x)^4 на [0, pi/2]';
    functions_data(23).f = @(x) cos(x).^4 + sin(x).^4;
    functions_data(23).a = 0; functions_data(23).b = pi/2;

    functions_data(24).name = 'arcsin(x^2) на [-1, 0.8]';
    functions_data(24).f = @(x) asin(x.^2);
    functions_data(24).a = -1; functions_data(24).b = 0.8;

    functions_data(25).name = '3 + |sh(2x)| на [-2, 3]';
    functions_data(25).f = @(x) 3 + abs(sinh(2*x));
    functions_data(25).a = -2; functions_data(25).b = 3;

    functions_data(26).name = 'ch(exp(x)-1) на [-2, 1]';
    functions_data(26).f = @(x) cosh(exp(x)-1);
    functions_data(26).a = -2; functions_data(26).b = 1;

    functions_data(27).name = '5 - exp[-(x-4)^4] на [1, 5]';
    functions_data(27).f = @(x) 5 - exp(-1*(x-4).^4);
    functions_data(27).a = 1; functions_data(27).b = 5;

    functions_data(28).name = 'exp[sqrt(x) + 1./(sqrt(x)-2)] на [8, 10]';
    functions_data(28).f = @(x) exp(sqrt(x) + 1./(sqrt(x)-2));
    functions_data(28).a = 8; functions_data(28).b = 10;

    functions_data(29).name = 'sqrt(|x^4 - 16|) на [1, 5]';
    functions_data(29).f = @(x) sqrt(abs(x.^4-16));
    functions_data(29).a = 1; functions_data(29).b = 5;

    functions_data(30).name = '-1/[ch(x)^4 + 2*cos(x)^2 + 3] на [-1, 2]';
    functions_data(30).f = @(x) -1 ./ (cosh(x).^4 + 2*cos(x).^2 + 3);
    functions_data(30).a = -1; functions_data(30).b = 2;

    functions_data(31).name = 'arcsin(|x|) на [-0.5, 0.7]';
    functions_data(31).f = @(x) asin(abs(x));
    functions_data(31).a = -0.5; functions_data(31).b = 0.7;

    functions_data(32).name = 'exp[x^(1/3) + 1/(x^(1/3) - 1)] на [5, 10]';
    functions_data(32).f = @(x) exp(x.^(1/3) + 1./(x.^(1/3) -1));
    functions_data(32).a = 5; functions_data(32).b = 10;

end