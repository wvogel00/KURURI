CON
  Led = 16
  Delay = 3_000_000

PUB Blink
  dira[Led]~~    'difine Led-pin as output (dira[Led]:=1)
  repeat
    !outa[Led]
    waitcnt(Delay + cnt)