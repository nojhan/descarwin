(define (problem ZTRAVEL-3-8)
(:domain zeno-travel)
(:objects
	plane1
	plane2
	person1
	person2
	person3
	person4
	person5
	person6
	city0
	city1
	city2
	city3
	city4
	)
(:init
	(= (total-cost) 0)
	(= (citythreat city0) 0)
	(= (citythreat city1) 3)
	(= (citythreat city2) 2)
	(= (citythreat city3) 1)
	(= (citythreat city4) 0)

        (= (timeTerre city0 city1) 2) (path city0 city1)
        (= (timeTerre city0 city2) 4) (path city0 city2)
        (= (timeTerre city0 city3) 6) (path city0 city3)
        (= (timeTerre city1 city4) 2) (path city1 city4)
        (= (timeTerre city2 city4) 4) (path city2 city4)
        (= (timeTerre city3 city4) 6) (path city3 city4)

        (= (timeTerre city1 city0) 2) (path city1 city0)
        (= (timeTerre city2 city0) 4) (path city2 city0)
        (= (timeTerre city3 city0) 6) (path city3 city0)
        (= (timeTerre city4 city1) 2) (path city4 city1)
        (= (timeTerre city4 city2) 4) (path city4 city2)
        (= (timeTerre city4 city3) 6) (path city4 city3)

        (= (timeTerre city1 city2) 3) (path city1 city2)
        (= (timeTerre city1 city3) 5) (path city1 city3)
        (= (timeTerre city2 city3) 3) (path city2 city3)

        (= (timeTerre city2 city1) 3) (path city2 city1)
        (= (timeTerre city3 city1) 5) (path city3 city1)
        (= (timeTerre city3 city2) 3) (path city3 city2)

	(at plane1 city0)
	(aircraft plane1)
	(at plane2 city0)
	(aircraft plane2)
	(at person1 city0)
	(person person1)
	(at person2 city0)
	(person person2)
	(at person3 city0)
	(person person3)
	(at person4 city0)
	(person person4)
	(at person5 city0)
	(person person5)
	(at person6 city0)
	(person person6)
	(city city0)
	(city city1)
	(city city2)
	(city city3)
	(city city4)
        (libre plane1)
        (libre plane2)
)
(:goal (and
	(at person1 city4)
	(at person2 city4)
	(at person3 city4)
	(at person4 city4)
	(at person5 city4)
	(at person6 city4)
	))


