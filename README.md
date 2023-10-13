# Actionneur_Automatique_Applique
# SEANCE 1 :

**OBJECTIFS**

Pour cette première séance, nous devions générer 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en
respectant le cahier des charges en incluant les temps mort. Ensuite nous devions visualiser et verifier les signaux sur un 
oscilloscope et réaliser un premier essai de commande de moteur. 
Puis controler la vitesse du moteur en envoyant une séquence speed XXXX via la liaison UART (par l'USB) et faire le traitement. 

**1. Réglages des PWM**

**PWM SIMPLE f=20kHz , alpha 60% , résolution de 10 bits**

Les PWM que nous avons utilisé sont les PWM du timer 1 (channel 1 -> 1°PWM,channel 2 -> 2°PWM,channel 3 -> Compl1°PWM,channel 4 -> Compl2°PWM). 
On veut une fréquence de PWM de 20kHz donc on a fais les réglages suivants : 
-PSC 1
-ARR de 8500
-PWM 1 et PWM2 channel Pulse=4950 (rapport cyclique de 60%)

On a pu visualiser les 4 PWM simples sur l'oscilloscope et verifier la fréquence. 

**PWM EN COMPLEMENTAIRE DECALEE**

Pour avoir une PWM en complémentaire décallé, on a changé certains réglages. Premièrement on a mis le mode centrée pour la PWM donc
nous devons changer la valeur de l'ARR pour avoir une fréquence de 20kHz: 
-PSC  & ARR 4125
-PWM1 Pulse = 2550 ( rapport cyclique de 60%)
-PWM2 Pulse = 4125-2550=637

**2. Temps morts**

En regardant la datasheet du transistor, on a pris une valeur de 200 ns. Dans les réglages sur STM32cubeMX il faut mettre une valeur 
entre 0 et 255. Nous avons utilisé le calcul non linéaire donné dans la datasheet pour trouver la valeur associé à 200ns. La valeur à 
mettre sur CubeMX est 34. 

**3. Essai de la commande du moteur **

Nous avons verifier les 4 PWM, le temps mort et la fréquence à l'oscilloscope, on a réalisé un essai sur le moteur. On a pu valider le7
bon donctionnemnt de la commande du moteur. (Les images de verification et du calcul de temps mort sont ci-dessous).
AJOUTER LES IMAGES DE L'OSCILLO ET DU CALCUL
Ainsi il nous reste à traiter la partie controler la vitesse via la liaison UART. 
