(deftemplate temperatura
   (slot valor (type NUMBER))
)

(deftemplate velocidad_ventilador
   (slot valor (type NUMBER))
)

; Definición de una función triangular para lógica difusa usando la sintaxis correcta de CLIPS
(deffunction fuzzy-trimf (?x ?a ?b ?c)
  (if (<= ?x ?a) then
    (return 0)
  else
    (if (<= ?x ?b) then
      (return (/ (- ?x ?a) (- ?b ?a)))
    else
      (if (<= ?x ?c) then
        (return (/ (- ?c ?x) (- ?c ?b)))
      else
        (return 0)
      )
    )
  )
)

(deftemplate temperatura_rango
   0 100 temperatura
   ( (low (0 1) (0 1) (50 0))
   (medium (0 0) (50 1) (100 0))
   (high (50 0) (100 1) (100 1)) ) 
)

(deftemplate velocidad_ventilador
   0 100 velocidad
   ( (low (0 1) (0 1) (50 0))
   (medium (0 0) (50 1) (100 0))
   (high (50 0) (100 1) (100 1)) )
)

(defrule ajustar-velocidad
   ?t <- (temperatura_rango (valor ?temp))
   =>
   (bind ?newVelocity (fuzzy-trimf (maximum-defuzzify ?temp) 0 50 100))
   (if (> ?newVelocity 0.5) then
      (assert (velocidad_ventilador (high)))
   else
      (assert (velocidad_ventilador (low)))
   (printout t "Temperature is " (maximum-defuzzify ?temp) " Celsius. "
              "Setting fan speed to " ?newVelocity crlf))
)
