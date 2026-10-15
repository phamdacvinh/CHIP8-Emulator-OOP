; Dodge.ch8
; Controls: left arrow/A/Q moves left, right arrow/D/E moves right.
; Avoid the falling block. A collision restarts the game.
;
; This source is documentation for the generated ROM bytes in Dodge.ch8.
; CHIP-8 programs start at 0x200.

0x200: 00 E0    ; CLS
0x202: 60 30    ; V0 = player X
0x204: 61 1C    ; V1 = player Y
0x206: C2 3F    ; V2 = random obstacle X
0x208: 63 00    ; V3 = obstacle Y
0x20A: 67 00    ; V7 = frame counter

0x20C: 00 E0    ; CLS
0x20E: 65 04    ; V5 = CHIP-8 key 4
0x210: E5 A1    ; Skip next if key 4 is not pressed
0x212: 70 FE    ; V0 -= 2
0x214: 65 06    ; V5 = CHIP-8 key 6
0x216: E5 A1    ; Skip next if key 6 is not pressed
0x218: 70 02    ; V0 += 2
0x21A: 77 01    ; V7 += 1
0x21C: 37 03    ; Skip next if V7 == 3
0x21E: 12 2C    ; Jump to draw
0x220: 67 00    ; V7 = 0
0x222: 73 01    ; V3 += 1
0x224: 33 20    ; Skip next if V3 == 32
0x226: 12 2C    ; Jump to draw
0x228: C2 3F    ; V2 = random obstacle X
0x22A: 63 00    ; V3 = 0

0x22C: A2 50    ; I = player sprite
0x22E: D0 14    ; Draw player at V0,V1, height 4
0x230: A2 54    ; I = obstacle sprite
0x232: D2 34    ; Draw obstacle at V2,V3, height 4
0x234: 3F 01    ; If collision flag is set, skip loop jump and restart
0x236: 12 0C    ; Jump to loop
0x238: 00 E0    ; CLS
0x23A: A2 58    ; I = hit marker
0x23C: 62 0A    ; V2 = 10
0x23E: 63 0A    ; V3 = 10
0x240: D2 35    ; Draw hit marker
0x242: 12 00    ; Restart

0x250: F0 F0 F0 F0       ; Player sprite
0x254: F0 F0 F0 F0       ; Obstacle sprite
0x258: 90 60 60 90 F0    ; Hit marker
