CON
  Led1 = 16
  Led2 = 17
  Delay1 = 3_000_000
  Delay2 = 2_000_000
VAR
  long Stack[9]       'stack space for new COG
PUB Main
  cognew(Blink(Led1, Delay1,10),@Stack)
  Blink(Led2, Delay2,20)
PUB Blink(Led, Delay, Count)  'deifne the public method, "Blink"
  dira[Led]~~    'difine Led-pin as output (dira[Led]:=1)
  repeat Count
    !outa[Led]
    waitcnt(Delay + cnt)