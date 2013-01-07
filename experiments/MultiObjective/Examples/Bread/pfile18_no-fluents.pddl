(define (problem prob18_no-fluents)
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
	
        m0 - mlevel
	m1 - mlevel
	m2 - mlevel
	m3 - mlevel
	m4 - mlevel
	m5 - mlevel
	m6 - mlevel
	m7 - mlevel
	m8 - mlevel
	m9 - mlevel
	m10 - mlevel
	m11 - mlevel
	m12 - mlevel
	m13 - mlevel
	m14 - mlevel
	m15 - mlevel
	m16 - mlevel
	m17 - mlevel
	m18 - mlevel
	m19 - mlevel
	m20 - mlevel
	
        d0 - dlevel
	d1 - dlevel
	d2 - dlevel
	d3 - dlevel
	d4 - dlevel
	d5 - dlevel
	d6 - dlevel
	d7 - dlevel
	d8 - dlevel
	d9 - dlevel
	d10 - dlevel

        b0 - blevel
	b1 - blevel
	b2 - blevel
	b3 - blevel
	b4 - blevel
	b5 - blevel
	b6 - blevel
        b7 - blevel
	b8 - blevel
	b9 - blevel
	b10 - blevel
	b11 - blevel
	b12 - blevel
	b13 - blevel
	b14 - blevel
	b15 - blevel
	b16 - blevel

        k0 - klevel
	k1 - klevel
	k2 - klevel
	k3 - klevel
	k4 - klevel
	k5 - klevel
	k6 - klevel
	k7 - klevel
	k8 - klevel
	k9 - klevel
	k10 - klevel
	k11 - klevel
	k12 - klevel
	k13 - klevel
	k14 - klevel
	k15 - klevel
		
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
	c11 - clevel
	c12 - clevel
	c13 - clevel
	c14 - clevel
	c15 - clevel
	c16 - clevel
	c17 - clevel
	c18 - clevel
	c19 - clevel
	c20 - clevel
		
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
	o11 - olevel
	o12 - olevel
	o13 - olevel
	o14 - olevel
	o15 - olevel
	o16 - olevel
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

        (next m0 m1)
	(next m1 m2)
	(next m2 m3)
	(next m3 m4)
	(next m4 m5)
	(next m5 m6)
        (next m6 m7)
	(next m7 m8)
	(next m8 m9)
	(next m9 m10)
	(next m10 m11)
	(next m11 m12)
	(next m12 m13)
	(next m13 m14)
	(next m14 m15)
	(next m15 m16)
	(next m16 m17)
	(next m17 m18)
	(next m18 m19)
	(next m19 m20)
	
        (next d0 d1)
	(next d1 d2)
	(next d2 d3)
	(next d3 d4)
	(next d4 d5)
	(next d5 d6)
	(next d6 d7)
	(next d7 d8)
	(next d8 d9)
	(next d9 d10)
	
        (next b0 b1)
	(next b1 b2)
	(next b2 b3)
	(next b3 b4)
	(next b4 b5)
	(next b5 b6)
        (next b6 b7)
	(next b7 b8)
	(next b8 b9)
	(next b9 b10)
	(next b10 b11)
	(next b11 b12)
	(next b12 b13)
	(next b13 b14)
	(next b14 b15)
	(next b15 b16)

        (next k0 k1)
	(next k1 k2)
	(next k2 k3)
	(next k3 k4)
	(next k4 k5)
	(next k5 k6)
        (next k6 k7)
	(next k7 k8)
	(next k8 k9)
	(next k9 k10)
	(next k10 k11)
	(next k11 k12)
	(next k12 k13)
	(next k13 k14)
	(next k14 k15)

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
	(next c10 c11)
	(next c11 c12)
	(next c12 c13)
	(next c13 c14)
	(next c14 c15)
	(next c15 c16)
	(next c16 c17)
	(next c17 c18)
	(next c18 c19)
	(next c19 c20)
	
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
	(next o10 o11)
	(next o11 o12)
	(next o12 o13)
	(next o13 o14)
	(next o14 o15)
	(next o15 o16)
	
	(ready-to-use machine0)
	(has-flour kitchen0 f10)
	(ready-mix kitchen0 m20)
	(ready-dough kitchen0 d0)
	(loaf-bread kitchen0 b0)
	(breakfast-bun kitchen0 k0)
	(cooked-bun kitchen0 c0)
	(cooked-bread kitchen0 o0)
	(= (energy) 0)
	(= (pollution) 0)
	(= (labour) 0)
    )
    (:goal (and
	(cooked-bun kitchen0 c20) ; >=
        (breakfast-bun kitchen0 k15) ; >=
	(loaf-bread kitchen0 b16) ; >=
	(ready-dough kitchen0 d10) ; >=
	   )
    )
    (:metric minimize (energy))
    (:metric minimize (labour))
    (:metric minimize (pollution))
)
