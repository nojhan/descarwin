
(define (problem MULTIZENO-A)
 (:domain  multi-zeno-travel)
 (:objects plane1 plane2 - aircraft
	   person1 person2 person3 - person
           city0 city1 city2 city3 city4 - city)

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


; Optimal Pareto Front:

;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city1 person2) [2] [30]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  2: (fly plane2 city1 city4 person1) [2] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  4: (flyvide plane2 city4 city1)     [2] [30]
;  6: (fly plane1 city1 city4 person3) [2] [0]
;  6: (fly plane2 city1 city4 person2) [2] [0]
; Makespan = 8
; Cost = 120

;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city2 person3) [4] [20]
;  8: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city2 city4 person3) [4] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; Makespan = 12
; Cost = 100

;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  4: (fly plane2 city2 city4 person1) [4] [0]
;  8: (fly plane1 city0 city2 person3) [4] [20]
;  8: (flyvide plane2 city4 city2)     [4] [20]
; 12: (fly plane1 city2 city4 person3) [4] [0]
; 12: (fly plane2 city2 city4 person2) [4] [0]
; Makespan = 16
; Cost = 80

;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  6: (fly plane2 city3 city4 person2) [6] [0]
;  8: (fly plane1 city0 city3 person3) [6] [10]
; 12: (flyvide plane2 city4 city2)     [4] [20]
; 14: (fly plane1 city3 city4 person3) [6] [0]
; 16: (fly plane2 city2 city4 person1) [4] [0]
; Makespan = 20
; Cost = 60

;  0: (fly plane1 city0 city3 person1) [6] [10]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  6: (flyvide plane1 city3 city0)     [6] [0]
;  6: (fly plane2 city3 city4 person1) [6] [0]
; 12: (fly plane1 city0 city3 person3) [6] [10]
; 12: (flyvide plane2 city4 city3)     [6] [10]
; 18: (fly plane1 city3 city4 person3) [6] [0]
; 18: (fly plane2 city3 city4 person2) [6] [0]
; Makespan = 24
; Cost = 40

