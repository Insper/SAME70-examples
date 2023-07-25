* 2x Motor de Passo + 2x Adafruit TB6612
* 28/02/2019
* Marco Mello

# Resumo :

Esse exemplo demonstra o uso de 2 drivers de motor Adafruit TB6612 para controlar 2 motores de passo bipolar (4 Fios).

Periféricos uC:

- Power Managment Controller (PMC)
- GPIO -> EXT1
   
Módulos : 

- 2x Driver de Motor -> Adafruit TB6612 1.2A DC/Stepper Motor Driver Breakout Board
- 2x Motor de Passo -> Mercury Motor SM-42BYG011-25 (4 Fios / Bipolar)

Pinos específicos:

```
                   Motor 1                                               Motor 2
Adafruit TB6612 (1)  -  SAME70-XPLD (EXT1)             Adafruit TB6612 (2) - SAME70-XPLD (EXT2)
         PWMA        ->        PA3                              PWMA       ->       PA3
         AIN2        ->        PB1                              AIN2       ->       PB1
         AIN         ->        PB0                              AIN1       ->       PB0
         STBY        ->        NC*                              STBY       ->       NC*
         BIN1        ->        PD25                             BIN1       ->       PD25
         BIN2        ->        PD28                             BIN2       ->       PD28
         PWMB        ->        PD20                             PWMB       ->       PD20
         GND         ->        GND                              GND        ->       GND
         VCC         ->        VCC                              VCC        ->       VCC
         VM          ->        NC*                              VM         ->       NC*
```

## Diagrama

--

## Motor de Passo Bipolar (4 Fios / Bipolar)

--

## Adafruit TB6612 1.2A DC/Stepper Motor Driver Breakout Board

--

### Como essa aplicação funciona

Os dois motores se movem simultaneamente, cada um em um sentido até completar uma volta (360º), após completar uma volta, aguardam 1 segundo e giram no sentido contrário até completar outra volta.
