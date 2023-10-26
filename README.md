# TP Actionneur_Automatique_Applique



# OBJECTIF 

L'objectif de ce TP est de pouvoir réaliser l' asservissement en vitesse et en courant d'un moteur. Ce TP est réparti en 3 parties : 
I. La commande d'une MCC  
II. La commande en boucle ouverte et mesure du courant et de la vitesse 
III. Asservissement 

#  PARTIE 1 : Commande d'une MCC 

# Génération de 4 PWM 

Dans cette première partie, nous devions généré 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en respectant le cahier des charges en incluant les temps mort. Ensuite nous devions visualiser et verifier les signaux sur un oscilloscope et réaliser un premier essai de commande de moteur. 

# 1. Réglages des PWM**
**PWM SIMPLE f=20kHz , alpha 60% , résolution de 10 bits**

Les PWM que nous avons utilisé sont les PWM du timer 1 
channel 1 -> 1°PWM,
channel 2 -> 2°PWM
channel 3 -> Compl1°PWM
channel 4 -> Compl2°PWM
Pour avoir une fréquence de PWM de 20 kHz, nous avons fais les réglages suivants : 
-PSC 1
-ARR de 8500
-PWM 1 et PWM2 channel Pulse=4950 (rapport cyclique de 60%)
On a pu visualiser les 4 PWM simples sur l'oscilloscope et verifier la fréquence. 

**PWM EN COMPLEMENTAIRE DECALEE**

Pour avoir une PWM en complémentaire décallé, on a changé certains réglages. Premièrement on a mis le mode centrée pour la PWM donc nous devons changer la valeur de l'ARR pour avoir une fréquence de 20kHz: 
-PSC  & ARR 4125
-PWM1 Pulse = 2550 ( rapport cyclique de 60%)
-PWM2 Pulse = 4125-2550=637

**2. Temps morts**

En regardant la datasheet du transistor, on a pris une valeur de 200 ns. Dans les réglages sur STM32cubeMX il faut mettre une valeur 
entre 0 et 255. Nous avons utilisé le calcul non linéaire donné dans la datasheet pour trouver la valeur associé à 200ns. La valeur à 
mettre sur CubeMX est 34. 

# Commande de la vitesse 

On veut controler la vitesse en envoyant une séquence via la liaison UART de la forme : speed XXXX. Le nombre qui est associé au speed est le rapport cyclique en pourcentage. 
Pour réaliser cette commande nous avons modifié le fichier shell.c : 

```c
# define PWM_MAX 100
#define PWM_MIN   0

else if(strcmp(argv[0],"speed")==0){
			float speedVal=atoi(argv[1]);
			if (speedVal> PWM_MAX){
				speedVal=PWM_MAX;
			}
			if (speedVal< PWM_MIN){
				speedVal=PWM_MIN;
			}

			speedVal=(DUTY_MAX*speedVal)/100; //PWM en pourcentage
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,speedVal);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,DUTY_MAX - speedVal);

		}

```
On fixe le rapport cyclique à 0 ou à 100 si la valeur après speed est inférieur à 0 ou supérieur à 100. 


# Premiers test 

Nous avons réalisé des test pour vérifier le bon fonctionnement de la commande speed. 
On augmente la vitesse de 10 en 10. 