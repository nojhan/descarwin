(define (domain bread_no-fluents)
	(:requirements :typing) 
	(:types kitchen 
		      machine
		      flevel mlevel dlevel blevel klevel clevel olevel - level)

	(:predicates 
	  (ready-to-use ?m - machine)
	  (has-flour ?k - kitchen ?l - flevel)
	  (ready-mix ?k - kitchen ?l - mlevel)
		(ready-dough ?k - kitchen ?l - dlevel)
		(loaf-bread ?k - kitchen ?l - blevel)
		(breakfast-bun ?k - kitchen ?l - klevel)
		(cooked-bun ?k - kitchen ?l - clevel)
		(cooked-bread ?k - kitchen ?l - olevel)
		(need-clean ?m - machine)
		(next ?l1 ?l2 - level))

	(:functions 
; 	(has-flour ?k - kitchen)
; 	(ready-mix ?k - kitchen)
;		(ready-dough ?k - kitchen)
;		(loaf-bread ?k - kitchen)
;		(breakfast-bun ?k - kitchen)
;		(cooked-bun ?k - kitchen)
;		(cooked-bread ?k - kitchen)
		(labour)
		(energy)
		(pollution))

;(:action prepare-mix
;		:parameters(?k - kitchen)
;		:precondition (and
;				(>=(has-flour ?k)1)
;			      )
;		:effect (and
;			       (increase (ready-mix ?k)1)
;			       (decrease (has-flour ?k)1)
;			)
;	)

(:action prepare-mix :parameters (?k - kitchen ?l1 ?l2 - flevel ?m1 ?m2 - mlevel)
	:precondition (and (ready-mix ?k ?m1) (next ?m1 ?m2)
                     (has-flour ?k ?l2) (next ?l1 ?l2))
		:effect (and (not (ready-mix ?k ?m1)) (ready-mix ?k ?m2)
                 (not (has-flour ?k ?l2)) (has-flour ?k ?l1)))

;	(:action kneed-dough-machine
;		:parameters (?k - kitchen ?m - machine)
;		:precondition (and
;			      (>=(ready-mix ?k )2)
;				(ready-to-use ?m)
;			      )
;		:effect (and
;				(decrease(ready-mix ?k)2)
;				(increase(ready-dough ?k)2)
;				(increase(energy)1)
;				(need-clean ?m)
;				(not(ready-to-use ?m))
;			)
;	)

(:action kneed-dough-machine :parameters (?k - kitchen ?m - machine ?m1 ?m2 ?m3 - mlevel ?d1 ?d2 ?d3 - dlevel)
		:precondition (and (ready-mix ?k ?m3) (next ?m1 ?m2) (next ?m2 ?m3)
		                   (ready-dough ?k ?d1) (next ?d1 ?d2) (next ?d2 ?d3)
				               (ready-to-use ?m))
		:effect (and (not (ready-mix ?k ?m3)) (ready-mix ?k ?m1)
		             (not (ready-dough ?k ?d1)) (ready-dough ?k ?d3)
				         (need-clean ?m)
				         (not(ready-to-use ?m))
				         (increase (energy) 1)))

;	(:action clean-machine
;		:parameters (?m - machine)
;		:precondition (and
;				(need-clean ?m)
;			      )
;		:effect (and
;				(not(need-clean ?m))
;				(ready-to-use ?m)
;			)
;	)

(:action clean-machine :parameters (?m - machine)
		:precondition (and (need-clean ?m))
		:effect (and (not(need-clean ?m))
				         (ready-to-use ?m)))

;	(:action kneed-hand
;		:parameters(?k - kitchen)
;		:precondition (and
;				(>=(ready-mix ?k)1)
;			      )
;		:effect (and
;				(decrease(ready-mix ?k)1)
;				(increase(ready-dough ?k)1)
;				(increase(labour)1)
;			)
;	)

(:action kneed-hand :parameters (?k - kitchen ?m1 ?m2 - mlevel ?d1 ?d2 - dlevel)
		:precondition (and (ready-mix ?k ?m2) (next ?m1 ?m2)
		                   (ready-dough ?k ?d1) (next ?d1 ?d2))
		:effect (and (not (ready-mix ?k ?m2)) (ready-mix ?k ?m1)
		             (not (ready-dough ?k ?d1)) (ready-dough ?k ?d2)
				         (increase (labour) 1)))
				         
;	(:action making-loaf-bread
;		:parameters (?k - kitchen)
;		:precondition (and
;				(>=(ready-dough?k)1)
;				)
;		:effect (and
;				(decrease(ready-dough ?k)1)
;				(increase(loaf-bread ?k)2)
;			)
;	)
	
(:action making-loaf-bread :parameters (?k - kitchen ?d1 ?d2 - dlevel ?b1 ?b2 ?b3 - blevel)
		:precondition (and (ready-dough ?k ?d2) (next ?d1 ?d2)
		            		   (loaf-bread ?k ?b1) (next ?b1 ?b2) (next ?b2 ?b3))
		:effect (and (not (ready-dough ?k ?d2)) (ready-dough ?k ?d1)
			           (not (loaf-bread ?k ?b1)) (loaf-bread ?k ?b3)))

;	(:action making-bun 
;		:parameters (?k - kitchen)
;		:precondition (and
;				(>=(ready-dough ?k)1)
;			      )
;		:effect (and
;			     (decrease(ready-dough ?k)1)
;			     (increase(breakfast-bun ?k)5)
;			)
;	)

(:action making-bun :parameters (?k - kitchen ?d1 ?d2 - dlevel ?k1 ?k2 ?k3 ?k4 ?k5 ?k6 - klevel)
		:precondition (and (ready-dough ?k ?d2) (next ?d1 ?d2)
                       (breakfast-bun ?k ?k1)
			            		 (next ?k1 ?k2)
			            		 (next ?k2 ?k3)
			            		 (next ?k3 ?k4)
			            		 (next ?k4 ?k5)
			            		 (next ?k5 ?k6))
		:effect (and (not (ready-dough ?k ?d2)) (ready-dough ?k ?d1)
			           (not (breakfast-bun ?k ?k1)) (breakfast-bun ?k ?k6)))

;	(:action baking-oven-bun
;		:parameters (?k - kitchen)
;		:precondition (and
;				(>=(breakfast-bun ?k)10)
;			       )
;		:effect	     (and
;				(decrease(breakfast-bun ?k)10)
;				(increase(cooked-bun ?k)10)
;				(increase(energy)1)
;			      )
;	)

(:action baking-oven-bun :parameters (?k - kitchen ?k1 ?k2 ?k3 ?k4 ?k5 ?k6 ?k7 ?k8 ?k9 ?k10 ?k11 - klevel ?c1 ?c2 ?c3 ?c4 ?c5 ?c6 ?c7 ?c8 ?c9 ?c10 ?c11 - clevel)
		:precondition (and (breakfast-bun ?k ?k11)
			            		 (next ?k1 ?k2)
			            		 (next ?k2 ?k3)
			            		 (next ?k3 ?k4)
			            		 (next ?k4 ?k5)
			            		 (next ?k5 ?k6)
			            		 (next ?k6 ?k7)
			            		 (next ?k7 ?k8)
			            		 (next ?k8 ?k9)
			            		 (next ?k9 ?k10)
			            		 (next ?k10 ?k11)
			            		 (cooked-bun ?k ?c1)
			            		 (next ?c1 ?c2)
			            		 (next ?c2 ?c3)
			            		 (next ?c3 ?c4)
			            		 (next ?c4 ?c5)
			            		 (next ?c5 ?c6)
			            		 (next ?c6 ?c7)
			            		 (next ?c7 ?c8)
			            		 (next ?c8 ?c9)
			            		 (next ?c9 ?c10)
			            		 (next ?c10 ?c11))
		:effect (and (not (breakfast-bun ?k ?k11)) (breakfast-bun ?k ?k1)
                 (not (cooked-bun ?k ?c1)) (cooked-bun ?k ?c11)
				         (increase (energy) 1)))

;	(:action baking-oven-loaf-bread
;		:parameters (?k - kitchen)
;		:precondition (and
;				(>=(loaf-bread ?k)4)
;			       )
;		:effect	     (and
;				(decrease(loaf-bread ?k)4)
;				(increase(cooked-bread ?k)4)
;				(increase(energy)1)
;			      )
;	)

(:action baking-oven-loaf-bread :parameters (?k - kitchen ?b1 ?b2 ?b3 ?b4 ?b5 - blevel ?o1 ?o2 ?o3 ?o4 ?o5 - olevel)
		:precondition (and (loaf-bread ?k ?b5) (next ?b1 ?b2) (next ?b2 ?b3) (next ?b3 ?b4) (next ?b4 ?b5)
		                   (cooked-bread ?k ?o1) (next ?o1 ?o2) (next ?o2 ?o3) (next ?o3 ?o4) (next ?o4 ?o5))
		:effect (and (not (loaf-bread ?k ?b5)) (loaf-bread ?k ?b1)
			           (not (cooked-bread ?k ?o1)) (cooked-bread ?k ?o5)
		      			 (increase (energy) 1)))

;	(:action baking-charcoal-bread
;		:parameters (?k - kitchen)
;		:precondition(and
;				(>=(loaf-bread ?k)2)
;			       )
;		:effect	     (and
;				(decrease(loaf-bread ?k)2)
;				(increase(cooked-bread ?k)2)
;				(increase(pollution)1)
;			      )
;	)
	
(:action baking-charcoal-bread :parameters (?k - kitchen ?b1 ?b2 ?b3 - blevel ?o1 ?o2 ?o3 - olevel)
		:precondition (and (loaf-bread ?k ?b3) (next ?b1 ?b2) (next ?b2 ?b3)
		            		   (cooked-bread ?k ?o1) (next ?o1 ?o2) (next ?o2 ?o3))
		:effect (and (not (loaf-bread ?k ?b3)) (loaf-bread ?k ?b1)
			           (not (cooked-bread ?k ?o1)) (cooked-bread ?k ?o3)
          			 (increase (pollution) 1)))
	
;	(:action baking-charcoal-bun
;		:parameters (?k - kitchen)
;		:precondition(and
;				(>=(breakfast-bun ?k)2)
;			       )
;		:effect	     (and
;				(decrease(breakfast-bun ?k)2)
;				(increase(cooked-bun ?k)2)
;				(increase(pollution)1)
;			      )
;	)

(:action baking-charcoal-bun: parameters (?k - kitchen ?k1 ?k2 ?k3 - klevel ?c1 ?c2 ?c3 - clevel)
		:precondition (and (breakfast-bun ?k ?k3) (next ?k1 ?k2) (next ?k2 ?k3)
			            		 (cooked-bun ?k ?c1) (next ?c1 ?c2) (next ?c2 ?c3))
		:effect (and (not (breakfast-bun ?k ?k3)) (breakfast-bun ?k ?k1)
                 (not (cooked-bun ?k ?c1)) (cooked-bun ?k ?c3)
		             (increase (pollution) 1)))
)
