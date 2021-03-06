clc; clear;
global g v_max v_min x_max x_min dSteps dT v_step x_step u_max
g = 9.81;

u_max = +2;

%%Discretization
v_max = 8;
v_min = -v_max;
x_max = pi; 
x_min = -x_max; 

dSteps = 55;
dT = 0.1;

v_step = (v_max - v_min)/dSteps;
x_step = (x_max - x_min)/dSteps; 

% J = zeros(dSteps^2 + 1,1); % unknown

up = ones(dSteps * dSteps,1);
un = ones(dSteps * dSteps,1)*2;

pij_p = transMatrix(up);
pij_n = transMatrix(un);
A = [eye(dSteps^2)-pij_p; eye(dSteps^2)-pij_n];

g = ones(dSteps^2,1);
b = [g;g];

f = -ones(dSteps^2,1);

% linprog(f,A,b): Ax <= b, min f'x
J = linprog(f, A, b);
