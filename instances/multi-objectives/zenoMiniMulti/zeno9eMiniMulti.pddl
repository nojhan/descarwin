; MULTIZENO Scénario C (IJCAI 2013)

(define (problem MULTIZENO-C)
 (:domain multi-zeno-travel)

 (:objects plane1 plane2 - aircraft
	   person1 person2 person3 person4 person5 person6 person7 person8 person9 - person
           city0 city1 city2 city3 city4 - city)

 (:init	(= (total-cost) 0)
	(= (citythreat city0) 0)
	(= (citythreat city1) 30)
	(= (citythreat city2) 29)
	(= (citythreat city3) 10)
	(= (citythreat city4) 0)

        (= (timeTerre city0 city1) 2)
        (= (timeTerre city0 city2) 3)
        (= (timeTerre city0 city3) 4)
        (= (timeTerre city1 city2) 1)
        (= (timeTerre city1 city3) 2)
        (= (timeTerre city2 city3) 1)
        (= (timeTerre city1 city4) 2)
        (= (timeTerre city2 city4) 3)
        (= (timeTerre city3 city4) 4)

        (= (timeTerre city1 city0) 2)
        (= (timeTerre city2 city0) 3)
        (= (timeTerre city3 city0) 4)
        (= (timeTerre city2 city1) 1)
        (= (timeTerre city3 city1) 2)
        (= (timeTerre city3 city2) 1)
        (= (timeTerre city4 city1) 2)
        (= (timeTerre city4 city2) 3)
        (= (timeTerre city4 city3) 4)

	(at plane1 city0) (at plane2 city0)
        (libre plane1) (libre plane2)
	(at person1 city0) (at person2 city0) (at person3 city0)
	(at person4 city0) (at person5 city0) (at person6 city0)
	(at person7 city0) (at person8 city0) (at person9 city0))

 (:goal (and (at person1 city4) (at person2 city4) (at person3 city4)
	     (at person4 city4) (at person5 city4) (at person6 city4)
	     (at person7 city4) (at person8 city4) (at person9 city4)))

 (:metric (and (minimize (total-time)) (minimize (total-cost)))))
