(define (domain CrewPlanning)
(:requirements :typing :durative-actions :action-costs)
(:types MedicalState FilterState CrewMember PayloadAct Day
          ExerEquipment RPCM - objects)

(:predicates
	(mcs_finished ?ps - MedicalState ?d - Day)

	(changed ?fs - FilterState ?d - Day)
	(available ?c - CrewMember)

	(done_sleep ?c - CrewMember ?d - Day)
	(done_pre_sleep ?c - CrewMember ?d - Day)
	(done_post_sleep ?c - CrewMember ?d - Day)
	(done_dpc ?c - CrewMember ?d - Day)
	(done_meal ?c - CrewMember ?d - Day)
	(done_exercise ?c - CrewMember ?d - Day)
	
	(done_remove_sleep_station ?ps - RPCM)
	(done_first_reconfigure_thermal_loop ?ps - RPCM)
	(done_replace_rpcm ?ps - RPCM)
	(done_assemble_sleep_station ?ps - RPCM)
	(done_second_reconfigure_thermal_loop ?ps - RPCM)
	(done_rpcm ?ps - RPCM ?d - Day)

	(payload_act_done ?pa - PayloadAct)
	(payload_act_completed ?pa - PayloadAct ?d - Day)

	(next ?d1 ?d2 - Day)
	(currentday ?c - CrewMember ?d - Day)
	(initiated ?d - Day)

	(unused ?e - ExerEquipment)
)


(:functions (total-cost))

(:durative-action initialize_day
 :parameters (?d1 ?d2 - Day)
 :duration (= ?duration 1440)
 :condition (and (at start (initiated ?d1))
	         (over all (next ?d1 ?d2)))
 :effect (and (at end (initiated ?d2))
              (increase (total-cost) (- 1441 1440)))
)



(:durative-action post_sleep
 :parameters (?c - CrewMember ?d1 ?d2 - Day)
 :duration (= ?duration 195)
 :condition (and (at start (done_sleep ?c ?d1))
	        (at start (currentday ?c ?d1))
	        (over all (next ?d1 ?d2))
	        (at start (initiated ?d2)))
 :effect (and (at start (not (currentday ?c ?d1)))
	   (at end (currentday ?c ?d2))
	   (at end (available ?c))
	   (at end (done_post_sleep ?c ?d2))
              (increase (total-cost) (- 1441 195)))
)


(:durative-action have_meal
 :parameters (?c - CrewMember ?d - Day)
 :duration (= ?duration 60)
 :condition (and  (at start (available ?c))
	         (at start (done_post_sleep ?c ?d))
	         (over all (currentday ?c ?d)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (done_meal ?c ?d))
              (increase (total-cost) (- 1441 60)))
)


(:durative-action exercise
 :parameters (?c - CrewMember ?d - Day ?e - ExerEquipment)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c))
	        (at start (done_post_sleep ?c ?d))
	        (at start (unused ?e))
	        (over all (currentday ?c ?d)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at start (not (unused ?e)))
	   (at end (unused ?e))
	   (at end (done_exercise ?c ?d))
              (increase (total-cost) (- 1441 60)))
)



(:durative-action sleep
 :parameters (?c - CrewMember ?d - Day)
 :duration ( = ?duration 600)
 :condition (and (at start (available ?c))
	         (at start (done_exercise ?c ?d))
	         (at start (done_meal ?c ?d))
	         (over all (currentday ?c ?d)))
 :effect (and (at start (not (available ?c)))
	   (at end (done_sleep ?c ?d))
              (increase (total-cost) (- 1441 600)))
)


(:durative-action change_filter
 :parameters (?fs - FilterState ?c - CrewMember ?d - Day)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c))
	         (over all (currentday ?c ?d)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (changed ?fs ?d))
              (increase (total-cost) (- 1441 60)))
)

(:durative-action medical_conference
 :parameters (?ps - MedicalState ?c - CrewMember ?d - Day)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c))
	        (over all (currentday ?c ?d)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (mcs_finished ?ps ?d))
              (increase (total-cost) (- 1441 60)))
)



(:durative-action conduct_payload_activity
 :parameters (?pa - PayloadAct ?c - CrewMember)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (payload_act_done ?pa))
              (increase (total-cost) (- 1441 60)))
)


(:durative-action report_payload_activity_at_deadline
 :parameters (?pa - PayloadAct ?c - CrewMember ?d - Day)
 :duration (= ?duration 1)
 :condition (and (over all (currentday ?c ?d))
	         (at start (payload_act_done ?pa)))
 :effect (and  (at end (payload_act_completed ?pa ?d))
              (increase (total-cost) 1440))
)




(:durative-action first_reconfigurate_thermal_loops
 :parameters (?ps - RPCM ?c - CrewMember)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (done_first_reconfigure_thermal_loop ?ps))
              (increase (total-cost) (- 1441 60)))
)


(:durative-action remove_sleep_station
 :parameters (?ps - RPCM ?c - CrewMember)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (done_remove_sleep_station ?ps))
              (increase (total-cost) (- 1441 60)))
)



(:durative-action replace_rpcm
 :parameters (?ps - RPCM ?c - CrewMember)
 :duration (= ?duration 180)
 :condition (and (at start (available ?c))
	         (at start (done_remove_sleep_station ?ps))
	         (at start (done_first_reconfigure_thermal_loop ?ps)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (done_replace_rpcm ?ps))
              (increase (total-cost) (- 1441 180)))
)



(:durative-action assemble_sleep_station
 :parameters (?ps - RPCM ?c - CrewMember)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c))
	         (at start (done_replace_rpcm ?ps)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (done_assemble_sleep_station ?ps))
              (increase (total-cost) (- 1441 60)))
)


(:durative-action second_reconfigurate_thermal_loops
 :parameters (?ps - RPCM ?c - CrewMember)
 :duration (= ?duration 60)
 :condition (and (at start (available ?c))
	         (at start (done_replace_rpcm ?ps)))
 :effect (and (at start (not (available ?c)))
	   (at end (available ?c))
	   (at end (done_second_reconfigure_thermal_loop ?ps))
              (increase (total-cost) (- 1441 60)))
)


(:durative-action finish_rpcm
 :parameters (?ps - RPCM ?c - CrewMember ?d - Day)
 :duration (= ?duration 1)
 :condition (and (at start (done_assemble_sleep_station ?ps))
	         (at start (done_second_reconfigure_thermal_loop ?ps))
	         (over all (currentday ?c ?d)))
 :effect (and (at end (done_rpcm ?ps ?d))
              (increase (total-cost) 1440))
)

)

