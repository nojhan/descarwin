; MULTIZENO (IJCAI 2013)

(define (domain multi-zeno-travel)
 (:requirements :durative-actions :action-costs)
 (:types aircraft person city - object)
 (:predicates (at ?x - object ?c - city) (libre ?x - aircraft)

 (:functions (citythreat ?x) (timeTerre ?x ?y) (total-cost))

 (:action fly :parameters (?a - aircraft ?c1 ?c2 - city ?x - person)
  :duration (= ?duration (timeTerre ?c1 ?c2))
  :condition (and (at ?a ?c1) (libre ?a) (at ?x ?c1))
  :effect (and (not (at ?a ?c1)) (at ?a ?c2) 
               (not (libre ?a)) (libre ?a) 
               (not (at ?x ?c1)) (at ?x ?c2)
               (increase (total-cost) (citythreat ?c2))))

 (:action flyVide :parameters (?a - aircraft ?c1 ?c2 - city)
  :duration (= ?duration (timeTerre ?c1 ?c2))
  :condition (and (at ?a ?c1) (libre ?a))
  :effect (and (not (at ?a ?c1)) (at ?a ?c2) 
               (not (libre ?a)) (libre ?a)
	       (increase (total-cost) (citythreat ?c2)))))
