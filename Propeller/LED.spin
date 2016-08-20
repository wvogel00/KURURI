VAR
  long Stack[9]       'stack space for new COG
PUB Start(Pin, Delay, Count)
  cognew(Blink(Pin, Delay, Count), @Stack)
PUB Blink(Led, Delay, Count)  'deifne the public method, "Blink"
  dira[Led]~~    'difine Led-pin as output (dira[Led]:=1)
  repeat Count
    !outa[Led]
    waitcnt(Delay + cnt)