#define PIN_ITR20001-LEFT   A2
#define PIN_ITR20001-MIDDLE A1
#define PIN_ITR20001-RIGHT  A0




void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_ITR20001-LEFT,INPUT) ;
  pinMode(PIN_ITR20001-MIDDLE,INPUT) ;
  pinMode(PIN_ITR20001-RIGHT,INPUT) ;

}

void loop() {
  // put your main code here, to run repeatedly:
  int valor_Left = digitalRead(PIN_ITR20001-LEFT);
  int valor_middle = digitalRead(PIN_ITR20001-MIDDLE);
  int valor_right = digitalRead(PIN_ITR20001-RIGHT);
  

}
