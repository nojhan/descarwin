; MULTIZENO Scénario A (IJCAI 2013)

(define (problem MULTIZENO-A)
 (:domain  multi-zeno-travel)
 (:objects plane1 - aircraft
           plane2 - aircraft
	   person1 - person
	   person2 - person
	   person3 - person
           city0 - city
	   city1 - city
	   city2 - city
	   city3 - city
	   city4 - city)

 (:init	(= (total-cost) 0)
	(= (citythreat city0) 0)
	(= (citythreat city1) 30)
	(= (citythreat city2) 20)
	(= (citythreat city3) 10)
	(= (citythreat city4) 0)

        (= (timeTerre city0 city1) 2)
        (= (timeTerre city0 city2) 4)
        (= (timeTerre city0 city3) 6)
        (= (timeTerre city1 city2) 3)
        (= (timeTerre city1 city3) 5)
        (= (timeTerre city2 city3) 3)
        (= (timeTerre city1 city4) 2)
        (= (timeTerre city2 city4) 4)
        (= (timeTerre city3 city4) 6)

        (= (timeTerre city1 city0) 2)
        (= (timeTerre city2 city0) 4)
        (= (timeTerre city3 city0) 6)
        (= (timeTerre city2 city1) 3)
        (= (timeTerre city3 city1) 5)
        (= (timeTerre city3 city2) 3)
        (= (timeTerre city4 city1) 2)
        (= (timeTerre city4 city2) 4)
        (= (timeTerre city4 city3) 6)

	(at plane1 city0) (at plane2 city0)
        (libre plane1) (libre plane2)
	(at person1 city0) (at person2 city0) (at person3 city0))

 (:goal (and (at person1 city4) (at person2 city4) (at person3 city4)))

 (:metric (and (minimize (total-time)) (minimize (total-cost)))))
