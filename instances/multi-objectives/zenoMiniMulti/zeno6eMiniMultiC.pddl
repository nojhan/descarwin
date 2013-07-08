; MULTIZENO Scénario C (IJCAI 2013)

(define (problem MULTIZENO-C)
 (:domain multi-zeno-travel)

 (:objects plane1 plane2 - aircraft
	   person1 person2 person3 person4 person5 person6 - person
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
	(at person4 city0) (at person5 city0) (at person6 city0))

 (:goal (and (at person1 city4) (at person2 city4) (at person3 city4)
	     (at person4 city4) (at person5 city4) (at person6 city4)))

 (:metric (and (minimize (total-time)) (minimize (total-cost)))))

; Optimal Pareto front:

; 10C1 0C2 0C3
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


; 8C1 2C2 0C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (fly plane1 city0 city1 person5) [2] [30]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane1 city1 city0)     [2] [0]
; 14: (flyvide plane2 city4 city1)     [2] [30]
; 16: (fly plane1 city0 city2 person6) [3] [29]
; 16: (fly plane2 city1 city4 person4) [2] [0]
; 18: (flyvide plane2 city4 city1)     [2] [30]
; 19: (fly plane1 city2 city4 person6) [3] [0]
; 20: (fly plane2 city1 city4 person5) [2] [0]
; Makespan = 22
; Cost = 298




; 8C1 0C2 2C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city1 person5) [2] [30]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (flyvide plane1 city1 city0)     [2] [0]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (fly plane1 city0 city3 person6) [4] [10]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (fly plane2 city1 city4 person4) [2] [0]
; 20: (fly plane1 city3 city4 person6) [4] [0]
; 20: (flyvide plane2 city4 city1)     [2] [30]
; 22: (fly plane2 city1 city4 person5) [2] [0]
; Makespan = 24
; Cost = 260



; 6C1 2C2 2C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (fly plane1 city0 city3 person5) [4] [10]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane2 city4 city1)     [2] [30]
; 16: (flyvide plane1 city3 city0)     [4] [0]
; 16: (fly plane2 city1 city4 person4) [2] [0]
; 18: (flyvide plane2 city4 city3)     [4] [10]
; 20: (fly plane1 city0 city2 person6) [3] [29]
; 22: (fly plane2 city3 city4 person5) [4] [0]
; 23: (fly plane1 city2 city4 person6) [3] [0]
; Makespan = 26
; Cost = 258



; 6C1 0C2 4C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city3 person5) [4] [10]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (flyvide plane1 city3 city0)     [4] [0]
; 16: (flyvide plane2 city4 city1)     [2] [30]
; 18: (fly plane2 city1 city4 person4) [2] [0]
; 20: (fly plane1 city0 city3 person6) [4] [10]
; 20: (flyvide plane2 city4 city3)     [4] [10]
; 24: (fly plane1 city3 city4 person6) [4] [0]
; 24: (fly plane2 city3 city4 person5) [4] [0]
; Makespan = 28
; Cost = 220



; 4C1 2C2 4C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city3 person4) [4] [10]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (flyvide plane1 city3 city0)     [4] [0]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane2 city4 city3)     [4] [10]
; 16: (fly plane1 city0 city3 person5) [4] [10]
; 18: (fly plane2 city3 city4 person4) [4] [0]
; 20: (flyvide plane1 city3 city0)     [4] [0]
; 22: (flyvide plane2 city4 city3)     [4] [10]
; 24: (fly plane1 city0 city2 person6) [3] [29]
; 26: (fly plane2 city3 city4 person5) [4] [0]
; 27: (fly plane1 city2 city4 person6) [3] [0]
; Makespan = 30
; Cost = 218



; 4C1 0C2 6C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  8: (fly plane1 city0 city3 person4) [4] [10]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (flyvide plane1 city3 city0)     [4] [0]
; 12: (flyvide plane2 city4 city1)     [2] [30]
; 14: (fly plane2 city1 city4 person3) [2] [0]
; 16: (fly plane1 city0 city3 person5) [4] [10]
; 16: (flyvide plane2 city4 city3)     [4] [10]
; 20: (fly plane2 city3 city4 person4) [4] [0]
; 20: (flyvide plane1 city3 city0)     [4] [0]
; 24: (flyvide plane2 city4 city3)     [4] [10]
; 24: (fly plane1 city0 city3 person6) [4] [10]
; 28: (fly plane2 city3 city4 person5) [4] [0]
; 28: (fly plane1 city3 city4 person6) [4] [0]
; Makespan = 32
; Cost = 180


; 2C1 2C2 6C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city3 person3) [4] [10]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (flyvide plane1 city3 city0)     [4] [0]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane2 city4 city3)     [4] [10]
; 12: (fly plane1 city0 city3 person4) [4] [10]
; 14: (fly plane2 city3 city4 person3) [4] [0]
; 16: (flyvide plane1 city3 city0)     [4] [0]
; 18: (flyvide plane2 city4 city3)     [4] [10]
; 20: (fly plane1 city0 city3 person5) [4] [10]
; 22: (fly plane2 city3 city4 person4) [4] [0]
; 24: (flyvide plane1 city3 city0)     [4] [0]
; 26: (flyvide plane2 city4 city3)     [4] [10]
; 28: (fly plane1 city0 city2 person6) [3] [29]
; 30: (fly plane2 city3 city4 person5) [4] [0]
; 31: (fly plane1 city2 city4 person6) [3] [0]
; Makespan = 34
; Cost = 178


; 2C1 0C2 8C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  4: (fly plane1 city0 city3 person3) [4] [10]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  8: (flyvide plane1 city3 city0)     [4] [0]
;  8: (flyvide plane2 city4 city1)     [2] [30]
; 10: (fly plane2 city1 city4 person1) [2] [0]
; 12: (fly plane1 city0 city3 person4) [4] [10]
; 12: (flyvide plane2 city4 city3)     [4] [10]
; 16: (flyvide plane1 city3 city0)     [4] [0]
; 16: (fly plane2 city3 city4 person3) [4] [0]
; 20: (fly plane1 city0 city3 person5) [4] [10]
; 20: (flyvide plane2 city4 city3)     [4] [10]
; 24: (flyvide plane1 city3 city0)     [4] [0]
; 24: (fly plane2 city3 city4 person4) [4] [0]
; 28: (fly plane1 city0 city3 person6) [4] [10]
; 28: (flyvide plane2 city4 city3)     [4] [10]
; 32: (fly plane1 city3 city4 person6) [4] [0]
; 32: (fly plane2 city3 city4 person5) [4] [0]
; Makespan = 36
; Cost = 140



; 0C1 2C2 8C3
;  0: (fly plane1 city0 city2 person1) [3] [29]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  3: (flyvide plane1 city2 city0)     [3] [0]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (fly plane1 city0 city3 person3) [4] [10]
;  8: (flyvide plane2 city4 city2)     [3] [29]
; 10: (flyvide plane1 city3 city0)     [4] [0]
; 11: (fly plane2 city2 city4 person1) [3] [0]
; 14: (fly plane1 city0 city3 person4) [4] [10]
; 14: (flyvide plane2 city4 city3)     [4] [10]
; 18: (flyvide plane1 city3 city0)     [4] [0]
; 18: (fly plane2 city3 city4 person3) [4] [0]
; 22: (fly plane1 city0 city3 person5) [4] [10]
; 22: (flyvide plane2 city4 city3)     [4] [10]
; 26: (flyvide plane1 city3 city0)     [4] [0]
; 26: (fly plane2 city3 city4 person4) [4] [0]
; 30: (fly plane1 city0 city3 person6) [4] [10]
; 30: (flyvide plane2 city4 city3)     [4] [10]
; 34: (fly plane1 city3 city4 person6) [4] [0]
; 34: (fly plane2 city3 city4 person5) [4] [0]
; Makespan = 38
; Cost = 138



; 0C1 0C2 10C3
;  0: (fly plane1 city0 city3 person1) [4] [10]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  4: (flyvide plane1 city3 city0)     [4] [0]
;  4: (fly plane2 city3 city4 person1) [4] [0]
;  8: (fly plane1 city0 city3 person3) [4] [10]
;  8: (flyvide plane2 city4 city3)     [4] [10]
; 12: (flyvide plane1 city3 city0)     [4] [0]
; 12: (fly plane2 city3 city4 person2) [4] [0]
; 16: (fly plane1 city0 city3 person4) [4] [10]
; 16: (flyvide plane2 city4 city3)     [4] [10]
; 20: (flyvide plane1 city3 city0)     [4] [0]
; 20: (fly plane2 city3 city4 person3) [4] [0]
; 24: (fly plane1 city0 city3 person5) [4] [10]
; 24: (flyvide plane2 city4 city3)     [4] [10]
; 28: (flyvide plane1 city3 city0)     [4] [0]
; 28: (fly plane2 city3 city4 person4) [4] [0]
; 32: (fly plane1 city0 city3 person6) [4] [10]
; 32: (flyvide plane2 city4 city3)     [4] [10]
; 36: (fly plane1 city3 city4 person6) [4] [0]
; 36: (fly plane2 city3 city4 person5) [4] [0]
; Makespan = 40
; Cost = 100

;=================== Dominated solutions ======

; dominated
; 6C1 4C2 0C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (fly plane1 city0 city2 person5) [3] [29]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane2 city4 city1)     [2] [30]
; 15: (flyvide plane1 city2 city0)     [3] [0]
; 16: (fly plane2 city1 city4 person4) [2] [0]
; 18: (fly plane1 city0 city2 person6) [3] [29]
; 18: (flyvide plane2 city4 city2)     [3] [29]
; 21: (fly plane1 city2 city4 person6) [3] [0]
; 21: (fly plane2 city2 city4 person5) [3] [0]
; Makespan = 24
; Cost = 296


; dominated
; 4C1 6C2 0C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city2 person4) [3] [29]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 11: (flyvide plane1 city2 city0)     [3] [0]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (fly plane1 city0 city2 person5) [3] [29]
; 14: (flyvide plane2 city4 city2)     [3] [29]
; 17: (flyvide plane1 city2 city0)     [3] [0]
; 17: (fly plane2 city2 city4 person4) [3] [0]
; 20: (fly plane1 city0 city2 person6) [3] [29]
; 20: (flyvide plane2 city4 city2)     [3] [29]
; 23: (fly plane1 city2 city4 person6) [3] [0]
; 23: (fly plane2 city2 city4 person5) [3] [0]
; Makespan = 26
; Cost = 294


; dominated
; 2C1 8C2 0C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city2 person3) [3] [29]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  7: (flyvide plane1 city2 city0)     [3] [0]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (fly plane1 city0 city2 person4) [3] [29]
; 10: (flyvide plane2 city4 city2)     [3] [29]
; 13: (flyvide plane1 city2 city0)     [3] [0]
; 13: (fly plane2 city2 city4 person3) [3] [0]
; 16: (fly plane1 city0 city2 person5) [3] [29]
; 16: (flyvide plane2 city4 city2)     [3] [29]
; 19: (flyvide plane1 city2 city0)     [3] [0]
; 19: (fly plane2 city2 city4 person4) [3] [0]
; 22: (fly plane1 city0 city2 person6) [3] [29]
; 22: (flyvide plane2 city4 city2)     [3] [29]
; 25: (fly plane1 city2 city4 person6) [3] [0]
; 25: (fly plane2 city2 city4 person5) [3] [0]
; Makespan = 28
; Cost = 292


; dominated
; 0C1 10C2 0C3
;  0: (fly plane1 city0 city2 person1) [3] [29]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  3: (flyvide plane1 city2 city0)     [3] [0]
;  3: (fly plane2 city2 city4 person1) [3] [0]
;  6: (fly plane1 city0 city2 person3) [3] [29]
;  6: (flyvide plane2 city4 city2)     [3] [29]
;  9: (flyvide plane1 city2 city0)     [3] [0]
;  9: (fly plane2 city2 city4 person2) [3] [0]
; 12: (fly plane1 city0 city2 person4) [3] [29]
; 12: (flyvide plane2 city4 city2)     [3] [29]
; 15: (flyvide plane1 city2 city0)     [3] [0]
; 15: (fly plane2 city2 city4 person3) [3] [0]
; 18: (fly plane1 city0 city2 person5) [3] [29]
; 18: (flyvide plane2 city4 city2)     [3] [29]
; 21: (flyvide plane1 city2 city0)     [3] [0]
; 21: (fly plane2 city2 city4 person4) [3] [0]
; 24: (fly plane1 city0 city2 person6) [3] [29]
; 24: (flyvide plane2 city4 city2)     [3] [29]
; 27: (fly plane1 city2 city4 person6) [3] [0]
; 27: (fly plane2 city2 city4 person5) [3] [0]
; Makespan = 30
; Cost = 290


; dominated
; 0C1 8C2 2C3
;  0: (fly plane1 city0 city2 person1) [3] [29]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  3: (flyvide plane1 city2 city0)     [3] [0]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (fly plane1 city0 city2 person3) [3] [29]
;  8: (flyvide plane2 city4 city2)     [3] [29]
;  9: (flyvide plane1 city2 city0)     [3] [0]
; 11: (fly plane2 city2 city4 person1) [3] [0]
; 12: (fly plane1 city0 city2 person4) [3] [29]
; 14: (flyvide plane2 city4 city2)     [3] [29]
; 15: (flyvide plane1 city2 city0)     [3] [0]
; 17: (fly plane2 city2 city4 person3) [3] [0]
; 18: (fly plane1 city0 city2 person5) [3] [29]
; 20: (flyvide plane2 city4 city2)     [3] [29]
; 21: (flyvide plane1 city2 city0)     [3] [0]
; 23: (fly plane2 city2 city4 person4) [3] [0]
; 24: (fly plane1 city0 city3 person6) [4] [10]
; 26: (flyvide plane2 city4 city2)     [3] [29]
; 28: (fly plane1 city3 city4 person6) [4] [0]
; 29: (fly plane2 city2 city4 person5) [3] [0]
; Makespan = 32
; Cost = 252


; dominated
; 0C1 6C2 4C3
;  0: (fly plane1 city0 city2 person1) [3] [29]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  3: (flyvide plane1 city2 city0)     [3] [0]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (fly plane1 city0 city2 person3) [3] [29]
;  8: (flyvide plane2 city4 city2)     [3] [29]
;  9: (flyvide plane1 city2 city0)     [3] [0]
; 11: (fly plane2 city2 city4 person1) [3] [0]
; 12: (fly plane1 city0 city2 person4) [3] [29]
; 14: (flyvide plane2 city4 city2)     [3] [29]
; 15: (flyvide plane1 city2 city0)     [3] [0]
; 17: (fly plane2 city2 city4 person3) [3] [0]
; 18: (fly plane1 city0 city3 person5) [4] [10]
; 20: (flyvide plane2 city4 city2)     [3] [29]
; 22: (flyvide plane1 city3 city0)     [4] [0]
; 23: (fly plane2 city2 city4 person4) [3] [0]
; 26: (fly plane1 city0 city3 person6) [4] [10]
; 26: (flyvide plane2 city4 city3)     [4] [10]
; 30: (fly plane1 city3 city4 person6) [4] [0]
; 30: (fly plane2 city3 city4 person5) [4] [0]
; Makespan = 34
; Cost = 214


; dominated
; 0C1 4C2 6C3
;  0: (fly plane1 city0 city2 person1) [3] [29]
;  0: (fly plane2 city0 city3 person2) [4] [10]
;  3: (flyvide plane1 city2 city0)     [3] [0]
;  4: (fly plane2 city3 city4 person2) [4] [0]
;  6: (fly plane1 city0 city2 person3) [3] [29]
;  8: (flyvide plane2 city4 city2)     [3] [29]
;  9: (flyvide plane1 city2 city0)     [3] [0]
; 11: (fly plane2 city2 city4 person1) [3] [0]
; 12: (fly plane1 city0 city3 person4) [4] [10]
; 14: (flyvide plane2 city4 city2)     [3] [29]
; 16: (flyvide plane1 city3 city0)     [4] [0]
; 17: (fly plane2 city2 city4 person3) [3] [0]
; 20: (fly plane1 city0 city3 person5) [4] [10]
; 20: (flyvide plane2 city4 city3)     [4] [10]
; 24: (fly plane2 city3 city4 person4) [4] [0]
; 24: (flyvide plane1 city3 city0)     [4] [0]
; 28: (flyvide plane2 city4 city3)     [4] [10]
; 28: (fly plane1 city0 city3 person6) [4] [10]
; 32: (fly plane2 city3 city4 person5) [4] [0]
; 32: (fly plane1 city3 city4 person6) [4] [0]
; Makespan = 36
; Cost = 176

;---
; dominated
; 6C1 3C2 1C3 
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (fly plane1 city0 city2 person5) [3] [29]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane2 city4 city1)     [2] [30]
; 15: (flyvide plane1 city2 city0)     [3] [0]
; 16: (fly plane2 city1 city4 person4) [2] [0]
; 18: (fly plane1 city0 city3 person6) [4] [10]
; 18: (flyvide plane2 city4 city2)     [3] [29]
; 21: (fly plane2 city2 city4 person5) [3] [0]
; 22: (fly plane1 city3 city4 person6) [4] [0]
; Makespan = 26
; Cost = 277


; dominated
; 4C1 5C2 1C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city2 person4) [3] [29]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 11: (flyvide plane1 city2 city0)     [3] [0]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (fly plane1 city0 city2 person5) [3] [29]
; 14: (flyvide plane2 city4 city2)     [3] [29]
; 17: (flyvide plane1 city2 city0)     [3] [0]
; 17: (fly plane2 city2 city4 person4) [3] [0]
; 20: (fly plane1 city0 city3 person6) [4] [10]
; 20: (flyvide plane2 city4 city2)     [3] [29]
; 23: (fly plane2 city2 city4 person5) [3] [0]
; 24: (fly plane1 city3 city4 person6) [4] [0]
; Makespan = 28
; Cost = 275


; dominated
; 6C1 1C2 3C3 
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (fly plane1 city0 city3 person5) [4] [10]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane2 city4 city1)     [2] [30]
; 16: (flyvide plane1 city3 city0)     [4] [0]
; 16: (fly plane2 city1 city4 person4) [2] [0]
; 18: (flyvide plane2 city4 city3)     [4] [10]
; 20: (fly plane1 city0 city3 person6) [4] [10]
; 22: (fly plane2 city3 city4 person5) [4] [0]
; 24: (fly plane1 city3 city4 person6) [4] [0]
; Makespan = 28
; Cost = 239


; dominated
; 4C1 1C2 5C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city3 person4) [4] [10]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (flyvide plane1 city3 city0)     [4] [0]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane2 city4 city3)     [4] [10]
; 16: (fly plane1 city0 city3 person5) [4] [10]
; 18: (fly plane2 city3 city4 person4) [4] [0]
; 20: (flyvide plane1 city3 city0)     [4] [0]
; 22: (flyvide plane2 city4 city3)     [4] [10]
; 24: (fly plane1 city0 city3 person6) [4] [10]
; 26: (fly plane2 city3 city4 person5) [4] [0]
; 28: (fly plane1 city3 city4 person6) [4] [0]
; Makespan = 32
; Cost = 199

;--
; dominated
; 8C1 1C2 1C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city1 person3) [2] [30]
;  6: (flyvide plane1 city1 city0)     [2] [0]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  8: (fly plane1 city0 city1 person4) [2] [30]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane1 city1 city0)     [2] [0]
; 10: (flyvide plane2 city4 city1)     [2] [30]
; 12: (fly plane1 city0 city1 person5) [2] [30]
; 12: (fly plane2 city1 city4 person3) [2] [0]
; 14: (flyvide plane1 city1 city0)     [2] [0]
; 14: (flyvide plane2 city4 city1)     [2] [30]
; 16: (fly plane1 city0 city3 person6) [4] [10]
; 16: (fly plane2 city1 city4 person4) [2] [0]
; 18: (flyvide plane2 city4 city1)     [2] [30]
; 20: (fly plane1 city3 city4 person6) [4] [0]
; 20: (fly plane2 city1 city4 person5) [2] [0]
; Makespan = 24
; Cost = 279





; dominated
; 2C1 4C2 4C3
;  0: (fly plane1 city0 city1 person1) [2] [30]
;  0: (fly plane2 city0 city2 person2) [3] [29]
;  2: (flyvide plane1 city1 city0)     [2] [0]
;  3: (fly plane2 city2 city4 person2) [3] [0]
;  4: (fly plane1 city0 city2 person3) [3] [29]
;  6: (flyvide plane2 city4 city1)     [2] [30]
;  7: (flyvide plane1 city2 city0)     [3] [0]
;  8: (fly plane2 city1 city4 person1) [2] [0]
; 10: (flyvide plane2 city4 city2)     [3] [29]
; 10: (fly plane1 city0 city3 person4) [4] [10]
; 13: (fly plane2 city2 city4 person3) [3] [0]
; 14: (flyvide plane1 city3 city0)     [4] [0]
; 16: (flyvide plane2 city4 city3)     [4] [10]
; 18: (fly plane1 city0 city3 person5) [4] [10]
; 20: (fly plane2 city3 city4 person4) [4] [0]
; 22: (flyvide plane1 city3 city0)     [4] [0]
; 24: (flyvide plane2 city4 city3)     [4] [10]
; 26: (fly plane1 city0 city2 person6) [3] [29]
; 28: (fly plane2 city3 city4 person5) [4] [0]
; 29: (fly plane1 city2 city4 person6) [3] [0]
; Makespan = 32
; Cost = 216
