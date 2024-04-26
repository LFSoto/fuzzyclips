; Definición de plantillas para las variables temperatura y velocidad del ventilador
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

; Definición de la función de membresía difusa para la velocidad del ventilador
(deftemplate velocidad-ventilador
   0 100 temperatura
   ( (baja (0 1) (50 1) (100 0))
   (media (0 0) (50 1) (100 1))
   (alta (50 0) (100 1)) )
)

; Regla para ajustar la velocidad del ventilador basada en la temperatura
(defrule ajustar-velocidad
   ?t <- (temperatura (valor ?temp)) ; Match any temperature fact
   =>
   (printout t "Ingrese la nueva temperatura (celsius): " t)
   (bind ?newTemp (read))
   (retract ?t)
   (assert (temperatura (valor ?newTemp))) 
   (printout t "Nueva temperatura " ?newTemp crlf)
   
   (bind ?newVelocity (fuzzy-trimf ?newTemp 0 50 100))
   (assert (velocidad_ventilador (valor ?newVelocity)))
   (printout t "La nueva velocidad del ventilador es de " ?newVelocity crlf)
)

;(assert (temperatura (valor 70)))
;(reset)
;(run)

(deftemplate temp
    (slot degrees (type NUMBER) (range 0 100))
)

(deftemplate temp
    (slot degrees (type FUZZY-VALUE))
)

(deffuzzyset temp:degrees
    (range 0 100)
    (Z 20 40 cold)
    (S 60 80 hot)
    (TERM freezing (0 0) (0 10))
    (TERM extremely_cold (0 0) (10 1) (30 0))
)

(defrule temp-rule
    ?t <- (temp (degrees ?d&:(and (> ?d 40) (< ?d 60))))
    =>  
    (printout t "It's such a pleasant day" crlf)
)
