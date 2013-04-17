; ./cpt -o Domain_NoeudSanteCoord.pddl -f  Problem_NoeudSanteCoord.pddl -K 1

; Modélisation d'un scénario réduit qui distingue les type de véhicules d'évacuation des victimes
; L'objectif est de répartir les victimes dans les hopitaux en exploitant aux mieux les différents 
; services d'intervention.
; 
; On considère que les véhicules sont rattachés à deux types de victimes:
; - UR : urgence relatives
; - UA : urgences absolues
;
; Types de véhicules pris en compte dans la modélisation
; Pour la BSPP : véhicules modélisés: 
; - AR : ambulance de réanimation (pour les UA)
; - PSE : premier secours évacuation (pour les UR, non retenu)
; - VES : véhicule d'évacuation sanitaire (pour les UR)
; - VSAV : véhicule de secours et d'assistance aux victimes (pour les UR)
; Pour les SAMU :
; - SMUR_UA (pour les UA)
; - SMUR_UR (pour les UR)
; Pour les ambulances privées:
; - APS : ambulance privée (pour les UR)
;


(define (domain domain_NoeudSanteCoord)
;(:requirements :typing :durative-actions)
;(:requirements :typing :action-costs)

(:requirements :typing :action-costs :durative-actions)

(:types 
	Helico Vehicle Victim - Object
	PoliceUnit TransportVictimVehicle - Vehicle
	TrafficConfig Place - Destination
	Police Caserne Hospital CrisisZone - Place
	DiameterSize ; small, medium, large
	Nlit
	Itineraire
	SituationTraf
	UrgenceType
	BedType
)

(:predicates
	(loc ?x - Object ?y - Destination)
	(available ?r - Helico)
	(available ?r - Vehicle)
	(currentSituationTrafForecasted ?s - SituationTraf)
	(currentTrafficConfig ?c - TrafficConfig)
	(default ?c - TrafficConfig)
	(special ?c - TrafficConfig)
	(differs ?c1 ?c2 - TrafficConfig)
	(onBoard ?p - Victim ?v - TransportVictimVehicle)
	(onBoard ?p - Victim ?v - Helico)

	(evacuee ?x - Victim)
	(notEvacuee ?x - Victim)
	(bedRequired ?p - Victim ?m - BedType)
	(VehicleRequired ?p - Victim ?u - UrgenceType)
	(VehicleRequired ?v - TransportVictimVehicle ?u - UrgenceType)

	(nbreLits ?x - Hospital ?m - BedType ?y -Nlit)

	(lnext ?l1 ?l2 - Nlit)

	(trigger-switchs2s@middle ?c1 ?c2 - trafficConfig ?v1 ?v2 - policeunit ?s - situationTraf)
	(trigger-switchs2s@end ?c1 ?c2 - trafficConfig ?v1 ?v2 - policeunit ?s - situationTraf)
 	(trigger-switchd2s@middle ?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
	(trigger-switchd2s@end ?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
	(trigger-switchs2d@middle ?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
	(trigger-switchs2d@end ?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
)

(:functions (tempsMoyen ?situationTraf - SituationTraf ?i - Itineraire ?cd ?ca - Place)
	    (tempsHelico ?cd ?ca - Place)
	    (tempsDeploy ?c1 ?c2 - TrafficConfig ?s - SituationTraf)
	    (tempsDeployOn ?c1 ?c2 - TrafficConfig ?s - SituationTraf)
	    (tempsDeployOff ?c1 ?c2 - TrafficConfig ?s - SituationTraf)
	    (tempsMoyenEscort ?situationTraf - SituationTraf ?i - Itineraire ?cd ?ca - Place)
	    (tempsLoadVictim)
	    (tempsUnLoadVictim)
	    (tempsSecureZone ?d - DiameterSize)
	    (total-cost)
	    (coutVehicle ?cd ?ca - Place)
	    (coutHelico ?cd ?ca - Place)
)

;=================================== Déplacements =======================================

(:action move
 :parameters (?v - Vehicle ?p1 ?p2 - Place ?s - SituationTraf ?i - Itineraire ?c - TrafficConfig)
 :duration (= ?duration (tempsMoyen ?c ?s ?i ?p1 ?p2))
 :condition (and (currentSituationTrafForecasted ?s)
		 (currentTrafficConfig ?c)
		 (loc ?v ?p1))
 :effect (and (not (loc ?v ?p1))
	      (loc ?v ?p2)
	      (increase (total-cost) (coutVehicle ?p1 ?p2))))


(:action moveHelico
 :parameters (?v - Helico ?p1 ?p2 - Place)
 :duration (= ?duration (tempsHelico ?p1 ?p2))
 :condition (and (loc ?v ?p1))
 :effect (and (not (loc ?v ?p1))
	      (loc ?v ?p2)
	      (increase (total-cost) (coutHelico ?p1 ?p2))))


;================================ Gestion de trafic ==========================================

; On a l'invariant que d'aller d'un point A à un point B est toujours plus rapide que de passer par un barycentre 
; (même s'il est sur le chemin entre A et B, on rajoute un epsilon pour qu'il soit toujours plus long). 
; Ainsi une solution optimale ne passera jamais par un barycentre pour connecter 2 points.

; On va de ?p1 à ?c2 sous condition ?c1
(:action moveToTrafficConfig
 :parameters (?c1 ?c2 - TrafficConfig ?v - PoliceUnit ?p1 - Destination ?s - SituationTraf ?i - Itineraire)
 :duration (= ?duration (tempsMoyen ?c1 ?s ?i ?p1 ?c2))
 :condition (and (currentSituationTrafForecasted ?s)
		 (currentTrafficConfig ?c1)
		 (loc ?v ?p1))
 :effect (and (not (loc ?v ?p1))
	      (loc ?v ?c2)))

; On va de ?c1 à ?p2 sous condition ?c2
(:action MoveFromTrafficConfig
 :parameters (?c1 ?c2 - TrafficConfig ?v - PoliceUnit ?p2 - Destination ?s - SituationTraf ?i - Itineraire)
 :duration (= ?duration (tempsMoyen ?c2 ?s ?i ?c1 ?p2))
 :condition (and (currentSituationTrafForecasted ?s)
		 (currentTrafficConfig ?c2)
		 (loc ?v ?c1))
 :effect (and (not (loc ?v ?c1))
	      (loc ?v ?p2)))

;;;=================================================================
; On passe de ?c1 à ?c2.

(:action switchs2sStart
 :parameters (?c1 ?c2 - trafficConfig ?v1 ?v2 - policeunit ?s - situationTraf)
 :duration (= ?duration (tempsDeploy ?c1 ?c2 ?s))
 :precondition (and (currentsituationTrafforecasted ?s)
		    (currenttrafficConfig ?c1)
  		    (special ?c1)
		    (special ?c2)
		    (differs ?c1 ?c2)
		    (loc ?v1 ?c1)
		    (loc ?v2 ?c2)
		    (available ?v2))
 :effect (and (not (available ?v2))
	      (trigger-switchs2s@end ?c1 ?c2 ?v1 ?v2 ?s)))
 
(:action switchs2sEnd
:parameters (?c1 ?c2 - trafficConfig ?v1 ?v2 - policeunit ?s - situationTraf)
 :duration (= ?duration 0)
 :precondition (and (trigger-switchs2s@end ?c1 ?c2 ?v1 ?v2 ?s))
 :effect (and (available ?v1)
	      (not (currenttrafficConfig ?c1))
	      (currenttrafficConfig ?c2)
	      (not (trigger-switchs2s@end ?c1 ?c2 ?v1 ?v2 ?s))))

(:action switchd2sStart
 :parameters (?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
 :duration (= ?duration (tempsDeployOn ?c1 ?c2 ?s))
 :precondition (and (currentsituationTrafforecasted ?s)
		    (currenttrafficConfig ?c1)
		    (default ?c1)
		    (special ?c2)
		    (loc ?v ?c2)
		    (available ?v))
 :effect (and (not (available ?v))
	      (trigger-switchd2s@end ?c1 ?c2 ?v ?s)))

(:action switchd2sEnd
 :parameters (?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
 :duration (= ?duration 0)
 :precondition (and (trigger-switchd2s@end ?c1 ?c2 ?v ?s))
 :effect (and (not (currenttrafficConfig ?c1))
	      (currenttrafficConfig ?c2)
	      (not (trigger-switchd2s@end ?c1 ?c2 ?v ?s))))

(:action switchs2dStart
 :parameters (?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
 :duration (= ?duration (tempsDeployOff ?c1 ?c2 ?s))
 :precondition (and (currentsituationTrafforecasted ?s)
		    (currenttrafficConfig ?c1)
		    (special ?c1)
		    (default ?c2)
		    (loc ?v ?c1))
 :effect (and (trigger-switchs2d@end ?c1 ?c2 ?v ?s)))

(:action switchs2dEnd
 :parameters (?c1 ?c2 - trafficConfig ?v - policeunit ?s - situationTraf)
 :duration (= ?duration 0)
 :precondition (and (trigger-switchs2d@end ?c1 ?c2 ?v ?s))
 :effect (and (available ?v)
	      (not (currenttrafficConfig ?c1))
	      (currenttrafficConfig ?c2)
	      (not (trigger-switchs2d@end ?c1 ?c2 ?v ?s))))

;================================== Evacuation des victimes ========================================
; Hyp : une seule victime par véhicule.

(:action loadVictim
 :parameters (?p - Victim ?v - TransportVictimVehicle ?s - CrisisZone ?u - UrgenceType)
 :duration (= ?duration (tempsLoadVictim))
 :condition (and (loc ?p ?s)
		 (loc ?v ?s)
		 (available ?v)
		 (VehicleRequired ?p ?u)
		 (VehicleRequired ?v ?u))
 :effect (and (not (available ?v))
	      (not (loc ?p ?s))
	      (onBoard ?p ?v)))

(:action unLoadVictim
 :parameters (?p - Victim ?v - TransportVictimVehicle ?s - Hospital)
 :duration (= ?duration (tempsUnLoadVictim))
 :condition (and (onBoard ?p ?v)
		 (loc ?v ?s))
 :effect (and (not (onBoard ?p ?v))
	      (loc ?p ?s)
	      (available ?v)))

;=== Evacuation par hélicoptère

(:action loadVictimHelico
 :parameters (?p - Victim ?v - Helico ?s - CrisisZone)
 :duration (= ?duration (tempsLoadVictim))
 :condition (and (loc ?p ?s)
		 (loc ?v ?s)
		 (available ?v))
 :effect (and (not (available ?v))
	      (not (loc ?p ?s))
	      (onBoard ?p ?v)))

(:action unLoadVictimHelico
 :parameters (?p - Victim ?v - Helico ?s - Hospital)
 :duration (= ?duration (tempsUnLoadVictim))
 :condition (and (onBoard ?p ?v)
		 (loc ?v ?s))
 :effect (and (not (onBoard ?p ?v))
	      (loc ?p ?s)
	      (available ?v)))


;;======================== Dispatch des victimes selon leur pathologie =============================================

(:action hostVictim 
 :parameters (?p - Victim ?h - Hospital ?n1 ?n2 - Nlit ?m - BedType) 
 :duration (= ?duration 1) 
 :condition (and (loc ?p ?h)
		 (nbreLits ?h ?m ?n1)
		 (lnext ?n2 ?n1)
		 (bedRequired ?p ?m))
 :effect (and (not (nbreLits ?h ?m ?n1))
	      (nbreLits ?h ?m ?n2)
	      (not (loc ?p ?h))
	      (evacuee ?p)))


)
