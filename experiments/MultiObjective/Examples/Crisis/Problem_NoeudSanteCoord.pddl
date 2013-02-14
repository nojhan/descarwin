;Problem generation for full domain with no police

(define (problem problemNoeudSanteCoord_AttentatGdN) (:domain Domain_NoeudSanteCoord)

;;;;;;;;;;;;;;;;;;;;;;;OBJECTS;;;;;;;;;;;;;;;;;;;;;;;
(:objects

; Location definitions
	lcrise1 - CrisisZone 
	lhop1 - Hospital 
	lhop2 - Hospital 
	lhop3 - Hospital 
	lhop4 - Hospital 
	lhop5 - Hospital 
	lunit1 - Caserne 
	lunit2 - Caserne 
	lunit3 - Caserne 
	lunit4 - Caserne 
	lunit5 - Caserne 
	lunit6 - Caserne 
	lunit7 - Caserne 
	lunit8 - Caserne 

; Resources definition
; Hélico
	puma1 - Helico

; Rescue units definition
	urn_asset_627 - TransportVictimVehicle 
	urn_asset_604 - TransportVictimVehicle 
	urn_asset_693 - TransportVictimVehicle 
	urn_asset_623 - TransportVictimVehicle 
	urn_asset_637 - TransportVictimVehicle 
	urn_asset_680 - TransportVictimVehicle 
	urn_asset_644 - TransportVictimVehicle 
	urn_asset_590 - TransportVictimVehicle 
	urn_asset_681 - TransportVictimVehicle 
	urn_asset_683 - TransportVictimVehicle 

; Victims definition
	victime1 - Victim 
	victime10 - Victim 
	victime11 - Victim 

	 IN1 - Itineraire  

; Area size definition

; Traffic configurations definition
	 trafconf0 - TrafficConfig 

; Traffic situations definition
	 trafficDense - situationTraf 
	 trafficFluide - situationTraf 
	 trafficSature - situationTraf 

; Pathologies gravity definition
	 UR - UrgenceType 
	 UA - UrgenceType 

; Pathologies types definition
	 sore - BedType 
	 blast - BedType 
	 burn - BedType 
	 ortho - BedType 

; Language required definitions
; Hospital bed number definition
	n0 - Nlit 
	n1 - Nlit 
	n2 - Nlit 
	n3 - Nlit 
	n4 - Nlit 
	n5 - Nlit 
	n6 - Nlit 
	n7 - Nlit 
	n8 - Nlit 
	n9 - Nlit 
	n10 - Nlit 
	n11 - Nlit 
	n12 - Nlit 
	n13 - Nlit 
	n14 - Nlit 
	n15 - Nlit 
	n16 - Nlit 
	n17 - Nlit 
	n18 - Nlit 
	n19 - Nlit 
	n20 - Nlit 
	n21 - Nlit 
	n22 - Nlit 
	n23 - Nlit 
	n24 - Nlit 
	n25 - Nlit 
	n26 - Nlit 
	n27 - Nlit 
	n28 - Nlit 
	n29 - Nlit 
	n30 - Nlit 
	n31 - Nlit 
	n32 - Nlit 
	n33 - Nlit 
	n34 - Nlit 
	n35 - Nlit 
	n36 - Nlit 
	n37 - Nlit 
	n38 - Nlit 
	n39 - Nlit 
	n40 - Nlit 
	n41 - Nlit 
	n42 - Nlit 
	n43 - Nlit 
	n44 - Nlit 
	n45 - Nlit 
	n46 - Nlit 
	n47 - Nlit 
	n48 - Nlit 
	n49 - Nlit 
	n50 - Nlit 
	n51 - Nlit 
	n52 - Nlit 
	n53 - Nlit 
	n54 - Nlit 
	n55 - Nlit 
	n56 - Nlit 
	n57 - Nlit 
	n58 - Nlit 
	n59 - Nlit 
	n60 - Nlit 
	n61 - Nlit 
	n62 - Nlit 
	n63 - Nlit 
	n64 - Nlit 
	n65 - Nlit 
	n66 - Nlit 
	n67 - Nlit 
	n68 - Nlit 
	n69 - Nlit 
	n70 - Nlit 
	n71 - Nlit 
	n72 - Nlit 
	n73 - Nlit 
	n74 - Nlit 
	n75 - Nlit 
	n76 - Nlit 
	n77 - Nlit 
	n78 - Nlit 
	n79 - Nlit 
	n80 - Nlit 
	n81 - Nlit 
	n82 - Nlit 
	n83 - Nlit 
	n84 - Nlit 
	n85 - Nlit 
	n86 - Nlit 
	n87 - Nlit 
	n88 - Nlit 
	n89 - Nlit 
	n90 - Nlit 
	n91 - Nlit 
	n92 - Nlit 
	n93 - Nlit 
	n94 - Nlit 
	n95 - Nlit 
	n96 - Nlit 
	n97 - Nlit 
	n98 - Nlit 
	n99 - Nlit 
	n100 - Nlit 
	n101 - Nlit 
	n102 - Nlit 
	n103 - Nlit 
	n104 - Nlit 
	n105 - Nlit 
	n106 - Nlit 
	n107 - Nlit 
	n108 - Nlit 
	n109 - Nlit 
	n110 - Nlit 
	n111 - Nlit 
	n112 - Nlit 
	n113 - Nlit 
	n114 - Nlit 
	n115 - Nlit 
	n116 - Nlit 
	n117 - Nlit 
	n118 - Nlit 
	n119 - Nlit 
	n120 - Nlit 
	n121 - Nlit 
	n122 - Nlit 
	n123 - Nlit 
	n124 - Nlit 
	n125 - Nlit 
	n126 - Nlit 
	n127 - Nlit 
	n128 - Nlit 
	n129 - Nlit 

)

;;;;;;;;;;;;;;;;;;;;;;;INIT;;;;;;;;;;;;;;;;;;;;;;;
(:init

; Traffic configurations initialisation
	 (default trafconf0) 
	 (currentTrafficConfig trafconf0) 

; Diameter of the crisis area initialisation 

; Hélico localisation initialisation 

	(loc puma1 lunit1)
	(available puma1)

; Rescue units localisation initialisation 
	 (loc urn_asset_627 lunit1) 
	 (loc urn_asset_604 lunit2) 
	 (loc urn_asset_693 lunit3) 
	 (loc urn_asset_623 lunit4) 
	 (loc urn_asset_637 lunit5) 
	 (loc urn_asset_680 lunit6) 
	 (loc urn_asset_644 lunit7) 
	 (loc urn_asset_590 lunit8) 
	 (loc urn_asset_681 lunit1) 
	 (loc urn_asset_683 lunit2) 

; Rescue units availability initialisation 
	 (available urn_asset_627) 
	 (available urn_asset_604) 
	 (available urn_asset_693) 
	 (available urn_asset_623) 
	 (available urn_asset_637) 
	 (available urn_asset_680) 
	 (available urn_asset_644) 
	 (available urn_asset_590) 
	 (available urn_asset_681) 
	 (available urn_asset_683) 

; Victims initialisation 
; Victims localisation initialisation 
	 (loc victime1 lcrise1) 
	 (loc victime10 lcrise1) 
	 (loc victime11 lcrise1) 

; Victims extracted state initialisation 

; Victims contaminated state initialisation 

; Victims sorted state initialisation 

; Vehicle urgence type required for victim initialisation 
	 (VehicleRequired victime1 UR) 
	 (VehicleRequired victime10 UR) 
	 (VehicleRequired victime11 UR) 

; Vehicle urgence type initialisation 
	 (VehicleRequired urn_asset_627 UR) 
	 (VehicleRequired urn_asset_604 UR) 
	 (VehicleRequired urn_asset_693 UA) 
	 (VehicleRequired urn_asset_623 UR) 
	 (VehicleRequired urn_asset_637 UR) 
	 (VehicleRequired urn_asset_680 UA) 
	 (VehicleRequired urn_asset_644 UR) 
	 (VehicleRequired urn_asset_590 UR) 
	 (VehicleRequired urn_asset_681 UA) 
	 (VehicleRequired urn_asset_683 UA) 

; Victims required bed initialisation 
	 (bedRequired victime1 sore) 
	 (bedRequired victime10 sore) 
	 (bedRequired victime11 sore) 

; Hospital beds initialisation 
	 (nbreLits lhop1 ortho n10) 
	 (nbreLits lhop1 sore n8) 
	 (nbreLits lhop1 burn n8) 
	 (nbreLits lhop2 burn n6) 
	 (nbreLits lhop3 sore n3) 
	 (nbreLits lhop3 burn n4) 
	 (nbreLits lhop4 sore n13) 
	 (nbreLits lhop5 blast n11) 

; Language required definitions
; Management of the hospital bed numbers
	(lnext n0 n1) 
	(lnext n1 n2) 
	(lnext n2 n3) 
	(lnext n3 n4) 
	(lnext n4 n5) 
	(lnext n5 n6) 
	(lnext n6 n7) 
	(lnext n7 n8) 
	(lnext n8 n9) 
	(lnext n9 n10) 
	(lnext n10 n11) 
	(lnext n11 n12) 
	(lnext n12 n13) 
	(lnext n13 n14) 
	(lnext n14 n15) 
	(lnext n15 n16) 
	(lnext n16 n17) 
	(lnext n17 n18) 
	(lnext n18 n19) 
	(lnext n19 n20) 
	(lnext n20 n21) 
	(lnext n21 n22) 
	(lnext n22 n23) 
	(lnext n23 n24) 
	(lnext n24 n25) 
	(lnext n25 n26) 
	(lnext n26 n27) 
	(lnext n27 n28) 
	(lnext n28 n29) 
	(lnext n29 n30) 
	(lnext n30 n31) 
	(lnext n31 n32) 
	(lnext n32 n33) 
	(lnext n33 n34) 
	(lnext n34 n35) 
	(lnext n35 n36) 
	(lnext n36 n37) 
	(lnext n37 n38) 
	(lnext n38 n39) 
	(lnext n39 n40) 
	(lnext n40 n41) 
	(lnext n41 n42) 
	(lnext n42 n43) 
	(lnext n43 n44) 
	(lnext n44 n45) 
	(lnext n45 n46) 
	(lnext n46 n47) 
	(lnext n47 n48) 
	(lnext n48 n49) 
	(lnext n49 n50) 
	(lnext n50 n51) 
	(lnext n51 n52) 
	(lnext n52 n53) 
	(lnext n53 n54) 
	(lnext n54 n55) 
	(lnext n55 n56) 
	(lnext n56 n57) 
	(lnext n57 n58) 
	(lnext n58 n59) 
	(lnext n59 n60) 
	(lnext n60 n61) 
	(lnext n61 n62) 
	(lnext n62 n63) 
	(lnext n63 n64) 
	(lnext n64 n65) 
	(lnext n65 n66) 
	(lnext n66 n67) 
	(lnext n67 n68) 
	(lnext n68 n69) 
	(lnext n69 n70) 
	(lnext n70 n71) 
	(lnext n71 n72) 
	(lnext n72 n73) 
	(lnext n73 n74) 
	(lnext n74 n75) 
	(lnext n75 n76) 
	(lnext n76 n77) 
	(lnext n77 n78) 
	(lnext n78 n79) 
	(lnext n79 n80) 
	(lnext n80 n81) 
	(lnext n81 n82) 
	(lnext n82 n83) 
	(lnext n83 n84) 
	(lnext n84 n85) 
	(lnext n85 n86) 
	(lnext n86 n87) 
	(lnext n87 n88) 
	(lnext n88 n89) 
	(lnext n89 n90) 
	(lnext n90 n91) 
	(lnext n91 n92) 
	(lnext n92 n93) 
	(lnext n93 n94) 
	(lnext n94 n95) 
	(lnext n95 n96) 
	(lnext n96 n97) 
	(lnext n97 n98) 
	(lnext n98 n99) 
	(lnext n99 n100) 
	(lnext n100 n101) 
	(lnext n101 n102) 
	(lnext n102 n103) 
	(lnext n103 n104) 
	(lnext n104 n105) 
	(lnext n105 n106) 
	(lnext n106 n107) 
	(lnext n107 n108) 
	(lnext n108 n109) 
	(lnext n109 n110) 
	(lnext n110 n111) 
	(lnext n111 n112) 
	(lnext n112 n113) 
	(lnext n113 n114) 
	(lnext n114 n115) 
	(lnext n115 n116) 
	(lnext n116 n117) 
	(lnext n117 n118) 
	(lnext n118 n119) 
	(lnext n119 n120) 
	(lnext n120 n121) 
	(lnext n121 n122) 
	(lnext n122 n123) 
	(lnext n123 n124) 
	(lnext n124 n125) 
	(lnext n125 n126) 
	(lnext n126 n127) 
	(lnext n127 n128) 
	(lnext n128 n129) 



;======================TRANSIT TIMES DEFINITION======================
; The transit times can be defined for different traffic situation and traffic configuration, and for several paths.
; (= (tempsMoyen <trafficConfig default(=no police action) or special(=police action)> <situationTraffic> <path number> <departure point> <arrival point>) <value>)


;---------------------- Cout de mobilisation d'un véhicule (sortie de la caserne) ----------------------
; hypothèse : tous les véhicules ont le même prix de mobilisation

	(= (coutVehicle lcrise1 lunit1) 0)	(= (coutVehicle lunit1 lcrise1) 1)
	(= (coutVehicle lcrise1 lunit2) 0)	(= (coutVehicle lunit2 lcrise1 ) 1)
	(= (coutVehicle lcrise1 lunit3) 0)	(= (coutVehicle lunit3 lcrise1) 1) 
	(= (coutVehicle lcrise1 lunit4) 0)	(= (coutVehicle lunit4 lcrise1) 1)
	(= (coutVehicle lcrise1 lunit5) 0)	(= (coutVehicle lunit5 lcrise1) 1)
	(= (coutVehicle lcrise1 lhop1) 0)	(= (coutVehicle lhop1 lcrise1) 0)
	(= (coutVehicle lcrise1 lhop2) 0)	(= (coutVehicle lhop2 lcrise1) 0)
	(= (coutVehicle lcrise1 lhop3) 0)	(= (coutVehicle lhop3 lcrise1) 0)
	(= (coutVehicle lcrise1 lhop4) 0)	(= (coutVehicle lhop4 lcrise1) 0)
	(= (coutVehicle lcrise1 lhop5) 0)	(= (coutVehicle lhop5 lcrise1) 0)


	(= (coutHelico lcrise1 lunit1) 0)	(= (coutHelico lunit1 lcrise1) 5)
	(= (coutHelico lcrise1 lunit2) 0)	(= (coutHelico lunit2 lcrise1 ) 5)
	(= (coutHelico lcrise1 lunit3) 0)	(= (coutHelico lunit3 lcrise1) 5) 
	(= (coutHelico lcrise1 lunit4) 0)	(= (coutHelico lunit4 lcrise1) 5)
	(= (coutHelico lcrise1 lunit5) 0)	(= (coutHelico lunit5 lcrise1) 5)
	(= (coutHelico lcrise1 lhop1) 0)	(= (coutHelico lhop1 lcrise1) 0)
	(= (coutHelico lcrise1 lhop2) 0)	(= (coutHelico lhop2 lcrise1) 0)
	(= (coutHelico lcrise1 lhop3) 0)	(= (coutHelico lhop3 lcrise1) 0)
	(= (coutHelico lcrise1 lhop4) 0)	(= (coutHelico lhop4 lcrise1) 0)
	(= (coutHelico lcrise1 lhop5) 0)	(= (coutHelico lhop5 lcrise1) 0)

	(= (total-cost) 0)

;----------------------Transit times for trafConf0----------------------
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lunit1) 3)  (= (tempsMoyen trafConf0 trafficFluide IN1 lunit1 lcrise1) 5)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lunit2) 7)  (= (tempsMoyen trafConf0 trafficFluide IN1 lunit2 lcrise1) 4)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lunit3) 4)  (= (tempsMoyen trafConf0 trafficFluide IN1 lunit3 lcrise1) 4)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lunit4) 6)  (= (tempsMoyen trafConf0 trafficFluide IN1 lunit4 lcrise1) 6)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lunit5) 8)  (= (tempsMoyen trafConf0 trafficFluide IN1 lunit5 lcrise1) 7)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lhop1) 3)  (= (tempsMoyen trafConf0 trafficFluide IN1 lhop1 lcrise1) 2)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lhop2) 2)  (= (tempsMoyen trafConf0 trafficFluide IN1 lhop2 lcrise1) 4)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lhop3) 9)  (= (tempsMoyen trafConf0 trafficFluide IN1 lhop3 lcrise1) 6)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lhop4) 22)  (= (tempsMoyen trafConf0 trafficFluide IN1 lhop4 lcrise1) 21)
	(= (tempsMoyen trafConf0 trafficFluide IN1 lcrise1 lhop5) 14)  (= (tempsMoyen trafConf0 trafficFluide IN1 lhop5 lcrise1) 13)

	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lunit1) 6)  (= (tempsMoyen trafConf0 trafficDense IN1 lunit1 lcrise1) 10)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lunit2) 14)  (= (tempsMoyen trafConf0 trafficDense IN1 lunit2 lcrise1) 8)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lunit3) 8)  (= (tempsMoyen trafConf0 trafficDense IN1 lunit3 lcrise1) 8)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lunit4) 12)  (= (tempsMoyen trafConf0 trafficDense IN1 lunit4 lcrise1) 12)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lunit5) 16)  (= (tempsMoyen trafConf0 trafficDense IN1 lunit5 lcrise1) 14)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lhop1) 6)  (= (tempsMoyen trafConf0 trafficDense IN1 lhop1 lcrise1) 4)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lhop2) 4)  (= (tempsMoyen trafConf0 trafficDense IN1 lhop2 lcrise1) 8)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lhop3) 18)  (= (tempsMoyen trafConf0 trafficDense IN1 lhop3 lcrise1) 12)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lhop4) 44)  (= (tempsMoyen trafConf0 trafficDense IN1 lhop4 lcrise1) 42)
	(= (tempsMoyen trafConf0 trafficDense IN1 lcrise1 lhop5) 28)  (= (tempsMoyen trafConf0 trafficDense IN1 lhop5 lcrise1) 26)

	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lunit1) 9)  (= (tempsMoyen trafConf0 trafficSature IN1 lunit1 lcrise1) 15)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lunit2) 21)  (= (tempsMoyen trafConf0 trafficSature IN1 lunit2 lcrise1) 12)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lunit3) 12)  (= (tempsMoyen trafConf0 trafficSature IN1 lunit3 lcrise1) 12)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lunit4) 18)  (= (tempsMoyen trafConf0 trafficSature IN1 lunit4 lcrise1) 18)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lunit5) 24)  (= (tempsMoyen trafConf0 trafficSature IN1 lunit5 lcrise1) 21)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lhop1) 9)  (= (tempsMoyen trafConf0 trafficSature IN1 lhop1 lcrise1) 6)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lhop2) 6)  (= (tempsMoyen trafConf0 trafficSature IN1 lhop2 lcrise1) 12)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lhop3) 27)  (= (tempsMoyen trafConf0 trafficSature IN1 lhop3 lcrise1) 18)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lhop4) 66)  (= (tempsMoyen trafConf0 trafficSature IN1 lhop4 lcrise1) 63)
	(= (tempsMoyen trafConf0 trafficSature IN1 lcrise1 lhop5) 42)  (= (tempsMoyen trafConf0 trafficSature IN1 lhop5 lcrise1) 39)

;== Hélico

	(= (tempsHelico  lcrise1 lunit1) 1)  (= (tempsHelico  lunit1 lcrise1) 2)
	(= (tempsHelico  lcrise1 lunit2) 3)  (= (tempsHelico  lunit2 lcrise1) 1)
	(= (tempsHelico  lcrise1 lunit3) 2)  (= (tempsHelico  lunit3 lcrise1) 2)
	(= (tempsHelico  lcrise1 lunit4) 3)  (= (tempsHelico  lunit4 lcrise1) 3)
	(= (tempsHelico  lcrise1 lunit5) 4)  (= (tempsHelico  lunit5 lcrise1) 3)
	(= (tempsHelico  lcrise1 lhop1) 1)   (= (tempsHelico  lhop1 lcrise1) 1)
	(= (tempsHelico  lcrise1 lhop2) 1)   (= (tempsHelico  lhop2 lcrise1) 2)
	(= (tempsHelico  lcrise1 lhop3) 3)   (= (tempsHelico  lhop3 lcrise1) 3)
	(= (tempsHelico  lcrise1 lhop4) 11)  (= (tempsHelico  lhop4 lcrise1) 10)
	(= (tempsHelico  lcrise1 lhop5) 7)   (= (tempsHelico  IN1 lhop5 lcrise1) 6)


; Note: transit times to get from trafConf1 to trafConf2 are not defined as the modelisation requires to transit first to trafConf0
; Actions time duration setting
	 (= (tempsLoadVictim) 2)
	 (= (tempsUnloadVictim) 5)



;----------------------Traffic forecast----------------------
; The traffic forecast is given through an ordered sequence of situations of reference 
	 (currentSituationTrafForecasted trafficDense)
	 (currentSituationTrafForecasted trafficSature)

)

;;;;;;;;;;;;;;;;;;;;;;;CONSTRAINSTS;;;;;;;;;;;;;;;;;;;;;;;

; The traffic evolution is defined through a sequence of traffic situation of reference using the constrainsts feature.

(:constraints (and
	 (active (currentSituationTrafForecasted trafficDense) 0 2) ;26
	 (active (currentSituationTrafForecasted trafficSature) 3 1000) ; 27
)
)

;;;;;;;;;;;;;;;;;;;;;;;GOAL;;;;;;;;;;;;;;;;;;;;;;;

(:goal (and
; Victims in hospital 

	 (evacuee victime1)
	 (evacuee victime10)
	 (evacuee victime11)
))

;;;;;;;;;;;;;;;;;;;;;;;METRIC;;;;;;;;;;;;;;;;;;;;;;;

;(:metric minimize (total-time))
;(:metric minimize (total-cost))

(:metric (and (minimize (total-time)) (minimize (total-cost))))

)