%% =========================================================================
%% PROJET BMS IA - SCRIPT D'INITIALISATION COMPLET (MBD & ISO 26262)
%% FST Al Hoceima - Master SER
%% =========================================================================
clearvars -except bms_data reseau_poids;
%%clearvars -except bms_data reseau_poids ds_in;
%%clc;

fprintf('Initialisation des parametres BMS et profil WLTP en cours...\n');

%% 1. PARAMETRES DU SOLVEUR ET D'ECHANTILLONNAGE
Ts       = 0.01;   % Periode d'echantillonnage = 10 ms (100 Hz)
sim_time = 1800.0;         % Duree totale = 30 minutes (1800 s)

%% 2. PARAMETRES PHYSIQUES DE LA CELLULE (Rapport Section 4.5)
Cn_Ah = single(2.5);            % Capacite nominale en Ah
Cn    = single(Cn_Ah * 3600.0); % Capacite nominale en Coulombs (As)
R0    = single(0.020);          % Resistance ohmique serie (Ohm)
R1    = single(0.015);          % Resistance de polarisation (Ohm)
C1    = single(1200.0);         % Capacite de polarisation (Farad)
tau_p = single(R1 * C1);        % Constante de temps = 18 s

% Parametres thermiques
C_th  = single(62.5);   % Capacite thermique (J/K)
hA    = single(0.085);  % Convection (W/K)
T_amb = single(25.0);   % Temperature ambiante (degC)

%% 3. CONDITIONS INITIALES DE SIMULATION
SOC_init = single(0.95); % SOC de depart = 95%
V1_init  = single(0.0);  % Tension RC initiale = 0 V
T_init   = single(25.0); % Temperature initiale = 25 degC

%% 4. SEUILS DE SECURITE ISO 26262 (Stateflow)
V_crit_high = single(4.25); % Surtension (V)
V_crit_low  = single(2.50); % Sous-tension (V)
T_crit      = single(55.0); % Emballement thermique (degC)
I_crit      = single(10.0); % Surcourant (A)
SOC_crit    = single(0.05); % Decharge destructive (5%)

T_derating_in   = single(45.0);
T_derating_out  = single(42.0);
SOC_derating_in = single(0.15);
SOC_derating_out= single(0.20);

%% 5. GENERATION DU SIGNAL D'ENTREE ts_I (Simulink From Workspace)
dt_prof = 0.1;                      % Pas de discretisation du profil (100 ms)
t_prof  = (0:dt_prof:sim_time)';     % Vecteur temps (colonne)
N_pts   = length(t_prof);
I_prof  = zeros(N_pts, 1, 'single'); % Pre-allocation en single

for k = 1:N_pts
    t_sec = t_prof(k);
    cycle = mod(t_sec, 120);
    
    if cycle < 40
        I_prof(k) = 2.0 + 1.2 * sin(0.1 * t_sec);   % Conduite normale
    elseif cycle < 70
        I_prof(k) = 5.0 + 1.5 * sin(0.3 * t_sec);   % Forte acceleration
    elseif cycle < 90
        I_prof(k) = -2.0;                            % Freinage regeneratif
    else
        I_prof(k) = 0.0;                             % Arret
    end
end

% Creation de l'objet timeseries officiel pour Simulink
% Donnees en 'single' pour la STM32, Temps en 'double' pour le solveur
ts_I = timeseries(single(I_prof), double(t_prof));
ts_I.Name = 'I_demand';

% Chargement des poids IA s'ils existent dans le dossier
if exist('reseau_poids.mat', 'file')
    load('reseau_poids.mat');
end
