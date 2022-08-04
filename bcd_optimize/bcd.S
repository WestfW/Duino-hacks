        // Double Dabble by WestfW

.global DD8
DD8:
;;; Enter 8bit binary in R24.
;;; exit 3 BCD digits in R25:R24
;;; (uint16_t DD8(uint8_t))
        eor r25, r25            ;zero low/middle nibbles.
        eor r18, r18            ;zero high nibble
;;; The one's "column" can't be > 5 until we've done at least three shifts.
;;; so we don't need to do comparisons for a while.
;;; (also, we'd only be shifting 0s into the high nibble, so skip that too.)
        add r24, r24
        adc r25, r25            ; shift once
	
        add r24, r24
        adc r25, r25            ; shift twice
	
        add r24, r24
        adc r25, r25            ; shift third time
;;; 1st nibble in r25 can now be >= 5.  but not other nibbles, yet.
	
        cpi r25, 5
        brlo 0f
          subi r25, -3            ;Add 3
0:      add r24, r24
        adc r25, r25              ; shift 4th time
;;;  high nibble is still getting zeros

        mov r19, r25             ;temp copy for nib comp
        andi r19, 0xF
        cpi r19, 5
        brlo 0f 
         subi r25, -3           ;Don't think this ever half-carries?
;;; don't need to check middle nibble for a while, either.
;;; Never need to check high nibble, since it'll never be > 2 for 8bits
0:      add r24, r24
        adc r25, r25            ; shift 5th time
;;;  high nibble is still getting zeros

        mov r19, r25             ;temp copy for nib comp
        andi r19, 0xF
        cpi r19, 5
        brlo 0f
         subi r25, -3           ;Don't think this ever half-carries?
0:      
;;; still don't need to compare middle nibble
;;; Never need to check high nibble!
        add r24, r24            ; shift 6th time.
        adc r25, r25
;;;  high nibble is still getting zeros

        mov r19, r25             ;temp copy for nib comp
        andi r19, 0xF
        cpi r19, 5
        brlo 0f
         subi r25, -3           ;Don't think this ever half-carries?
;;; It's about time to check the middle nibble (tens col)
0:      cpi r25, 0x50
        brlo 0f
         subi r25, -0x30
;;; Never need to check high nibble!
0:      add r24, r24
        adc r25, r25
        adc r18, r18             ; shift all 3 bytes (7th shft)

        mov r19, r25             ;temp copy for nib comp
        andi r19, 0xF
        cpi r19, 5
        brlo 0f
         subi r25, -3           ;Don't think this ever half-carries?
0:      cpi r25, 0x50
        brlo 0f
         subi r25, -0x30
;;; Never need to check high nibble!
0:      add r24, r24
        adc r25, r25
        adc r18, r18             ; shift all 3 bytes (8th shft)

;;; move results to proper place
        mov r24, r25
        mov r25, r18
        ret

#if 1
        .global DD8s
DD8s:
;;; Enter 8bit binary in R24.
;;; exit 3 BCD digits in R25:R24
;;; (uint16_t DD8(uint8_t))
        eor r25, r25            ;zero low/middle nibbles.
        eor r18, r18            ;zero high nibble
;;; The one's "column" can't be > 5 until we've done at least three shifts.
;;; so we don't need to do comparisons for a while.
;;; (also, we'd only be shifting 0s into the high nibble, so skip that too.)
        add r24, r24
        adc r25, r25            ; shift once
        add r24, r24
        adc r25, r25            ; shift twice
        add r24, r24
        adc r25, r25            ; shift third time
	rcall checkAndShift     ; start checking, do 4th shift
	rcall checkAndShift     ;  5
	rcall checkAndShift     ;   6
	rcall checkAndShift     ;    7
	rcall checkAndShift     ;     8
	
;;; move results to proper place
        mov r24, r25
        mov r25, r18
        ret

checkAndShift:  
        mov r19, r25            ;temp copy for nib comp
        andi r19, 0xF
        cpi r19, 5              ;compare ones column
        brlo 0f
         subi r25, -3           ;shouldn't ever half-carry
0:      cpi r25, 0x50           ;check tens column
        brlo 0f
         subi r25, -0x30
;;; Never need to check hundreds (max of 2!)
0:      add r24, r24
        adc r25, r25
        adc r18, r18             ; shift all 3 bytes
        ret
#endif
