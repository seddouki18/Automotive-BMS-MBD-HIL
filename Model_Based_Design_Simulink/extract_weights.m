% Extraction des parametres du reseau pour Simulink / Code C
clear; clc;
load('soc_model.mat');

% 1. Matrices des poids (Weights)
W1 = net.IW{1,1};  % Couche 1 : taille 12x3
W2 = net.LW{2,1};  % Couche 2 : taille 6x12
W3 = net.LW{3,2};  % Sortie   : taille 1x6

% 2. Vecteurs des biais (Biases)
b1 = net.b{1};     % Taille 12x1
b2 = net.b{2};     % Taille 6x1
b3 = net.b{3};     % Taille 1x1

% 3. Sauvegarde directe
save('reseau_poids.mat', 'W1', 'W2', 'W3', 'b1', 'b2', 'b3');
disp('Extraction terminee ! reseau_poids.mat cree avec succes.');