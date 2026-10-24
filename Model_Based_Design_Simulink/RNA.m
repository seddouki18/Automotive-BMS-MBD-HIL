% Projet BMS - Phase 2 : Entrainement Reseau de Neurones (2 Couches)
clear; clc; close all;

% 1. Chargement des donnees
load('bms_data.mat');

% 2. Entrees (I, V, T) et Cible (SOC)
X = [I_mes; V_mes; T]; 
Y = soc;

% 3. Creation du reseau avec 2 couches cachees :
% Couche 1 : 12 neurones
% Couche 2 : 6 neurones
architecture = [12, 6];
net = fitnet(architecture, 'trainlm');

% Division des donnees
net.divideParam.trainRatio = 0.70; % pour les poids et biais 
net.divideParam.valRatio   = 0.15; % les donnes pour controle l entrqinement des que le resau
net.divideParam.testRatio  = 0.15; % les donnes pour la verification

net.trainParam.epochs = 250;
net.trainParam.goal   = 1e-5;

% 4. Entrainement
disp('Entrainement avec 2 couches cachees...');
[net, tr] = train(net, X, Y);

% 5. Evaluation des erreurs
soc_est = net(X);

erreur = abs(soc - soc_est) * 100;
rmse = sqrt(mean((soc - soc_est).^2)) * 100;
err_max = max(erreur);

fprintf('Resultats : \n');
fprintf('RMSE : %.2f %%\n', rmse);
fprintf('Erreur Max : %.2f %%\n', err_max);

% 6. Sauvegarde du modele a 2 couches
save('soc_model.mat', 'net');
disp('Modele sauvegarde dans soc_model.mat');

% 7. Tracé
figure;
subplot(2,1,1);
plot(t, soc * 100, 'k', 'LineWidth', 1.5); hold on;
plot(t, soc_est * 100, 'r--', 'LineWidth', 1.2);
ylabel('SOC (%)');
title('Estimation SOC avec 2 Couches Cachees [12, 6]');
legend('Vrai SOC', 'SOC Estime');
grid on;

subplot(2,1,2);
plot(t, erreur, 'b');
ylabel('Erreur Absolue (%)');
xlabel('Temps (s)');
title('Erreur d''estimation');
grid on;