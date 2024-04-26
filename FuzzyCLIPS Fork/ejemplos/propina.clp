; Definir los universos de discurso
(deffacts universos ""
   (calidad (z 0 10))
   (servicio (s 1 10))
)

; Definir las funciones de membresía para cada variable
(deffuzzyset calidad-baja
   (Sigmoid 5 -1.5))
(deffuzzyset calidad-media
   (Sigmoid 5 1.5))
(deffuzzyset calidad-alta
   (Sigmoid 7 1.5))

(deffuzzyset servicio-bajo
   (Sigmoid 5 -1.5))
(deffuzzyset servicio-medio
   (Sigmoid 5 1.5))
(deffuzzyset servicio-alto
   (Sigmoid 7 1.5))

(deffuzzyset propina-baja
   (points (0 1) (5 1) (10 0)))
(deffuzzyset propina-media
   (points (0 0) (10 1) (20 0)))
(deffuzzyset propina-alta
   (points (10 0) (15 1) (20 1)))

; Definir reglas para controlar la propina
(defrule regla1
   (or (calidad calidad-baja) (servicio servicio-bajo))
   =>
   (assert (propina propina-baja)))

(defrule regla2
   (servicio servicio-medio)
   =>
   (assert (propina propina-media)))

(defrule regla3
   (or (servicio servicio-alto) (calidad calidad-alta))
   =>
   (assert (propina propina-alta)))

; Sistema de control y simulación
;(reset)
;(assert (calidad 6.5))
;(assert (servicio 9.8))
;(run)
;(facts)
