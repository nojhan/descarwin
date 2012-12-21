(define (problem prob1)
	(:domain bread)
	
	(:objects
		kitchen0 - kitchen
		machine0 - machine
	)

	(:init
		(ready-to-use machine0)
		(=(has-flour kitchen0)10)
		(=(ready-mix kitchen0)20)
		(=(ready-dough kitchen0)0) 
		(=(loaf-bread kitchen0)0)
		(=(breakfast-bun kitchen0)0)
		(=(cooked-bun kitchen0)0)
		(=(cooked-bread kitchen0)0)
		(=(energy)0)
		(=(pollution)0)
		(=(labour)0)
	
	)
	(:goal (and
		 (>=(cooked-bun kitchen0)20)
		 (>=(breakfast-bun kitchen0)15)
		 (>=(cooked-bread kitchen0)16)
		 (>=(loaf-bread kitchen0)6)
		 (>=(ready-dough kitchen0)10)
		 
		)
	)
(:metric minimize (energy))
(:metric minimize (labour))
(:metric minimize (pollution))


)