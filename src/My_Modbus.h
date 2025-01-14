/*
    My_Modbus.h - ModbusIP Library Implementation for HomIS
    
*/

#include <Arduino.h>

float fnMin(float fInput);
float fnMax(float fInput); 
float fnAverage(float fInput);
void ReadModbus();

const IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box 

constexpr uint16_t MB_READ_INTERVAL = 1000; 

constexpr uint16_t START_REG = 12688;       // Starting holding register
constexpr uint16_t NB_REGS = 11;           // Number of holding registers to read

constexpr uint16_t START_REG_ANI = 12745;       // Starting holding register
constexpr uint16_t NB_REGS_ANI = 5;           // Number of holding registers to read  y compris les Alarmes

constexpr uint16_t START_OUTPUT1 = 512;     // Starting Output bits Ppes et Chaudière
constexpr uint16_t NB_OUTPUT1 = 16;         // Number of Output Bits

constexpr uint16_t MASK_CHAUD = 0x1; // Bit 0
constexpr uint16_t MASK_BOOST = 0x4000; // Bit 5
constexpr uint16_t MASK_PPERADIAT = 0x2; // Bit 2
constexpr uint16_t MASK_PPEPLANCHER = 0x4; // Bit 2


//Lecture des Bits de Sortie 
// de "000512   , 16 ); 

//$OutPuts["000514"] =  Porte de Garage
//$OutPuts["000514"] =  Ppe Radiateur
//$OutPuts["000515"] =  Ppe Plancher
//$OutPuts["000516"] =  Vanne Ouvre
//$OutPuts["000517"] =  Vanne ferme
//$OutPuts["000518"] =  Chaudière
//$OutPuts["000519"] =  Boot Chaudière

//Ou bien Registre 12745 pour les retours Animation 
// Voir ci-dessous

/*
 === Ecriture Modbus ===

registre 12743 pour les BP Marche
registre 12744 pour les BP Arret
Registre 12745 pour les retours Animation Equipements en Marche

Bit de chaque Mot (de 0 à 15)

CHAUD           = Bit 0  
BOOST           = bit 5 pour BPs M/A
BOOST           = bit 14 pour animation
PPE Radiateur   = Bit 1
PPE Plancher    = Bit 2

*/