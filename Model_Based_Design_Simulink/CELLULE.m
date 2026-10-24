% Projet BMS - Simulation Cellule Li-ion (Modele Thevenin 1-RC)
clear; clc; close all;

% 1. Parametres de la cellule (Type 18650)
Cn = 2.5;          % Capacite nominale en Ah
R0 = 0.025;        % Resistance ohmique (Ohm)
R1 = 0.015;        % Resistance de polarisation (Ohm)
C1 = 1500;         % Capacite de polarisation (F)
tau = R1 * C1;

% Parametres thermiques
Cth = 65;          % Capacite thermique (J/K)
hA = 0.08;         % Coefficient de convection (W/K)
Tamb = 25;         % Temperature ambiante (deg C)

% Courbe OCV en fonction du SOC (mesures experimentales)
soc_pts = [0.0,  0.1,  0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0];
ocv_pts = [3.00, 3.45, 3.62, 3.68, 3.74, 3.80, 3.87, 3.95, 4.05, 4.14, 4.20];

% 2. Vecteur temps et profil de courant
dt = 0.1;                  % Pas d'echantillonnage (100 ms)
t = 0:dt:1800;             % Duree du test : 30 min
N = length(t);

I = zeros(1, N);

% Profil de conduite simplifie (decharge, acceleration, freinage)
for k = 1:N
    t_sec = t(k);
    cycle = mod(t_sec, 120);
    
    if cycle < 40
        I(k) = 2.0 + 1.2 * sin(0.1 * t_sec);   % Conduite normale
    elseif cycle < 70
        I(k) = 5.0 + 1.5 * sin(0.3 * t_sec);   % Forte acceleration
    elseif cycle < 90
        I(k) = -2.0;                           % Freinage regeneratif
    else
        I(k) = 0.0;                            % Arret
    end
end

% Bruit de mesure capteur
I_mes = I + 0.05 * randn(1, N);

% 3. Simulation temporelle (Euler explicite)
soc = zeros(1, N);
V1 = zeros(1, N);
V = zeros(1, N);
T = zeros(1, N);

% Conditions initiales
soc(1) = 0.95;     % Batterie chargee a 95%
V1(1) = 0.0;
T(1) = Tamb;

for k = 1:N-1
    % Mise a jour du SOC (Coulomb counting)
    soc(k+1) = soc(k) - (I(k) * dt) / (Cn * 3600);
    
    % Interpolation de la tension OCV
    Voc = interp1(soc_pts, ocv_pts, soc(k), 'linear');
    
    % Tension aux bornes du circuit RC
    V1(k+1) = V1(k) + dt * (-V1(k)/tau + I(k)/C1);
    
    % Tension totale aux bornes de la cellule
    V(k) = Voc - R0 * I(k) - V1(k);
    
    % Modele thermique simple (Joule - Convection)
    P_perte = R0 * (I(k)^2);
    Q_conv = hA * (T(k) - Tamb);
    T(k+1) = T(k) + dt * (P_perte - Q_conv) / Cth;
end

% Dernier point de tension
Voc_fin = interp1(soc_pts, ocv_pts, soc(N), 'linear');
V(N) = Voc_fin - R0 * I(N) - V1(N);

% Ajout bruit sur la tension
V_mes = V + 0.01 * randn(1, N);

% 4. Sauvegarde des donnees pour l'entrainement
save('bms_data.mat', 't', 'I_mes', 'V_mes', 'T', 'soc');
disp('Simulation terminee. Donnees enregistrees dans bms_data.mat');

% 5. Tracé des resultats
figure;

subplot(2,2,1);
plot(t, I_mes, 'b');
title('Courant de decharge I (A)');
xlabel('Temps (s)'); grid on;

subplot(2,2,2);
plot(t, V_mes, 'r');
title('Tension cellule V (V)');
xlabel('Temps (s)'); grid on;

subplot(2,2,3);
plot(t, T, 'm');
title('Temperature T (^oC)');
xlabel('Temps (s)'); grid on;

subplot(2,2,4);
plot(t, soc * 100, 'k', 'LineWidth', 1.2);
title('Vrai SOC (%)');
xlabel('Temps (s)'); grid on;