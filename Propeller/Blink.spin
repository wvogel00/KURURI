CON
  Led1 = 16
  Led2 = 17
  Delay1 = 3_000_000
  Delay2 = 2_000_000
OBJ
  LED : "LED"                                
PUB Main
  LED.Start(Led1,Delay1,10)
  LED.Blink(Led2, Delay2,20)