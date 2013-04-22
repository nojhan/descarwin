(define (domain zeno-travel)
(:requirements :durative-actions :fluents)
(:predicates
	 (at ?x ?c) (aircraft ?p) (person ?x) (city ?x) (libre ?x) (path ?c1 ?c2)
)
(:functions (citythreat ?x) (timeTerre ?x ?y) (total-cost)
)

(:durative-action fly
 :parameters (?a ?c1 ?c2 ?x)
 :duration (= ?duration (timeTerre ?c1 ?c2))
 :condition
	(and (at start (aircraft ?a)) (at start (city ?c1)) (at start (city ?c2)) (at start (path ?c1 ?c2))
             (at start (at ?a ?c1))
	     (at start (person ?x)) (at start (at ?x ?c1)) (at start (libre ?a))
        )
 :effect
	(and (at end (at ?a ?c2)) (at start (not (at ?a ?c1)))
             (at start (not (libre ?a))) (at end (libre ?a))
	     (at start (not (at ?x ?c1))) (at end (at ?x ?c2))
	     (at end (increase (total-cost) (citythreat ?c2)))
        )
)

(:durative-action flyVide
 :parameters (?a ?c1 ?c2)
 :duration (= ?duration (timeTerre ?c1 ?c2))
 :condition
	(and (at start (aircraft ?a)) (at start (city ?c1)) (at start (city ?c2)) (at start (at ?a ?c1)) (at start (path ?c1 ?c2))
        (at start (libre ?a))
        )
 :effect
	(and (at end (at ?a ?c2)) (at start (not (at ?a ?c1)))
             (at start (not (libre ?a))) (at end (libre ?a))
	     (at end (increase (total-cost) (citythreat ?c2)))
        )
)
)
