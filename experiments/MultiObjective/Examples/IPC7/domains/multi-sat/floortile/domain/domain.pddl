
(define (domain floor-tile)
 (:requirements :typing :durative-actions :action-costs)
 (:types robot tile color - object)

(:predicates 	
		(robot-at ?r - robot ?x - tile)
		(up ?x - tile ?y - tile)
		(down ?x - tile ?y - tile)
		(right ?x - tile ?y - tile)
		(left ?x - tile ?y - tile)		
		(clear ?x - tile)
                (painted ?x - tile ?c - color)
		(robot-has ?r - robot ?c - color)
                (available-color ?c - color)
                (free-color ?r - robot))

(:functions (total-cost))

(:durative-action change-color
  :parameters (?r - robot ?c - color ?c2 - color)
  :duration (= ?duration 5)
  :condition (and (at start (robot-has ?r ?c))
  		  (over all (available-color ?c2)))
  :effect (and (at start (not (robot-has ?r ?c)))
  	       (at end (robot-has ?r ?c2))
               (increase (total-cost) (/ 1 5))))

(:durative-action paint-up
  :parameters (?r - robot ?y - tile ?x - tile ?c - color)
  :duration (= ?duration 2)
  :condition (and (over all (robot-has ?r ?c))
  		  (at start (robot-at ?r ?x))
		  (over all (up ?y ?x))
		  (at start (clear ?y)))
  :effect (and (at start (not (clear ?y)))
  	       (at end (painted ?y ?c))
               (increase (total-cost) (/ 1 2))))

(:durative-action paint-down
  :parameters (?r - robot ?y - tile ?x - tile ?c - color)
  :duration (= ?duration 2)
  :condition (and (over all (robot-has ?r ?c))
  		  (at start (robot-at ?r ?x))
		  (over all (down ?y ?x))
		  (at start (clear ?y)))
  :effect (and (at start (not (clear ?y)))
  	       (at end (painted ?y ?c))
               (increase (total-cost) (/ 1 2))))


(:durative-action up 
  :parameters (?r - robot ?x - tile ?y - tile)
  :duration (= ?duration 3)
  :condition (and (at start (robot-at ?r ?x)) 
  		  (over all (up ?y ?x)) 
		  (at start (clear ?y)))
  :effect (and 
  	       (at start (not (robot-at ?r ?x)))
	       (at end (robot-at ?r ?y))
	       (at start (not (clear ?y)))
               (at end (clear ?x))
               (increase (total-cost) (/ 1 3))))

(:durative-action down 
  :parameters (?r - robot ?x - tile ?y - tile)
  :duration (= ?duration 1)
  :condition (and (at start (robot-at ?r ?x))
  		  (over all (down ?y ?x)) 
		  (at start (clear ?y)))
  :effect (and (at start (not (robot-at ?r ?x)))
  	       (at end (robot-at ?r ?y))
	       (at start (not (clear ?y)))
               (at end (clear ?x))
               (increase (total-cost) 1)))

(:durative-action right 
  :parameters (?r - robot ?x - tile ?y - tile)
  :duration (= ?duration 1)
  :condition (and (at start (robot-at ?r ?x))
  		  (over all (right ?y ?x))
		  (at start (clear ?y)))
  :effect (and (at start (not (robot-at ?r ?x)))
  	       (at end (robot-at ?r ?y))
	       (at start (not (clear ?y)))
               (at end (clear ?x))
               (increase (total-cost) 1)))

(:durative-action left 
  :parameters (?r - robot ?x - tile ?y - tile)
  :duration (= ?duration 1)
  :condition (and (at start (robot-at ?r ?x)) 
  		  (over all (left ?y ?x)) 
		  (at start (clear ?y)))
  :effect (and (at start (not (robot-at ?r ?x)))
  	       (at end (robot-at ?r ?y))
	       (at start (not (clear ?y)))
               (at end (clear ?x))
               (increase (total-cost) 1)))
)