(clear)
(defrule foo (factoid $?x&:(> (length$ ?x) 3)) =>)
(assert (factoid a) (factoid a b c) (factoid a b c d e))
(agenda)
(clear)
(defglobal ?*x* = 0)
(bind ?*x* (assert (this)))
(assert (that ?*x*))
(bind ?*x* (assert (what)))
(assert (that ?*x*))
(defrule foo ?f <- (this) (that ?f) =>)
(agenda)
(get-dynamic-deftemplate-checking) ;; CR0191
(set-dynamic-deftemplate-checking)
(crsv-trace-on) ;; CR0207
(crsv-trace-off)
(defexternal) ;; CR0219
(defrelation) 
(defrule foo (?x ?y) =>) ;; CR0220
(defrule foo (red | blue) =>)
(clear) ;; CR0222
(remove blah.clp)
(load blah.clp)
(save blah.clp)
(load blah.clp)
(remove blah.clp)
(remove blah.bin)
(bload blah.bin)
(bsave blah.bin)
(bload blah.bin)
(remove blah.bin)
(clear) ;; CR0223
(assert (a =(+ 3 4)))
(assert (a (+ 5 6)))
(facts)
(remove) ;; CR0240
(remove 7)
(remove blah.txt 7)
(open "blah.txt" blah "w")
(printout blah "(printout t Greetings from blah.txt crlf)" crlf)
(close blah)
(batch blah.txt)
(remove blah.txt)
(batch blah.txt)
(remove yuck.txt)
(rename)
(rename 7)
(rename blah.txt 7)
(rename blah.txt yuck.txt 7)
(open "blah.txt" blah "w")
(printout blah "(printout t Greetings from blah.txt crlf)" crlf)
(close blah)
(rename blah.txt yuck.txt)
(remove blah.txt)
(batch yuck.txt)
(remove yuck.txt)
(apropos) ;; CR0241
(apropos 7)
(apropos "dynamic" 7)
(apropos "dynamic")
(apropos dynamic)
(list-watch-items 7) ;; CR0263
(unwatch all)
(list-watch-items)
(clear) ;; CR0264
(assert (foo))
(list-deftemplates)
(undeftemplate foo)
(retract 0)
(undeftemplate foo)
(clear) ;; CR0265
(defglobal ?*x* = 3)
(str-assert "(foo ?*x* (+ 3 4))")
(facts)
(str-assert "(foo ?*x* (+ 3 4))")
(clear) ;; CR0269
(remove blah.txt)
(load-facts blah.txt)
(batch blah.txt)
(assert (foo))
(list-deftemplates)
(ppdeftemplate foo)
(trunc) ;; CR0271
(clear) ;; CR0272
(defmodule A)
(defrule A::foo =>)
(deftemplate A::bar)
(deffacts A::yak)
(defglobal A ?*woz* = 3)
(defrule-module)
(defrule-module 7)
(defrule-module foo 7)
(defrule-module huh)
(defrule-module foo)
(deftemplate-module)
(deftemplate-module 7)
(deftemplate-module bar 7)
(deftemplate-module huh)
(deftemplate-module bar)
(deffacts-module)
(deffacts-module 7)
(deffacts-module yak 7)
(deffacts-module huh)
(deffacts-module yak)
(defglobal-module)
(defglobal-module 7)
(defglobal-module woz 7)
(defglobal-module huh)
(defglobal-module woz)
(clear) ;; CR0280
(assert-string "(a b c)")
(assert-string "d e f")
(facts)
