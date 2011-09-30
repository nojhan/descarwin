(define (problem elevators-sequencedstrips-p16_24_1)
(:domain elevators-tempo-cost)

(:objects 
n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16  - count
p0 p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 p17 p18 p19 p20 p21 p22 p23  - passenger
fast0 fast1  - fast-elevator
slow0-0 slow1-0 - slow-elevator
)

(:init
(next n0 n1) (next n1 n2) (next n2 n3) (next n3 n4) (next n4 n5) (next n5 n6) (next n6 n7) (next n7 n8) (next n8 n9) (next n9 n10) (next n10 n11) (next n11 n12) (next n12 n13) (next n13 n14) (next n14 n15) (next n15 n16) 

(above n0 n1) (above n0 n2) (above n0 n3) (above n0 n4) (above n0 n5) (above n0 n6) (above n0 n7) (above n0 n8) (above n0 n9) (above n0 n10) (above n0 n11) (above n0 n12) (above n0 n13) (above n0 n14) (above n0 n15) (above n0 n16) 
(above n1 n2) (above n1 n3) (above n1 n4) (above n1 n5) (above n1 n6) (above n1 n7) (above n1 n8) (above n1 n9) (above n1 n10) (above n1 n11) (above n1 n12) (above n1 n13) (above n1 n14) (above n1 n15) (above n1 n16) 
(above n2 n3) (above n2 n4) (above n2 n5) (above n2 n6) (above n2 n7) (above n2 n8) (above n2 n9) (above n2 n10) (above n2 n11) (above n2 n12) (above n2 n13) (above n2 n14) (above n2 n15) (above n2 n16) 
(above n3 n4) (above n3 n5) (above n3 n6) (above n3 n7) (above n3 n8) (above n3 n9) (above n3 n10) (above n3 n11) (above n3 n12) (above n3 n13) (above n3 n14) (above n3 n15) (above n3 n16) 
(above n4 n5) (above n4 n6) (above n4 n7) (above n4 n8) (above n4 n9) (above n4 n10) (above n4 n11) (above n4 n12) (above n4 n13) (above n4 n14) (above n4 n15) (above n4 n16) 
(above n5 n6) (above n5 n7) (above n5 n8) (above n5 n9) (above n5 n10) (above n5 n11) (above n5 n12) (above n5 n13) (above n5 n14) (above n5 n15) (above n5 n16) 
(above n6 n7) (above n6 n8) (above n6 n9) (above n6 n10) (above n6 n11) (above n6 n12) (above n6 n13) (above n6 n14) (above n6 n15) (above n6 n16) 
(above n7 n8) (above n7 n9) (above n7 n10) (above n7 n11) (above n7 n12) (above n7 n13) (above n7 n14) (above n7 n15) (above n7 n16) 
(above n8 n9) (above n8 n10) (above n8 n11) (above n8 n12) (above n8 n13) (above n8 n14) (above n8 n15) (above n8 n16) 
(above n9 n10) (above n9 n11) (above n9 n12) (above n9 n13) (above n9 n14) (above n9 n15) (above n9 n16) 
(above n10 n11) (above n10 n12) (above n10 n13) (above n10 n14) (above n10 n15) (above n10 n16) 
(above n11 n12) (above n11 n13) (above n11 n14) (above n11 n15) (above n11 n16) 
(above n12 n13) (above n12 n14) (above n12 n15) (above n12 n16) 
(above n13 n14) (above n13 n15) (above n13 n16) 
(above n14 n15) (above n14 n16) 
(above n15 n16) 

(lift-at fast0 n4)
(passengers fast0 n0)
(can-hold fast0 n1) (can-hold fast0 n2) (can-hold fast0 n3) (can-hold fast0 n4) 
(reachable-floor fast0 n0)(reachable-floor fast0 n4)(reachable-floor fast0 n8)(reachable-floor fast0 n12)(reachable-floor fast0 n16)

(lift-at fast1 n8)
(passengers fast1 n0)
(can-hold fast1 n1) (can-hold fast1 n2) (can-hold fast1 n3) (can-hold fast1 n4) 
(reachable-floor fast1 n0)(reachable-floor fast1 n4)(reachable-floor fast1 n8)(reachable-floor fast1 n12)(reachable-floor fast1 n16)

(lift-at slow0-0 n1)
(passengers slow0-0 n0)
(can-hold slow0-0 n1) (can-hold slow0-0 n2) (can-hold slow0-0 n3) 
(reachable-floor slow0-0 n0)(reachable-floor slow0-0 n1)(reachable-floor slow0-0 n2)(reachable-floor slow0-0 n3)(reachable-floor slow0-0 n4)(reachable-floor slow0-0 n5)(reachable-floor slow0-0 n6)(reachable-floor slow0-0 n7)(reachable-floor slow0-0 n8)

(lift-at slow1-0 n16)
(passengers slow1-0 n0)
(can-hold slow1-0 n1) (can-hold slow1-0 n2) (can-hold slow1-0 n3) 
(reachable-floor slow1-0 n8)(reachable-floor slow1-0 n9)(reachable-floor slow1-0 n10)(reachable-floor slow1-0 n11)(reachable-floor slow1-0 n12)(reachable-floor slow1-0 n13)(reachable-floor slow1-0 n14)(reachable-floor slow1-0 n15)(reachable-floor slow1-0 n16)

(passenger-at p0 n12)
(passenger-at p1 n7)
(passenger-at p2 n5)
(passenger-at p3 n3)
(passenger-at p4 n4)
(passenger-at p5 n8)
(passenger-at p6 n0)
(passenger-at p7 n11)
(passenger-at p8 n15)
(passenger-at p9 n12)
(passenger-at p10 n11)
(passenger-at p11 n11)
(passenger-at p12 n15)
(passenger-at p13 n9)
(passenger-at p14 n2)
(passenger-at p15 n8)
(passenger-at p16 n11)
(passenger-at p17 n16)
(passenger-at p18 n12)
(passenger-at p19 n7)
(passenger-at p20 n4)
(passenger-at p21 n14)
(passenger-at p22 n0)
(passenger-at p23 n14)

(= (travel-slow-time n0 n1) 12) (= (travel-slow-time n0 n2) 20) (= (travel-slow-time n0 n3) 28) (= (travel-slow-time n0 n4) 36) (= (travel-slow-time n0 n5) 44) (= (travel-slow-time n0 n6) 52) (= (travel-slow-time n0 n7) 60) (= (travel-slow-time n0 n8) 68) (= (travel-slow-time n1 n2) 12) (= (travel-slow-time n1 n3) 20) (= (travel-slow-time n1 n4) 28) (= (travel-slow-time n1 n5) 36) (= (travel-slow-time n1 n6) 44) (= (travel-slow-time n1 n7) 52) (= (travel-slow-time n1 n8) 60) (= (travel-slow-time n2 n3) 12) (= (travel-slow-time n2 n4) 20) (= (travel-slow-time n2 n5) 28) (= (travel-slow-time n2 n6) 36) (= (travel-slow-time n2 n7) 44) (= (travel-slow-time n2 n8) 52) (= (travel-slow-time n3 n4) 12) (= (travel-slow-time n3 n5) 20) (= (travel-slow-time n3 n6) 28) (= (travel-slow-time n3 n7) 36) (= (travel-slow-time n3 n8) 44) (= (travel-slow-time n4 n5) 12) (= (travel-slow-time n4 n6) 20) (= (travel-slow-time n4 n7) 28) (= (travel-slow-time n4 n8) 36) (= (travel-slow-time n5 n6) 12) (= (travel-slow-time n5 n7) 20) (= (travel-slow-time n5 n8) 28) (= (travel-slow-time n6 n7) 12) (= (travel-slow-time n6 n8) 20) (= (travel-slow-time n7 n8) 12) 

(= (travel-slow-time n8 n9) 12) (= (travel-slow-time n8 n10) 20) (= (travel-slow-time n8 n11) 28) (= (travel-slow-time n8 n12) 36) (= (travel-slow-time n8 n13) 44) (= (travel-slow-time n8 n14) 52) (= (travel-slow-time n8 n15) 60) (= (travel-slow-time n8 n16) 68) (= (travel-slow-time n9 n10) 12) (= (travel-slow-time n9 n11) 20) (= (travel-slow-time n9 n12) 28) (= (travel-slow-time n9 n13) 36) (= (travel-slow-time n9 n14) 44) (= (travel-slow-time n9 n15) 52) (= (travel-slow-time n9 n16) 60) (= (travel-slow-time n10 n11) 12) (= (travel-slow-time n10 n12) 20) (= (travel-slow-time n10 n13) 28) (= (travel-slow-time n10 n14) 36) (= (travel-slow-time n10 n15) 44) (= (travel-slow-time n10 n16) 52) (= (travel-slow-time n11 n12) 12) (= (travel-slow-time n11 n13) 20) (= (travel-slow-time n11 n14) 28) (= (travel-slow-time n11 n15) 36) (= (travel-slow-time n11 n16) 44) (= (travel-slow-time n12 n13) 12) (= (travel-slow-time n12 n14) 20) (= (travel-slow-time n12 n15) 28) (= (travel-slow-time n12 n16) 36) (= (travel-slow-time n13 n14) 12) (= (travel-slow-time n13 n15) 20) (= (travel-slow-time n13 n16) 28) (= (travel-slow-time n14 n15) 12) (= (travel-slow-time n14 n16) 20) (= (travel-slow-time n15 n16) 12) 


(= (travel-fast-time n0 n4) 13) (= (travel-fast-time n0 n8) 17) (= (travel-fast-time n0 n12) 20) (= (travel-fast-time n0 n16) 22) 

(= (travel-fast-time n4 n8) 13) (= (travel-fast-time n4 n12) 17) (= (travel-fast-time n4 n16) 20) 

(= (travel-fast-time n8 n12) 13) (= (travel-fast-time n8 n16) 17) 

(= (travel-fast-time n12 n16) 13) 

(= (travel-slow-cost n0 n1) 6) (= (travel-slow-cost n0 n2) 7) (= (travel-slow-cost n0 n3) 8) (= (travel-slow-cost n0 n4) 9) (= (travel-slow-cost n0 n5) 10) (= (travel-slow-cost n0 n6) 11) (= (travel-slow-cost n0 n7) 12) (= (travel-slow-cost n0 n8) 13) (= (travel-slow-cost n1 n2) 6) (= (travel-slow-cost n1 n3) 7) (= (travel-slow-cost n1 n4) 8) (= (travel-slow-cost n1 n5) 9) (= (travel-slow-cost n1 n6) 10) (= (travel-slow-cost n1 n7) 11) (= (travel-slow-cost n1 n8) 12) (= (travel-slow-cost n2 n3) 6) (= (travel-slow-cost n2 n4) 7) (= (travel-slow-cost n2 n5) 8) (= (travel-slow-cost n2 n6) 9) (= (travel-slow-cost n2 n7) 10) (= (travel-slow-cost n2 n8) 11) (= (travel-slow-cost n3 n4) 6) (= (travel-slow-cost n3 n5) 7) (= (travel-slow-cost n3 n6) 8) (= (travel-slow-cost n3 n7) 9) (= (travel-slow-cost n3 n8) 10) (= (travel-slow-cost n4 n5) 6) (= (travel-slow-cost n4 n6) 7) (= (travel-slow-cost n4 n7) 8) (= (travel-slow-cost n4 n8) 9) (= (travel-slow-cost n5 n6) 6) (= (travel-slow-cost n5 n7) 7) (= (travel-slow-cost n5 n8) 8) (= (travel-slow-cost n6 n7) 6) (= (travel-slow-cost n6 n8) 7) (= (travel-slow-cost n7 n8) 6) 

(= (travel-slow-cost n8 n9) 6) (= (travel-slow-cost n8 n10) 7) (= (travel-slow-cost n8 n11) 8) (= (travel-slow-cost n8 n12) 9) (= (travel-slow-cost n8 n13) 10) (= (travel-slow-cost n8 n14) 11) (= (travel-slow-cost n8 n15) 12) (= (travel-slow-cost n8 n16) 13) (= (travel-slow-cost n9 n10) 6) (= (travel-slow-cost n9 n11) 7) (= (travel-slow-cost n9 n12) 8) (= (travel-slow-cost n9 n13) 9) (= (travel-slow-cost n9 n14) 10) (= (travel-slow-cost n9 n15) 11) (= (travel-slow-cost n9 n16) 12) (= (travel-slow-cost n10 n11) 6) (= (travel-slow-cost n10 n12) 7) (= (travel-slow-cost n10 n13) 8) (= (travel-slow-cost n10 n14) 9) (= (travel-slow-cost n10 n15) 10) (= (travel-slow-cost n10 n16) 11) (= (travel-slow-cost n11 n12) 6) (= (travel-slow-cost n11 n13) 7) (= (travel-slow-cost n11 n14) 8) (= (travel-slow-cost n11 n15) 9) (= (travel-slow-cost n11 n16) 10) (= (travel-slow-cost n12 n13) 6) (= (travel-slow-cost n12 n14) 7) (= (travel-slow-cost n12 n15) 8) (= (travel-slow-cost n12 n16) 9) (= (travel-slow-cost n13 n14) 6) (= (travel-slow-cost n13 n15) 7) (= (travel-slow-cost n13 n16) 8) (= (travel-slow-cost n14 n15) 6) (= (travel-slow-cost n14 n16) 7) (= (travel-slow-cost n15 n16) 6) 


(= (travel-fast-cost n0 n4) 13) (= (travel-fast-cost n0 n8) 25) (= (travel-fast-cost n0 n12) 37) (= (travel-fast-cost n0 n16) 49) 

(= (travel-fast-cost n4 n8) 13) (= (travel-fast-cost n4 n12) 25) (= (travel-fast-cost n4 n16) 37) 

(= (travel-fast-cost n8 n12) 13) (= (travel-fast-cost n8 n16) 25) 

(= (travel-fast-cost n12 n16) 13) 

(= (total-cost) 0)

)

(:goal
(and
(passenger-at p0 n15)
(passenger-at p1 n12)
(passenger-at p2 n13)
(passenger-at p3 n14)
(passenger-at p4 n13)
(passenger-at p5 n14)
(passenger-at p6 n3)
(passenger-at p7 n13)
(passenger-at p8 n6)
(passenger-at p9 n11)
(passenger-at p10 n13)
(passenger-at p11 n10)
(passenger-at p12 n1)
(passenger-at p13 n15)
(passenger-at p14 n14)
(passenger-at p15 n13)
(passenger-at p16 n8)
(passenger-at p17 n2)
(passenger-at p18 n10)
(passenger-at p19 n14)
(passenger-at p20 n11)
(passenger-at p21 n3)
(passenger-at p22 n6)
(passenger-at p23 n3)
))

(:metric (and (minimize (total-time)) (minimize (total-cost))))

)
