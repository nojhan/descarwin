(define (domain bread)
	(:requirements :typing :fluents) 
	(:types kitchen 
		machine
	)

	(:predicates 
	     	(ready-to-use ?m - machine)
		(need-clean ?m - machine)
		
	)

	(:functions 
		(has-flour ?k - kitchen)
		(ready-mix ?k - kitchen)
		(ready-dough ?k - kitchen)
		(loaf-bread ?k - kitchen)
		(breakfast-bun ?k - kitchen)
		(cooked-bun ?k - kitchen)
		(cooked-bread ?k - kitchen)
		(labour)
		(energy)
		(pollution)
	)

	(:action prepare-mix
		:parameters(?k - kitchen)
		:precondition (and
				(>=(has-flour ?k)1)
			      )
		:effect (and
			       (increase (ready-mix ?k)1)
			       (decrease (has-flour ?k)1)
			)
	)

	(:action kneed-dough-machine	
		:parameters (?k - kitchen ?m - machine)
		:precondition (and
			      	(>=(ready-mix ?k )2)
				(ready-to-use ?m)
			      )
		:effect (and
				(decrease(ready-mix ?k)2)
				(increase(ready-dough ?k)2)
				(increase(energy)1)
				(need-clean ?m)
				(not(ready-to-use ?m))
			)
	)

	(:action clean-machine
		:parameters (?m - machine)
		:precondition (and
				(need-clean ?m)
			      )
		:effect (and
				(not(need-clean ?m))
				(ready-to-use ?m)
			)
	)

	(:action kneed-hand
		:parameters(?k - kitchen)
		:precondition (and
				(>=(ready-mix ?k)1)
			      )
		:effect (and
				(decrease(ready-mix ?k)1)
				(increase(ready-dough ?k)1)
				(increase(labour)1)
			)
	)

	(:action making-loaf-bread
		:parameters (?k - kitchen)
		:precondition (and
				(>=(ready-dough?k)1)
				)
		:effect (and
				(decrease(ready-dough ?k)1)
				(increase(loaf-bread ?k)2)
			)
	)

	(:action making-bun 
		:parameters (?k - kitchen)
		:precondition (and
				(>=(ready-dough ?k)1)
			      )
		:effect (and
			     (decrease(ready-dough ?k)1)
			     (increase(breakfast-bun ?k)5)
			)
	)

	(:action baking-oven-bun
		:parameters (?k - kitchen)
		:precondition (and
				(>=(breakfast-bun ?k)10)
			       )
		:effect	     (and
				(decrease(breakfast-bun ?k)10)
				(increase(cooked-bun ?k)10)
				(increase(energy)1)
			      )
	)

	(:action baking-oven-loaf-bread
		:parameters (?k - kitchen)
		:precondition (and
				(>=(loaf-bread ?k)4)
			       )
		:effect	     (and
				(decrease(loaf-bread ?k)4)
				(increase(cooked-bread ?k)4)
				(increase(energy)1)
			      )
	)

	(:action baking-charcoal-bread
		:parameters (?k - kitchen)
		:precondition(and
				(>=(loaf-bread ?k)2)
			       )
		:effect	     (and
				(decrease(loaf-bread ?k)2)
				(increase(cooked-bread ?k)2)
				(increase(pollution)1)
			      )
	)
	
	(:action baking-charcoal-bun
		:parameters (?k - kitchen)
		:precondition(and
				(>=(breakfast-bun ?k)2)
			       )
		:effect	     (and
				(decrease(breakfast-bun ?k)2)
				(increase(cooked-bun ?k)2)
				(increase(pollution)1)
			      )
	)	
)
