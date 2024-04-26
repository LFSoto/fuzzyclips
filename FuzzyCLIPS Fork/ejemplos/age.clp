(deftemplate age ;definition of fuzzy variable ‘age’
    0 120 years
    ( (young (25 1) (50 0))
    (old (50 0) (65 1))
    )
)
(deffacts fuzzy-fact
    (age young) ; a fuzzy fact
)

(defrule is-old
    (age old)
=>
    (printout t "That is old" crlf)
)

(defrule is-young
    (age young)
=>
    (printout t "That is young" crlf)
)

;Ejemplos de uso
;Cargar archivo en terminal FuzzyCLIPS
;(clear)
;(reset)
;(load age.clp)


;(assert (age young))
;(assert (age old))
;(assert (young 20))
;(facts)

;(assert (age young))
;(assert (young 20))
;(run) That is young
;(assert (young 50))
;(run) empty


