(define (domain zeno-travel)
(:requirements :strips)
(:predicates
	 (at ?x ?c) (aircraft ?p) (person ?x) (city ?x) (libre ?x)
)
(:functions (citythreat ?x) (timeTerre ?x ?y)
)

(:action fly
 :parameters (?a ?c1 ?c2 ?x)
 :duration (= ?duration (timeTerre ?c1 ?c2))
 :risk (= ?risk (citythreat ?c2))
 :precondition
	(and (aircraft ?a) (city ?c1) (city ?c2)    
             (at ?a ?c1) 
             (person ?x) (at ?x ?c1) (libre ?a)
        )
 :effect
	(and (at ?a ?c2) (not (at ?a ?c1)) 
             (not (libre ?a)) (libre ?a) (not (at ?x ?c1)) (at ?x ?c2)
        )
)

(:action flyVide
 :parameters (?a ?c1 ?c2)
 :duration (= ?duration (timeTerre ?c1 ?c2))
 :risk (= ?risk (citythreat ?c2))
 :precondition
	(and (aircraft ?a) (city ?c1) (city ?c2) (at ?a ?c1) 
        (libre ?a)
        )
 :effect
	(and (at ?a ?c2) (not (at ?a ?c1))
             (not (libre ?a)) (libre ?a)
        )
)
)
