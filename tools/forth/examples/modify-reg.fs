: modify-reg ( value mask pos reg -- )
    >r tuck         \ -- value pos mask pos
    lshift r@ bic!  \ clear mask first
    lshift r> bis!  \ set the value bits
;
