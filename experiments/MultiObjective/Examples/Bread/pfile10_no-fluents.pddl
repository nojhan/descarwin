(define (problem prob10_no-fluents)
  (:domain bread_no-fluents)
    (:objects
	kitchen0 - kitchen
	machine0 - machine
	f0 - flevel
	f1 - flevel
	f2 - flevel
	f3 - flevel
	f4 - flevel
	f5 - flevel
	f6 - flevel
	f7 - flevel
	f8 - flevel
	f9 - flevel
	f10 - flevel
        f11 - flevel
	f12 - flevel
	f12 - flevel
	f13 - flevel
	f14 - flevel
	f15 - flevel

        m0 - mlevel
	m1 - mlevel
	m2 - mlevel
	m3 - mlevel
	m4 - mlevel
	
        d0 - dlevel
	d1 - dlevel
	d2 - dlevel

        b0 - blevel
	b1 - blevel
	b2 - blevel
	b3 - blevel
	b4 - blevel
	b5 - blevel
	b6 - blevel

        k0 - klevel
	k1 - klevel
	k2 - klevel
	k3 - klevel
	k4 - klevel
	k5 - klevel

	c0 - clevel
	c1 - clevel
	c2 - clevel
	c3 - clevel
	c4 - clevel
	c5 - clevel
	c6 - clevel
	c7 - clevel
	c8 - clevel
	c9 - clevel
	c10 - clevel
		
	o0 - olevel
	o1 - olevel
	o2 - olevel
	o3 - olevel
	o4 - olevel
	o5 - olevel
	o6 - olevel
	o7 - olevel
	o8 - olevel
	o9 - olevel
	o10 - olevel
    )

    (:init
	(next f0 f1)
	(next f1 f2)
	(next f2 f3)
	(next f3 f4)
	(next f4 f5)
	(next f5 f6)
	(next f6 f7)
	(next f7 f8)
	(next f8 f9)
	(next f9 f10)
	(next f10 f11)
	(next f11 f12)
	(next f12 f13)
	(next f13 f14)
	(next f14 f15)

        (next m0 m1)
	(next m1 m2)
	(next m2 m3)
	(next m3 m4)
	
        (next d0 d1)
	(next d1 d2)

        (next b0 b1)
	(next b1 b2)
	(next b2 b3)
	(next b3 b4)
	(next b4 b5)
	(next b5 b6)

        (next k0 k1)
	(next k1 k2)
	(next k2 k3)
	(next k3 k4)
	(next k4 k5)

        (next c0 c1)
	(next c1 c2)
	(next c2 c3)
	(next c3 c4)
	(next c4 c5)
	(next c5 c6)
	(next c6 c7)
	(next c7 c8)
	(next c8 c9)
	(next c9 c10)
	
        (next o0 o1)
	(next o1 o2)
	(next o2 o3)
	(next o3 o4)
	(next o4 o5)
	(next o5 o6)
	(next o6 o7)
	(next o7 o8)
	(next o8 o9)
	(next o9 o10)
	
	(ready-to-use machine0)
	(has-flour kitchen0 f15)
	(ready-mix kitchen0 m4)
	(ready-dough kitchen0 d0)
	(loaf-bread kitchen0 b0)
	(breakfast-bun kitchen0 k0)
	(cooked-bun kitchen0 c0)
	(cooked-bread kitchen0 o4)
	(= (energy) 0)
	(= (pollution) 0)
	(= (labour) 0)
    )
    (:goal (and
	(cooked-bun kitchen0 c10) ; >=
        (breakfast-bun kitchen0 k5) ; >=
	(cooked-bread kitchen0 o10) ; >=
	(ready-dough kitchen0 d2) ; >=
	   )
    )
    (:metric minimize (energy))
    (:metric minimize (labour))
    (:metric minimize (pollution))
)
