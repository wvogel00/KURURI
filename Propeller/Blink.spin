CON
  Led = 16
  Delay = 3_000_000

PUB Blink
  dira[Led]~~
  repeat
    !outa[Led]
    waitcnt(Delay + cnt)