(deftemplate age ;definition of fuzzy variable ‘age’
    0 120 years
    ( (young (25 1) (50 0))
    (old (50 0) (65 1))
    )
)
(deffacts fuzzy-fact
    (age young) ; a fuzzy fact
)
(defrule one ; a rule that matches and asserts fuzzy facts
    (Speed_error big)
=>
    (assert (Throttle_change small))
)
