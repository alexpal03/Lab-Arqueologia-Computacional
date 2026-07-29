#define CLOCK 2
#define READ_WRITE 3
#define RESET 20
#define N 20   //cant de muestras

const char ADDR[] = {22, 23, 24, 25, 26, 27, 28, 29, 37, 36, 35, 34, 33, 32, 31, 30};
const char DATA[] = {49, 48, 47, 46, 45, 44, 43, 42};

volatile int dataBus[N] = {0};
// volatile int addressBus[N] = {0};
uint16_t addressBus = 0;
volatile int readWrite[N] = {0};
volatile int ciclo = 0;

void setup() {
  for (int n = 0; n < 16; n += 1) {
    pinMode(ADDR[n], OUTPUT);
  }
  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], INPUT);
  }
  pinMode(CLOCK, INPUT);
  pinMode(READ_WRITE, INPUT);
  pinMode(RESET, INPUT);

  attachInterrupt(digitalPinToInterrupt(RESET), onReset, RISING);
  
  Serial.begin(115200);
  Serial.println("Inicializando...");
  PORTC = (uint8_t)addressBus;  //A7-A0
  PORTA = addressBus >> 8;      //A15-A8
}

void onReset(){
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
  detachInterrupt(digitalPinToInterrupt(RESET));
  Serial.println("RESET");
}

void onClock() {
  dataBus[addressBus] = PINL;
  PORTC = (uint8_t)addressBus;  //A7-A0
  PORTA = addressBus >> 8;      //A15-A8
  addressBus++;
}

void loop() {
  if(addressBus >= N ){
    detachInterrupt(digitalPinToInterrupt(CLOCK));
    addressBus = 0;

    char output[20];
    for(int i=0; i<N; i++){
      sprintf(output, "%04x -> %02x", i, dataBus[i]);
      Serial.println(output);  
    }
    attachInterrupt(digitalPinToInterrupt(RESET), onReset, RISING);
  }

}
