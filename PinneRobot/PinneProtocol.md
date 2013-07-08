Legger det opp slik at det kan bli ganske lett for deg å parse beskjeder i koden din. Definerer to hovedtyper bytes: kommandobyte og databytes. Ikke ulikt MIDI slik at du kun trenger å forholde deg til to hovedstates i parse funksjonen (f.eks. WAITING_FOR_COMMAND, WAITING_FOR_DATA). Alle beskjeder ender med CR/LF.
Du kan jo se over dette og komme med tilbakemeldinger. Hvis du heller vil ha en mer lesbar protokoll, så er jeg med på den.


* bit7: 1=kommando byte, 0=data byte. i.e. 1nnn nnnn er en kommando byte, 0nnn nnnn er en data byte
* bit6: 1=denne byten blir ettefulgt av data bytes, 0= denne byten har ingen etterfølgende bytes
* bit5: 0=kommando gjelder venstre pinne, 1=byten gjelder høyre pinne
* bit4: 1=set data byte, 0=get data byte
* bit0-3: hvilken beskjed det er av følgende muligheter:
* 0001: state
* 0010: target posisjon
* 0100: telleverk, for enkodere, current position
* 1000: speed
* 0101: Posisjon for rotasjonsservomotor
* 1100: Speed for rotasjonsservomotor
* 0000: Stopp

Vi har plass til flere typer kommandoer i bit 0 - 3, hvis det skulle være behov for det. 

Det første som kommer inn er alltid en kommando byte med bit8==1. Hvis beskjeden har tilhørende data, f.eks. sett posisjon i etterfølgende bytes, så er bit7==1. Beskjeden vil i så tilfelle bli etterfulgt av data bytes hvor bit8==0.

Data bytes varierer med hvilken kommando som er sendt. 
State data byte: Data om hvilken state pinne motoren er i, er bygd opp slik: 
`0 0 0 0 0 0 <Direction bit: up:0,down:1> <Motor off:0,on:1> `

Telleverk/Target position/speed data bytes:
0<bits:20,19,18,17,16,15,14> 0<bits:13,12,11,10,9,8,7> 0<bits:6,5,4,3,2,1,0>
Dette utgjør en unsigned 32 bit type, selv om oppløsning kun går til 21-bits verdi. Dette er overkill for fart, servoposisjon og servofart, men jeg tror det er lurt å ha mer å gå på for telleverk/posisjon. Greit at databytes alltid er tre bytes med denne formateringen, slik at du kun trenger èn rutine for å parse de.


Vi får følgende beskjeder sendt til pinnerobot: ('x' betyr at bit ikke er viktig, men settes til 0 pr default, '->>' indikerer hva som pinnerobot skal returnere)

* Non-replying commands: 
    * Left motor go up:                     11010001 0xxxxx01 = D1 01 0D 0A
    * Left motor go down:                 11010001 0xxxxx11 = D1 03 0D 0A
    * Left motor stop:                       10010000                = 90 0D 0A
    * Left motor set speed:               11011000 0nnnnnnn 0nnnnnnn = D8 nn nn nn 0D 0A
    * Left motor set current position: 11010100 0nnnnnnn 0nnnnnnn  = D4 nn nn nn 0D 0A
    * Left motor set target position:   11010010 0nnnnnnn 0nnnnnnn  = D2 nn nn nn 0D 0A
  
   * Right motor go up:                     11110001 00000001 = F1 01 0D 0A
   * Right motor go down:                 11110001 00000011 = F1 03 0D 0A
   * Right motor stop:                       10110000                = B0 0D 0A
   * Right motor set speed:               11111000 0nnnnnnn 0nnnnnnn  = F8 nn nn nn 0D 0A
   * Right motor set current position: 11110100 0nnnnnnn 0nnnnnnn  = F4 nn nn nn 0D 0A
   * Right motor set target position:   11110010 0nnnnnnn 0nnnnnnn  = F2 nn nn nn 0D 0A
  
   * Rotation servo set position:        10x10101 0nnnnnnn 0nnnnnnn  = 95 nn nn nn 0D 0A
   * Rotation servo set speed:           10x11100 0nnnnnnn 0nnnnnnn  = 9C nn nn nn 0D 0A

* Replying commands:
   * Left motor get state:                  10000001 = 81 0D 0A 
      * ->> (returnerer) 11000001 0xxxxx<direction><on|off> = C1 nn 0D 0A
   * Left motor get target position:        10000010 = 82 0D 0A
      * ->> 11000010 0nnnnnnn 0nnnnnnn = C2 nn nn nn 0D 0A
   * Left motor get current position:     10000100 = 84 0D 0A
      * ->> 11000100 0nnnnnnn 0nnnnnnn = C4 nn nn nn 0D 0A
   * Left motor get speed:                 10001000 = 88 0D 0A
      * ->> 11001000 0nnnnnnn 0nnnnnnn = C8 nn nn nn 0D 0A
  
   * Right motor get state:                  10100001 = A1 0D 0A 
      * ->> (returnerer) 11100001 0xxxxx<direction><on|off> = E1 nn 0D 0A
   * Right motor get target position:        10100010 = A2 0D 0A
      * ->> 11100010 0nnnnnnn 0nnnnnnn = E2 nn nn nn 0D 0A
   * Right motor get current position:     10100100 = A4 0D 0A
      * ->> 11100100 0nnnnnnn 0nnnnnnn = E4 nn nn nn 0D 0A
   * Right motor get speed:                 10101000 = A8 0D 0A
      * ->> 11101000 0nnnnnnn 0nnnnnnn = E8 nn nn nn 0D 0A

   * Servo motor get speed:               10x01100 = 8C 0D 0A
      * ->> 11x01100 0nnnnnnn 0nnnnnnn = CC nn nn nn 0D 0A
   * Servo motor get position:            10x00101 = 85 0D 0A
      * ->> 11x00101 0nnnnnnn 0nnnnnnn = C5 nn nn nn 0D 0A


Det er et par punkter jeg ville bare nevne utover dette:
* Ved init av roboten, så må servo speed settes helt lavt, slik at motoren ikke hopper til kjapt rett til init posisjon.
* Ved init av roboten så trenger jeg at den sender en notify om at den er online. Det kan hende jeg trenger dette til gjenoppretting av state. Vi kan si at dette notify signalet er FF 0D 0A 
* Jeg har sjekket databladet til Leonardoene litt nærmere og det viser seg at de ekstra interruptene egentlig er på pin 0 og 1, dvs. de samme som vi sannsynligvis vil bruke til å koble UART til XBee. Så er litt usikker på om vi får til interrupts på alle pinnen til encoderene. Men det går kanskje an å bruke noe shift-register e.l. Uansett, så tror jeg at vi kan prøve med kun ett encodersignal per motor og teste med det.
* Har du tenkt nærmere på hvordan du skal sette sammen roboten, rent hardwaremessig? Vi burde tatt en prat om dette. Skal begynne med budsjettet for pinnerobotene i løpet av denne uken.
