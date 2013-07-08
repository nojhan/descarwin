; MULTIZENO Scénario A (IJCAI 2013)

(define (problem MULTIZENO-A)
 (:domain multi-zeno-travel)

 (:objects plane1 plane2 - aircraft
	   person1 person2 person3 person4 person5 person6 - person
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
        (at person1 city0) (at person2 city0) (at person3 city0)
	(at person4 city0) (at person5 city0) (at person6 city0))

 (:goal (and (at person1 city4) (at person2 city4) (at person3 city4)
	     (at person4 city4) (at person5 city4) (at person6 city4)))

 (:metric (and (minimize (total-time)) (minimize (total-cost)))))


; Optimal Pareto front:

;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city1 person2) [2] [30]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  2: (fly plane2 city1 city4 person1) [2] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  4: (flyvide plane2 city4 city1)     [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (fly plane2 city1 city4 person2) [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person3) [2] [0]
; 12: (fly plane1 city0 city1 person5) [2] [30]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (flyvide plane1 city1 city0)     [2] [0]
; 14: (fly plane2 city1 city4 person4) [2] [0]
; 16: (fly plane1 city0 city1 person6) [2] [30]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (fly plane1 city1 city4 person6) [2] [0]
; 18: (fly plane2 city1 city4 person5) [2] [0]
; Makespan = 20
; Cost = 300


;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city1 person5) [2] [30]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (flyvide plane1 city1 city0)     [2] [0]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (fly plane1 city0 city2 person6) [4] [20]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (fly plane2 city1 city4 person4) [2] [0]
; 20: (fly plane1 city2 city4 person6) [4] [0]
; 20: (flyvide plane2 city4 city1)     [2] [30]
; 22: (fly plane2 city1 city4 person5) [2] [0]
; Makespan = 24
; Cost = 280


;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city2 person5) [4] [20]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (flyvide plane1 city2 city0)     [4] [0]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (fly plane2 city1 city4 person4) [2] [0]
; 20: (fly plane1 city0 city2 person6) [4] [20]
; 20: (flyvide plane2 city4 city2)     [4] [20]
; 24: (fly plane1 city2 city4 person6) [4] [0]
; 24: (fly plane2 city2 city4 person5) [4] [0]
; Makespan = 28
; Cost = 260


;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city2 person4) [4] [20]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (flyvide plane1 city2 city0)     [4] [0]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (fly plane1 city0 city2 person5) [4] [20]
; 16: (flyvide plane2 city4 city2)     [4] [20]
; 20: (flyvide plane1 city2 city0)     [4] [0]
; 20: (fly plane2 city2 city4 person4) [4] [0]
; 24: (fly plane1 city0 city2 person6) [4] [20]
; 24: (flyvide plane2 city4 city2)     [4] [20]
; 28: (fly plane1 city2 city4 person6) [4] [0]
; 28: (fly plane2 city2 city4 person5) [4] [0]
; Makespan = 32
; Cost = 240


;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city2 person3) [4] [20]
;  8: (flyvide plane2 city4 city1)     [2] [30]
;  8: (flyvide plane1 city2 city0)     [4] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city2 person4) [4] [20]
; 12: (flyvide plane2 city4 city2)     [4] [20]
; 16: (flyvide plane1 city2 city0)     [4] [0]
; 16: (fly plane2 city2 city4 person3) [4] [0]
; 20: (fly plane1 city0 city2 person5) [4] [20]
; 20: (flyvide plane2 city4 city2)     [4] [20]
; 24: (flyvide plane1 city2 city0)     [4] [0]
; 24: (fly plane2 city2 city4 person4) [4] [0]
; 28: (fly plane1 city0 city2 person6) [4] [20]
; 28: (flyvide plane2 city4 city2)     [4] [20]
; 32: (fly plane1 city2 city4 person6) [4] [0]
; 32: (fly plane2 city2 city4 person5) [4] [0]
; Makespan = 36
; Cost = 220


;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  4: (fly plane2 city2 city4 person1) [4] [0]
;  8: (fly plane1 city0 city2 person3) [4] [20]
;  8: (flyvide plane2 city4 city2)     [4] [20]
; 12: (flyvide plane1 city2 city0)     [4] [0]
; 12: (fly plane2 city2 city4 person2) [4] [0]
; 16: (fly plane1 city0 city2 person4) [4] [20]
; 16: (flyvide plane2 city4 city2)     [4] [20]
; 20: (flyvide plane1 city2 city0)     [4] [0]
; 20: (fly plane2 city2 city4 person3) [4] [0]
; 24: (fly plane1 city0 city2 person5) [4] [20]
; 24: (flyvide plane2 city4 city2)     [4] [20]
; 28: (flyvide plane1 city2 city0)     [4] [0]
; 28: (fly plane2 city2 city4 person4) [4] [0]
; 32: (fly plane1 city0 city2 person6) [4] [20]
; 32: (flyvide plane2 city4 city2)     [4] [20]
; 36: (fly plane1 city2 city4 person6) [4] [0]
; 36: (fly plane2 city2 city4 person5) [4] [0]
; Makespan = 40
; Cost = 200


;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  6: (fly plane2 city3 city4 person2) [6] [0]
;  8: (fly plane1 city0 city2 person3) [4] [20]
; 12: (flyvide plane1 city2 city0)     [4] [0]
; 12: (flyvide plane2 city4 city2)     [4] [20]
; 16: (fly plane1 city0 city2 person4) [4] [20]
; 16: (fly plane2 city2 city4 person1) [4] [0]
; 20: (flyvide plane1 city2 city0)     [4] [0]
; 20: (flyvide plane2 city4 city2)     [4] [20]
; 24: (fly plane1 city0 city2 person5) [4] [20]
; 24: (fly plane2 city2 city4 person3) [4] [0]
; 28: (flyvide plane1 city2 city0)     [4] [0]
; 28: (flyvide plane2 city4 city2)     [4] [20]
; 32: (fly plane1 city0 city3 person6) [6] [10]
; 32: (fly plane2 city2 city4 person4) [4] [0]
; 36: (flyvide plane2 city4 city2)     [4] [20]
; 38: (fly plane1 city3 city4 person6) [6] [0]
; 40: (fly plane2 city2 city4 person5) [4] [0]
; Makespan = 44
; Cost = 180


;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  6: (fly plane2 city3 city4 person2) [6] [0]
;  8: (fly plane1 city0 city2 person3) [4] [20]
; 12: (flyvide plane1 city2 city0)     [4] [0]
; 12: (flyvide plane2 city4 city2)     [4] [20]
; 16: (fly plane1 city0 city2 person4) [4] [20]
; 16: (fly plane2 city2 city4 person1) [4] [0]
; 20: (flyvide plane1 city2 city0)     [4] [0]
; 20: (flyvide plane2 city4 city2)     [4] [20]
; 24: (fly plane1 city0 city3 person5) [6] [10]
; 24: (fly plane2 city2 city4 person3) [4] [0]
; 28: (flyvide plane2 city4 city2)     [4] [20]
; 30: (flyvide plane1 city3 city0)     [6] [0]
; 32: (fly plane2 city2 city4 person4) [4] [0]
; 36: (fly plane1 city0 city3 person6) [6] [10]
; 36: (flyvide plane2 city4 city3)     [6] [10]
; 42: (fly plane1 city3 city4 person6) [6] [0]
; 42: (fly plane2 city3 city4 person5) [6] [0]
; Makespan = 48
; Cost = 160


;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  6: (fly plane2 city3 city4 person2) [6] [0]
;  8: (fly plane1 city0 city2 person3) [4] [20]
; 12: (flyvide plane1 city2 city0)     [4] [0]
; 12: (flyvide plane2 city4 city2)     [4] [20]
; 16: (fly plane1 city0 city3 person4) [6] [10]
; 16: (fly plane2 city2 city4 person1) [4] [0]
; 20: (flyvide plane2 city4 city2)     [4] [20]
; 22: (flyvide plane1 city3 city0)     [6] [0]
; 24: (fly plane2 city2 city4 person3) [4] [0]
; 28: (fly plane1 city0 city3 person5) [6] [10]
; 28: (flyvide plane2 city4 city3)     [6] [10]
; 34: (fly plane2 city3 city4 person4) [6] [0]
; 34: (flyvide plane1 city3 city0)     [6] [0]
; 40: (flyvide plane2 city4 city3)     [6] [10]
; 40: (fly plane1 city0 city3 person6) [6] [10]
; 46: (fly plane2 city3 city4 person5) [6] [0]
; 46: (fly plane1 city3 city4 person6) [6] [0]
; Makespan = 52
; Cost = 140


;  0: (fly plane1 city0 city2 person1) [4] [20]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  4: (flyvide plane1 city2 city0)     [4] [0]
;  6: (fly plane2 city3 city4 person2) [6] [0]
;  8: (fly plane1 city0 city3 person3) [6] [10]
; 12: (flyvide plane2 city4 city2)     [4] [20]
; 14: (flyvide plane1 city3 city0)     [6] [0]
; 16: (fly plane2 city2 city4 person1) [4] [0]
; 20: (fly plane1 city0 city3 person4) [6] [10]
; 20: (flyvide plane2 city4 city3)     [6] [10]
; 26: (flyvide plane1 city3 city0)     [6] [0]
; 26: (fly plane2 city3 city4 person3) [6] [0]
; 32: (fly plane1 city0 city3 person5) [6] [10]
; 32: (flyvide plane2 city4 city3)     [6] [10]
; 38: (flyvide plane1 city3 city0)     [6] [0]
; 38: (fly plane2 city3 city4 person4) [6] [0]
; 44: (fly plane1 city0 city3 person6) [6] [10]
; 44: (flyvide plane2 city4 city3)     [6] [10]
; 50: (fly plane1 city3 city4 person6) [6] [0]
; 50: (fly plane2 city3 city4 person5) [6] [0]
; Makespan = 56
; Cost = 120


;  0: (fly plane1 city0 city3 person1) [6] [10]
;  0: (fly plane2 city0 city3 person2) [6] [10]
;  6: (flyvide plane1 city3 city0)     [6] [0]
;  6: (fly plane2 city3 city4 person1) [6] [0]
; 12: (fly plane1 city0 city3 person3) [6] [10]
; 12: (flyvide plane2 city4 city3)     [6] [10]
; 18: (flyvide plane1 city3 city0)     [6] [0]
; 18: (fly plane2 city3 city4 person2) [6] [0]
; 24: (fly plane1 city0 city3 person4) [6] [10]
; 24: (flyvide plane2 city4 city3)     [6] [10]
; 30: (flyvide plane1 city3 city0)     [6] [0]
; 30: (fly plane2 city3 city4 person3) [6] [0]
; 36: (fly plane1 city0 city3 person5) [6] [10]
; 36: (flyvide plane2 city4 city3)     [6] [10]
; 42: (flyvide plane1 city3 city0)     [6] [0]
; 42: (fly plane2 city3 city4 person4) [6] [0]
; 48: (fly plane1 city0 city3 person6) [6] [10]
; 48: (flyvide plane2 city4 city3)     [6] [10]
; 54: (fly plane1 city3 city4 person6) [6] [0]
; 54: (fly plane2 city3 city4 person5) [6] [0]
; Makespan = 60
; Cost = 100


;=================== Dominated solutions ======

; dominated
; 6C1 3C2 1C3 
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city2 person5) [4] [20]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (flyvide plane1 city2 city0)     [4] [0]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (fly plane2 city1 city4 person4) [2] [0]
; 20: (fly plane1 city0 city3 person6) [6] [10]
; 20: (flyvide plane2 city4 city2)     [4] [20]
; 24: (fly plane2 city2 city4 person5) [4] [0]
; 26: (fly plane1 city3 city4 person6) [6] [0]
; Makespan = 32
; Cost = 250


; dominated
; 4C1 5C2 1C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city2 person4) [4] [20]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (flyvide plane1 city2 city0)     [4] [0]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (fly plane1 city0 city2 person5) [4] [20]
; 16: (flyvide plane2 city4 city2)     [4] [20]
; 20: (flyvide plane1 city2 city0)     [4] [0]
; 20: (fly plane2 city2 city4 person4) [4] [0]
; 24: (fly plane1 city0 city3 person6) [6] [10]
; 24: (flyvide plane2 city4 city2)     [4] [20]
; 28: (fly plane2 city2 city4 person5) [4] [0]
; 30: (fly plane1 city3 city4 person6) [6] [0]
; Makespan = 36
; Cost = 230


; dominated
; 6C1 1C2 3C3 
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [4] [20]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane2 city2 city4 person2) [4] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city3 person5) [6] [10]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (flyvide plane1 city3 city0)     [6] [0]
; 18: (fly plane2 city1 city4 person4) [2] [0]
; 20: (flyvide plane2 city4 city3)     [6] [10]
; 24: (fly plane1 city0 city3 person6) [6] [10]
; 26: (fly plane2 city3 city4 person5) [6] [0]
; 30: (fly plane1 city3 city4 person6) [6] [0]
; Makespan = 36
; Cost = 230