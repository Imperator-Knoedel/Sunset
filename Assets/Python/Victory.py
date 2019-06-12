# Rhye's and Fall of Civilization - Historical Victory Goals

from CvPythonExtensions import *
from StoredData import data
from Consts import *
from RFCUtils import utils
import heapq
import Areas
import CityNameManager as cnm
import DynamicCivs as dc

### GLOBALS ###

gc = CyGlobalContext()
localText = CyTranslator()

### CONSTANTS ###

# general constants
lWonders = [i for i in range(iBeginWonders, iNumBuildings)]
lGreatPeople = [iSpecialistGreatProphet, iSpecialistGreatArtist, iSpecialistGreatScientist, iSpecialistGreatMerchant, iSpecialistGreatEngineer, iSpecialistGreatStatesman, iSpecialistGreatGeneral, iSpecialistGreatSpy]

# first Polynesian goal: settle two out of the following island groups by 800 AD: Hawaii, New Zealand, Marquesas and Easter Island
# second Polynesian goal: settle Hawaii, New Zealand, Marquesas and Easter Island by 1000 AD
tHawaiiTL = (0, 34)
tHawaiiBR = (6, 39)
tNewZealandTL = (119, 4)
tNewZealandBR = (123, 12)
tMarquesasTL = (14, 22)
tMarquesasBR = (16, 24)
tEasterIslandTL = (20, 15)
tEasterIslandBR = (22, 17)

# second Roman goal: control Iberia, Gaul, Britain, Africa, Greece, Asia Minor and Egypt in 320 AD
tFranceTL = (51, 47)

# second Roman goal: control Iberia, Gaul, Britain, Africa, Greece, Asia Minor and Egypt in 320 AD
# second Arabian goal: control or vassalize Spain, the Maghreb, Egypt, Mesopotamia and Persia in 1300 AD
tCarthageTL = (50, 36)
tCarthageBR = (61, 39)

# second Tamil goal: control or vassalize the Deccan and Srivijaya in 1000 AD
tDeccanTL = (88, 28)
tDeccanBR = (94, 36)
tSrivijayaTL = (98, 25)
tSrivijayaBR = (105, 29)

# third Ethiopian goal: allow no European colonies and East and Subequatorial Africa in 1500 AD and 1910 AD
tSomaliaTL = (73, 24)
tSomaliaBR = (77, 29)
tSubeqAfricaTL = (60, 10)
tSubeqAfricaBR = (72, 29)

# third Byzantine goal: control three cities in the Balkans, Northern Africa and the Near East in 1450 AD
tNearEastTL = (69, 37)
tNearEastBR = (76, 45)
tBalkansTL = (64, 40)
tBalkansBR = (68, 47)
tNorthAfricaTL = (58, 32)
tNorthAfricaBR = (71, 38)

# second Japanese goal: control or vassalize Korea, Manchuria, China, Indochina, Indonesia and the Philippines in 1940
tManchuriaTL = (104, 50)
tManchuriaBR = (112, 55)
tKoreaTL = (108, 45)
tKoreaBR = (110, 49)
tChinaTL = (99, 39)
tChinaBR = (107, 49)
tIndochinaTL = (97, 31)
tIndochinaBR = (104, 38)
tIndochinaExceptions = ((103, 38), (104, 37))
tIndonesiaTL = (98, 24)
tIndonesiaBR = (109, 30)
tPhilippinesTL = (108, 30)
tPhilippinesBR = (110, 36)

# second Turkic goal: create an overland trade route from a city in China to a Mediterranean port by 1100 AD
lMediterraneanPorts = [(66, 37), (66, 36), (67, 36), (68, 36), (69, 36), (70, 36), (71, 36), (72, 37), (73, 37), (73, 38), (73, 39), (73, 40), (73, 41), (73, 42), (71, 42), (70, 42), (70, 43), (69, 43), (69, 44), (68, 45)]

# first Moorish goal: control three cities in Iberia, the Maghreb and West Africa in 1200 AD
tIberiaTL = (49, 40)
tIberiaBR = (55, 46)
tMaghrebTL = (49, 35)
tMaghrebBR = (58, 39)
tWestAfricaTL = (48, 26)
tWestAfricaBR = (56, 32)

# third Spanish goal: spread Catholicism to 40% and allow no Protestant civilization in Europe in 1700 AD
# second French goal: control 40% of Europe and North America in 1800 AD
tEuropeTL = (44, 40)
tEuropeBR = (68, 65)

# second French goal: control 40% of Europe and North America in 1800 AD
tEasternEuropeTL = (69, 48)
tEasternEuropeBR = (73, 64)

# second French goal: control 40% of Europe and North America in 1800 AD
# first English goal: colonize every continent by 1730 AD
# third Maya goal: make contact with a European civilization before they have discovered America
tNorthAmericaTL = (10, 40)
tNorthAmericaBR = (37, 58)

# first English goal: colonize every continent by 1730 AD
tOceaniaTL = (99, 5)
tOceaniaBR = (123, 28)

# first English goal: colonize every continent by 1730 AD
# third Maya goal: make contact with a European civilization before they have discovered America
tSouthCentralAmericaTL = (13, 3)
tSouthCentralAmericaBR = (41, 39)

# first English goal: colonize every continent by 1730 AD
# third Portuguese goal: control 15 cities in Brazil, Africa and Asia in 1700 AD
tAfricaTL = (45, 10)
tAfricaBR = (76, 39)
tAsiaTL = (73, 24)
tAsiaBR = (121, 64)

# third English goal: Cape to Cairo Railway by 1920 AD
lNorthernEgypt = [(66, 36), (66, 37), (67, 36), (68, 36), (69, 36), (70, 36), (71, 36)]

# first Russian goal: found seven cities in Siberia by 1700 AD and build the Trans-Siberian Railway by 1920 AD
tSiberiaTL = (82, 50)
tSiberiaBR = (112, 64)
lSiberianCoast = [(109, 50), (109, 51), (110, 51), (111, 51), (112, 52), (114, 54), (113, 55), (111, 54), (111, 55), (110, 55), (110, 58), (111, 58), (112, 59)]

# second Portuguese goal: acquire 12 colonial resources by 1650 AD
lColonialResources = [iBanana, iSpices, iSugar, iCoffee, iTea, iTobacco]

# third Portuguese goal: control 15 cities in Brazil, Africa and Asia in 1700 AD
tBrazilTL = (32, 14)
tBrazilBR = (43, 30)

# third Italian goal: control 65% of the Mediterranean by 1930 AD
tMediterraneanTL = (51, 36)
tMediterraneanBR = (73, 47)
tMediterraneanExceptions = ((51,36),(51,46),(52,46),(53,46),(53,47),(67,47),(67,46),(73,44),(73,45),(72,45),(71,45),(71,44),(70,44),(73,36))

# first Incan goal: build five Tambos and a road along the Andean coast by 1500 AD
lAndeanCoast = [(25, 29), (24, 28), (24, 27), (24, 26), (24, 25), (25, 24), (25, 23), (26, 22), (27, 21), (28, 20), (29, 19), (30, 18), (30, 17), (30, 16), (30, 15), (30, 14)]

# third Incan goal: control 60% of South America in 1700 AD
# second Colombian goal: control South America in 1920 AD
tSAmericaTL = (24, 3)
tSAmericaBR = (43, 32)
tSouthAmericaExceptions = ((24, 31), (25, 32))

# third Holy Roman goal: settle three great artists in Vienna by 1700 AD
# second Ottoman goal: control the Eastern Mediterranean, the Black Sea, Cairo, Mecca, Baghdad and Vienna by 1700 AD
tVienna = (62, 49)

# second Ottoman goal: control the Eastern Mediterranean, the Black Sea, Cairo, Mecca, Baghdad and Vienna by 1700 AD
tCairo = (69, 34)
tMecca = (75, 33)
tBaghdad = (77, 40)
lEasternMediterranean = [(58, 39), (58, 38), (58, 37), (59, 37), (60, 37), (61, 37), (61, 36), (62, 36), (63, 36), (64, 36), (65, 36), (66, 36), (67, 36), (68, 36), (69, 36), (70, 36), (71, 36), (65, 37), (66, 37), (72, 37), (73, 37), (73, 38), (73, 39), (73, 40), (73, 41), (73, 42), (70, 42), (71, 42), (72, 42), (69, 43), (70, 43), (69, 44), (68, 45), (67, 44), (67, 45), (66, 44), (65, 43), (66, 43), (65, 42), (66, 42), (67, 42), (67, 41), (65, 40), (66, 40)]
lBlackSea = [(69, 44), (70, 44), (71, 44), (71, 45), (72, 45), (73, 45), (73, 44), (74, 44), (75, 44), (76, 44), (76, 45), (76, 46), (76, 47), (75, 47), (74, 48), (75, 48), (72, 48), (74, 49), (73, 49), (71, 49), (69, 49), (69, 50), (70, 50), (71, 50), (72, 50), (73, 50), (68, 49), (68, 48), (67, 45), (67, 46), (67, 47), (67, 48), (68, 45)]

# first Egyptian goal: control Jerusalem in 1070 BC
tJerusalem = (73, 38)

# third Thai goal: allow no foreign powers in South Asia in 1900 AD
tSouthAsiaTL = (88, 24)
tSouthAsiaBR = (110, 38)
lSouthAsianCivs = [iIndia, iTamils, iIndonesia, iKhmer, iMughals, iThailand]

# second Iranian goal: control Mesopotamia, Transoxania and Northwest India in 1750 AD
tSafavidMesopotamiaTL = (75, 39)
tSafavidMesopotamiaBR = (79, 43)
tTransoxaniaTL = (82, 42)
tTransoxaniaBR = (86, 49)
tNWIndiaTL = (85, 36)
tNWIndiaBR = (91, 43)
tNWIndiaExceptions = ((89, 36), (90, 36), (91, 36), (89, 37), (90, 37), (91, 37), (89, 38), (90, 38), (91, 38))

# second German goal: control Austria and a bunch of other countries
tAustriaTL = (61, 47)
tAustriaBR = (66, 50)

# first American goal: allow no European colonies in North America, Central America and the Caribbean
tNCAmericaTL = (3, 33)
tNCAmericaBR = (37, 63)

# first Colombian goal: allow no European civilizations in Peru, Gran Colombia, Guayanas and the Caribbean in 1870 AD
tPeruTL = (25, 16)
tPeruBR = (32, 24)
tGranColombiaTL = (21, 25)
tGranColombiaBR = (32, 35)
tGuayanasTL = (33, 27)
tGuayanasBR = (37, 31)
tCaribbeanTL = (25, 33)
tCaribbeanBR = (33, 39)

# first Canadian goal: connect your capital to an Atlantic and a Pacific port by 1920 AD
lAtlanticCoast = [(34, 50), (33, 51), (35, 51), (30, 52), (31, 52), (32, 52), (30, 53), (35, 53), (30, 54), (31, 54), (32, 54), (35, 54), (36, 54), (32, 55), (33, 55), (34, 55)]
lPacificCoast = [(11, 51), (10, 52), (11, 53), (10, 56)]

# second Canadian goal: control all cities and 90% of the territory in Canada without ever conquering a city by 1950 AD
tCanadaWestTL = (10, 52)
tCanadaWestBR = (26, 61)
tCanadaWestExceptions = ((10, 59), (10, 60), (10, 61), (21, 61), (22, 61), (23, 61), (24, 61), (25, 61))
tCanadaEastTL = (27, 50)
tCanadaEastBR = (36, 59)
tCanadaEastExceptions = ((30, 50), (31, 50), (32, 50), (32, 51))

### GOAL CONSTANTS ###

dTechGoals = {
	iChina: (1, [iCompass, iPaper, iGunpowder, iPrinting]),
	iBabylonia: (0, [iConstruction, iArithmetics, iWriting, iCalendar, iContract]),
	iGreece: (0, [iMathematics, iLiterature, iAesthetics, iPhilosophy, iMedicine]),
	iRome: (2, [iArchitecture, iPolitics, iScholarship, iMachinery, iCivilService]),
	iKorea: (1, [iPrinting]),
	iPoland: (0, [iCivilLiberties]),
}

dEraGoals = {}

dWonderGoals = {
	iEgypt: (1, [iPyramids, iGreatSphinx, iGreatLibrary, iGreatLighthouse], True),
	iGreece: (2, [iOracle, iParthenon, iStatueOfZeus, iTempleOfArtemis, iColossus], True),
	iCarthage: (0, [iGreatCothon], False),
	iPolynesia: (2, [iMoaiStatues], True),
	iMaya: (1, [iTempleOfKukulkan], True),
	iMoors: (0, [iMezquita], False),
	iKhmer: (0, [iWatPreahPisnulok], False),
	iFrance: (2, [iNotreDame, iVersailles, iLouvre, iEiffelTower, iMetropolitain], True),
	iMali: (1, [iUniversityOfSankore], False),
	iItaly: (0, [iSanMarcoBasilica, iSistineChapel, iSantaMariaDelFiore], True),
	iMughals: (1, [iTajMahal, iRedFort, iShalimarGardens], True),
	iAmerica: (1, [iStatueOfLiberty, iBrooklynBridge, iEmpireStateBuilding, iGoldenGateBridge, iPentagon, iUnitedNations], True),
	iBrazil: (1, [iWembley, iCristoRedentor, iItaipuDam], True),
}

dReligionGoals = {}
		
### EVENT HANDLING ###

def setup():

	# 1700 AD scenario: handle dates that have already been passed
	if utils.getScenario() == i1700AD:
		for iPlayer in [iChina, iIndia, iTamils, iKorea, iVikings, iTurks, iSpain, iHolyRome, iPoland, iPortugal, iMughals, iOttomans, iThailand]:
			loseAll(iPlayer)
			
		win(iPersia, 0)
		win(iCongo, 0)
		
		# French goal needs to be winnable
		data.setWonderBuilder(iNotreDame, iFrance)
		data.setWonderBuilder(iVersailles, iFrance)
		
		# help Congo
		data.iCongoSlaveCounter += 500
		
		# help Netherlands
		data.iDutchColonies += 2
	
	# ignore AI goals
	bIgnoreAI = (gc.getDefineINT("NO_AI_UHV_CHECKS") == 1)
	data.bIgnoreAI = bIgnoreAI
	
	if bIgnoreAI:
		for iPlayer in range(iNumPlayers):
			if utils.getHumanID() != iPlayer:
				loseAll(iPlayer)
				
def checkTurn(iGameTurn, iPlayer):

	if not gc.getGame().isVictoryValid(7): return
	
	if iPlayer >= iNumPlayers: return
	
	if iGameTurn == utils.getScenarioStartTurn(): return
	
	if not gc.getPlayer(iPlayer).isAlive(): return
	
	# Don't check AI civilizations to improve speed
	if utils.getHumanID() != iPlayer and data.bIgnoreAI: return
	
	pPlayer = gc.getPlayer(iPlayer)
	
	if iPlayer == iEgypt:
	
		# first goal: have 500 culture and control Jerusalem in 1070 BC
		if iGameTurn == getTurnForYear(-1070):
			if pEgypt.countTotalCulture() >= utils.getTurns(500) and controlsCity(iEgypt, tJerusalem):
				win(iEgypt, 0)
			else:
				lose(iEgypt, 0)
				
		# first goal: build the Pyramids, the Great Sphinx, the Great Lighthouse and the Great Library by 245 BC
		if iGameTurn == getTurnForYear(-245):
			expire(iEgypt, 1)
				
		# third goal: have 5000 culture in 30 BC
		if iGameTurn == getTurnForYear(-30):
			if pEgypt.countTotalCulture() >= utils.getTurns(5000):
				win(iEgypt, 2)
			else:
				lose(iEgypt, 2)
				
	elif iPlayer == iBabylonia:
	
		# first goal: be the first to discover Construction, Arithmetics, Writing, Calendar and Contract
		
		# second goal: make Babylon the most populous city in the world in 690 BC
		if iGameTurn == getTurnForYear(-690):
			if isBestCity(iBabylonia, (76, 40), cityPopulation):
				win(iBabylonia, 1)
			else:
				lose(iBabylonia, 1)
			
		# third goal: make Babylon the most cultured city in the world in 540 BC
		if iGameTurn == getTurnForYear(-540):
			if isBestCity(iBabylonia, (76, 40), cityCulture):
				win(iBabylonia, 2)
			else:
				lose(iBabylonia, 2)
				
	elif iPlayer == iHarappa:
	
		# first goal: establish a trade connection with another civilization by 1900 BC
		if isPossible(iHarappa, 0):
			if isTradeConnected(iHarappa):
				win(iHarappa, 0)
				
		if iGameTurn == getTurnForYear(-1900):
			expire(iHarappa, 0)
			
		# second goal: build three Baths and two Walls by 1700 BC
		if iGameTurn == getTurnForYear(-1700):
			expire(iHarappa, 1)
			
		# third goal: have a total population of 25 by 1300 BC
		if isPossible(iHarappa, 2):
			if pHarappa.getTotalPopulation() >= 25:
				win(iHarappa, 2)
				
		if iGameTurn == getTurnForYear(-1300):
			expire(iHarappa, 2)
				
	elif iPlayer == iChina:
	
		# first goal: build two Confucian and Taoist Cathedrals by 1270 AD
		if iGameTurn == getTurnForYear(1270):
			expire(iChina, 0)
			
		# second goal: be first to discover Compass, Gunpowder, Paper and Printing Press
		
		# third goal: experience eight golden ages by 1840 AD
		if isPossible(iChina, 2):
			if data.iChineseGoldenAgeTurns >= utils.getTurns(64):
				win(iChina, 2)
				
			if pChina.isGoldenAge() and not pChina.isAnarchy():
				data.iChineseGoldenAgeTurns += 1
				
		if iGameTurn == getTurnForYear(1840):
			expire(iChina, 2)
			
	elif iPlayer == iGreece:
	
		# first goal: be the first to discover Mathematics, Literature, Aesthetics, Medicine and Philosophy
			
		# second goal: control Egypt, Phoenicia, Babylonia and Persia in 325 BC
		if iGameTurn == getTurnForYear(-325):
			bEgypt = checkOwnedCiv(iGreece, iEgypt)
			bPhoenicia = checkOwnedCiv(iGreece, iCarthage)
			bBabylonia = checkOwnedCiv(iGreece, iBabylonia)
			bPersia = checkOwnedCiv(iGreece, iPersia)
			if bEgypt and bPhoenicia and bBabylonia and bPersia:
				win(iGreece, 1)
			else:
				lose(iGreece, 1)
		
		# third goal: build the Parthenon, the Colossus, the Statue of Zeus and the Temple of Artemis by 280 BC
		if iGameTurn == getTurnForYear(-280):
			expire(iGreece, 2)
				
	elif iPlayer == iIndia:
	
		# first goal: control the Hindu and Buddhist shrine in 230 BC
		if iGameTurn == getTurnForYear(-230):
			bBuddhistShrine = getNumBuildings(iIndia, iBuddhistShrine) > 0
			bHinduShrine = getNumBuildings(iIndia, iHinduShrine) > 0
			if bHinduShrine and bBuddhistShrine:
				win(iIndia, 0)
			else:
				lose(iIndia, 0)
				
		# second goal: build 18 temples by 650 AD
		if iGameTurn == getTurnForYear(650):
			expire(iIndia, 1)
			
		# third goal: control 18% of the world's population in 1200 AD
		if iGameTurn == getTurnForYear(1200):
			if getPopulationPercent(iIndia) >= 18.0:
				win(iIndia, 2)
			else:
				lose(iIndia, 2)
				
	elif iPlayer == iCarthage:
	
		# first goal: build a Palace and the Great Cothon in Carthagee by 265 BC
		if isPossible(iCarthage, 0):
			bPalace = isBuildingInCity((58, 39), iPalace)
			bGreatCothon = isBuildingInCity((58, 39), iGreatCothon)
			if bPalace and bGreatCothon:
				win(iCarthage, 0)
		
		if iGameTurn == getTurnForYear(-265):
			expire(iCarthage, 0)
				
		# second goal: control Italy and Iberia in 200 BC
		if iGameTurn == getTurnForYear(-200):
			bItaly = isControlled(iCarthage, utils.getPlotList(Areas.tNormalArea[iItaly][0], Areas.tNormalArea[iItaly][1], [(62, 47), (63, 47), (63, 46)]))
			bIberia = isControlled(iCarthage, Areas.getNormalArea(iSpain, False))
			if bItaly and bIberia:
				win(iCarthage, 1)
			else:
				lose(iCarthage, 1)
				
		# third goal: have 3000 gold in 145 BC
		if iGameTurn == getTurnForYear(-145):
			if pCarthage.getGold() >= utils.getTurns(3000):
				win(iCarthage, 2)
			else:
				lose(iCarthage, 2)
				
	elif iPlayer == iPolynesia:
	
		# first goal: settle two of the following island groups by 1025 AD: Hawaii, New Zealand, Marquesas and Easter Island
		if iGameTurn == getTurnForYear(1025):
			expire(iPolynesia, 0)
			
		# second goal: settle Hawaii, New Zealand, Marquesas and Easter Island by 1290 AD
		if iGameTurn == getTurnForYear(1290):
			expire(iPolynesia, 1)
			
		# third goal: build the Moai Statues by 1500 AD
		if iGameTurn == getTurnForYear(1500):
			expire(iPolynesia, 2)
			
	elif iPlayer == iPersia:
	
		# Persia
		if not pPersia.isReborn():
		
			# first goal: control 6% of world territory by 330 BC
			if isPossible(iPersia, 0):
				if getLandPercent(iPersia) >= 5.995:
					win(iPersia, 0)
			
			if iGameTurn == getTurnForYear(-330):
				expire(iPersia, 0)
				
			# second goal: control seven wonders by 225 AD
			if isPossible(iPersia, 1):
				if countWonders(iPersia) >= 7:
					win(iPersia, 1)
					
			if iGameTurn == getTurnForYear(225):
				expire(iPersia, 1)
						
			# third goal: control three holy shrines in 600 AD
			if iGameTurn == getTurnForYear(600):
				if countShrines(iPersia) >= 3:
					win(iPersia, 2)
				else:
					lose(iPersia, 2)
					
		# Iran			
		else:
		
			# first goal: have open borders with 6 European civilizations in 1630
			if iGameTurn == getTurnForYear(1630):
				if countOpenBorders(iPersia, lCivGroups[0]) >= 6:
					win(iPersia, 0)
				else:
					lose(iPersia, 0)
					
			# second goal: control Mesopotamia, Transoxania and Northwest India in 1745 AD
			if iGameTurn == getTurnForYear(1745):
				bMesopotamia = isControlled(iPersia, utils.getPlotList(tSafavidMesopotamiaTL, tSafavidMesopotamiaBR))
				bTransoxania = isControlled(iPersia, utils.getPlotList(tTransoxaniaTL, tTransoxaniaBR))
				bNWIndia = isControlled(iPersia, utils.getPlotList(tNWIndiaTL, tNWIndiaBR, tNWIndiaExceptions))
				if bMesopotamia and bTransoxania and bNWIndia:
					win(iPersia, 1)
				else:
					lose(iPersia, 1)
					
			# third goal: have a city with 20000 culture in 1780 AD
			if iGameTurn == getTurnForYear(1780):
				mostCulturedCity = getMostCulturedCity(iPersia)
				if mostCulturedCity.getCulture(iPersia) >= utils.getTurns(20000):
					win(iPersia, 2)
				else:
					lose(iPersia, 2)
					
	elif iPlayer == iRome:
	
		# first goal: build 6 Barracks, 5 Aqueducts, 4 Amphitheatres and 3 Forums by 180 AD
		if iGameTurn == getTurnForYear(180):
			expire(iRome, 0)
			
		# second goal: control Iberia, Gaul, Britain, Africa, Greece, Asia Minor and Egypt in 180 AD
		if iGameTurn == getTurnForYear(180):
			bSpain = getNumCitiesInArea(iRome, Areas.getNormalArea(iSpain, False)) >= 2
			bFrance = getNumCitiesInArea(iRome, utils.getPlotList(tFranceTL, Areas.tNormalArea[iFrance][1])) >= 3
			bEngland = getNumCitiesInArea(iRome, Areas.getCoreArea(iEngland, False)) >= 1
			bCarthage = getNumCitiesInArea(iRome, utils.getPlotList(tCarthageTL, tCarthageBR)) >= 2
			bByzantium = getNumCitiesInArea(iRome, Areas.getCoreArea(iByzantium, False)) >= 4
			bEgypt = getNumCitiesInArea(iRome, Areas.getCoreArea(iEgypt, False)) >= 2
			if bSpain and bFrance and bEngland and bCarthage and bByzantium and bEgypt:
				win(iRome, 1)
			else:
				lose(iRome, 1)
					
		# third goal: be first to discover Theology, Machinery and Civil Service
		
	elif iPlayer == iMaya:
	
		# Maya
		if not pMaya.isReborn():
		
			# first goal: discover Calendar by 830AD, 830 because that's 10.0.0.0.0 in their Calendar
			if iGameTurn == getTurnForYear(830):
				expire(iMaya, 0)
				
			# second goal: build the Temple of Kukulkan by 1225 AD, because 1224AD is 11.0.0.0.0
			if iGameTurn == getTurnForYear(1225):
				expire(iMaya, 1)
				
			# third goal: make contact with a European civilization before they discover America
			if isPossible(iMaya, 2):
				for iEuropean in lCivGroups[0]:
					if teamMaya.canContact(iEuropean):
						win(iMaya, 2)
						break
			
		# Colombia
		else:
		
			# first goal: allow no European civilizations in Peru, Gran Colombia, the Guayanas and the Caribbean in 1860 AD
			if iGameTurn == getTurnForYear(1860):
				bPeru = isAreaFreeOfCivs(utils.getPlotList(tPeruTL, tPeruBR), lCivGroups[0])
				bGranColombia = isAreaFreeOfCivs(utils.getPlotList(tGranColombiaTL, tGranColombiaBR), lCivGroups[0])
				bGuayanas = isAreaFreeOfCivs(utils.getPlotList(tGuayanasTL, tGuayanasBR), lCivGroups[0])
				bCaribbean = isAreaFreeOfCivs(utils.getPlotList(tCaribbeanTL, tCaribbeanBR), lCivGroups[0])
				if bPeru and bGranColombia and bGuayanas and bCaribbean:
					win(iMaya, 0)
				else:
					lose(iMaya, 0)
					
			# second goal: control South America in 1900 AD
			if iGameTurn == getTurnForYear(1900):
				if isControlled(iMaya, utils.getPlotList(tSAmericaTL, tSAmericaBR, tSouthAmericaExceptions)):
					win(iMaya, 1)
				else:
					lose(iMaya, 1)
			
			# third goal: acquire 3000 gold by selling resources by 1960 AD
			if isPossible(iMaya, 2):
				iTradeGold = 0
				
				for iLoopPlayer in range(iNumPlayers):
					iTradeGold += pMaya.getGoldPerTurnByPlayer(iLoopPlayer)
				
				data.iColombianTradeGold += iTradeGold
				
				if data.iColombianTradeGold >= utils.getTurns(3000):
					win(iMaya, 2)
					
			if iGameTurn == getTurnForYear(1960):
				expire(iMaya, 2)
		
	elif iPlayer == iTamils:
	
		# first goal: have 2000 gold and 2000 culture in 300 AD
		if iGameTurn == getTurnForYear(300):
			if pTamils.getGold() >= utils.getTurns(2000) and pTamils.countTotalCulture() >= utils.getTurns(2000):
				win(iTamils, 0)
			else:
				lose(iTamils, 0)
				
		# second goal: control or vassalize the Deccan and Srivijaya in 1070 AD
		if iGameTurn == getTurnForYear(1070):
			bDeccan = isControlledOrVassalized(iTamils, utils.getPlotList(tDeccanTL, tDeccanBR))
			bSrivijaya = isControlledOrVassalized(iTamils, utils.getPlotList(tSrivijayaTL, tSrivijayaBR))
			if bDeccan and bSrivijaya:
				win(iTamils, 1)
			else:
				lose(iTamils, 1)
				
		# third goal: acquire 5000 gold by trade by 1280 AD
		if isPossible(iTamils, 2):
			iTradeGold = 0
			
			# gold from city trade routes
			iTradeCommerce = 0
			for city in utils.getCityList(iTamils):
				iTradeCommerce += city.getTradeYield(2)
			iTradeGold += iTradeCommerce * pTamils.getCommercePercent(0)
			
			# gold from per turn gold trade
			for iPlayer in range(iNumPlayers):
				iTradeGold += pTamils.getGoldPerTurnByPlayer(iPlayer) * 100
			
			data.iTamilTradeGold += iTradeGold
			
			if data.iTamilTradeGold / 100 >= utils.getTurns(4000):
				win(iTamils, 2)
				
		if iGameTurn == getTurnForYear(1280):
			expire(iTamils, 2)
					
	elif iPlayer == iEthiopia:
		
		# first goal: acquire three incense resources by 400 AD
		if isPossible(iEthiopia, 0):
			if pEthiopia.getNumAvailableBonuses(iIncense) >= 3:
				win(iEthiopia, 0)
				
		if iGameTurn == getTurnForYear(400):
			expire(iEthiopia, 0)
			
		# second goal: convert to Orthodoxy 5 turns after it is founded and have three settled Great Prophets and an Orthodox Cathedral by 1200 AD
		if isPossible(iEthiopia, 1):
			iNumOrthodoxCathedrals = getNumBuildings(iEthiopia, iOrthodoxCathedral)
			iGreatProphets = countSpecialists(iEthiopia, iSpecialistGreatProphet)
			if data.bEthiopiaConverted and iNumOrthodoxCathedrals >= 1 and iGreatProphets >= 3:
				win(iEthiopia, 1)
		
			if gc.getGame().isReligionFounded(iOrthodoxy) and iGameTurn > gc.getGame().getReligionGameTurnFounded(iOrthodoxy) + utils.getTurns(5):
				if not data.bEthiopiaConverted:
					expire(iEthiopia, 1)
				
		if iGameTurn == getTurnForYear(1200):
			expire(iEthiopia, 1)
			
		# third goal: make sure there are more Orthodox than Muslim cities in Africa in 1500 AD
		if iGameTurn == getTurnForYear(1500):
			iOrthodoxCities = countRegionReligion(iOrthodoxy, lAfrica)
			iMuslimCities = countRegionReligion(iIslam, lAfrica)
			if iOrthodoxCities > iMuslimCities:
				win(iEthiopia, 2)
			else:
				lose(iEthiopia, 2)
				
	elif iPlayer == iKorea:
	
		# first goal: build a Buddhist Stupa and a Confucian Academy by 1230 AD
		if iGameTurn == getTurnForYear(1230):
			expire(iKorea, 0)
			
		# second goal: be first to discover Printing Press
		
		# third goal: sink 20 enemy ships by 1600 AD
		if iGameTurn == getTurnForYear(1600):
			expire(iKorea, 2)
					
	elif iPlayer == iByzantium:
		
		# first goal: have 5000 gold in 1070 AD
		if iGameTurn == getTurnForYear(1070):
			if pByzantium.getGold() >= utils.getTurns(5000):
				win(iByzantium, 0)
			else:
				lose(iByzantium, 0)
				
		# second goal: make Constantinople the world's largest and most cultured city in 1205 AD
		if iGameTurn == getTurnForYear(1205):
			bLargest = isBestCity(iByzantium, (68, 45), cityPopulation)
			bCulture = isBestCity(iByzantium, (68, 45), cityCulture)
			if bLargest and bCulture:
				win(iByzantium, 1)
			else:
				lose(iByzantium, 1)
				
		# third goal: control three cities in the Balkans, Northern Africa and the Near East in 1455 AD
		if iGameTurn == getTurnForYear(1455):
			bBalkans = getNumCitiesInArea(iByzantium, utils.getPlotList(tBalkansTL, tBalkansBR)) >= 3
			bNorthAfrica = getNumCitiesInArea(iByzantium, utils.getPlotList(tNorthAfricaTL, tNorthAfricaBR)) >= 3
			bNearEast = getNumCitiesInArea(iByzantium, utils.getPlotList(tNearEastTL, tNearEastBR)) >= 3
			if bBalkans and bNorthAfrica and bNearEast:
				win(iByzantium, 2)
			else:
				lose(iByzantium, 2)
					
	elif iPlayer == iJapan:
	
		# first goal: have an average culture of 8000 by 1870 AD without ever losing a city
		if isPossible(iJapan, 0):
			if getAverageCulture(iJapan) >= utils.getTurns(8000):
				win(iJapan, 0)
				
		if iGameTurn == getTurnForYear(1870):
			expire(iJapan, 0)
				
		# second goal: control or vassalize Korea, Manchuria, China, Indochina, Indonesia and the Philippines in 1945 AD
		if iGameTurn == getTurnForYear(1945):
			bKorea = isControlledOrVassalized(iJapan, utils.getPlotList(tKoreaTL, tKoreaBR))
			bManchuria = isControlledOrVassalized(iJapan, utils.getPlotList(tManchuriaTL, tManchuriaBR))
			bChina = isControlledOrVassalized(iJapan, utils.getPlotList(tChinaTL, tChinaBR))
			bIndochina = isControlledOrVassalized(iJapan, utils.getPlotList(tIndochinaTL, tIndochinaBR, tIndochinaExceptions))
			bIndonesia = isControlledOrVassalized(iJapan, utils.getPlotList(tIndonesiaTL, tIndonesiaBR))
			bPhilippines = isControlledOrVassalized(iJapan, utils.getPlotList(tPhilippinesTL, tPhilippinesBR))
			if bKorea and bManchuria and bChina and bIndochina and bIndonesia and bPhilippines:
				win(iJapan, 1)
			else:
				lose(iJapan, 1)
				
		# third goal: be the first to discover 5 modern techs
		
	elif iPlayer == iVikings:
	
		# first goal: found a city in America by 1020 AD
		if iGameTurn == getTurnForYear(1020):
			expire(iVikings, 0)

		# second goal: control the core of a European civilization in 1070 AD
		if iGameTurn == getTurnForYear(1070):
			lEuroCivs = [iLoopPlayer for iLoopPlayer in lCivGroups[0] if tBirth[iLoopPlayer] < 1070 and iPlayer != iLoopPlayer]
			if isCoreControlled(iVikings, lEuroCivs):
				win(iVikings, 1)
			else:
				lose(iVikings, 1)
				
		# third goal: acquire 2000 gold by pillaging, conquering cities and sinking ships by 1295 AD
		if isPossible(iVikings, 2):
			if data.iVikingGold >= utils.getTurns(2000):
				win(iVikings, 2)
				
		if iGameTurn == getTurnForYear(1295):
			expire(iVikings, 2)
			
	elif iPlayer == iTurks:
	
		# first goal: control 6% of the world's territory and pillage 20 improvements by 900 AD
		if isPossible(iTurks, 0):
			if getLandPercent(iTurks) >= 5.995 and data.iTurkicPillages >= 20:
				win(iTurks, 0)
				
		if iGameTurn == getTurnForYear(900):
			expire(iTurks, 0)
			
		# second goal: create an overland trade route between a Chinese and a Mediterranean city and spread the Silk Route to ten of your cities by 1100 AD
		if isPossible(iTurks, 1):
			if isConnectedByTradeRoute(iTurks, utils.getPlotList(tChinaTL, tChinaBR), lMediterraneanPorts) and pTurks.countCorporations(iSilkRoute) >= 10:
				win(iTurks, 1)
				
		if iGameTurn == getTurnForYear(1100):
			expire(iTurks, 1)
			
		# third goal: have a capital with developing culture by 900 AD, a different capital with refined culture by 1100 AD and another capital with influential culture by 1400 AD
		if isPossible(iTurks, 2):
			capital = pTurks.getCapitalCity()
			tCapital = (capital.getX(), capital.getY())
			
			if iGameTurn <= getTurnForYear(900):
				if not data.tFirstTurkicCapital and capital.getCulture(iTurks) >= gc.getCultureLevelInfo(3).getSpeedThreshold(gc.getGame().getGameSpeedType()):
					data.tFirstTurkicCapital = tCapital
			
			if iGameTurn <= getTurnForYear(1100):
				if data.tFirstTurkicCapital and not data.tSecondTurkicCapital and tCapital != data.tFirstTurkicCapital and capital.getCulture(iTurks) >= gc.getCultureLevelInfo(4).getSpeedThreshold(gc.getGame().getGameSpeedType()):
					data.tSecondTurkicCapital = tCapital
					
			if iGameTurn <= getTurnForYear(1400):
				if tCapital != data.tFirstTurkicCapital and tCapital != data.tSecondTurkicCapital and data.tFirstTurkicCapital and data.tSecondTurkicCapital and capital.getCulture(iTurks) >= gc.getCultureLevelInfo(5).getSpeedThreshold(gc.getGame().getGameSpeedType()):
					win(iTurks, 2)
					
		if iGameTurn == getTurnForYear(900):
			if not data.tFirstTurkicCapital:
				expire(iTurks, 2)
				
		if iGameTurn == getTurnForYear(1100):
			if not data.tSecondTurkicCapital:
				expire(iTurks, 2)
				
		if iGameTurn == getTurnForYear(1400):
			expire(iTurks, 2)
			
	elif iPlayer == iArabia:
	
		# first goal: be the most advanced civilization in 1260 AD
		if iGameTurn == getTurnForYear(1260):
			if isBestPlayer(iArabia, playerTechs):
				win(iArabia, 0)
			else:
				lose(iArabia, 0)
				
		# second goal: control or vassalize Spain, the Maghreb, Egypt, Mesopotamia and Persia in 1260 AD
		if iGameTurn == getTurnForYear(1260):
			bEgypt = isControlledOrVassalized(iArabia, Areas.getCoreArea(iEgypt, False))
			bMaghreb = isControlledOrVassalized(iArabia, utils.getPlotList(tCarthageTL, tCarthageBR))
			bMesopotamia = isControlledOrVassalized(iArabia, Areas.getCoreArea(iBabylonia, False))
			bPersia = isControlledOrVassalized(iArabia, Areas.getCoreArea(iPersia, False))
			bSpain = isControlledOrVassalized(iArabia, Areas.getNormalArea(iSpain, False))
			if bSpain and bMaghreb and bEgypt and bMesopotamia and bPersia:
				win(iArabia, 1)
			else:
				lose(iArabia, 1)
		
		# third goal: spread Islam to 30% of the cities in the world by 1515 AD
		if isPossible(iArabia, 2):
			if gc.getGame().calculateReligionPercent(iIslam) >= 30.0:
				win(iArabia, 2)
				
		if iGameTurn == getTurnForYear(1515):
			expire(iArabia, 2)

	elif iPlayer == iTibet:
	
		# first goal: acquire four cities by 840 AD
		if iGameTurn == getTurnForYear(840):
			expire(iTibet, 0)
			
		# second goal: spread Buddhism to 30% by 1685 AD
		if isPossible(iTibet, 1):
			if gc.getGame().calculateReligionPercent(iBuddhism) >= 30.0:
				win(iTibet, 1)
				
		if iGameTurn == getTurnForYear(1685):
			expire(iTibet, 1)
			
		# third goal: settle five great prophets in Lhasa by 1685 AD
		if isPossible(iTibet, 2):
			if countCitySpecialists(iTibet, Areas.getCapital(iPlayer), iSpecialistGreatProphet) >= 5:
				win(iTibet, 2)
				
		if iGameTurn == getTurnForYear(1685):
			expire(iTibet, 2)
			
	elif iPlayer == iIndonesia:
	
		# first goal: have the largest population in the world in 1390 AD
		if iGameTurn == getTurnForYear(1390):
			if isBestPlayer(iIndonesia, playerRealPopulation):
				win(iIndonesia, 0)
			else:
				lose(iIndonesia, 0)
				
		# second goal: acquire 10 different happiness resources by 1510 AD
		if isPossible(iIndonesia, 1):
			if countHappinessResources(iIndonesia) >= 10:
				win(iIndonesia, 1)
				
		if iGameTurn == getTurnForYear(1510):
			expire(iIndonesia, 1)
		
		# third goal: control 9% of the world's population in 1950 AD
		if iGameTurn == getTurnForYear(1950):
			if getPopulationPercent(iIndonesia) >= 9.0:
				win(iIndonesia, 2)
			else:
				lose(iIndonesia, 2)
				
	elif iPlayer == iMoors:
	
		# first goal: build La Mezquita and settle three great prophets, scientists or engineers in Cordoba by 1030 AD
		if isPossible(iMoors, 0):
			bMezquita = data.getWonderBuilder(iMezquita) == iMoors
		
			iCounter = 0
			iCounter += countCitySpecialists(iMoors, (51, 41), iSpecialistGreatProphet)
			iCounter += countCitySpecialists(iMoors, (51, 41), iSpecialistGreatScientist)
			iCounter += countCitySpecialists(iMoors, (51, 41), iSpecialistGreatEngineer)
			
			if bMezquita and iCounter >= 3:
				win(iMoors, 0)
				
		if iGameTurn == getTurnForYear(1030):
			expire(iMoors, 0)
		
		# second goal: control three cities in the Maghreb and conquer two cities in Iberia and West Africa
		if iGameTurn == getTurnForYear(1210):
			bIberia = getNumConqueredCitiesInArea(iMoors, utils.getPlotList(tIberiaTL, tIberiaBR)) >= 2
			bMaghreb = getNumCitiesInArea(iMoors, utils.getPlotList(tMaghrebTL, tMaghrebBR)) >= 3
			bWestAfrica = getNumConqueredCitiesInArea(iMoors, utils.getPlotList(tWestAfricaTL, tWestAfricaBR)) >= 2
			
			if bIberia and bMaghreb and bWestAfrica:
				win(iMoors, 1)
			else:
				lose(iMoors, 1)
					
		# third goal: acquire 3000 gold through piracy by 1680 AD
		if isPossible(iMoors, 2):
			if data.iMoorishGold >= utils.getTurns(3000):
				win(iMoors, 2)
				
		if iGameTurn == getTurnForYear(1680):
			expire(iMoors, 2)
			
	elif iPlayer == iSpain:
	
		# first goal: be the first to found a colony in America
		
		# second goal: secure 10 gold or silver resources by 1600 AD
		if isPossible(iSpain, 1):
			iNumGold = countResources(iSpain, iGold)
			iNumSilver = countResources(iSpain, iSilver)
			
			if iNumGold + iNumSilver >= 10:
				win(iSpain, 1)
				
		if iGameTurn == getTurnForYear(1600):
			expire(iSpain, 1)
			
		# third goal: spread Catholicism to 30% and allow no Protestant civilizations in Europe in 1650 AD
		if iGameTurn == getTurnForYear(1650):
			fReligionPercent = gc.getGame().calculateReligionPercent(iCatholicism)
			
			bProtestantsEurope = isStateReligionInArea(iProtestantism, tEuropeTL, tEuropeBR)
			bProtestantsEasternEurope = isStateReligionInArea(iProtestantism, tEasternEuropeTL, tEasternEuropeBR)
			
			if fReligionPercent >= 30.0 and not bProtestantsEurope and not bProtestantsEasternEurope:
				win(iSpain, 2)
			else:
				lose(iSpain, 2)
				
	elif iPlayer == iFrance:
	
		# first goal: have legendary culture in Paris in 1790 AD
		if iGameTurn == getTurnForYear(1790):
			if getCityCulture(iFrance, (55, 50)) >= utils.getTurns(40000):
				win(iFrance, 0)
			else:
				lose(iFrance, 0)
				
		# second goal: control 40% of Europe and North America in 1815 AD
		if iGameTurn == getTurnForYear(1815):
			iEurope, iTotalEurope = countControlledTiles(iFrance, tEuropeTL, tEuropeBR, True)
			iEasternEurope, iTotalEasternEurope = countControlledTiles(iFrance, tEasternEuropeTL, tEasternEuropeBR, True)
			iNorthAmerica, iTotalNorthAmerica = countControlledTiles(iFrance, tNorthAmericaTL, tNorthAmericaBR, True)
			
			fEurope = (iEurope + iEasternEurope) * 100.0 / (iTotalEurope + iTotalEasternEurope)
			fNorthAmerica = iNorthAmerica * 100.0 / iTotalNorthAmerica
			
			if fEurope >= 40.0 and fNorthAmerica >= 40.0:
				win(iFrance, 1)
			else:
				lose(iFrance, 1)
				
		# third goal: build Notre Dame, Versailles, the Louvre, the Eiffel Tower and the Metropolitain by 1890 AD
		if iGameTurn == getTurnForYear(1890):
			expire(iFrance, 2)
			
	elif iPlayer == iKhmer:
	
		# first Khmer goal: build four Buddhist and Hindu monasteries and Wat Preah Pisnulok in 1220 AD
		if iGameTurn == getTurnForYear(1220):
			if isPossible(iKhmer, 0):
				iBuddhist = getNumBuildings(iKhmer, iBuddhistMonastery)
				iHindu = getNumBuildings(iKhmer, iHinduMonastery)
				bWatPreahPisnulok = data.getWonderBuilder(iWatPreahPisnulok) == iKhmer
				if iBuddhist >= 4 and iHindu >= 4 and bWatPreahPisnulok:
					win(iKhmer, 0)
				else:
					lose(iKhmer, 0)
				
		# second goal: have an average city size of 10 in 1220 AD
		if iGameTurn == getTurnForYear(1220):
			if isPossible(iKhmer, 1):
				if getAverageCitySize(iKhmer) >= 10.0:
					win(iKhmer, 1)
				else:
					lose(iKhmer, 1)
			
		# third goal: have 7000 culture by 1350 AD
		if isPossible(iKhmer, 2):
			if pKhmer.countTotalCulture() >= utils.getTurns(7000):
				win(iKhmer, 2)
				
		if iGameTurn == getTurnForYear(1350):
			expire(iKhmer, 2)
			
	elif iPlayer == iEngland:
			
		# first goal: control a total of 25 frigates and ships of the line and sink 50 enemy ships by 1815 AD
		if isPossible(iEngland, 0):
			iEnglishNavy = 0
			iEnglishNavy += pEngland.getUnitClassCount(gc.getUnitInfo(iFrigate).getUnitClassType())
			iEnglishNavy += pEngland.getUnitClassCount(gc.getUnitInfo(iShipOfTheLine).getUnitClassType())
			
			if iEnglishNavy >= 25 and data.iEnglishSinks >= 50:
				win(iEngland, 0)
		
		if iGameTurn == getTurnForYear(1815):
			expire(iEngland, 0)
			
		# second goal: be the first to discover a number of Industrial techs

		# third goal: colonize every continent by 1860 AD and build the Cape to Cairo Railway by 1920 AD
		if iGameTurn == getTurnForYear(1860):
			bNAmerica = getNumCitiesInArea(iEngland, utils.getPlotList(tNorthAmericaTL, tNorthAmericaBR)) < 5
			bSCAmerica = getNumCitiesInArea(iEngland, utils.getPlotList(tSouthCentralAmericaTL, tSouthCentralAmericaBR)) < 3
			bAfrica = getNumCitiesInArea(iEngland, utils.getPlotList(tAfricaTL, tAfricaBR)) < 4
			bAsia = getNumCitiesInArea(iEngland, utils.getPlotList(tAsiaTL, tAsiaBR)) < 6
			bOceania = getNumCitiesInArea(iEngland, utils.getPlotList(tOceaniaTL, tOceaniaBR)) < 6
			if bNAmerica or bSCAmerica or bAfrica or bAsia or bOceania:
				lose(iEngland, 2)
				
		if isPossible(iEngland, 2):
			if isConnectedByRailroad(iEngland, (63, 10), lNorthernEgypt):
				if gc.getGame().getGameTurn() >= getTurnForYear(1860):
					win(iEngland, 2)
				else:
					bNAmerica = getNumCitiesInArea(iEngland, utils.getPlotList(tNorthAmericaTL, tNorthAmericaBR)) >= 5
					bSCAmerica = getNumCitiesInArea(iEngland, utils.getPlotList(tSouthCentralAmericaTL, tSouthCentralAmericaBR)) >= 3
					bAfrica = getNumCitiesInArea(iEngland, utils.getPlotList(tAfricaTL, tAfricaBR)) >= 4
					bAsia = getNumCitiesInArea(iEngland, utils.getPlotList(tAsiaTL, tAsiaBR)) >= 6
					bOceania = getNumCitiesInArea(iEngland, utils.getPlotList(tOceaniaTL, tOceaniaBR)) >= 6
					if bNAmerica and bSCAmerica and bAfrica and bAsia and bOceania:
						win(iEngland, 2)

		if iGameTurn == getTurnForYear(1920):
			expire(iEngland, 2)
		
	elif iPlayer == iHolyRome:
	
		# first goal: control Saint Peter's Basilica in 1000 AD, the Church of the Anastasis in 1200 AD and All Saint's Church in 1550 AD
		if iGameTurn == getTurnForYear(1000):
			if isPossible(iHolyRome, 0):
				if getNumBuildings(iHolyRome, iCatholicShrine) > 0:
					data.lHolyRomanShrines[0] = True
				else:
					expire(iHolyRome, 0)
					
		if iGameTurn == getTurnForYear(1200):
			if isPossible(iHolyRome, 0):
				if getNumBuildings(iHolyRome, iOrthodoxShrine) > 0:
					data.lHolyRomanShrines[1] = True
				else:
					expire(iHolyRome, 0)
					
		if iGameTurn == getTurnForYear(1550):
			if isPossible(iHolyRome, 0):
				if getNumBuildings(iHolyRome, iProtestantShrine) > 0:
					data.lHolyRomanShrines[2] = True
					win(iHolyRome, 0)
				else:
					expire(iHolyRome, 0)

		# second goal: have three Catholic vassals in Europe by 1650 AD
		if isPossible(iHolyRome, 1):
			if countVassals(iHolyRome, lCivGroups[0], iCatholicism) >= 3:
				win(iHolyRome, 1)
		
		if iGameTurn == getTurnForYear(1650):
			expire(iHolyRome, 1)
		
		# third goal: settle a total of ten great artists and statesmen in Vienna and have pleased or better relations with eight independent European civilizations by 1850 AD
		if isPossible(iHolyRome, 2):
			iGreatArtists = countCitySpecialists(iHolyRome, tVienna, iSpecialistGreatArtist)
			iGreatStatesmen = countCitySpecialists(iHolyRome, tVienna, iSpecialistGreatStatesman)
			iPleasedOrBetterEuropeans = countPlayersWithAttitudeInGroup(iHolyRome, AttitudeTypes.ATTITUDE_PLEASED, lCivGroups[0])
			
			if iGreatArtists + iGreatStatesmen >= 10 and iPleasedOrBetterEuropeans >= 8:
				win(iHolyRome, 2)
		
		if iGameTurn == getTurnForYear(1850):
			expire(iHolyRome, 2)
			
	elif iPlayer == iRussia:
	
		# first goal: found eight cities in Siberia by 1720 AD and build the Trans-Siberian Railway by 1920 AD
		if iGameTurn == getTurnForYear(1720):
			if getNumFoundedCitiesInArea(iRussia, utils.getPlotList(tSiberiaTL, tSiberiaBR)) < 8:
				lose(iRussia, 0)
				
		if isPossible(iRussia, 0):
			if isConnectedByRailroad(iRussia, Areas.getCapital(iRussia), lSiberianCoast):
				win(iRussia, 0)
					
		if iGameTurn == getTurnForYear(1920):
			expire(iRussia, 0)
			
		# second goal: be the first civilization to complete the Manhattan Project and the Apollo Program
		
		# third goal: have friendly relations with six communist civilizations by 1990 AD
		if isPossible(iRussia, 2):
			if dc.isCommunist(iPlayer) and countPlayersWithAttitudeAndCriteria(iPlayer, AttitudeTypes.ATTITUDE_FRIENDLY, dc.isCommunist) >= 5:
				win(iRussia, 2)
				
		if iGameTurn == getTurnForYear(1990):
			expire(iRussia, 2)
			
	elif iPlayer == iMali:
		
		# first goal: conduct a trade mission to your holy city by 1330 AD
		if iGameTurn == getTurnForYear(1330):
			expire(iMali, 0)
			
		# second goal: build the University of Sankore and settle a great prophet in its city by 1470 AD
		if isPossible(iMali, 1):
			for city in utils.getCityList(iMali):
				if city.isHasRealBuilding(iUniversityOfSankore) and city.getFreeSpecialistCount(iSpecialistGreatProphet) >= 1:
					win(iMali, 1)
		
		if iGameTurn == getTurnForYear(1470):
			expire(iMali, 1)
			
		# third goal: have 5000 gold in 1500 AD and 10000 gold in 1600 AD
		if iGameTurn == getTurnForYear(1500):
			if pMali.getGold() < utils.getTurns(5000):
				lose(iMali, 2)
				
		if iGameTurn == getTurnForYear(1600) and isPossible(iMali, 2):
			if pMali.getGold() >= utils.getTurns(10000):
				win(iMali, 2)
			else:
				lose(iMali, 2)
				
	elif iPlayer == iPoland:

		# first goal: be the first to discover Civil Liberties
	
		# second goal: have four cities with a population of 14 by 1650 AD
		if isPossible(iPoland, 1):
			if countCitiesOfSize(iPoland, 14) >= 4:
				win(iPoland, 1)
				
		if iGameTurn == getTurnForYear(1650):
			expire(iPoland, 1)
		
		# third goal: build three Christian Cathedrals by 1685 AD
		if iGameTurn == getTurnForYear(1685):
			expire(iPoland, 2)
			
	elif iPlayer == iPortugal:
	
		# first goal: have open borders with 15 civilizations by 1580 AD
		if isPossible(iPortugal, 0):
			if countOpenBorders(iPortugal) >= 15:
				win(iPortugal, 0)
				
		if iGameTurn == getTurnForYear(1580):
			expire(iPortugal, 0)
			
		# second goal: acquire 12 colonial resources by 1640 AD
		if isPossible(iPortugal, 1):
			if countAcquiredResources(iPortugal, lColonialResources) >= 12:
				win(iPortugal, 1)
				
		if iGameTurn == getTurnForYear(1640):
			expire(iPortugal, 1)
			
		# third goal: control 20 cities in Brazil, Africa and Asia in 1760 AD
		if iGameTurn == getTurnForYear(1760):
			iCount = 0
			iCount += getNumCitiesInArea(iPortugal, utils.getPlotList(tBrazilTL, tBrazilBR))
			iCount += getNumCitiesInRegions(iPortugal, lAfrica)
			iCount += getNumCitiesInRegions(iPortugal, lAsia)
			if iCount >= 20:
				win(iPortugal, 2)
			else:
				lose(iPortugal, 2)
				
	elif iPlayer == iInca:
	
		# first goal: build five Tambos and a road along the Andean coast by 1530 AD
		if isPossible(iInca, 0):
			if isRoad(iInca, lAndeanCoast) and getNumBuildings(iInca, iTambo) >= 5:
				win(iInca, 0)
				
		if iGameTurn == getTurnForYear(1530):
			expire(iInca, 0)
			
		# second goal: have 2500 gold in 1530 AD
		if iGameTurn == getTurnForYear(1530):
			if pInca.getGold() >= utils.getTurns(2500):
				win(iInca, 1)
			else:
				lose(iInca, 1)
			
		# third goal: allow no other civilisations in South America in 1570 AD
		if iGameTurn == getTurnForYear(1570):
			if isAreaOnlyCivs(tSAmericaTL, tSAmericaBR, [iInca]):
				win(iInca, 2)
			else:
				lose(iInca, 2)
				
	elif iPlayer == iItaly:
	
		# first goal: build San Marco Basilica, the Sistine Chapel and Santa Maria del Fiore by 1525 AD
		if iGameTurn == getTurnForYear(1525):
			expire(iItaly, 0)
			
		# second goal: have three cities with influential culture by 1560 AD
		if isPossible(iItaly, 1):
			if countCitiesWithCultureLevel(iItaly, 5) >= 3:
				win(iItaly, 1)
				
		if iGameTurn == getTurnForYear(1560):
			expire(iItaly, 1)
			
		# third goal: control 75% of the Mediterranean by 1945 AD
		if isPossible(iItaly, 2):
			iMediterranean, iTotalMediterranean = countControlledTiles(iItaly, tMediterraneanTL, tMediterraneanBR, False, tMediterraneanExceptions, True)
			fMediterranean = iMediterranean * 100.0 / iTotalMediterranean
			
			if fMediterranean >= 75.0:
				win(iItaly, 2)
				
		if iGameTurn == getTurnForYear(1945):
			expire(iItaly, 2)
			
	elif iPlayer == iMongolia:
	
		# first goal: control China by 1280 AD
		if isPossible(iMongolia, 0):
			if checkOwnedCiv(iMongolia, iChina):
				win(iMongolia, 0)
				
		if iGameTurn == getTurnForYear(1280):
			expire(iMongolia, 0)
			
		# second goal: raze 7 cities by 1405
		if iGameTurn == getTurnForYear(1405):
			expire(iMongolia, 1)

		# third goal: control 10% of world territory by 1405 AD
		if isPossible(iMongolia, 2):
			if getLandPercent(iMongolia) >= 9.995:
				win(iMongolia, 2)
				
		if iGameTurn == getTurnForYear(1405):
			expire(iMongolia, 2)
					
	elif iPlayer == iMughals:
	
		# first goal: build three Muslim Cathedrals and one Hindu Cathedral by 1605 AD
		if iGameTurn == getTurnForYear(1605):
			expire(iMughals, 0)
			
		# second goal: build the Red Fort, Shalimar Gardens and the Taj Mahal by 1660 AD
		if iGameTurn == getTurnForYear(1660):
			expire(iMughals, 1)
			
		# third goal: have more than 50000 culture in 1740 AD
		if iGameTurn == getTurnForYear(1740):
			if pMughals.countTotalCulture() >= utils.getTurns(50000):
				win(iMughals, 2)
			else:
				lose(iMughals, 2)
			
	elif iPlayer == iAztecs:
	
		# Aztecs
		if not pAztecs.isReborn():
		
			# first goal: make Tenochtitlan the largest city in the world in 1520 AD
			if iGameTurn == getTurnForYear(1520):
				if isBestCity(iAztecs, (18, 37), cityPopulation):
					win(iAztecs, 0)
				else:
					lose(iAztecs, 0)
					
			# second goal: build five step pyramids and sacrificial altars by 1520 AD
			if isPossible(iAztecs, 1):
				if getNumBuildings(iAztecs, utils.getUniqueBuilding(iAztecs, iPaganTemple)) >= 5 and getNumBuildings(iAztecs, iSacrificialAltar) >= 5:
					win(iAztecs, 1)
			
			if iGameTurn == getTurnForYear(1520):
				expire(iAztecs, 1)
				
			# third goal: enslave 20 old world units
			if isPossible(iAztecs, 2):
				if data.iAztecSlaves >= 20:
					win(iAztecs, 2)
					
		# Mexico
		else:
		
			# first goal: build three cathedrals of your state religion by 1910 AD
			if iGameTurn == getTurnForYear(1910):
				expire(iAztecs, 0)
				
			# second goal: create three great generals by 1940 AD
			if iGameTurn == getTurnForYear(1940):
				expire(iAztecs, 1)
				
			# third goal: make Mexico City the largest city in the world in 1970 AD
			if iGameTurn == getTurnForYear(1970):
				if isBestCity(iAztecs, (18, 37), cityPopulation):
					win(iAztecs, 2)
				else:
					lose(iAztecs, 2)
				
	elif iPlayer == iOttomans:
	
		# first goal: have four non-obsolete wonders in your capital in 1565 AD
		if iGameTurn == getTurnForYear(1565):
			capital = pOttomans.getCapitalCity()
			if countCityWonders(iOttomans, (capital.getX(), capital.getY()), False) >= 4:
				win(iOttomans, 0)
			else:
				lose(iOttomans, 0)
				
		# second goal: control the Eastern Mediterranean, the Black Sea, Cairo, Mecca, Baghdad and Vienna by 1700 AD
		if isPossible(iOttomans, 1):
			bEasternMediterranean = isCultureControlled(iOttomans, lEasternMediterranean)
			bBlackSea = isCultureControlled(iOttomans, lBlackSea)
			bCairo = controlsCity(iOttomans, tCairo)
			bMecca = controlsCity(iOttomans, tMecca)
			bBaghdad = controlsCity(iOttomans, tBaghdad)
			bVienna = controlsCity(iOttomans, tVienna)
			
			if bEasternMediterranean and bBlackSea and bCairo and bMecca and bBaghdad and bVienna:
				win(iOttomans, 1)
				
		if iGameTurn == getTurnForYear(1700):
			expire(iOttomans, 1)
			
		# third goal: have more culture than all European civilizations combined in 1730 AD
		if iGameTurn == getTurnForYear(1730):
			if pOttomans.countTotalCulture() > getTotalCulture(lCivGroups[0]):
				win(iOttomans, 2)
			else:
				lose(iOttomans, 2)
				
	elif iPlayer == iThailand:
	
		# first goal: have open borders with 12 civilizations by 1690 AD
		if isPossible(iThailand, 0):
			if countOpenBorders(iThailand) >= 12:
				win(iThailand, 0)
				
		if iGameTurn == getTurnForYear(1690):
			expire(iThailand, 0)
			
		# second goal: make Ayutthaya the most populous city in the world in 1690 AD
		if iGameTurn == getTurnForYear(1690):
			if isBestCity(iThailand, (101, 33), cityPopulation) or isBestCity(iThailand, (102, 33), cityPopulation):
				win(iThailand, 1)
			else:
				lose(iThailand, 1)
				
		# third goal: allow no foreign powers in South Asia in 1945 AD
		if iGameTurn == getTurnForYear(1945):
			if isAreaOnlyCivs(tSouthAsiaTL, tSouthAsiaBR, lSouthAsianCivs):
				win(iThailand, 2)
			else:
				lose(iThailand, 2)
				
	elif iPlayer == iCongo:
	
		# first goal: acquire 15% of the votes in the Apostolic Palace by 1660 AD
		if isPossible(iCongo, 0):
			if getApostolicVotePercent(iCongo) >= 15.0:
				win(iCongo, 0)
				
		if iGameTurn == getTurnForYear(1660):
			expire(iCongo, 0)
			
		# second goal: gain 1000 gold through slave trade by 1840 AD
		if iGameTurn == getTurnForYear(1840):
			expire(iCongo, 1)
			
		# third goal: enter the Industrial Era before anyone enters the Modern Era
		
	elif iPlayer == iNetherlands:
	
		# first goal: build a Stock Exchange and settle four great merchants, scientists or artists in Amsterdam by 1745 AD
		if isPossible(iNetherlands, 0):
			bStockExchange = pNetherlands.getCapitalCity().isHasRealBuilding(iStockExchange)
		
			iCounter = 0
			iCounter += countCitySpecialists(iNetherlands, Areas.getCapital(iNetherlands), iSpecialistGreatMerchant)
			iCounter += countCitySpecialists(iNetherlands, Areas.getCapital(iNetherlands), iSpecialistGreatScientist)
			iCounter += countCitySpecialists(iNetherlands, Areas.getCapital(iNetherlands), iSpecialistGreatArtist)
			
			if bStockExchange and iCounter >= 4:
				win(iNetherlands, 0)
				
		if iGameTurn == getTurnForYear(1745):
			expire(iNetherlands, 0)
			
		# second goal: conquer five European colonies by 1785 AD
		if iGameTurn == getTurnForYear(1785):
			expire(iNetherlands, 1)
			
		# third goal: secure or get by trade eight spice resources by 1795 AD
		if isPossible(iNetherlands, 2):
			if pNetherlands.getNumAvailableBonuses(iSpices) >= 8:
				win(iNetherlands, 2)
				
		if iGameTurn == getTurnForYear(1795):
			expire(iNetherlands, 2)
			
	elif iPlayer == iGermany:
	
		# first goal: settle seven great people in Berlin in 1870 AD
		if iGameTurn == getTurnForYear(1870):
			iCount = 0
			for iSpecialist in lGreatPeople:
				iCount += countCitySpecialists(iPrussia, Areas.getCapital(iGermany), iSpecialist)
			if iCount >= 7:
				win(iGermany, 0)
			else:
				lose(iGermany, 0)
				
		# second goal: control Austria, Poland, the Netherlands, Scandinavia, France, England and Russia in 1945
		if iGameTurn == getTurnForYear(1945):
			bAustria = isControlled(iGermany, utils.getPlotList(tAustriaTL, tAustriaBR))
			bPoland = checkOwnedCiv(iGermany, iPoland)
			bNetherlands = checkOwnedCiv(iGermany, iNetherlands)
			bScandinavia = checkOwnedCiv(iGermany, iVikings)
			bFrance = checkOwnedCiv(iGermany, iFrance)
			bEngland = checkOwnedCiv(iGermany, iEngland)
			bRussia = checkOwnedCiv(iGermany, iRussia)
			if bAustria and bPoland and bNetherlands and bScandinavia and bFrance and bEngland and bRussia:
				win(iGermany, 1)
			else:
				lose(iGermany, 1)
				
		# third goal: be the first to complete the tech tree
		
	elif iPlayer == iAmerica:
	
		# first goal: allow no European colonies in North America, Central America and the Caribbean and control or vassalize Mexico in 1900 AD
		if iGameTurn == getTurnForYear(1900):
			if isAreaFreeOfCivs(utils.getPlotList(tNCAmericaTL, tNCAmericaBR), lCivGroups[0]) and isControlledOrVassalized(iAmerica, Areas.getCoreArea(iAztecs, True)):
				win(iAmerica, 0)
			else:
				lose(iAmerica, 0)
				
		# second goal: build the Statue of Liberty, the Brooklyn Bridge, the Empire State Building, the Golden Gate Bridge, the Pentagon and the United Nations by 1960 AD
		if iGameTurn == getTurnForYear(1960):
			expire(iAmerica, 1)
			
		# third goal: control 75% of the world's commerce output and military power between you, your vassals and allies by 2000 AD
		if isPossible(iAmerica, 2):
			if calculateAlliedCommercePercent(iAmerica) >= 75.0 and calculateAlliedPowerPercent(iAmerica) >= 75.0:
				win(iAmerica, 2)
				
		if iGameTurn == getTurnForYear(2000):
			expire(iAmerica, 2)
			
	elif iPlayer == iArgentina:
	
		# first goal: get 6000 gold through trade by 1930 AD
		if isPossible(iArgentina, 0):
			iTradeGold = 0
			
			# gold from city trade routes
			iTradeCommerce = 0
			for city in utils.getCityList(iArgentina):
				iTradeCommerce += city.getTradeYield(2)
			iTradeGold += iTradeCommerce * pArgentina.getCommercePercent(0)
			
			# gold from per turn gold trade
			for iPlayer in range(iNumPlayers):
				iTradeGold += pArgentina.getGoldPerTurnByPlayer(iPlayer)
			
			data.iArgentineTradeGold += iTradeGold
			
			if data.iArgentineTradeGold / 100 >= utils.getTurns(6000):
				win(iArgentina, 0)

		if iGameTurn == getTurnForYear(1930):
			expire(iArgentina, 0)
			
		# second goal: have lengendary culture in Buenos Aires by 1955 AD
		if isPossible(iArgentina, 1):
			if getCityCulture(iArgentina, Areas.getCapital(iArgentina)) >= utils.getTurns(40000):
				win(iArgentina, 1)
				
		if iGameTurn == getTurnForYear(1955):
			expire(iArgentina, 1)
			
		# third goal: experience six golden ages by 1975 AD
		if isPossible(iArgentina, 2):
			if data.iArgentineGoldenAgeTurns >= utils.getTurns(48):
				win(iArgentina, 2)
				
		if iGameTurn == getTurnForYear(1975):
			expire(iArgentina, 2)
			
		if pArgentina.isGoldenAge() and not pArgentina.isAnarchy():
			data.iArgentineGoldenAgeTurns += 1
			
	elif iPlayer == iBrazil:
	
		# first goal: control 10 slave plantations and 4 pastures in 1890 AD
		if iGameTurn == getTurnForYear(1890):
			if countImprovements(iBrazil, iSlavePlantation) >= 10 and countImprovements(iBrazil, iPasture) >= 4:
				win(iBrazil, 0)
			else:
				lose(iBrazil, 0)
				
		# second goal: build Wembley, Cristo Redentor and the Three Gorges Dam
		
		# third goal: control 20 forest preserves and have a national park in your capital by 1965 AD
		if isPossible(iBrazil, 2):
			if countImprovements(iBrazil, iForestPreserve) >= 20 and pBrazil.getCapitalCity() and pBrazil.getCapitalCity().isHasRealBuilding(iNationalPark):
				win(iBrazil, 2)
				
		if iGameTurn == getTurnForYear(1965):
			expire(iBrazil, 2)
				
	elif iPlayer == iCanada:
	
		# first goal: connect your capital to an Atlantic and a Pacific port by 1930 AD
		if isPossible(iCanada, 0):
			capital = pCanada.getCapitalCity()
			tCapital = (capital.getX(), capital.getY())
			bAtlantic = isConnectedByRailroad(iCanada, tCapital, lAtlanticCoast)
			bPacific = isConnectedByRailroad(iCanada, tCapital, lPacificCoast)
			if bAtlantic and bPacific:
				win(iCanada, 0)
				
		if iGameTurn == getTurnForYear(1930):
			expire(iCanada, 0)
			
		# second goal: control all cities and 95% of the territory in Canada without ever conquering a city by 1965 AD
		if isPossible(iCanada, 1):
			iEast, iTotalEast = countControlledTiles(iCanada, tCanadaEastTL, tCanadaEastBR, False, tCanadaEastExceptions)
			iWest, iTotalWest = countControlledTiles(iCanada, tCanadaWestTL, tCanadaWestBR, False, tCanadaWestExceptions)
			
			fCanada = (iEast + iWest) * 100.0 / (iTotalEast + iTotalWest)
			
			bAllCitiesEast = controlsAllCities(iCanada, tCanadaEastTL, tCanadaEastBR, tCanadaEastExceptions)
			bAllCitiesWest = controlsAllCities(iCanada, tCanadaWestTL, tCanadaWestBR, tCanadaWestExceptions)
			
			if fCanada >= 95.0 and bAllCitiesEast and bAllCitiesWest:
				win(iCanada, 1)
				
		if iGameTurn == getTurnForYear(1965):
			expire(iCanada, 1)
			
		# third goal: end twelve wars through diplomacy by 2000 AD
		if iGameTurn == getTurnForYear(2000):
			expire(iCanada, 2)
			
			
	# check religious victory (human only)
	if utils.getHumanID() == iPlayer:
		iVictoryType = utils.getReligiousVictoryType(iPlayer)
		
		if iVictoryType == iCatholicism:
			if gc.getGame().getSecretaryGeneral(1) == iPlayer:
				data.iPopeTurns += 1
				
		elif iVictoryType == iHinduism:
			if pPlayer.isGoldenAge():
				data.iHinduGoldenAgeTurns += 1
				
		elif iVictoryType == iBuddhism:
			if isAtPeace(iPlayer):
				data.iBuddhistPeaceTurns += 1
				
			if isHappiest(iPlayer):
				data.iBuddhistHappinessTurns += 1
				
		elif iVictoryType == iTaoism:
			if isHealthiest(iPlayer):
				data.iTaoistHealthTurns += 1
				
		elif iVictoryType == iVictoryPaganism:
			if 2 * countReligionCities(iPlayer) > pPlayer.getNumCities():
				data.bPolytheismNeverReligion = False
				
			if gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getPaganReligionName(0) == "Vedism":
				for city in utils.getCityList(iPlayer):
					if city.isWeLoveTheKingDay():
						data.iVedicHappiness += 1
				
		if checkReligiousGoals(iPlayer):
			gc.getGame().setWinner(iPlayer, 8)
			
def checkHistoricalVictory(iPlayer):
	pPlayer = gc.getPlayer(iPlayer)
	
	if not data.players[iPlayer].bHistoricalGoldenAge:
		if countAchievedGoals(iPlayer) >= 2:	
			data.players[iPlayer].bHistoricalGoldenAge = True
			
			iGoldenAgeTurns = gc.getPlayer(iPlayer).getGoldenAgeLength()
			if not gc.getPlayer(iPlayer).isAnarchy(): iGoldenAgeTurns += 1
			
			gc.getPlayer(iPlayer).changeGoldenAgeTurns(iGoldenAgeTurns)
			
			if pPlayer.isHuman():
				CyInterface().addMessage(iPlayer, False, iDuration, CyTranslator().getText("TXT_KEY_VICTORY_INTERMEDIATE", ()), "", 0, "", ColorTypes(iPurple), -1, -1, True, True)
				
				for iLoopPlayer in range(iNumPlayers):
					if iLoopPlayer != iPlayer:
						pLoopPlayer = gc.getPlayer(iLoopPlayer)
						if pLoopPlayer.isAlive():
							pLoopPlayer.AI_changeAttitudeExtra(iPlayer, -2)
			
	if gc.getGame().getWinner() == -1:
		if countAchievedGoals(iPlayer) == 3:
			gc.getGame().setWinner(iPlayer, 7)
		
def onCityBuilt(iPlayer, city):

	if not gc.getGame().isVictoryValid(7): return
	
	if utils.getHumanID() != iPlayer and data.bIgnoreAI: return
	
	if iPlayer >= iNumPlayers: return
	
	iGameTurn = gc.getGame().getGameTurn()
	
	# record first colony in the Americas for various UHVs
	if not data.isFirstWorldColonized():
		if city.getRegionID() in lNorthAmerica + lSouthAmerica:
			if iPlayer not in lCivGroups[5]:
				data.iFirstNewWorldColony = iPlayer
			
				# first Viking goal: found a city in America by 1020 AD
				if isPossible(iVikings, 0):
					if iPlayer == iVikings:
						win(iVikings, 0)
					else:
						lose(iVikings, 0)
					
				# first Spanish goal: be the first to found a colony in America
				if isPossible(iSpain, 0):
					if iPlayer == iSpain:
						win(iSpain, 0)
					else:
						lose(iSpain, 0)
				
	# first Polynesian goal: settle two of the following island groups by 1025 AD: Hawaii, New Zealand, Marquesas, Easter Island
	# second Polynesian goal: control Hawaii, New Zealand, Marquesas and Easter Island by 1290 AD
	if iPlayer == iPolynesia:
		iCount = 0
		if getNumCitiesInArea(iPolynesia, utils.getPlotList(tHawaiiTL, tHawaiiBR)) >= 1: iCount += 1
		if getNumCitiesInArea(iPolynesia, utils.getPlotList(tNewZealandTL, tNewZealandBR)) >= 1: iCount += 1
		if getNumCitiesInArea(iPolynesia, utils.getPlotList(tMarquesasTL, tMarquesasBR)) >= 1: iCount += 1
		if getNumCitiesInArea(iPolynesia, utils.getPlotList(tEasterIslandTL, tEasterIslandBR)) >= 1: iCount += 1
		
		if isPossible(iPolynesia, 0):
			if iCount >= 2:
				win(iPolynesia, 0)
				
		if isPossible(iPolynesia, 1):
			if iCount >= 4:
				win(iPolynesia, 1)
				
	# first Tibetan goal: acquire four cities by 840 AD
	elif iPlayer == iTibet:
		if isPossible(iTibet, 0):
			if pTibet.getNumCities() >= 4:
				win(iTibet, 0)
	
def onCityAcquired(iPlayer, iOwner, city, bConquest):

	if not gc.getGame().isVictoryValid(7): return
	
	# first Japanese goal: have an average city culture of 8000 by 1870 AD without ever losing a city
	if iOwner == iJapan:
		expire(iJapan, 0)
	
	if utils.getHumanID() != iPlayer and data.bIgnoreAI: return
				
	# first Tibetan goal: acquire five cities by 840 AD
	if iPlayer == iTibet:
		if isPossible(iTibet, 0):
			if pTibet.getNumCities() >= 4:
				win(iTibet, 0)
				
	# second Dutch goal: conquer five European colonies by 1785 AD
	elif iPlayer == iNetherlands:
		if isPossible(iNetherlands, 1):
			if iOwner in [iSpain, iFrance, iEngland, iPortugal, iVikings, iItaly, iRussia, iGermany, iHolyRome, iPoland]:
				bColony = city.getRegionID() not in [rBritain, rIberia, rItaly, rBalkans, rEurope, rScandinavia, rRussia]
			
				if bColony and bConquest:
					data.iDutchColonies += 1
					if data.iDutchColonies >= 5:
						win(iNetherlands, 1)
				
	# second Canadian goal: control all cities and 95% of the territory in Canada by 1965 AD without ever conquering a city
	elif iPlayer == iCanada:
		if bConquest:
			expire(iCanada, 1)
			
def onTechAcquired(iPlayer, iTech):
	if not gc.getGame().isVictoryValid(7): return
	
	if iPlayer >= iNumPlayers: return
	
	iGameTurn = gc.getGame().getGameTurn()
	iEra = gc.getTechInfo(iTech).getEra()
	
	# handle all "be the first to discover" goals
	if not isDiscovered(iTech):
		data.lFirstDiscovered[iTech] = iPlayer
		
		for iLoopPlayer in dTechGoals.keys():
			iGoal = dTechGoals[iLoopPlayer][0]
			lTechs = dTechGoals[iLoopPlayer][1]
			
			if not isPossible(iLoopPlayer, iGoal): continue
			if iLoopPlayer == iMaya and pMaya.isReborn(): continue
			
			if iTech in lTechs:
				if iPlayer != iLoopPlayer: lose(iLoopPlayer, iGoal)
				elif checkTechGoal(iLoopPlayer, lTechs): win(iLoopPlayer, iGoal)
				
		# third Japanese goal: be the first to discover seven Global and seven Digital technologies
		if isPossible(iJapan, 2):
			if countFirstDiscovered(iPlayer, iGlobal) >= 7 and countFirstDiscovered(iPlayer, iDigital) >= 7:
				if iPlayer == iJapan: win(iJapan, 2)
				else: lose(iJapan, 2)
				
		# second English goal: be the first to discover ten Industrial technologies
		if isPossible(iEngland, 1):
			if countFirstDiscovered(iPlayer, iIndustrial) >= 10:
				if iPlayer == iEngland: win(iEngland, 1)
				else: lose(iEngland, 1)
				
		# third German goal: be the first to discover six Industrial and eight Global technologies
		if isPossible(iGermany, 2):
			if countFirstDiscovered(iPlayer, iIndustrial) >= 6 and countFirstDiscovered(iPlayer, iGlobal) >= 8:
				if iPlayer == iGermany: win(iGermany, 2)
				else: lose(iGermany, 2)
			
	# handle all "be the first to enter" goals
	if not isEntered(iEra):
		data.lFirstEntered[iEra] = iPlayer
		
		for iLoopPlayer in dEraGoals.keys():
			iGoal = dEraGoals[iLoopPlayer][0]
			lEras = dEraGoals[iLoopPlayer][1]
			
			if not isPossible(iLoopPlayer, iGoal): continue
			
			if iEra in lEras:
				if iPlayer != iLoopPlayer: lose(iLoopPlayer, iGoal)
				elif checkEraGoal(iLoopPlayer, lEras): win(iLoopPlayer, iGoal)
				
	# first Maya goal: discover Calendar by 600 AD
	if iPlayer == iMaya:
		if not pMaya.isReborn() and isPossible(iMaya, 0):
			if iTech == iCalendar:
				if teamMaya.isHasTech(iCalendar):
					win(iMaya, 0)
				
	# third Congolese goal: enter the Industrial era before anyone enters the Modern era
	if isPossible(iCongo, 2):
		if iEra == iIndustrial and iPlayer == iCongo:
			win(iCongo, 2)
		if iEra == iGlobal and iPlayer != iCongo:
			lose(iCongo, 2)
				
def checkTechGoal(iPlayer, lTechs):
	for iTech in lTechs:
		if data.lFirstDiscovered[iTech] != iPlayer:
			return False
	return True
	
def checkEraGoal(iPlayer, lEras):
	for iEra in lEras:
		if data.lFirstEntered[iEra] != iPlayer:
			return False
	return True
	
def onBuildingBuilt(iPlayer, iBuilding):

	if not gc.getGame().isVictoryValid(7): return False
	
	# handle all "build wonders" goals
	if isWonder(iBuilding) and not isWonderBuilt(iBuilding):
		data.setWonderBuilder(iBuilding, iPlayer)
		
		for iLoopPlayer in dWonderGoals.keys():
			iGoal, lWonders, bCanWin = dWonderGoals[iLoopPlayer]
			
			if not isPossible(iLoopPlayer, iGoal): continue
			
			if iBuilding in lWonders:
				if iPlayer != iLoopPlayer: lose(iLoopPlayer, iGoal)
				elif bCanWin and checkWonderGoal(iLoopPlayer, lWonders): win(iLoopPlayer, iGoal)
				
	if utils.getHumanID() != iPlayer and data.bIgnoreAI: return
	
	if not gc.getPlayer(iPlayer).isAlive(): return
	
	# first Chinese goal: build two Confucian and Taoist Cathedrals by 1270 AD
	if iPlayer == iChina:
		if isPossible(iChina, 0):
			if iBuilding in [iConfucianCathedral, iTaoistCathedral]:
				iConfucian = getNumBuildings(iChina, iConfucianCathedral)
				iTaoist = getNumBuildings(iChina, iTaoistCathedral)
				if iConfucian >= 2 and iTaoist >= 2:
					win(iChina, 0)
					
	# second Harappan goal: build three Baths, two Granaries and two Smokehouses by 1700 BC
	elif iPlayer == iHarappa:
		if isPossible(iHarappa, 1):
			if iBuilding in [iReservoir, iGranary, iSmokehouse]:
				iNumBaths = getNumBuildings(iHarappa, iReservoir)
				iNumGranaries = getNumBuildings(iHarappa, iGranary)
				iNumSmokehouses = getNumBuildings(iHarappa, iSmokehouse)
				if iNumBaths >= 3 and iNumGranaries >= 2 and iNumSmokehouses >= 2:
					win(iHarappa, 1)
					
	# second Indian goal: build 18 temples by 650 AD
	elif iPlayer == iIndia:
		if isPossible(iIndia, 1):
			lTemples = [iTemple + i*4 for i in range(iNumReligions)]
			if iBuilding in lTemples:
				iCounter = 0
				for iGoalTemple in lTemples:
					iCounter += getNumBuildings(iIndia, iGoalTemple)
				if iCounter >= 18:
					win(iIndia, 1)
	
	# first Roman goal: build 6 Barracks, 5 Aqueducts, 4 Arenas and 3 Forums by 180 AD
	elif iPlayer == iRome:
		if isPossible(iRome, 0):
			if iBuilding in [iBarracks, iAqueduct, iArena, iForum]:
				iNumBarracks = getNumBuildings(iRome, iBarracks)
				iNumAqueducts = getNumBuildings(iRome, iAqueduct)
				iNumArenas = getNumBuildings(iRome, iArena)
				iNumForums = getNumBuildings(iRome, iForum)
				if iNumBarracks >= 6 and iNumAqueducts >= 5 and iNumArenas >= 4 and iNumForums >= 3:
					win(iRome, 0)
					
	# first Korean goal: build a Confucian and a Buddhist Cathedral
	elif iPlayer == iKorea:
		if isPossible(iKorea, 0):
			if iBuilding in [iConfucianCathedral, iBuddhistCathedral]:
				bBuddhist = getNumBuildings(iKorea, iBuddhistCathedral) > 0
				bConfucian = getNumBuildings(iKorea, iConfucianCathedral) > 0
				if bBuddhist and bConfucian:
					win(iKorea, 0)
					
	# third Polish goal: build a total of three Catholic, Orthodox and Protestant Cathedrals by 1600 AD
	elif iPlayer == iPoland:
		if isPossible(iPoland, 2):
			iCatholic = getNumBuildings(iPoland, iCatholicCathedral)
			iOrthodox = getNumBuildings(iPoland, iOrthodoxCathedral)
			iProtestant = getNumBuildings(iPoland, iProtestantCathedral)
			if iCatholic + iOrthodox + iProtestant >= 3:
				win(iPoland, 2)
				
	elif iPlayer == iAztecs:
	
		# second Aztec goal: build 5 pagan temples and sacrificial altars
		if not pAztecs.isReborn():
			if isPossible(iAztecs, 1):
				if iBuilding in [iPaganTemple, iSacrificialAltar]:
					iTemples = getNumBuildings(iAztecs, iPaganTemple)
					iAltars = getNumBuildings(iAztecs, iSacrificialAltar)
					if iTemples >= 5 and iAltars >= 5:
						win(iAztecs, 1)
						
		# first Mexican goal: build three cathedrals of your state religion by 1910 AD
		else:
			if isPossible(iAztecs, 0):
				iStateReligion = pAztecs.getStateReligion()
				if iStateReligion >= 0:
					iStateReligionCathedral = iCathedral + 4 * iStateReligion
					if iBuilding == iStateReligionCathedral:
						if getNumBuildings(iAztecs, iStateReligionCathedral) >= 3:
							win(iAztecs, 0)
	
	# first Mughal goal: build three Islamic Mosques and one Hindu Cathedral by 1605 AD
	elif iPlayer == iMughals:
		if isPossible(iMughals, 0):
			if iBuilding == iIslamicCathedral or iBuilding == iHinduCathedral:
				if getNumBuildings(iMughals, iIslamicCathedral) >= 3 and getNumBuildings(iMughals, iHinduCathedral) >= 1:
					win(iMughals, 0)
		
	# first Incan goal: build 5 tambos and a road along the Andean coast by 1530 AD
	elif iPlayer == iInca:
		if isPossible(iInca, 0):
			if iBuilding == iTambo:
				if isRoad(iInca, lAndeanCoast) and getNumBuildings(iInca, iTambo) >= 5:
					win(iInca, 0)
				
def checkWonderGoal(iPlayer, lWonders):
	for iWonder in lWonders:
		if data.getWonderBuilder(iWonder) != iPlayer:
			return False
	return True
				
def onReligionFounded(iPlayer, iReligion):

	if not gc.getGame().isVictoryValid(7): return
	
	# handle all "be the first to found" goals
	if not isFounded(iReligion):
		data.lReligionFounder[iReligion] = iPlayer
		
		for iLoopPlayer in dReligionGoals.keys():
			iGoal = dReligionGoals[iLoopPlayer][0]
			lReligions = dReligionGoals[iLoopPlayer][1]
			
			if not isPossible(iLoopPlayer, iGoal): continue
			
			if iReligion in lReligions:
				if iPlayer != iLoopPlayer: lose(iLoopPlayer, iGoal)
				elif checkReligionGoal(iLoopPlayer, lReligions): win(iLoopPlayer, iGoal)
				
def checkReligionGoal(iPlayer, lReligions):
	for iReligion in lReligions:
		if data.lReligionFounder[iReligion] != iPlayer:
			return False
	return True
				
def onCityRazed(iPlayer, city):
	if not gc.getGame().isVictoryValid(7): return
	
	if utils.getHumanID() != iPlayer and data.bIgnoreAI: return
	
	# second Mongol goal: raze seven cities
	if iPlayer == iMongolia:
		if isPossible(iMongolia, 1):
			data.iMongolRazes += 1
			if data.iMongolRazes >= 7:
				win(iMongolia, 1)
				
def onProjectBuilt(iPlayer, iProject):

	if not gc.getGame().isVictoryValid(7): return
	
	# second Russian goal: be the first civilization to complete the Manhattan Project and the Apollo Program
	if isPossible(iRussia, 1):
		if iProject in [iLunarLanding, iManhattanProject]:
			if iPlayer == iRussia:
				bApolloProgram = iProject == iLunarLanding or teamRussia.getProjectCount(iLunarLanding) > 0
				bManhattanProject = iProject == iManhattanProject or teamRussia.getProjectCount(iManhattanProject) > 0
				if bApolloProgram and bManhattanProject:
					win(iRussia, 1)
			else:
				lose(iRussia, 1)
				
def onCombatResult(pWinningUnit, pLosingUnit):

	iWinningPlayer = pWinningUnit.getOwner()
	iLosingPlayer = pLosingUnit.getOwner()
	
	if utils.getHumanID() != iWinningPlayer and data.bIgnoreAI: return
	
	pLosingUnitInfo = gc.getUnitInfo(pLosingUnit.getUnitType())
	iDomainSea = DomainTypes.DOMAIN_SEA
	
	# first English goal: control a total of 25 frigates and ships of the line and sink 50 ships by 1815 AD
	if iWinningPlayer == iEngland:
		if isPossible(iEngland, 0):
			if pLosingUnitInfo.getDomainType() == iDomainSea:
				data.iEnglishSinks += 1
				
	# third Korean goal: sink 20 enemy ships
	elif iWinningPlayer == iKorea:
		if isPossible(iKorea, 2):
			if pLosingUnitInfo.getDomainType() == iDomainSea:
				data.iKoreanSinks += 1
				if data.iKoreanSinks >= 20:
					win(iKorea, 2)
					
def onGreatPersonBorn(iPlayer, unit):
	iUnitType = utils.getBaseUnit(unit.getUnitType())
	pUnitInfo = gc.getUnitInfo(iUnitType)
	
	if not isGreatPersonTypeBorn(iUnitType):
		data.lFirstGreatPeople[lGreatPeopleUnits.index(iUnitType)] = iPlayer
	
	# second Mexican goal: get three great generals by 1940 AD
	if iPlayer == iAztecs:
		if pAztecs.isReborn() and isPossible(iAztecs, 1):
			if pUnitInfo.getGreatPeoples(iSpecialistGreatGeneral):
				data.iMexicanGreatGenerals += 1
				
				if data.iMexicanGreatGenerals >= 3:
					win(iAztecs, 1)
					
def onUnitPillage(iPlayer, iGold, iUnit):
	if iGold >= 1000: return

	# third Viking goal: acquire 2000 gold by pillaging, conquering cities and sinking ships by 1295 AD
	if iPlayer == iVikings:
		if isPossible(iVikings, 2):
			data.iVikingGold += iGold
			
	# first Turkic goal: pillage 20 improvements by 900 AD
	elif iPlayer == iTurks:
		if isPossible(iTurks, 0):
			data.iTurkicPillages += 1
			
	elif iPlayer == iMoors:
		if isPossible(iMoors, 2) and iUnit == iCorsair:
			data.iMoorishGold += iGold
		
def onCityCaptureGold(iPlayer, iGold):

	# third Viking goal: acquire 2000 gold by pillaging, conquering cities and sinking ships by 1295 AD
	if iPlayer == iVikings:
		if isPossible(iVikings, 2):
			data.iVikingGold += iGold
		
def onPlayerGoldTrade(iPlayer, iGold):

	# third Tamil goal: acquire 5000 gold by trade by 1280 AD
	if iPlayer == iTamils:
		if isPossible(iTamils, 2):
			data.iTamilTradeGold += iGold * 100
			
def onPlayerSlaveTrade(iPlayer, iGold):

	# second Congolese goal: gain 1000 gold through slave trade by 1840 AD
	if iPlayer == iCongo:
		if isPossible(iCongo, 1):
			data.iCongoSlaveCounter += iGold
			if data.iCongoSlaveCounter >= utils.getTurns(1000):
				win(iCongo, 1)
				
def onTradeMission(iPlayer, iX, iY, iGold):

	# third Tamil goal: acquire 5000 gold by trade by 1280 AD
	if iPlayer == iTamils:
		data.iTamilTradeGold += iGold * 100
		
	# first Mande goal: conduct a trade mission in your state religion's holy city by 1350 AD
	elif iPlayer == iMali:
		if isPossible(iMali, 0):
			iStateReligion = pMali.getStateReligion()
			if iStateReligion != -1:
				pHolyCity = gc.getGame().getHolyCity(iStateReligion)
				
				if pHolyCity.getX() == iX and pHolyCity.getY() == iY:
					win(iMali, 0)
					
def onPeaceBrokered(iBroker, iPlayer1, iPlayer2):

	# third Canadian goal: end twelve wars through diplomacy by 2000 AD
	if iBroker == iCanada:
		if isPossible(iCanada, 2):
			data.iCanadianPeaceDeals += 1
			if data.iCanadianPeaceDeals >= 12:
				win(iCanada, 2)
			
def onBlockade(iPlayer, iGold):

	# third Moorish goal: acquire 3000 gold through piracy by 1680 AD
	if iPlayer == iMoors:
		if isPossible(iMoors, 2):
			data.iMoorishGold += iGold
			
def onFirstContact(iPlayer, iHasMetPlayer):

	# third Maya goal: make contact with a European civilization before they have discovered America
	if not pMaya.isReborn() and isPossible(iMaya, 2):
		if iPlayer == iMaya or iHasMetPlayer == iMaya:
			if iPlayer == iMaya and iHasMetPlayer in lCivGroups[0]: iEuropean = iHasMetPlayer
			elif iHasMetPlayer == iMaya and iPlayer in lCivGroups[0]: iEuropean = iPlayer
			else: return
		
			lPlots = utils.getPlotList(tNorthAmericaTL, (tNorthAmericaBR[0]+2, tNorthAmericaBR[1])) + utils.getPlotList(tSouthCentralAmericaTL, (tSouthCentralAmericaBR[0]+2, tSouthCentralAmericaBR[1]))
			for (x, y) in lPlots:
				plot = gc.getMap().plot(x, y)
				if plot.isRevealed(iEuropean, False) and not plot.isWater():
					lose(iMaya, 2)
					return
					
def onPlayerChangeStateReligion(iPlayer, iStateReligion):

	# second Ethiopian goal: convert to Orthodoxy five turns after it is founded
	if iPlayer == iEthiopia:
		if iStateReligion == iOrthodoxy:
			if gc.getGame().isReligionFounded(iOrthodoxy):
				if gc.getGame().getGameTurn() <= gc.getGame().getReligionGameTurnFounded(iOrthodoxy) + utils.getTurns(5):
					data.bEthiopiaConverted = True
			
def checkReligiousGoals(iPlayer):
	for i in range(3):
		if checkReligiousGoal(iPlayer, i) != 1:
			return False
	return True
	
def checkReligiousGoal(iPlayer, iGoal):
	pPlayer = gc.getPlayer(iPlayer)
	iVictoryType = utils.getReligiousVictoryType(iPlayer)
	
	if iVictoryType == -1: return -1
	
	elif iVictoryType == iJudaism:
	
		# first Jewish goal: have a total of 15 Great Prophets, Scientists and Statesmen in Jewish cities
		if iGoal == 0:
			iProphets = countSpecialists(iJudaism, iSpecialistGreatProphet)
			iScientists = countSpecialists(iJudaism, iSpecialistGreatScientist)
			iStatesmen = countSpecialists(iJudaism, iSpecialistGreatStatesman)
			if iProphets + iScientists + iStatesmen >= 15: return 1
		
		# second Jewish goal: have legendary culture in the Jewish holy city
		elif iGoal == 1:
			pHolyCity = gc.getGame().getHolyCity(iJudaism)
			if pHolyCity.getOwner() == iPlayer and pHolyCity.getCultureLevel() >= 6: return 1
			
		# third Jewish goal: have friendly relations with six civilizations with Jewish minorities
		elif iGoal == 2:
			iFriendlyRelations = countPlayersWithAttitudeAndReligion(iPlayer, AttitudeTypes.ATTITUDE_FRIENDLY, iJudaism)
			if iFriendlyRelations >= 6: return 1
			
	elif iVictoryType == iOrthodoxy:
	
		# first Orthodox goal: build four Orthodox cathedrals
		if iGoal == 0:
			if getNumBuildings(iPlayer, iOrthodoxCathedral) >= 4: return 1
			
		# second Orthodox goal: make sure the five most cultured cities in the world are Orthodox
		elif iGoal == 1:
			if countBestCitiesReligion(iOrthodoxy, cityCulture, 5) >= 5: return 1
			
		# third Orthodox goal: make sure there are no Catholic civilizations in the world
		elif iGoal == 2:
			if countReligionPlayers(iCatholicism)[0] == 0: return 1
			
	elif iVictoryType == iCatholicism:
	
		# first Catholic goal: be pope for 100 turns
		if iGoal == 0:
			if data.iPopeTurns >= utils.getTurns(100): return 1
			
		# second Catholic goal: control the Catholic shrine and make sure 12 great prophets are settled in Catholic civilizations
		elif iGoal == 1:
			bShrine = getNumBuildings(iPlayer, iCatholicShrine) > 0
			iSaints = countReligionSpecialists(iCatholicism, iSpecialistGreatProphet)
			
			if bShrine and iSaints >= 12: return 1
			
		# third Catholic goal: make sure 50% of world territory is controlled by Catholic civilizations
		elif iGoal == 2:
			if getReligiousLand(iCatholicism) >= 50.0: return 1
	
	elif iVictoryType == iProtestantism:
		
		# first Protestant goal: be first to discover Civil Liberties, Constitution and Economics
		if iGoal == 0:
			lProtestantTechs = [iCivilLiberties, iSocialContract, iLogistics]
			if checkTechGoal(iPlayer, lProtestantTechs): return 1
			elif data.lFirstDiscovered[iCivilLiberties] not in [iPlayer, -1] or data.lFirstDiscovered[iSocialContract] not in [iPlayer, -1] or data.lFirstDiscovered[iLogistics] not in [iPlayer, -1]: return 0
			
		# second Protestant goal: make sure five great merchants and great engineers are settled in Protestant civilizations
		elif iGoal == 1:
			iEngineers = countReligionSpecialists(iProtestantism, iSpecialistGreatEngineer)
			iMerchants = countReligionSpecialists(iProtestantism, iSpecialistGreatMerchant)
			if iEngineers >= 5 and iMerchants >= 5: return 1
			
		# third Protestant goal: make sure at least half of all civilizations are Protestant or Secular
		elif iGoal == 2:
			iProtestantCivs, iTotal = countReligionPlayers(iProtestantism)
			iSecularCivs, iTotal = countCivicPlayers(iCivicsReligion, iSecularism)
			
			if 2 * (iProtestantCivs + iSecularCivs) >= iTotal: return 1
			
	elif iVictoryType == iIslam:
	
		# first Muslim goal: spread Islam to 40%
		if iGoal == 0:
			fReligionPercent = gc.getGame().calculateReligionPercent(iIslam)
			if fReligionPercent >= 40.0: return 1
			
		# second Muslim goal: settle seven great people in the Muslim holy city
		elif iGoal == 1:
			iCount = 0
			pHolyCity = gc.getGame().getHolyCity(iIslam)
			for iGreatPerson in lGreatPeople:
				iCount += pHolyCity.getFreeSpecialistCount(iGreatPerson)
			if iCount >= 7: return 1
			
		# third Muslim goal: control five shrines
		elif iGoal == 2:
			iCount = 0
			for iReligion in range(iNumReligions):
				iCount += getNumBuildings(iPlayer, iShrine + 4*iReligion)
			if iCount >= 5: return 1
			
	elif iVictoryType == iHinduism:
	
		# first Hindu goal: settle five different great people in the Hindu holy city
		if iGoal == 0:
			iCount = 0
			pHolyCity = gc.getGame().getHolyCity(iHinduism)
			for iGreatPerson in lGreatPeople:
				if pHolyCity.getFreeSpecialistCount(iGreatPerson) > 0:
					iCount += 1
			if iCount >= 5: return 1
		
		# second Hindu goal: experience 36 turns of golden age
		elif iGoal == 1:
			if data.iHinduGoldenAgeTurns >= utils.getTurns(36): return 1
			
		# third Hindu goal: make sure the five largest cities in the world are Hindu
		elif iGoal == 2:
			if countBestCitiesReligion(iHinduism, cityPopulation, 5) >= 5: return 1
			
	elif iVictoryType == iBuddhism:
	
		# first Buddhist goal: be at peace for 100 turns
		if iGoal == 0:
			if data.iBuddhistPeaceTurns >= utils.getTurns(100): return 1
			
		# second Buddhist goal: have the highest approval rating for 100 turns
		elif iGoal == 1:
			if data.iBuddhistHappinessTurns >= utils.getTurns(100): return 1
			
		# third Buddhist goal: have cautious or better relations with all civilizations in the world
		elif iGoal == 2:
			if countGoodRelationPlayers(iPlayer, AttitudeTypes.ATTITUDE_CAUTIOUS) >= countLivingPlayers()-1: return 1
			
	elif iVictoryType == iConfucianism:
	
		# first Confucian goal: have friendly relations with five civilizations
		if iGoal == 0:
			if countGoodRelationPlayers(iPlayer, AttitudeTypes.ATTITUDE_FRIENDLY) >= 5: return 1
			
		# second Confucian goal: have five wonders in the Confucian holy city
		elif iGoal == 1:
			pHolyCity = gc.getGame().getHolyCity(iConfucianism)
			if countCityWonders(iPlayer, (pHolyCity.getX(), pHolyCity.getY()), True) >= 5: return 1
			
		# third Confucian goal: control an army of 200 non-obsolete melee or gunpowder units
		elif iGoal == 2:
			iUnitCombatMelee = gc.getInfoTypeForString("UNITCOMBAT_MELEE")
			iUnitCombatGunpowder = gc.getInfoTypeForString("UNITCOMBAT_GUN")
			if countUnitsOfType(iPlayer, [iUnitCombatMelee, iUnitCombatGunpowder]) >= 200: return 1
			
	elif iVictoryType == iTaoism:
	
		# first Taoist goal: have the highest life expectancy in the world for 100 turns
		if iGoal == 0:
			if data.iTaoistHealthTurns >= utils.getTurns(100): return 1
			
		# second Taoist goal: control the Confucian and Taoist shrine and combine their income to 40 gold
		elif iGoal == 1:
			iConfucianIncome = calculateShrineIncome(iPlayer, iConfucianism)
			iTaoistIncome = calculateShrineIncome(iPlayer, iTaoism)
			if getNumBuildings(iPlayer, iConfucianShrine) > 0 and getNumBuildings(iPlayer, iTaoistShrine) > 0 and iConfucianIncome + iTaoistIncome >= 40: return 1
			
		# third Taoist goal: have legendary culture in the Tao holy city
		elif iGoal == 2:
			pHolyCity = gc.getGame().getHolyCity(iTaoism)
			if pHolyCity.getOwner() == iPlayer and pHolyCity.getCultureLevel() >= 6: return 1
		
	elif iVictoryType == iZoroastrianism:

		# first Zoroastrian goal: acquire six incense resources
		if iGoal == 0:
			if pPlayer.getNumAvailableBonuses(iIncense) >= 6: return 1
			
		# second Zoroastrian goal: spread Zoroastrianism to 10%
		if iGoal == 1:
			fReligionPercent = gc.getGame().calculateReligionPercent(iZoroastrianism)
			if fReligionPercent >= 10.0: return 1
			
		# third Zoroastrian goal: have legendary culture in the Zoroastrian holy city
		elif iGoal == 2:
			pHolyCity = gc.getGame().getHolyCity(iZoroastrianism)
			if pHolyCity.getOwner() == iPlayer and pHolyCity.getCultureLevel() >= 6: return 1
			
	elif iVictoryType == iVictoryPaganism:
	
		# first Pagan goal: make sure there are 15 pagan temples in the world
		if iGoal == 0:
			if countWorldBuildings(iPaganTemple) >= 15: return 1
			
		# second Pagan goal: depends on Pagan religion
		elif iGoal == 1:
			paganReligion = gc.getCivilizationInfo(pPlayer.getCivilizationType()).getPaganReligionName(0)
			
			# Anunnaki: have more wonders in your capital than any other city in the world
			if paganReligion == "Anunnaki":
				capital = pPlayer.getCapitalCity()
				
				if capital and isBestCity(iPlayer, (capital.getX(), capital.getY()), cityWonders):
					return 1
					
			# Asatru: have five units of level five
			elif paganReligion == "Asatru":
				if countUnitsOfLevel(iPlayer, 5) >= 5:
					return 1
					
			# Atua: acquire four pearl resources and 50 Ocean tiles
			elif paganReligion == "Atua":
				if pPlayer.getNumAvailableBonuses(iPearls) >= 4 and countControlledTerrain(iPlayer, iOcean) >= 50:
					return 1
			
			# Baalism: make your capital the city with the highest trade income in the world
			elif paganReligion == "Baalism":
				capital = pPlayer.getCapitalCity()
				
				if capital and isBestCity(iPlayer, (capital.getX(), capital.getY()), cityTradeIncome):
					return 1
					
			# Druidism: control 20 unimproved Forest or Marsh tiles
			elif paganReligion == "Druidism":
				if countControlledFeatures(iPlayer, iForest, -1) + countControlledFeatures(iPlayer, iMud, -1) >= 20:
					return 1
					
			# Inti: have more gold in your treasury than the rest of the world combined
			elif paganReligion == "Inti":
				if 2 * pPlayer.getGold() >= getGlobalTreasury():
					return 1
					
			# Mazdaism: acquire six incense resources
			elif paganReligion == "Mazdaism":
				if pPlayer.getNumAvailableBonuses(iIncense) >= 6:
					return 1
					
			# Olympianism: control ten wonders that require no state religion
			elif paganReligion == "Olympianism":
				if countReligionWonders(iPlayer, -1) >= 10:
					return 1
					
			# Pesedjet: be the first to create to three different types of great person
			elif paganReligion == "Pesedjet":
				if countFirstGreatPeople(iPlayer) >= 3:
					return 1
				
			# Rodnovery: acquire seven fur resources
			elif paganReligion == "Rodnovery":
				if pPlayer.getNumAvailableBonuses(iFur) >= 7:
					return 1
					
			# Shendao: control 25% of the world's population
			elif paganReligion == "Shendao":
				if getPopulationPercent(iPlayer) >= 25.0:
					return 1
					
			# Shinto: settle three great spies in your capital
			elif paganReligion == "Shinto":
				capital = pPlayer.getCapitalCity()
				
				if capital and countCitySpecialists(iPlayer, (capital.getX(), capital.getY()), iSpecialistGreatSpy) >= 3:
					return 1
			
			# Tengri: acquire eight horse resources
			elif paganReligion == "Tengri":
				if pPlayer.getNumAvailableBonuses(iHorse) >= 8:
					return 1
				
			# Teotl: sacrifice ten slaves
			elif paganReligion == "Teotl":
				if data.iTeotlSacrifices >= 10:
					return 1
					
			# Vedism: have 100 turns of cities celebrating "We Love the King" day
			elif paganReligion == "Vedism":
				if data.iVedicHappiness >= 100:
					return 1
					
			# Yoruba: acquire eight ivory resources and six gem resources
			elif paganReligion == "Yoruba":
				if pPlayer.getNumAvailableBonuses(iIvory) >= 8 and pPlayer.getNumAvailableBonuses(iGems) >= 6:
					return 1
			
		# third Pagan goal: don't allow more than half of your cities to have a religion
		elif iGoal == 2:
			if data.bPolytheismNeverReligion: return 1
			
	elif iVictoryType == iVictorySecularism:
	
		# first Secular goal: control the cathedrals of every religion
		if iGoal == 0:
			iCount = 0
			for iReligion in range(iNumReligions):
				if getNumBuildings(iPlayer, iCathedral + 4*iReligion) > 0:
					iCount += 1
			if iCount >= iNumReligions: return 1
			
		# second Secular goal: make sure there are 25 universities, 10 Great Scientists and 10 Great Statesmen in secular civilizations
		elif iGoal == 1:
			iUniversities = countCivicBuildings(iCivicsReligion, iSecularism, iUniversity)
			iScientists = countCivicSpecialists(iCivicsReligion, iSecularism, iSpecialistGreatScientist)
			iStatesmen = countCivicSpecialists(iCivicsReligion, iSecularism, iSpecialistGreatStatesman)
			if iUniversities >= 25 and iScientists >= 10 and iStatesmen >= 10: return 1
			
		# third Secular goal: make sure the five most advanced civilizations are secular
		elif iGoal == 2:
			iCount = 0
			lAdvancedPlayers = utils.getSortedList([iLoopPlayer for iLoopPlayer in range(iNumPlayers) if gc.getPlayer(iLoopPlayer).isAlive() and not utils.isAVassal(iLoopPlayer)], lambda iLoopPlayer: gc.getTeam(iLoopPlayer).getTotalTechValue(), True)
			for iLoopPlayer in lAdvancedPlayers[:5]:
				if gc.getPlayer(iLoopPlayer).getCivics(iCivicsReligion) == iSecularism:
					iCount += 1
			if iCount >= 5: return 1
			
	return -1

### UTILITY METHODS ###

def lose(iPlayer, iGoal):
	data.players[iPlayer].lGoals[iGoal] = 0
	if utils.getHumanID() == iPlayer and gc.getGame().getGameTurn() > utils.getScenarioStartTurn():
		utils.show(localText.getText("TXT_KEY_VICTORY_GOAL_FAILED_ANNOUNCE", (iGoal+1,)))
	
def win(iPlayer, iGoal):
	data.players[iPlayer].lGoals[iGoal] = 1
	data.players[iPlayer].lGoalTurns[iGoal] = gc.getGame().getGameTurn()
	checkHistoricalVictory(iPlayer)
	
def expire(iPlayer, iGoal):
	if isPossible(iPlayer, iGoal): lose(iPlayer, iGoal)
	
def isWon(iPlayer, iGoal):
	return data.players[iPlayer].lGoals[iGoal] == 1
	
def isLost(iPlayer, iGoal):
	return data.players[iPlayer].lGoals[iGoal] == 0
	
def isPossible(iPlayer, iGoal):
	return data.players[iPlayer].lGoals[iGoal] == -1
	
def loseAll(iPlayer):
	for i in range(3): data.players[iPlayer].lGoals[i] = 0
	
def countAchievedGoals(iPlayer):
	iCount = 0
	for i in range(3):
		if isWon(iPlayer, i): iCount += 1
	return iCount
	
def isFounded(iReligion):
	return data.lReligionFounder[iReligion] != -1
	
def isWonderBuilt(iWonder):
	return data.getWonderBuilder(iWonder) != -1
	
def isDiscovered(iTech):
	return data.lFirstDiscovered[iTech] != -1
	
def isEntered(iEra):
	return data.lFirstEntered[iEra] != -1
	
def isGreatPersonTypeBorn(iGreatPerson):
	if iGreatPerson not in lGreatPeopleUnits: return True
	return getFirstBorn(iGreatPerson) != -1
	
def getFirstBorn(iGreatPerson):
	if iGreatPerson not in lGreatPeopleUnits: return -1
	return data.lFirstGreatPeople[lGreatPeopleUnits.index(iGreatPerson)]
	
	
def getBestCity(iPlayer, tPlot, function):
	x, y = tPlot
	#if not gc.getMap().plot(x, y).isCity(): return None
	
	bestCity = gc.getMap().plot(x, y).getPlotCity()
	iBestValue = function(bestCity)
	
	for city in utils.getAllCities():
		if function(city) > iBestValue:
			bestCity = city
			iBestValue = function(city)
	
	return bestCity
	
def isBestCity(iPlayer, tPlot, function):
	x, y = tPlot
	city = getBestCity(iPlayer, tPlot, function)
	if not city: return False
	
	return (city.getOwner() == iPlayer and city.getX() == x and city.getY() == y)
	
def cityPopulation(city):
	if not city: return 0
	return city.getPopulation()
	
def cityCulture(city):
	if not city: return 0
	return city.getCulture(city.getOwner())
	
def cityWonders(city):
	if not city: return 0
	return len([iWonder for iWonder in lWonders if city.isHasRealBuilding(iWonder)])

def cityTradeIncome(city):
	if not city: return 0
	return city.getTradeYield(YieldTypes.YIELD_COMMERCE)
	
def cityHappiness(city):
	if not city: return 0
	
	iHappiness = 0
	iHappiness += city.happyLevel()
	iHappiness -= city.unhappyLevel(0)
	iHappiness += city.getPopulation()
	
	return iHappiness
	
def getBestPlayer(iPlayer, function):
	iBestPlayer = iPlayer
	iBestValue = function(iPlayer)
	
	for iLoopPlayer in range(iNumPlayers):
		pLoopPlayer = gc.getPlayer(iLoopPlayer)
		if pLoopPlayer.isAlive():
			if function(iLoopPlayer) > iBestValue:
				iBestPlayer = iLoopPlayer
				iBestValue = function(iLoopPlayer)
				
	return iBestPlayer
	
def isBestPlayer(iPlayer, function):
	return getBestPlayer(iPlayer, function) == iPlayer
	
def playerTechs(iPlayer):
	iValue = 0
	for iTech in range(iNumTechs):
		if gc.getTeam(iPlayer).isHasTech(iTech):
			iValue += gc.getTechInfo(iTech).getResearchCost()
	return iValue
	
def playerRealPopulation(iPlayer):
	return gc.getPlayer(iPlayer).getRealPopulation()
	
def getNumBuildings(iPlayer, iBuilding):
	return gc.getPlayer(iPlayer).countNumBuildings(iBuilding)
	
def getPopulationPercent(iPlayer):
	iTotalPopulation = gc.getGame().getTotalPopulation()
	iOurPopulation = gc.getTeam(iPlayer).getTotalPopulation()
	
	if iTotalPopulation <= 0: return 0.0
	
	return iOurPopulation * 100.0 / iTotalPopulation
	
def getLandPercent(iPlayer):
	iTotalLand = gc.getMap().getLandPlots()
	iOurLand = gc.getPlayer(iPlayer).getTotalLand()
	
	if iTotalLand <= 0: return 0.0
	
	return iOurLand * 100.0 / iTotalLand
	
def getReligionLandPercent(iReligion):
	fPercent = 0.0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive() and pPlayer.getStateReligion() == iReligion:
			fPercent += getLandPercent(iPlayer)
	return fPercent
	
def isBuildingInCity(tPlot, iBuilding):
	x, y = tPlot
	plot = gc.getMap().plot(x, y)
	
	if not plot.isCity(): return False
	
	return plot.getPlotCity().isHasRealBuilding(iBuilding)
	
def getNumCitiesInArea(iPlayer, lPlots):
	return len(utils.getAreaCitiesCiv(iPlayer, lPlots))
	
def getNumCitiesInRegions(iPlayer, lRegions):
	return len([city for city in utils.getCityList(iPlayer) if city.getRegionID() in lRegions])
	
def getNumFoundedCitiesInArea(iPlayer, lPlots):
	return len([city for city in utils.getAreaCitiesCiv(iPlayer, lPlots) if city.getOriginalOwner() == iPlayer])
	
def getNumConqueredCitiesInArea(iPlayer, lPlots):
	return len([city for city in utils.getAreaCitiesCiv(iPlayer, lPlots) if city.getOriginalOwner() != iPlayer])
	
def checkOwnedCiv(iPlayer, iOwnedPlayer):
	iPlayerCities = getNumCitiesInArea(iPlayer, Areas.getNormalArea(iOwnedPlayer, False))
	iOwnedCities = getNumCitiesInArea(iOwnedPlayer, Areas.getNormalArea(iOwnedPlayer, False))
	
	return (iPlayerCities >= 2 and iPlayerCities > iOwnedCities) or (iPlayerCities >= 1 and not gc.getPlayer(iOwnedPlayer).isAlive()) or (iPlayerCities >= 1 and iOwnedPlayer == iCarthage)
	
def isControlled(iPlayer, lPlots):
	lOwners = []
	for city in utils.getAreaCities(lPlots):
		iOwner = city.getOwner()
		if iOwner not in lOwners and iOwner < iNumPlayers:
			lOwners.append(iOwner)
	
	return iPlayer in lOwners and len(lOwners) == 1
	
def isControlledOrVassalized(iPlayer, lPlots):
	bControlled = False
	lOwners = []
	lValidOwners = [iPlayer]
	for city in utils.getAreaCities(lPlots):
		iOwner = city.getOwner()
		if iOwner not in lOwners and iOwner < iNumPlayers:
			lOwners.append(iOwner)
	for iLoopPlayer in range(iNumPlayers):
		if gc.getTeam(gc.getPlayer(iLoopPlayer).getTeam()).isVassal(iPlayer):
			lValidOwners.append(iLoopPlayer)
	for iLoopPlayer in lValidOwners:
		if iLoopPlayer in lOwners:
			bControlled = True
			lOwners.remove(iLoopPlayer)
	if lOwners:
		bControlled = False
	return bControlled
	
def isCoreControlled(iPlayer, lOtherPlayers):
	for iOtherPlayer in lOtherPlayers:
		if checkOwnedCiv(iPlayer, iOtherPlayer):
			return True
	return False
	
def countControlledTiles(iPlayer, tTopLeft, tBottomRight, bVassals=False, lExceptions=[], bCoastalOnly=False):
	lValidOwners = [iPlayer]
	iCount = 0
	iTotal = 0
	
	if bVassals:
		for iLoopPlayer in range(iNumPlayers):
			if gc.getTeam(gc.getPlayer(iLoopPlayer).getTeam()).isVassal(iPlayer):
				lValidOwners.append(iLoopPlayer)
				
	for (x, y) in utils.getPlotList(tTopLeft, tBottomRight, lExceptions):
		plot = gc.getMap().plot(x, y)
		if plot.isWater(): continue
		if bCoastalOnly and not plot.isCoastalLand(): continue
		iTotal += 1
		if plot.getOwner() in lValidOwners: iCount += 1
		
	return iCount, iTotal
	
def countWonders(iPlayer):
	iCount = 0
	for iWonder in range(iBeginWonders, iNumBuildings):
		iCount += getNumBuildings(iPlayer, iWonder)
	return iCount
	
def countShrines(iPlayer):
	iCount = 0
	for iReligion in range(iNumReligions):
		iCurrentShrine = iShrine + iReligion * 4
		iCount += getNumBuildings(iPlayer, iCurrentShrine)
	return iCount
	
def countOpenBorders(iPlayer, lContacts = [i for i in range(iNumPlayers)]):
	tPlayer = gc.getTeam(iPlayer)
	iCount = 0
	for iContact in lContacts:
		if tPlayer.isOpenBorders(iContact):
			iCount += 1
	return iCount
	
def getMostCulturedCity(iPlayer):
	return utils.getHighestEntry(utils.getCityList(iPlayer), lambda x: x.getCulture(iPlayer))

def isAreaFreeOfCivs(lPlots, lCivs):
	for city in utils.getAreaCities(lPlots):
		if city.getOwner() in lCivs: return False
	return True
	
def isAreaOnlyCivs(tTopLeft, tBottomRight, lCivs):
	for city in utils.getAreaCities(utils.getPlotList(tTopLeft, tBottomRight)):
		iOwner = city.getOwner()
		if iOwner < iNumPlayers and iOwner not in lCivs: return False
	return True
	
def countCitySpecialists(iPlayer, tPlot, iSpecialist):
	x, y = tPlot
	plot = gc.getMap().plot(x, y)
	if not plot.isCity(): return 0
	if plot.getPlotCity().getOwner() != iPlayer: return 0
	
	return plot.getPlotCity().getFreeSpecialistCount(iSpecialist)
	
def countSpecialists(iPlayer, iSpecialist):
	iCount = 0
	for city in utils.getCityList(iPlayer):
		iCount += countCitySpecialists(iPlayer, (city.getX(), city.getY()), iSpecialist)
	return iCount
	
def countReligionSpecialists(iReligion, iSpecialist):
	iCount = 0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive() and pPlayer.getStateReligion() == iReligion:
			iCount += countSpecialists(iPlayer, iSpecialist)
	return iCount
	
def countCivicSpecialists(iCategory, iCivic, iSpecialist):
	iCount = 0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive() and pPlayer.getCivics(iCategory) == iCivic:
			iCount += countSpecialists(iPlayer, iSpecialist)
	return iCount
	
def getAverageCitySize(iPlayer):
	pPlayer = gc.getPlayer(iPlayer)
	iNumCities = pPlayer.getNumCities()
	
	if iNumCities == 0: return 0.0
	
	return pPlayer.getTotalPopulation() * 1.0 / iNumCities
	
def getAverageCulture(iPlayer):
	pPlayer = gc.getPlayer(iPlayer)
	iNumCities = pPlayer.getNumCities()
	
	if iNumCities == 0: return 0
	
	return pPlayer.countTotalCulture() / iNumCities
	
def countHappinessResources(iPlayer):
	iCount = 0
	pPlayer = gc.getPlayer(iPlayer)
	for iBonus in range(iNumBonuses):
		if gc.getBonusInfo(iBonus).getHappiness() > 0:
			if pPlayer.getNumAvailableBonuses(iBonus) > 0:
				iCount += 1
	return iCount
	
def countResources(iPlayer, iBonus):
	iNumBonus = 0
	pPlayer = gc.getPlayer(iPlayer)
	
	iNumBonus += pPlayer.getNumAvailableBonuses(iBonus)
	iNumBonus -= pPlayer.getBonusImport(iBonus)
	
	for iLoopPlayer in range(iNumPlayers):
		if iLoopPlayer != iPlayer:
			pLoopPlayer = gc.getPlayer(iLoopPlayer)
			if pLoopPlayer.isAlive() and gc.getTeam(pLoopPlayer.getTeam()).isVassal(iPlayer):
				iNumBonus += pLoopPlayer.getNumAvailableBonuses(iBonus)
				iNumBonus -= pLoopPlayer.getBonusImport(iBonus)
				
	return iNumBonus
	
def isStateReligionInArea(iReligion, tTopLeft, tBottomRight):
	lPlots = utils.getPlotList(tTopLeft, tBottomRight)
	
	for iPlayer in range(iNumPlayers):
		if gc.getPlayer(iPlayer).getStateReligion() == iReligion:
			for city in utils.getCityList(iPlayer):
				if (city.getX(), city.getY()) in utils.getPlotList(tTopLeft, tBottomRight):
					return True
					
	return False
	
def getCityCulture(iPlayer, tPlot):
	x, y = tPlot
	plot = gc.getMap().plot(x, y)
	if not plot.isCity(): return 0
	if plot.getPlotCity().getOwner() != iPlayer: return 0
	
	return plot.getPlotCity().getCulture(iPlayer)
	
def isConnected(tStart, lTargets, plotFunction):
	if not lTargets: return False
	if not plotFunction(tStart): return False
	
	if tStart in lTargets: return True
	if not [tTarget for tTarget in lTargets if plotFunction(tTarget)]: return False
	
	lNodes = [(utils.minimalDistance(tStart, lTargets, plotFunction), tStart)]
	heapq.heapify(lNodes)
	lVisitedNodes = []
	
	while lNodes:
		h, tNode = heapq.heappop(lNodes)
		lVisitedNodes.append((h, tNode))
		
		for tPlot in utils.surroundingPlots(tNode):
			if plotFunction(tPlot):
				if tPlot in lTargets: return True
				
				tTuple = (utils.minimalDistance(tPlot, lTargets, plotFunction), tPlot)
				if not tTuple in lVisitedNodes and not tTuple in lNodes:
					heapq.heappush(lNodes, tTuple)
							
	return False
	
def isConnectedByTradeRoute(iPlayer, lStarts, lTargets):
	for tStart in lStarts:
		startPlot = utils.plot(tStart)
		if not startPlot.isCity(): continue
		
		plotFunction = lambda tPlot: utils.plot(tPlot).getOwner() in [iPlayer, startPlot.getOwner()] and (utils.plot(tPlot).isCity() or utils.plot(tPlot).getRouteType() in [iRouteRoad, iRouteRailroad, iRouteRomanRoad, iRouteHighway])
	
		if isConnected(tStart, lTargets, plotFunction): return True
		
	return False
	
def isConnectedByRailroad(iPlayer, tStart, lTargets):
	if not gc.getTeam(iPlayer).isHasTech(iRailroad): return False
	
	startPlot = utils.plot(tStart)
	if not (startPlot.isCity() and startPlot.getOwner() == iPlayer): return False
	
	plotFunction = lambda tPlot: utils.plot(tPlot).getOwner() == iPlayer and (utils.plot(tPlot).isCity() or utils.plot(tPlot).getRouteType() == iRouteRailroad)
	
	return isConnected(tStart, lTargets, plotFunction)

def countPlayersWithAttitudeAndCriteria(iPlayer, eAttitude, function):
	return len([iOtherPlayer for iOtherPlayer in range(iNumPlayers) if gc.getPlayer(iPlayer).canContact(iOtherPlayer) and gc.getPlayer(iOtherPlayer).AI_getAttitude(iPlayer) >= eAttitude and function(iOtherPlayer)])
	
def countPlayersWithAttitudeAndReligion(iPlayer, eAttitude, iReligion):
	iCount = 0
	for iLoopPlayer in range(iNumPlayers):
		if iLoopPlayer == iPlayer: continue
		pLoopPlayer = gc.getPlayer(iLoopPlayer)
		if pLoopPlayer.AI_getAttitude(iPlayer) >= eAttitude:
			for city in utils.getCityList(iLoopPlayer):
				if city.isHasReligion(iReligion):
					iCount += 1
					break
	return iCount
	
def countPlayersWithAttitudeInGroup(iPlayer, eAttitude, lOtherPlayers):
	return countPlayersWithAttitudeAndCriteria(iPlayer, eAttitude, lambda x: not gc.getTeam(gc.getPlayer(x).getTeam()).isAVassal())
	
def getLargestCities(iPlayer, iNumCities):
	lCities = utils.getSortedList(utils.getCityList(iPlayer), lambda x: x.getPopulation(), True)
	return lCities[:iNumCities]
	
def countCitiesOfSize(iPlayer, iThreshold):
	iCount = 0
	for city in utils.getCityList(iPlayer):
		if city.getPopulation() >= iThreshold:
			iCount += 1
	return iCount
	
def countCitiesWithCultureLevel(iPlayer, iThreshold):
	iCount = 0
	for city in utils.getCityList(iPlayer):
		if city.getCultureLevel() >= iThreshold:
			iCount += 1
	return iCount
	
def countAcquiredResources(iPlayer, lResources):
	iCount = 0
	pPlayer = gc.getPlayer(iPlayer)
	for iBonus in lResources:
		iCount += pPlayer.getNumAvailableBonuses(iBonus)
	return iCount
	
def isRoad(iPlayer, lPlots):
	iRoad = gc.getInfoTypeForString("ROUTE_ROAD")
	
	for tPlot in lPlots:
		x, y = tPlot
		plot = gc.getMap().plot(x, y)
		if plot.getOwner() != iPlayer: return False
		if not plot.getRouteType() == iRoad and not plot.isCity(): return False
		
	return True
	
def countCityWonders(iPlayer, tPlot, bIncludeObsolete=False):
	iCount = 0
	x, y = tPlot
	plot = gc.getMap().plot(x, y)
	if not plot.isCity(): return 0
	if plot.getPlotCity().getOwner() != iPlayer: return 0
	
	for iWonder in lWonders:
		iObsoleteTech = gc.getBuildingInfo(iWonder).getObsoleteTech()
		if not bIncludeObsolete and iObsoleteTech != -1 and gc.getTeam(iPlayer).isHasTech(iObsoleteTech): continue
		if plot.getPlotCity().isHasRealBuilding(iWonder): iCount += 1
		
	return iCount
	
def isCultureControlled(iPlayer, lPlots):
	for tPlot in lPlots:
		x, y = tPlot
		plot = gc.getMap().plot(x, y)
		if plot.getOwner() != -1 and plot.getOwner() != iPlayer:
			return False
	return True
	
def controlsCity(iPlayer, tPlot):
	for (x, y) in utils.surroundingPlots(tPlot):
		plot = gc.getMap().plot(x, y)
		if plot.isCity() and plot.getPlotCity().getOwner() == iPlayer:
			return True
	return False
	
def getTotalCulture(lPlayers):
	iTotalCulture = 0
	for iPlayer in lPlayers:
		iTotalCulture += gc.getPlayer(iPlayer).countTotalCulture()
	return iTotalCulture
	
def countImprovements(iPlayer, iImprovement):
	if iImprovement <= 0: return 0
	return gc.getPlayer(iPlayer).getImprovementCount(iImprovement)
	
def controlsAllCities(iPlayer, tTopLeft, tBottomRight, tExceptions=()):
	for city in utils.getAreaCities(utils.getPlotList(tTopLeft, tBottomRight, tExceptions)):
		if city.getOwner() != iPlayer: return False
	return True
	
def isAtPeace(iPlayer):
	for iLoopPlayer in range(iNumPlayers):
		if gc.getPlayer(iLoopPlayer).isAlive() and gc.getTeam(iPlayer).isAtWar(iLoopPlayer):
			return False
	return True
	
def getHappiest():
	lApprovalList = [utils.getApprovalRating(i) for i in range(iNumPlayers)]
	return utils.getHighestIndex(lApprovalList)
	
def isHappiest(iPlayer):
	return getHappiest() == iPlayer
	
def getHealthiest():
	lLifeExpectancyList = [utils.getLifeExpectancyRating(i) for i in range(iNumPlayers)]
	return utils.getHighestIndex(lLifeExpectancyList)
	
def isHealthiest(iPlayer):
	return getHealthiest() == iPlayer
	
def countReligionCities(iPlayer):
	iCount = 0
	for city in utils.getCityList(iPlayer):
		if city.getReligionCount() > 0:
			iCount += 1
	return iCount
	
def isCompleteTechTree(iPlayer):
	if gc.getPlayer(iPlayer).getCurrentEra() < iGlobal: return False
	
	tPlayer = gc.getTeam(iPlayer)
	for iTech in range(iNumTechs):
		if not (tPlayer.isHasTech(iTech) or tPlayer.getTechCount(iTech) > 0): return False
		
	return True
	
def countFirstDiscovered(iPlayer, iEra):
	iCount = 0
	for iTech in range(iNumTechs):
		if gc.getTechInfo(iTech).getEra() == iEra and data.lFirstDiscovered[iTech] == iPlayer:
			iCount += 1
	return iCount
	
def isFirstDiscoveredPossible(iPlayer, iEra, iRequired):
	iCount = countFirstDiscovered(iPlayer, iEra)
	iNotYetDiscovered = countFirstDiscovered(-1, iEra)
	return iCount + iNotYetDiscovered >= iRequired
	
def isWonder(iBuilding):
	return iBeginWonders <= iBuilding < iNumBuildings
	
def countReligionPlayers(iReligion):
	iReligionPlayers = 0
	iTotalPlayers = 0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive():
			iTotalPlayers += 1
			if pPlayer.getStateReligion() == iReligion:
				iReligionPlayers += 1
	return iReligionPlayers, iTotalPlayers
	
def countCivicPlayers(iCivicType, iCivic):
	iCivicPlayers = 0
	iTotalPlayers = 0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive():
			iTotalPlayers += 1
			if pPlayer.getCivics(iCivicType) == iCivic:
				iCivicPlayers += 1
	return iCivicPlayers, iTotalPlayers
	
def getBestCities(function):
	lCities = []
	for iLoopPlayer in range(iNumPlayers):
		lCities.extend(utils.getCityList(iLoopPlayer))
	
	return utils.getSortedList(lCities, function, True)
	
def countBestCitiesReligion(iReligion, function, iNumCities):
	lCities = getBestCities(function)
	
	iCount = 0
	for city in lCities[:iNumCities]:
		if city.isHasReligion(iReligion) and gc.getPlayer(city.getOwner()).getStateReligion() == iReligion:
			iCount += 1
			
	return iCount
	
def getReligiousLand(iReligion):
	fLandPercent = 0.0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive() and pPlayer.getStateReligion() == iReligion:
			fLandPercent += getLandPercent(iPlayer)
	return fLandPercent
	
def countLivingPlayers():
	iCount = 0
	for iPlayer in range(iNumPlayers):
		if gc.getPlayer(iPlayer).isAlive():
			iCount += 1
	return iCount
	
def countGoodRelationPlayers(iPlayer, iAttitudeThreshold):
	iCount = 0
	tPlayer = gc.getTeam(iPlayer)
	for iLoopPlayer in range(iNumPlayers):
		if iPlayer != iLoopPlayer and tPlayer.isHasMet(iLoopPlayer):
			if gc.getPlayer(iLoopPlayer).AI_getAttitude(iPlayer) >= iAttitudeThreshold:
				iCount += 1
	return iCount
	
def countUnitsOfType(iPlayer, lTypes, bIncludeObsolete=False):
	iCount = 0
	pPlayer = gc.getPlayer(iPlayer)
	for iUnit in range(iNumUnits):
		if bIncludeObsolete or pPlayer.canTrain(iUnit, False, False):
			if gc.getUnitInfo(iUnit).getUnitCombatType() in lTypes:
				iUnitClass = gc.getUnitInfo(iUnit).getUnitClassType()
				iCount += pPlayer.getUnitClassCount(iUnitClass)
	return iCount
	
def calculateShrineIncome(iPlayer, iReligion):
	if getNumBuildings(iPlayer, iShrine  + 4*iReligion) == 0: return 0
	
	iThreshold = 20
	if getNumBuildings(iPlayer, iDomeOfTheRock) > 0 and not gc.getTeam(iPlayer).isHasTech(iLiberalism): iThreshold = 40
	
	return min(iThreshold, gc.getGame().countReligionLevels(iReligion))
	
def countWorldBuildings(iBuilding):
	iCount = 0
	for iPlayer in range(iNumPlayers):
		if gc.getPlayer(iPlayer).isAlive():
			iCount += getNumBuildings(iPlayer, utils.getUniqueBuilding(iPlayer, iBuilding))
	return iCount
	
def countReligionWonders(iPlayer, iReligion):
	iCount = 0
	for iWonder in lWonders:
		if gc.getBuildingInfo(iWonder).getPrereqReligion() == iReligion and getNumBuildings(iPlayer, iWonder) > 0:
			iCount += 1
	return iCount
	
def countCivicBuildings(iCivicType, iCivic, iBuilding):
	iCount = 0
	for iPlayer in range(iNumPlayers):
		pPlayer = gc.getPlayer(iPlayer)
		if pPlayer.isAlive() and pPlayer.getCivics(iCivicType) == iCivic:
			iCount += getNumBuildings(iPlayer, utils.getUniqueBuilding(iPlayer, iBuilding))
	return iCount
	
def getApostolicVotePercent(iPlayer):
	iTotal = 0
	for iLoopPlayer in range(iNumPlayers):
		iTotal += gc.getPlayer(iLoopPlayer).getVotes(16, 1)
		
	if iTotal == 0: return 0.0
	
	return gc.getPlayer(iPlayer).getVotes(16, 1) * 100.0 / iTotal
	
def countNativeCulture(iPlayer, iPercent):
	iPlayerCulture = 0
	
	for city in utils.getCityList(iPlayer):
		iCulture = city.getCulture(iPlayer)
		iTotal = 0
		
		for iLoopPlayer in range(iNumTotalPlayersB): iTotal += city.getCulture(iLoopPlayer)
		
		if iTotal > 0 and iCulture * 100 / iTotal >= iPercent:
			iPlayerCulture += iCulture
			
	return iPlayerCulture
	
def isTradeConnected(iPlayer):
	for iOtherPlayer in range(iNumPlayers):
		if iPlayer != iOtherPlayer and gc.getPlayer(iPlayer).canContact(iOtherPlayer) and gc.getPlayer(iPlayer).canTradeNetworkWith(iOtherPlayer):
			return True
			
	return False
	
def countUnitsOfLevel(iPlayer, iLevel):
	pPlayer = gc.getPlayer(iPlayer)
	iCount = 0
	
	for iUnit in range(pPlayer.getNumUnits()):
		unit = pPlayer.getUnit(iUnit)
		if unit.getLevel() >= iLevel:
			iCount += 1
			
	return iCount
	
def countControlledTerrain(iPlayer, iTerrain):
	iCount = 0
	
	for (x, y) in utils.getWorldPlotsList():
		plot = gc.getMap().plot(x, y)
		if plot.getOwner() == iPlayer and plot.getTerrainType() == iTerrain:
			iCount += 1
			
	return iCount
	
def countControlledFeatures(iPlayer, iFeature, iImprovement):
	iCount = 0
	
	for (x, y) in utils.getWorldPlotsList():
		plot = gc.getMap().plot(x, y)
		if plot.getOwner() == iPlayer and plot.getFeatureType() == iFeature and plot.getImprovementType() == iImprovement:
			iCount += 1
			
	return iCount
	
def getGlobalTreasury():
	iTreasury = 0

	for iPlayer in range(iNumPlayers):
		iTreasury += gc.getPlayer(iPlayer).getGold()
		
	return iTreasury
	
def countFirstGreatPeople(iPlayer):
	return len([iGreatPerson for iGreatPerson in lGreatPeopleUnits if getFirstBorn(iGreatPerson) == iPlayer])
	
def countReligionSpecialistCities(iPlayer, iReligion, iSpecialist):
	iCount = 0
	for city in utils.getCityList(iPlayer):
		if city.isHasReligion(iReligion) and city.getFreeSpecialistCount(iSpecialist) > 0:
			iCount += 1
	return iCount
	
def calculateAlliedPercent(iPlayer, function):
	pTeam = gc.getTeam(gc.getPlayer(iPlayer).getTeam())

	iAlliedValue = 0
	iTotalValue = 0
	
	for iLoopPlayer in range(iNumPlayers):
		pLoopPlayer = gc.getPlayer(iLoopPlayer)
		pLoopTeam = gc.getTeam(pLoopPlayer.getTeam())
		
		if not pLoopPlayer.isAlive(): continue
		
		iValue = function(iLoopPlayer)
		
		iTotalValue += iValue
		
		if iLoopPlayer == iPlayer or pLoopTeam.isVassal(gc.getPlayer(iPlayer).getTeam()) or pTeam.isDefensivePact(pLoopPlayer.getTeam()):
			iAlliedValue += iValue
			
	if iTotalValue == 0: return 0
	
	return 100.0 * iAlliedValue / iTotalValue
	
def calculateAlliedCommercePercent(iPlayer):
	return calculateAlliedPercent(iPlayer, lambda x: gc.getPlayer(x).calculateTotalCommerce())
	
def calculateAlliedPowerPercent(iPlayer):
	return calculateAlliedPercent(iPlayer, lambda x: gc.getPlayer(x).getPower())
	
def countRegionReligion(iReligion, lRegions):
	lCities = [gc.getMap().plot(x, y).getPlotCity() for (x, y) in utils.getRegionPlots(lRegions) if gc.getMap().plot(x, y).isCity()]
	return len([city for city in lCities if city.isHasReligion(iReligion)])
	
def findBestCityWith(iPlayer, filter, sort):
	lCities = [city for city in utils.getCityList(iPlayer) if filter(city)]
	return utils.getHighestEntry(lCities, sort)
	
def countVassals(iPlayer, lPlayers=None, iReligion=-1):
	lVassals = [iVassal for iVassal in range(iNumPlayers) if gc.getTeam(iVassal).isVassal(iPlayer) and (not lPlayers or iVassal in lPlayers) and (iReligion < 0 or gc.getPlayer(iVassal).getStateReligion() == iReligion)]
	return len(lVassals)
	
### UHV HELP SCREEN ###

def getIcon(bVal):
	if bVal:
		return u"%c" %(CyGame().getSymbolID(FontSymbols.SUCCESS_CHAR))
	else:
		return u"%c" %(CyGame().getSymbolID(FontSymbols.FAILURE_CHAR))

def getURVHelp(iPlayer, iGoal):
	pPlayer = gc.getPlayer(iPlayer)
	iVictoryType = utils.getReligiousVictoryType(iPlayer)
	aHelp = []

	if checkReligiousGoal(iPlayer, iGoal) == 1:
		aHelp.append(getIcon(True) + localText.getText("TXT_KEY_VICTORY_GOAL_ACCOMPLISHED", ()))
		return aHelp
	elif checkReligiousGoal(iPlayer, iGoal) == 0:
		aHelp.append(getIcon(False) + localText.getText("TXT_KEY_VICTORY_GOAL_FAILED", ()))
		return aHelp
	
	if iVictoryType == iJudaism:
		if iGoal == 0:
			iProphets = countSpecialists(iPlayer, iSpecialistGreatProphet)
			iScientists = countSpecialists(iPlayer, iSpecialistGreatScientist)
			iStatesmen = countSpecialists(iPlayer, iSpecialistGreatStatesman)
			aHelp.append(getIcon(iProphets + iScientists + iStatesmen) + localText.getText("TXT_KEY_VICTORY_JEWISH_SPECIALISTS", (iProphets + iScientists + iStatesmen, 15)))
		elif iGoal == 1:
			holyCity = gc.getGame().getHolyCity(iJudaism)
			aHelp.append(getIcon(holyCity.getOwner() == iPlayer) + localText.getText("TXT_KEY_VICTORY_CONTROL_HOLY_CITY", (holyCity.getName(),)) + ' ' + getIcon(holyCity.getCultureLevel() >= 6) + localText.getText("TXT_KEY_VICTORY_LEGENDARY_CULTURE_CITY", (holyCity.getName(),)))
		elif iGoal == 2:
			iFriendlyRelations = countPlayersWithAttitudeAndReligion(iPlayer, AttitudeTypes.ATTITUDE_FRIENDLY, iJudaism)
			aHelp.append(getIcon(iFriendlyRelations >= 6) + localText.getText("TXT_KEY_VICTORY_FRIENDLY_RELIGION", (gc.getReligionInfo(iJudaism).getAdjectiveKey(), iFriendlyRelations, 6)))

	elif iVictoryType == iOrthodoxy:
		if iGoal == 0:
			iOrthodoxCathedrals = getNumBuildings(iPlayer, iOrthodoxCathedral)
			aHelp.append(getIcon(iOrthodoxCathedrals >= 4) + localText.getText("TXT_KEY_VICTORY_ORTHODOX_CATHEDRALS", (iOrthodoxCathedrals, 4)))
		elif iGoal == 1:
			lCultureCities = getBestCities(cityCulture)[:5]
			iCultureCities = countBestCitiesReligion(iOrthodoxy, cityCulture, 5)
			for city in lCultureCities:
				aHelp.append(getIcon(city.isHasReligion(iOrthodoxy) and gc.getPlayer(city.getOwner()).getStateReligion() == iOrthodoxy) + city.getName())
		elif iGoal == 2:
			bNoCatholics = countReligionPlayers(iCatholicism)[0] == 0
			aHelp.append(getIcon(bNoCatholics) + localText.getText("TXT_KEY_VICTORY_NO_CATHOLICS", ()))

	elif iVictoryType == iCatholicism:
		if iGoal == 0:
			iPopeTurns = data.iPopeTurns
			aHelp.append(getIcon(iPopeTurns >= utils.getTurns(100)) + localText.getText("TXT_KEY_VICTORY_POPE_TURNS", (iPopeTurns, utils.getTurns(100))))
		elif iGoal == 1:
			bShrine = pPlayer.countNumBuildings(iCatholicShrine) > 0
			iSaints = countReligionSpecialists(iCatholicism, iSpecialistGreatProphet)
			aHelp.append(getIcon(bShrine) + localText.getText("TXT_KEY_BUILDING_CATHOLIC_SHRINE", ()) + ' ' + getIcon(iSaints >= 12) + localText.getText("TXT_KEY_VICTORY_CATHOLIC_SAINTS", (iSaints, 12)))
		elif iGoal == 2:
			fLandPercent = getReligiousLand(iCatholicism)
			aHelp.append(getIcon(fLandPercent >= 50.0) + localText.getText("TXT_KEY_VICTORY_CATHOLIC_WORLD_TERRITORY", (str(u"%.2f%%" % fLandPercent), str(50))))

	elif iVictoryType == iProtestantism:
		if iGoal == 0:
			bCivilLiberties = data.lFirstDiscovered[iCivilLiberties] == iPlayer
			bConstitution = data.lFirstDiscovered[iSocialContract] == iPlayer
			bEconomics = data.lFirstDiscovered[iEconomics] == iPlayer
			aHelp.append(getIcon(bCivilLiberties) + localText.getText("TXT_KEY_TECH_CIVIL_LIBERTIES", ()) + ' ' + getIcon(bConstitution) + localText.getText("TXT_KEY_TECH_CONSTITUTION", ()) + ' ' + getIcon(bEconomics) + localText.getText("TXT_KEY_TECH_ECONOMICS", ()))
		elif iGoal == 1:
			iMerchants = countReligionSpecialists(iProtestantism, iSpecialistGreatMerchant)
			iEngineers = countReligionSpecialists(iProtestantism, iSpecialistGreatEngineer)
			aHelp.append(getIcon(iMerchants >= 5) + localText.getText("TXT_KEY_VICTORY_PROTESTANT_MERCHANTS", (iMerchants, 5)) + ' ' + getIcon(iEngineers >= 5) + localText.getText("TXT_KEY_VICTORY_PROTESTANT_ENGINEERS", (iEngineers, 5)))
		elif iGoal == 2:
			iProtestantCivs, iTotal = countReligionPlayers(iProtestantism)
			iSecularCivs, iTotal = countCivicPlayers(iCivicsReligion, iSecularism)
			iNumProtestantCivs = iProtestantCivs + iSecularCivs
			aHelp.append(getIcon(2 * iNumProtestantCivs >= iTotal) + localText.getText("TXT_KEY_VICTORY_PROTESTANT_CIVS", (iNumProtestantCivs, iTotal)))

	elif iVictoryType == iIslam:
		if iGoal == 0:
			fReligionPercent = gc.getGame().calculateReligionPercent(iIslam)
			aHelp.append(getIcon(fReligionPercent >= 40.0) + localText.getText("TXT_KEY_VICTORY_SPREAD_RELIGION_PERCENT", (gc.getReligionInfo(iIslam).getTextKey(), str(u"%.2f%%" % fReligionPercent), str(40))))
		elif iGoal == 1:
			iCount = 0
			pHolyCity = gc.getGame().getHolyCity(iIslam)
			for iGreatPerson in lGreatPeople:
				iCount += pHolyCity.getFreeSpecialistCount(iGreatPerson)
			aHelp.append(getIcon(iCount >= 7) + localText.getText("TXT_KEY_VICTORY_CITY_GREAT_PEOPLE", (gc.getGame().getHolyCity(iIslam).getName(), iCount, 7)))
		elif iGoal == 2:
			iCount = 0
			for iReligion in range(iNumReligions):
				iCount += getNumBuildings(iPlayer, iShrine + 4*iReligion)
			aHelp.append(getIcon(iCount >= 5) + localText.getText("TXT_KEY_VICTORY_NUM_SHRINES", (iCount, 5)))

	elif iVictoryType == iHinduism:
		if iGoal == 0:
			iCount = 0
			pHolyCity = gc.getGame().getHolyCity(iHinduism)
			for iGreatPerson in lGreatPeople:
				if pHolyCity.getFreeSpecialistCount(iGreatPerson) > 0:
					iCount += 1
			aHelp.append(getIcon(iCount >= 5) + localText.getText("TXT_KEY_VICTORY_CITY_DIFFERENT_GREAT_PEOPLE", (gc.getGame().getHolyCity(iHinduism).getName(), iCount, 5)))
		elif iGoal == 1:
			iGoldenAgeTurns = data.iHinduGoldenAgeTurns
			aHelp.append(getIcon(iGoldenAgeTurns >= utils.getTurns(36)) + localText.getText("TXT_KEY_VICTORY_GOLDEN_AGE_TURNS", (iGoldenAgeTurns, utils.getTurns(36))))
		elif iGoal == 2:
			iLargestCities = countBestCitiesReligion(iHinduism, cityPopulation, 5)
			aHelp.append(getIcon(iLargestCities >= 5) + localText.getText("TXT_KEY_VICTORY_HINDU_LARGEST_CITIES", (iLargestCities, 5)))

	elif iVictoryType == iBuddhism:
		if iGoal == 0:
			iPeaceTurns = data.iBuddhistPeaceTurns
			aHelp.append(getIcon(iPeaceTurns >= utils.getTurns(100)) + localText.getText("TXT_KEY_VICTORY_PEACE_TURNS", (iPeaceTurns, utils.getTurns(100))))
		elif iGoal == 1:
			iHappinessTurns = data.iBuddhistHappinessTurns
			aHelp.append(getIcon(iHappinessTurns >= utils.getTurns(100)) + localText.getText("TXT_KEY_VICTORY_HAPPINESS_TURNS", (iHappinessTurns, utils.getTurns(100))))
		elif iGoal == 2:
			iGoodRelations = countGoodRelationPlayers(iPlayer, AttitudeTypes.ATTITUDE_CAUTIOUS)
			iTotalPlayers = countLivingPlayers()-1
			aHelp.append(getIcon(iGoodRelations >= iTotalPlayers) + localText.getText("TXT_KEY_VICTORY_CAUTIOUS_OR_BETTER_RELATIONS", (iGoodRelations, iTotalPlayers)))

	elif iVictoryType == iConfucianism:
		if iGoal == 0:
			iFriendlyCivs = countGoodRelationPlayers(iPlayer, AttitudeTypes.ATTITUDE_FRIENDLY)
			aHelp.append(getIcon(iFriendlyCivs >= 5) + localText.getText("TXT_KEY_VICTORY_FRIENDLY_CIVS", (iFriendlyCivs, 5)))
		elif iGoal == 1:
			holyCity = gc.getGame().getHolyCity(iConfucianism)
			iCount = countCityWonders(iPlayer, (holyCity.getX(), holyCity.getY()), True)
			aHelp.append(getIcon(holyCity.getOwner() == iPlayer) + localText.getText("TXT_KEY_VICTORY_CONTROL_HOLY_CITY", (holyCity.getName(),)) + ' ' + getIcon(iCount >= 5) + localText.getText("TXT_KEY_VICTORY_HOLY_CITY_WONDERS", (holyCity.getName(), iCount, 5)))
		elif iGoal == 2:
			iUnitCombatMelee = gc.getInfoTypeForString("UNITCOMBAT_MELEE")
			iUnitCombatGunpowder = gc.getInfoTypeForString("UNITCOMBAT_GUN")
			iCount = countUnitsOfType(iPlayer, [iUnitCombatMelee, iUnitCombatGunpowder])
			aHelp.append(getIcon(iCount >= 200) + localText.getText("TXT_KEY_VICTORY_CONTROL_NUM_UNITS", (iCount, 200)))

	elif iVictoryType == iTaoism:
		if iGoal == 0:
			iHealthTurns = data.iTaoistHealthTurns
			aHelp.append(getIcon(iHealthTurns >= utils.getTurns(100)) + localText.getText("TXT_KEY_VICTORY_HEALTH_TURNS", (iHealthTurns, utils.getTurns(100))))
		elif iGoal == 1:
			bConfucianShrine = getNumBuildings(iPlayer, iConfucianShrine) > 0
			bTaoistShrine = getNumBuildings(iPlayer, iTaoistShrine) > 0

			iConfucianIncome = calculateShrineIncome(iPlayer, iConfucianism)
			iTaoistIncome = calculateShrineIncome(iPlayer, iTaoism)
			
			aHelp.append(getIcon(bConfucianShrine) + localText.getText("TXT_KEY_BUILDING_CONFUCIAN_SHRINE", ()) + ' ' + getIcon(bTaoistShrine) + localText.getText("TXT_KEY_BUILDING_TAOIST_SHRINE", ()) + ' ' + getIcon(iConfucianIncome + iTaoistIncome >= 40) + localText.getText("TXT_KEY_VICTORY_CHINESE_SHRINE_INCOME", (iConfucianIncome + iTaoistIncome, 40)))
		elif iGoal == 2:
			holyCity = gc.getGame().getHolyCity(iTaoism)
			aHelp.append(getIcon(holyCity.getOwner() == iPlayer) + localText.getText("TXT_KEY_VICTORY_CONTROL_HOLY_CITY", (holyCity.getName(),)) + ' ' + getIcon(holyCity.getCultureLevel() >= 6) + localText.getText("TXT_KEY_VICTORY_LEGENDARY_CULTURE_CITY", (holyCity.getName(),)))

	elif iVictoryType == iZoroastrianism:
		if iGoal == 0:
			iNumIncense = pPlayer.getNumAvailableBonuses(iIncense)
			aHelp.append(getIcon(iNumIncense >= 6) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_INCENSE_RESOURCES", (iNumIncense, 6)))
		elif iGoal == 1:
			fReligionPercent = gc.getGame().calculateReligionPercent(iZoroastrianism)
			aHelp.append(getIcon(fReligionPercent >= 10.0) + localText.getText("TXT_KEY_VICTORY_SPREAD_RELIGION_PERCENT", (gc.getReligionInfo(iZoroastrianism).getTextKey(), str(u"%.2f%%" % fReligionPercent), str(10))))
		elif iGoal == 2:
			holyCity = gc.getGame().getHolyCity(iZoroastrianism)
			aHelp.append(getIcon(holyCity.getOwner() == iPlayer) + localText.getText("TXT_KEY_VICTORY_CONTROL_HOLY_CITY", (holyCity.getName(),)) + ' ' + getIcon(holyCity.getCultureLevel() >= 6) + localText.getText("TXT_KEY_VICTORY_LEGENDARY_CULTURE_CITY", (holyCity.getName(),)))

	elif iVictoryType == iVictoryPaganism:
		if iGoal == 0:
			iCount = countWorldBuildings(iPaganTemple)
			aHelp.append(getIcon(iCount >= 15) + localText.getText("TXT_KEY_VICTORY_NUM_PAGAN_TEMPLES_WORLD", (iCount, 15)))
		elif iGoal == 1:
			aHelp.append(getPaganGoalHelp(iPlayer))
		elif iGoal == 2:
			bPolytheismNeverReligion = data.bPolytheismNeverReligion
			aHelp.append(getIcon(bPolytheismNeverReligion) + localText.getText("TXT_KEY_VICTORY_POLYTHEISM_NEVER_RELIGION", ()))

	elif iVictoryType == iVictorySecularism:
		if iGoal == 0:
			iCount = 0
			for iReligion in range(iNumReligions):
				if getNumBuildings(iPlayer, iCathedral + 4 * iReligion) > 0:
					iCount += 1
			aHelp.append(getIcon(iCount >= iNumReligions) + localText.getText("TXT_KEY_VICTORY_DIFFERENT_CATHEDRALS", (iCount, iNumReligions)))
		elif iGoal == 1:
			iUniversities = countCivicBuildings(iCivicsReligion, iSecularism, iUniversity)
			iScientists = countCivicSpecialists(iCivicsReligion, iSecularism, iSpecialistGreatScientist)
			iStatesmen = countCivicSpecialists(iCivicsReligion, iSecularism, iSpecialistGreatStatesman)
			aHelp.append(getIcon(iUniversities >= 25) + localText.getText("TXT_KEY_VICTORY_SECULAR_UNIVERSITIES", (iUniversities, 25)))
			aHelp.append(getIcon(iScientists >= 10) + localText.getText("TXT_KEY_VICTORY_SECULAR_SCIENTISTS", (iScientists, 10)) + ' ' + getIcon(iStatesmen >= 10) + localText.getText("TXT_KEY_VICTORY_SECULAR_STATESMEN", (iStatesmen, 10)))
		elif iGoal == 2:
			sString = ""
			lAdvancedPlayers = utils.getSortedList([iLoopPlayer for iLoopPlayer in range(iNumPlayers) if gc.getPlayer(iLoopPlayer).isAlive() and not utils.isAVassal(iLoopPlayer)], lambda iLoopPlayer: gc.getTeam(iLoopPlayer).getTotalTechValue(), True)
			for iLoopPlayer in lAdvancedPlayers[:5]:
				pLoopPlayer = gc.getPlayer(iLoopPlayer)
				sString += getIcon(pLoopPlayer.getCivics(iCivicsReligion) == iSecularism) + pLoopPlayer.getCivilizationShortDescription(0) + ' '
			aHelp.append(sString)
				
	return aHelp
	
def getPaganGoalHelp(iPlayer):
	pPlayer = gc.getPlayer(iPlayer)
	paganReligion = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getPaganReligionName(0)

	if paganReligion == "Anunnaki":
		x, y = 0, 0
		capital = pPlayer.getCapitalCity()
		
		if capital:
			x, y = capital.getX(), capital.getY()
		pBestCity = getBestCity(iPlayer, (x, y), cityWonders)
		bBestCity = isBestCity(iPlayer, (x, y), cityWonders)
		sBestCity = "(none)"
		if pBestCity:
			sBestCity = pBestCity.getName()
		return getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_CITY_WITH_MOST_WONDERS", (sBestCity,))
		
	elif paganReligion == "Asatru":
		iCount = countUnitsOfLevel(iPlayer, 5)
		return getIcon(iCount >= 5) + localText.getText("TXT_KEY_VICTORY_UNITS_OF_LEVEL", (5, iCount, 5))
		
	elif paganReligion == "Atua":
		iNumPearls = pPlayer.getNumAvailableBonuses(iPearls)
		iOceanTiles = countControlledTerrain(iPlayer, iOcean)
		return getIcon(iNumPearls >= 4) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_RESOURCES", (gc.getBonusInfo(iPearls).getText().lower(), iNumPearls, 4)) + ' ' + getIcon(iOceanTiles >= 50) + localText.getText("TXT_KEY_VICTORY_CONTROLLED_OCEAN_TILES", (iOceanTiles, 50))
		
	elif paganReligion == "Baalism":
		x, y = 0, 0
		capital = pPlayer.getCapitalCity()
		
		if capital:
			x, y = capital.getX(), capital.getY()
		pBestCity = getBestCity(iPlayer, (x, y), cityTradeIncome)
		bBestCity = isBestCity(iPlayer, (x, y), cityTradeIncome)
		return getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_HIGHEST_TRADE_CITY", (pBestCity.getName(),))
		
	elif paganReligion == "Druidism":
		iCount = countControlledFeatures(iPlayer, iForest, -1) + countControlledFeatures(iPlayer, iMud, -1)
		return getIcon(iCount >= 20) + localText.getText("TXT_KEY_VICTORY_CONTROLLED_FOREST_AND_MARSH_TILES", (iCount, 20))
	
	elif paganReligion == "Inti":
		iOurTreasury = pPlayer.getGold()
		iWorldTreasury = getGlobalTreasury()
		return getIcon(2 * iOurTreasury >= iWorldTreasury) + localText.getText("TXT_KEY_VICTORY_TOTAL_GOLD", (iOurTreasury, iWorldTreasury - iOurTreasury))
	
	elif paganReligion == "Mazdaism":
		iCount = pPlayer.getNumAvailableBonuses(iIncense)
		return getIcon(iCount >= 6) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_RESOURCES", (gc.getBonusInfo(iIncense).getText().lower(), iCount, 6))
	
	elif paganReligion == "Olympianism":
		iCount = countReligionWonders(iPlayer, -1)
		return getIcon(iCount >= 10) + localText.getText("TXT_KEY_VICTORY_NUM_NONRELIGIOUS_WONDERS", (iCount, 10))
		
	elif paganReligion == "Pesedjet":
		iCount = countFirstGreatPeople(iPlayer)
		return getIcon(iCount >= 3) + localText.getText("TXT_KEY_VICTORY_FIRST_GREAT_PEOPLE", (iCount, 3))
	
	elif paganReligion == "Rodnovery":
		iCount = pPlayer.getNumAvailableBonuses(iFur)
		return getIcon(iCount >= 7) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_RESOURCES", (gc.getBonusInfo(iFur).getText().lower(), iCount, 7))
	
	elif paganReligion == "Shendao":
		fPopulationPercent = getPopulationPercent(iPlayer)
		return getIcon(fPopulationPercent >= 25.0) + localText.getText("TXT_KEY_VICTORY_PERCENTAGE_WORLD_POPULATION", (str(u"%.2f%%" % fPopulationPercent), str(25)))
	
	elif paganReligion == "Shinto":
		capital = pPlayer.getCapitalCity()
		iCount = 0
		if capital: iCount = countCitySpecialists(iPlayer, (capital.getX(), capital.getY()), iSpecialistGreatSpy)
		return getIcon(iCount >= 3) + localText.getText("TXT_KEY_VICTORY_CAPITAL_GREAT_SPIES", (iCount, 3))
	
	elif paganReligion == "Tengri":
		iCount = pPlayer.getNumAvailableBonuses(iHorse)
		return getIcon(iCount >= 8) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_RESOURCES", (gc.getBonusInfo(iHorse).getText().lower(), iCount, 8))
	
	elif paganReligion == "Teotl":
		iCount = data.iTeotlSacrifices
		if iPlayer == iMaya:
			return getIcon(iCount >= 10) + localText.getText("TXT_KEY_VICTORY_FOOD_FROM_COMBAT", (iCount * 5, 50))
		return getIcon(iCount >= 10) + localText.getText("TXT_KEY_VICTORY_SACRIFICED_SLAVES", (iCount, 10))
	
	elif paganReligion == "Vedism":
		iCount = data.iVedicHappiness
		return getIcon(iCount >= 100) + localText.getText("TXT_KEY_VICTORY_WE_LOVE_RULER_TURNS", (iCount, 100))
	
	elif paganReligion == "Yoruba":
		iNumIvory = pPlayer.getNumAvailableBonuses(iIvory)
		iNumGems = pPlayer.getNumAvailableBonuses(iGems)
		return getIcon(iNumIvory >= 8) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_RESOURCES", (gc.getBonusInfo(iIvory).getText().lower(), iNumIvory, 8)) + ' ' + getIcon(iNumGems >= 6) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_RESOURCES", (gc.getBonusInfo(iGems).getText().lower(), iNumGems, 6))

def getUHVHelp(iPlayer, iGoal):
	"Returns an array of help strings used by the Victory Screen table"

	aHelp = []

	# the info is outdated or irrelevant once the goal has been accomplished or failed
	if data.players[iPlayer].lGoals[iGoal] == 1:
		iWinTurn = data.players[iPlayer].lGoalTurns[iGoal]
		iTurnYear = gc.getGame().getTurnYear(iWinTurn)
		if iTurnYear < 0:
			sWinDate = localText.getText("TXT_KEY_TIME_BC", (-iTurnYear,))
		else:
			sWinDate = localText.getText("TXT_KEY_TIME_AD", (iTurnYear,))
		if not gc.getPlayer(iPlayer).isOption(PlayerOptionTypes.PLAYEROPTION_MODDER_1):
			aHelp.append(getIcon(True) + localText.getText("TXT_KEY_VICTORY_GOAL_ACCOMPLISHED_DATE", (sWinDate,)))
		else:
			aHelp.append(getIcon(True) + localText.getText("TXT_KEY_VICTORY_GOAL_ACCOMPLISHED_DATE_TURN", (sWinDate, iWinTurn - utils.getScenarioStartTurn())))
		return aHelp
	elif data.players[iPlayer].lGoals[iGoal] == 0:
		aHelp.append(getIcon(False) + localText.getText("TXT_KEY_VICTORY_GOAL_FAILED", ()))
		return aHelp

	if iPlayer == iEgypt:
		if iGoal == 0:
			iCulture = pEgypt.countTotalCulture()
			bJerusalem = controlsCity(iEgypt, tJerusalem)
			aHelp.append(getIcon(bJerusalem) + localText.getText("TXT_KEY_VICTORY_JERUSALEM", ()))
			aHelp.append(getIcon(iCulture >= utils.getTurns(500)) + localText.getText("TXT_KEY_VICTORY_TOTAL_CULTURE", (iCulture, utils.getTurns(500))))
		elif iGoal == 1:
			bPyramids = data.getWonderBuilder(iPyramids) == iEgypt
			bSphinx = data.getWonderBuilder(iGreatSphinx) == iEgypt
			bLibrary = data.getWonderBuilder(iGreatLibrary) == iEgypt
			bLighthouse = data.getWonderBuilder(iGreatLighthouse) == iEgypt
			aHelp.append(getIcon(bPyramids) + localText.getText("TXT_KEY_BUILDING_PYRAMIDS", ()) + getIcon(bSphinx) + localText.getText("TXT_KEY_BUILDING_GREAT_SPHINX", ()) + getIcon(bLibrary) + localText.getText("TXT_KEY_BUILDING_GREAT_LIBRARY", ()) + getIcon(bLighthouse) + localText.getText("TXT_KEY_BUILDING_GREAT_LIGHTHOUSE", ()))
		elif iGoal == 2:
			iCulture = pEgypt.countTotalCulture()
			aHelp.append(getIcon(iCulture >= utils.getTurns(5000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_CULTURE", (iCulture, utils.getTurns(5000))))

	elif iPlayer == iHarappa:
		if iGoal == 1:
			iNumReservoirs = getNumBuildings(iHarappa, iReservoir)
			iNumGranaries = getNumBuildings(iHarappa, iGranary)
			iNumSmokehouses = getNumBuildings(iHarappa, iSmokehouse)
			aHelp.append(getIcon(iNumReservoirs >= 3) + localText.getText("TXT_KEY_VICTORY_NUM_RESERVOIRS", (iNumReservoirs, 3)) + ' ' + getIcon(iNumGranaries >= 2) + localText.getText("TXT_KEY_VICTORY_NUM_GRANARIES", (iNumGranaries, 2)) + ' ' + getIcon(iNumSmokehouses >= 2) + localText.getText("TXT_KEY_VICTORY_NUM_SMOKEHOUSES", (iNumSmokehouses, 2)))
		elif iGoal == 2:
			iNumPopulation = pHarappa.getTotalPopulation()
			aHelp.append(getIcon(iNumPopulation >= 25) + localText.getText("TXT_KEY_VICTORY_TOTAL_POPULATION", (iNumPopulation, 25)))
			
	elif iPlayer == iBabylonia:
		if iGoal == 0:
			bConstruction = data.lFirstDiscovered[iConstruction] == iBabylonia
			bArithmetics = data.lFirstDiscovered[iArithmetics] == iBabylonia
			bWriting = data.lFirstDiscovered[iWriting] == iBabylonia
			bCalendar = data.lFirstDiscovered[iCalendar] == iBabylonia
			bContract = data.lFirstDiscovered[iContract] == iBabylonia
			aHelp.append(getIcon(bConstruction) + localText.getText("TXT_KEY_TECH_CONSTRUCTION", ()) + ' ' + getIcon(bArithmetics) + localText.getText("TXT_KEY_TECH_ARITHMETICS", ()) + ' ' + getIcon(bWriting) + localText.getText("TXT_KEY_TECH_WRITING", ()))
			aHelp.append(getIcon(bCalendar) + localText.getText("TXT_KEY_TECH_CALENDAR", ()) + ' ' + getIcon(bContract) + localText.getText("TXT_KEY_TECH_CONTRACT", ()))
		elif iGoal == 1:
			pBestCity = getBestCity(iBabylonia, (76, 40), cityPopulation)
			bBestCity = isBestCity(iBabylonia, (76, 40), cityPopulation)
			aHelp.append(getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_MOST_POPULOUS_CITY", (pBestCity.getName(),)))
		elif iGoal == 2:
			pBestCity = getBestCity(iBabylonia, (76, 40), cityCulture)
			bBestCity = isBestCity(iBabylonia, (76, 40), cityCulture)
			aHelp.append(getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_MOST_CULTURED_CITY", (pBestCity.getName(),)))
			
	elif iPlayer == iChina:
		if iGoal == 0:
			iConfucianCounter = getNumBuildings(iChina, iConfucianCathedral)
			iTaoistCounter = getNumBuildings(iChina, iTaoistCathedral)
			aHelp.append(getIcon(iConfucianCounter >= 2) + localText.getText("TXT_KEY_VICTORY_NUM_CONFUCIAN_ACADEMIES", (iConfucianCounter, 2)) + ' ' + getIcon(iTaoistCounter >= 2) + localText.getText("TXT_KEY_VICTORY_NUM_TAOIST_PAGODAS", (iTaoistCounter, 2)))
		elif iGoal == 1:
			bCompass = data.lFirstDiscovered[iCompass] == iChina
			bPaper = data.lFirstDiscovered[iPaper] == iChina
			bGunpowder = data.lFirstDiscovered[iGunpowder] == iChina
			bPrintingPress = data.lFirstDiscovered[iPrinting] == iChina
			aHelp.append(getIcon(bCompass) + localText.getText("TXT_KEY_TECH_COMPASS", ()) + ' ' + getIcon(bPaper) + localText.getText("TXT_KEY_TECH_PAPER", ()) + ' ' + getIcon(bGunpowder) + localText.getText("TXT_KEY_TECH_GUNPOWDER", ()) + ' ' + getIcon(bPrintingPress) + localText.getText("TXT_KEY_TECH_PRINTING", ()))
		elif iGoal == 2:
			iGoldenAgeTurns = data.iChineseGoldenAgeTurns
			aHelp.append(getIcon(iGoldenAgeTurns >= utils.getTurns(64)) + localText.getText("TXT_KEY_VICTORY_GOLDEN_AGES", (iGoldenAgeTurns / utils.getTurns(8), 8)))

	elif iPlayer == iGreece:
		if iGoal == 0:
			bMathematics = data.lFirstDiscovered[iMathematics] == iGreece
			bLiterature = data.lFirstDiscovered[iLiterature] == iGreece
			bAesthetics = data.lFirstDiscovered[iAesthetics] == iGreece
			bPhilosophy = data.lFirstDiscovered[iPhilosophy] == iGreece
			bMedicine = data.lFirstDiscovered[iMedicine] == iGreece
			aHelp.append(getIcon(bMathematics) + localText.getText("TXT_KEY_TECH_MATHEMATICS", ()) + ' ' + getIcon(bLiterature) + localText.getText("TXT_KEY_TECH_LITERATURE", ()) + ' ' + getIcon(bAesthetics) + localText.getText("TXT_KEY_TECH_AESTHETICS", ()))
			aHelp.append(getIcon(bPhilosophy) + localText.getText("TXT_KEY_TECH_PHILOSOPHY", ()) + ' ' + getIcon(bMedicine) + localText.getText("TXT_KEY_TECH_MEDICINE", ()))
		elif iGoal == 1:
			bEgypt = checkOwnedCiv(iGreece, iEgypt)
			bPhoenicia = checkOwnedCiv(iGreece, iCarthage)
			bBabylonia = checkOwnedCiv(iGreece, iBabylonia)
			bPersia = checkOwnedCiv(iGreece, iPersia)
			aHelp.append(getIcon(bEgypt) + localText.getText("TXT_KEY_CIV_EGYPT_SHORT_DESC", ()) + ' ' + getIcon(bPhoenicia) + localText.getText("TXT_KEY_CIV_PHOENICIA_SHORT_DESC", ()) + ' ' + getIcon(bBabylonia) + localText.getText("TXT_KEY_CIV_BABYLONIA_SHORT_DESC", ()) + ' ' + getIcon(bPersia) + localText.getText("TXT_KEY_CIV_PERSIA_SHORT_DESC", ()))
		elif iGoal == 2:
			bOracle = (getNumBuildings(iGreece, iOracle) > 0)
			bParthenon = (getNumBuildings(iGreece, iParthenon) > 0)
			bColossus = (getNumBuildings(iGreece, iColossus) > 0)
			bStatueOfZeus = (getNumBuildings(iGreece, iStatueOfZeus) > 0)
			bArtemis = (getNumBuildings(iGreece, iTempleOfArtemis) > 0)
			aHelp.append(getIcon(bOracle) + localText.getText("TXT_KEY_BUILDING_ORACLE", ()) + ' ' + getIcon(bParthenon) + localText.getText("TXT_KEY_BUILDING_PARTHENON", ()) + ' ' + getIcon(bColossus) + localText.getText("TXT_KEY_BUILDING_COLOSSUS", ()) + ' ' + getIcon(bStatueOfZeus) + localText.getText("TXT_KEY_BUILDING_STATUE_OF_ZEUS", ()) + ' ' + getIcon(bArtemis) + localText.getText("TXT_KEY_BUILDING_TEMPLE_OF_ARTEMIS", ()))

	elif iPlayer == iIndia:
		if iGoal == 0:
			bBuddhistShrine = (getNumBuildings(iIndia, iBuddhistShrine) > 0)
			bHinduShrine = (getNumBuildings(iIndia, iHinduShrine) > 0)
			aHelp.append(getIcon(bHinduShrine) + localText.getText("TXT_KEY_VICTORY_HINDU_SHRINE", ()) + ' ' + getIcon(bBuddhistShrine) + localText.getText("TXT_KEY_VICTORY_BUDDHIST_SHRINE", ()))
		elif iGoal == 1:
			lTemples = [iTemple + 4 * i for i in range(iNumReligions)]
			iCounter = 0
			for iGoalTemple in lTemples:
				iCounter += getNumBuildings(iIndia, iGoalTemple)
			aHelp.append(getIcon(iCounter >= 18) + localText.getText("TXT_KEY_VICTORY_TEMPLES_BUILT", (iCounter, 18)))
		elif iGoal == 2:
			popPercent = getPopulationPercent(iIndia)
			aHelp.append(getIcon(popPercent >= 18.0) + localText.getText("TXT_KEY_VICTORY_PERCENTAGE_WORLD_POPULATION", (str(u"%.2f%%" % popPercent), str(18))))

	elif iPlayer == iCarthage:
		if iGoal == 0:
			bPalace = isBuildingInCity((58, 39), iPalace)
			bGreatCothon = isBuildingInCity((58, 39), iGreatCothon)
			aHelp.append(getIcon(bPalace) + localText.getText("TXT_KEY_BUILDING_PALACE", ()) + ' ' + getIcon(bGreatCothon) + localText.getText("TXT_KEY_BUILDING_GREAT_COTHON", ()))
		elif iGoal == 1:
			bItaly = isControlled(iCarthage, utils.getPlotList(Areas.tNormalArea[iItaly][0], Areas.tNormalArea[iItaly][1], [(62, 47), (63, 47), (63, 46)]))
			bIberia = isControlled(iCarthage, Areas.getNormalArea(iSpain, False))
			aHelp.append(getIcon(bItaly) + localText.getText("TXT_KEY_VICTORY_ITALY", ()) + ' ' + getIcon(bIberia) + localText.getText("TXT_KEY_VICTORY_IBERIA_CARTHAGE", ()))
		elif iGoal == 2:
			iTreasury = pCarthage.getGold()
			aHelp.append(getIcon(iTreasury >= utils.getTurns(3000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_GOLD", (iTreasury, utils.getTurns(3000))))

	elif iPlayer == iPolynesia:
		if iGoal == 0 or iGoal == 1:
			bHawaii = getNumCitiesInArea(iPolynesia, utils.getPlotList(tHawaiiTL, tHawaiiBR)) > 0
			bNewZealand = getNumCitiesInArea(iPolynesia, utils.getPlotList(tNewZealandTL, tNewZealandBR)) > 0
			bMarquesas = getNumCitiesInArea(iPolynesia, utils.getPlotList(tMarquesasTL, tMarquesasBR)) > 0
			bEasterIsland = getNumCitiesInArea(iPolynesia, utils.getPlotList(tEasterIslandTL, tEasterIslandBR)) > 0
			aHelp.append(getIcon(bHawaii) + localText.getText("TXT_KEY_VICTORY_HAWAII", ()) + getIcon(bNewZealand) + localText.getText("TXT_KEY_VICTORY_NEW_ZEALAND", ()) + getIcon(bMarquesas) + localText.getText("TXT_KEY_VICTORY_MARQUESAS", ()) + getIcon(bEasterIsland) + localText.getText("TXT_KEY_VICTORY_EASTER_ISLAND", ()))

	elif iPlayer == iPersia:
		if not pPersia.isReborn():
			if iGoal == 0:
				landPercent = getLandPercent(iPersia)
				aHelp.append(getIcon(landPercent >= 5.995) + localText.getText("TXT_KEY_VICTORY_PERCENTAGE_WORLD_TERRITORY", (str(u"%.2f%%" % landPercent), str(6))))
			elif iGoal == 1:
				iCounter = countWonders(iPersia)
				aHelp.append(getIcon(iCounter >= 7) + localText.getText("TXT_KEY_VICTORY_NUM_WONDERS", (iCounter, 7)))
			elif iGoal == 2:
				iCounter = countShrines(iPersia)
				aHelp.append(getIcon(iCounter >= 3) + localText.getText("TXT_KEY_VICTORY_NUM_SHRINES", (iCounter, 3)))
		else:
			if iGoal == 0:
				iCount = countOpenBorders(iPersia, lCivGroups[0])
				aHelp.append(getIcon(iCount >= 6) + localText.getText("TXT_KEY_VICTORY_OPEN_BORDERS", (iCount, 6)))
			elif iGoal == 1:
				bMesopotamia = isControlled(iPersia, utils.getPlotList(tSafavidMesopotamiaTL, tSafavidMesopotamiaBR))
				bTransoxania = isControlled(iPersia, utils.getPlotList(tTransoxaniaTL, tTransoxaniaBR))
				bNWIndia = isControlled(iPersia, utils.getPlotList(tNWIndiaTL, tNWIndiaBR, tNWIndiaExceptions))
				aHelp.append(getIcon(bMesopotamia) + localText.getText("TXT_KEY_VICTORY_MESOPOTAMIA", ()) + ' ' + getIcon(bTransoxania) + localText.getText("TXT_KEY_VICTORY_TRANSOXANIA", ()) + ' ' + getIcon(bNWIndia) + localText.getText("TXT_KEY_VICTORY_NORTHWEST_INDIA", ()))
			elif iGoal == 2:
				pBestCity = getMostCulturedCity(iPersia)
				iCulture = pBestCity.getCulture(iPersia)
				aHelp.append(getIcon(iCulture >= utils.getTurns(20000)) + localText.getText("TXT_KEY_VICTORY_MOST_CULTURED_CITY_VALUE", (pBestCity.getName(), iCulture, utils.getTurns(20000))))
				
	elif iPlayer == iRome:
		if iGoal == 0:
			iNumBarracks = getNumBuildings(iRome, iBarracks)
			iNumAqueducts = getNumBuildings(iRome, iAqueduct)
			iNumArenas = getNumBuildings(iRome, iArena)
			iNumForums = getNumBuildings(iRome, iForum)
			aHelp.append(getIcon(iNumBarracks >= 6) + localText.getText("TXT_KEY_VICTORY_NUM_BARRACKS", (iNumBarracks, 6)) + ' ' + getIcon(iNumAqueducts >= 5) + localText.getText("TXT_KEY_VICTORY_NUM_AQUEDUCTS", (iNumAqueducts, 5)) + ' ' + getIcon(iNumArenas >= 4) + localText.getText("TXT_KEY_VICTORY_NUM_ARENAS", (iNumArenas, 4)) + ' ' + getIcon(iNumForums >= 3) + localText.getText("TXT_KEY_VICTORY_NUM_FORUMS", (iNumForums, 3)))
		elif iGoal == 1:
			iCitiesSpain = getNumCitiesInArea(iRome, Areas.getNormalArea(iSpain, False))
			iCitiesFrance = getNumCitiesInArea(iRome, utils.getPlotList(tFranceTL, Areas.tNormalArea[iFrance][1]))
			iCitiesEngland = getNumCitiesInArea(iRome, Areas.getCoreArea(iEngland, False))
			iCitiesCarthage = getNumCitiesInArea(iRome, utils.getPlotList(tCarthageTL, tCarthageBR))
			iCitiesByzantium = getNumCitiesInArea(iRome, Areas.getCoreArea(iByzantium, False))
			iCitiesEgypt = getNumCitiesInArea(iRome, Areas.getCoreArea(iEgypt, False))
			aHelp.append(getIcon(iCitiesSpain >= 2) + localText.getText("TXT_KEY_VICTORY_ROME_CONTROL_SPAIN", (iCitiesSpain, 2)) + ' ' + getIcon(iCitiesFrance >= 3) + localText.getText("TXT_KEY_VICTORY_ROME_CONTROL_FRANCE", (iCitiesFrance, 3)) + ' ' + getIcon(iCitiesEngland >= 1) + localText.getText("TXT_KEY_VICTORY_ROME_CONTROL_ENGLAND", (iCitiesEngland, 1)))
			aHelp.append(getIcon(iCitiesCarthage >= 2) + localText.getText("TXT_KEY_VICTORY_ROME_CONTROL_CARTHAGE", (iCitiesCarthage, 2)) + ' ' + getIcon(iCitiesByzantium >= 4) + localText.getText("TXT_KEY_VICTORY_ROME_CONTROL_BYZANTIUM", (iCitiesByzantium, 4)) + ' ' + getIcon(iCitiesEgypt >= 2) + localText.getText("TXT_KEY_VICTORY_ROME_CONTROL_EGYPT", (iCitiesEgypt, 2)))
		elif iGoal == 2:
			bArchitecture = data.lFirstDiscovered[iArchitecture] == iRome
			bPolitics = data.lFirstDiscovered[iPolitics] == iRome
			bScholarship = data.lFirstDiscovered[iScholarship] == iRome
			bMachinery = data.lFirstDiscovered[iMachinery] == iRome
			bCivilService = data.lFirstDiscovered[iCivilService] == iRome
			aHelp.append(getIcon(bArchitecture) + localText.getText("TXT_KEY_TECH_ARCHITECTURE", ()) + ' ' + getIcon(bPolitics) + localText.getText("TXT_KEY_TECH_POLITICS", ()) + ' ' + getIcon(bScholarship) + localText.getText("TXT_KEY_TECH_SCHOLARSHIP", ()))
			aHelp.append(getIcon(bMachinery) + localText.getText("TXT_KEY_TECH_MACHINERY", ()) + ' ' + getIcon(bCivilService) + localText.getText("TXT_KEY_TECH_CIVIL_SERVICE", ()))

	# Maya goals have no stages
	elif iPlayer == iMaya:
		if pMaya.isReborn():
			if iGoal == 0:
				bPeru = isAreaFreeOfCivs(utils.getPlotList(tPeruTL, tPeruBR), lCivGroups[0])
				bGranColombia = isAreaFreeOfCivs(utils.getPlotList(tGranColombiaTL, tGranColombiaBR), lCivGroups[0])
				bGuayanas = isAreaFreeOfCivs(utils.getPlotList(tGuayanasTL, tGuayanasBR), lCivGroups[0])
				bCaribbean = isAreaFreeOfCivs(utils.getPlotList(tCaribbeanTL, tCaribbeanBR), lCivGroups[0])
				aHelp.append(getIcon(bPeru) + localText.getText("TXT_KEY_VICTORY_NO_COLONIES_PERU", ()) + ' ' + getIcon(bGranColombia) + localText.getText("TXT_KEY_VICTORY_NO_COLONIES_GRAN_COLOMBIA", ()))
				aHelp.append(getIcon(bGuayanas) + localText.getText("TXT_KEY_VICTORY_NO_COLONIES_GUAYANAS", ()) + ' ' + getIcon(bCaribbean) + localText.getText("TXT_KEY_VICTORY_NO_COLONIES_CARIBBEAN", ()))
			elif iGoal == 1:
				bSouthAmerica = isControlled(iMaya, utils.getPlotList(tSAmericaTL, tSAmericaBR, tSouthAmericaExceptions))
				aHelp.append(getIcon(bSouthAmerica) + localText.getText("TXT_KEY_VICTORY_CONTROL_SOUTH_AMERICA", ()))
			elif iGoal == 2:
				iTradeGold = data.iColombianTradeGold
				aHelp.append(getIcon(iTradeGold >= utils.getTurns(3000)) + localText.getText("TXT_KEY_VICTORY_TRADE_GOLD_RESOURCES", (iTradeGold, utils.getTurns(3000))))

	elif iPlayer == iTamils:
		if iGoal == 0:
			iTreasury = pTamils.getGold()
			iCulture = pTamils.countTotalCulture()
			aHelp.append(getIcon(iTreasury >= utils.getTurns(2000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_GOLD", (iTreasury, utils.getTurns(2000))))
			aHelp.append(getIcon(iCulture >= utils.getTurns(2000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_CULTURE", (iCulture, utils.getTurns(2000))))
		elif iGoal == 1:
			bDeccan = isControlledOrVassalized(iTamils, utils.getPlotList(tDeccanTL, tDeccanBR))
			bSrivijaya = isControlledOrVassalized(iTamils, utils.getPlotList(tSrivijayaTL, tSrivijayaBR))
			aHelp.append(getIcon(bDeccan) + localText.getText("TXT_KEY_VICTORY_DECCAN", ()) + ' ' + getIcon(bSrivijaya) + localText.getText("TXT_KEY_VICTORY_SRIVIJAYA", ()))
		elif iGoal == 2:
			iTradeGold = data.iTamilTradeGold / 100
			aHelp.append(getIcon(iTradeGold >= utils.getTurns(5000)) + localText.getText("TXT_KEY_VICTORY_TRADE_GOLD", (iTradeGold, utils.getTurns(5000))))

	elif iPlayer == iEthiopia:
		if iGoal == 0:
			iNumIncense = pEthiopia.getNumAvailableBonuses(iIncense)
			aHelp.append(getIcon(iNumIncense >= 3) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_INCENSE_RESOURCES", (iNumIncense, 3)))
		elif iGoal == 1:
			bConverted = data.bEthiopiaConverted
			iNumOrthodoxCathedrals = getNumBuildings(iEthiopia, iOrthodoxCathedral)
			iGreatProphets = countSpecialists(iEthiopia, iSpecialistGreatProphet)
			aHelp.append(getIcon(bConverted) + localText.getText("TXT_KEY_VICTORY_CONVERTED_TO_ORTHODOXY", ()))
			aHelp.append(getIcon(iNumOrthodoxCathedrals >= 1) + localText.getText("TXT_KEY_BUILDING_ORTHODOX_CATHEDRAL", ()))
			aHelp.append(getIcon(iGreatProphets >= 3) + localText.getText("TXT_KEY_VICTORY_GREAT_PROPHETS", (iGreatProphets, 3)))
		elif iGoal == 2:
			iOrthodoxCities = countRegionReligion(iOrthodoxy, lAfrica)
			iMuslimCities = countRegionReligion(iIslam, lAfrica)
			aHelp.append(getIcon(iOrthodoxCities > iMuslimCities) + localText.getText("TXT_KEY_VICTORY_ORTHODOX_CITIES", (iOrthodoxCities,)) + ' ' + localText.getText("TXT_KEY_VICTORY_MUSLIM_CITIES", (iMuslimCities,)))

	elif iPlayer == iKorea:
		if iGoal == 0:
			bConfucianCathedral = (getNumBuildings(iKorea, iConfucianCathedral) > 0)
			bBuddhistCathedral = (getNumBuildings(iKorea, iBuddhistCathedral) > 0)
			aHelp.append(getIcon(bBuddhistCathedral) + localText.getText("TXT_KEY_BUILDING_BUDDHIST_CATHEDRAL", ()) + ' ' + getIcon(bConfucianCathedral) + localText.getText("TXT_KEY_BUILDING_CONFUCIAN_CATHEDRAL", ()))
		elif iGoal == 2:
			iNumSinks = data.iKoreanSinks
			aHelp.append(getIcon(iNumSinks >= 20) + localText.getText("TXT_KEY_VICTORY_ENEMY_SHIPS_SUNK", (iNumSinks, 20)))

	elif iPlayer == iByzantium:
		if iGoal == 0:
			iTreasury = pByzantium.getGold()
			aHelp.append(getIcon(iTreasury >= utils.getTurns(5000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_GOLD", (iTreasury, utils.getTurns(5000))))
		elif iGoal == 1:
			pBestPopCity = getBestCity(iByzantium, (68, 45), cityPopulation)
			bBestPopCity = isBestCity(iByzantium, (68, 45), cityPopulation)
			pBestCultureCity = getBestCity(iByzantium, (68, 45), cityCulture)
			bBestCultureCity = isBestCity(iByzantium, (68, 45), cityCulture)
			aHelp.append(getIcon(bBestPopCity) + localText.getText("TXT_KEY_VICTORY_MOST_POPULOUS_CITY", (pBestPopCity.getName(),)) + ' ' + getIcon(bBestCultureCity) + localText.getText("TXT_KEY_VICTORY_MOST_CULTURED_CITY", (pBestCultureCity.getName(),)))
		elif iGoal == 2:
			iBalkans = getNumCitiesInArea(iByzantium, utils.getPlotList(tBalkansTL, tBalkansBR))
			iNorthAfrica = getNumCitiesInArea(iByzantium, utils.getPlotList(tNorthAfricaTL, tNorthAfricaBR))
			iNearEast = getNumCitiesInArea(iByzantium, utils.getPlotList(tNearEastTL, tNearEastBR))
			aHelp.append(getIcon(iBalkans >= 3) + localText.getText("TXT_KEY_VICTORY_BALKANS", (iBalkans, 3)) + ' ' + getIcon(iNorthAfrica >= 3) + localText.getText("TXT_KEY_VICTORY_NORTH_AFRICA", (iNorthAfrica, 3)) + ' ' + getIcon(iNearEast >= 3) + localText.getText("TXT_KEY_VICTORY_NEAR_EAST", (iNearEast, 3)))

	elif iPlayer == iJapan:
		if iGoal == 0:
			iAverageCulture = getAverageCulture(iJapan)
			aHelp.append(getIcon(iAverageCulture >= utils.getTurns(8000)) + localText.getText("TXT_KEY_VICTORY_AVERAGE_CULTURE", (iAverageCulture, utils.getTurns(8000))))
		elif iGoal == 1:
			bKorea = isControlledOrVassalized(iJapan, utils.getPlotList(tKoreaTL, tKoreaBR))
			bManchuria = isControlledOrVassalized(iJapan, utils.getPlotList(tManchuriaTL, tManchuriaBR))
			bChina = isControlledOrVassalized(iJapan, utils.getPlotList(tChinaTL, tChinaBR))
			bIndochina = isControlledOrVassalized(iJapan, utils.getPlotList(tIndochinaTL, tIndochinaBR, tIndochinaExceptions))
			bIndonesia = isControlledOrVassalized(iJapan, utils.getPlotList(tIndonesiaTL, tIndonesiaBR))
			bPhilippines = isControlledOrVassalized(iJapan, utils.getPlotList(tPhilippinesTL, tPhilippinesBR))
			aHelp.append(getIcon(bKorea) + localText.getText("TXT_KEY_CIV_KOREA_SHORT_DESC", ()) + ' ' + getIcon(bManchuria) + localText.getText("TXT_KEY_VICTORY_MANCHURIA", ()) + ' ' + getIcon(bChina) + localText.getText("TXT_KEY_CIV_CHINA_SHORT_DESC", ()))
			aHelp.append(getIcon(bIndochina) + localText.getText("TXT_KEY_VICTORY_INDOCHINA", ()) + ' ' + getIcon(bIndonesia) + localText.getText("TXT_KEY_CIV_INDONESIA_SHORT_DESC", ()) + ' ' + getIcon(bPhilippines) + localText.getText("TXT_KEY_VICTORY_PHILIPPINES", ()))
		elif iGoal == 2:
			iGlobalTechs = countFirstDiscovered(iJapan, iGlobal)
			iDigitalTechs = countFirstDiscovered(iJapan, iDigital)
			aHelp.append(getIcon(iGlobalTechs >= 7) + localText.getText("TXT_KEY_VICTORY_TECHS_FIRST_DISCOVERED", (gc.getEraInfo(iGlobal).getText(), iGlobalTechs, 7)) + ' ' + getIcon(iDigitalTechs >= 7) + localText.getText("TXT_KEY_VICTORY_TECHS_FIRST_DISCOVERED", (gc.getEraInfo(iDigital).getText(), iDigitalTechs, 7)))
			
	elif iPlayer == iVikings:
		if iGoal == 1:
			lEuroCivs = [iLoopPlayer for iLoopPlayer in lCivGroups[0] if tBirth[iLoopPlayer] < 1050 and iLoopPlayer != iPlayer]
			bEuropeanCore = isCoreControlled(iVikings, lEuroCivs)
			aHelp.append(getIcon(bEuropeanCore) + localText.getText("TXT_KEY_VICTORY_EUROPEAN_CORE", ()))
		elif iGoal == 2:
			iRaidGold = data.iVikingGold
			aHelp.append(getIcon(iRaidGold >= utils.getTurns(2000)) + localText.getText("TXT_KEY_VICTORY_ACQUIRED_GOLD", (iRaidGold, utils.getTurns(2000))))
			
	elif iPlayer == iTurks:
		if iGoal == 0:
			fLandPercent = getLandPercent(iTurks)
			iPillagedImprovements = data.iTurkicPillages
			aHelp.append(getIcon(fLandPercent >= 5.995) + localText.getText("TXT_KEY_VICTORY_PERCENTAGE_WORLD_TERRITORY", (str(u"%.2f%%" % fLandPercent), str(6))))
			aHelp.append(getIcon(iPillagedImprovements >= 20) + localText.getText("TXT_KEY_VICTORY_PILLAGED_IMPROVEMENTS", (iPillagedImprovements, 20)))
		elif iGoal == 1:
			bConnected = isConnectedByTradeRoute(iTurks, utils.getPlotList(tChinaTL, tChinaBR), lMediterraneanPorts)
			iSilkRouteCities = pTurks.countCorporations(iSilkRoute)
			aHelp.append(getIcon(bConnected) + localText.getText("TXT_KEY_VICTORY_SILK_ROUTE_CONNECTION", ()))
			aHelp.append(getIcon(iSilkRouteCities >= 10) + localText.getText("TXT_KEY_VICTORY_CITIES_WITH_SILK_ROUTE", (iSilkRouteCities, 10)))
		elif iGoal == 2:
			iCultureLevel = 3
			for tCapital in [data.tFirstTurkicCapital, data.tSecondTurkicCapital]:
				if tCapital:
					iCultureLevel += 1
					capitalPlot = utils.plot(tCapital)
					if capitalPlot.isCity():
						name = capitalPlot.getPlotCity().getName()
						ownName = cnm.getRenameName(iTurks, name)
						if ownName: name = ownName
						aHelp.append(getIcon(True) + name)
			
			if pTurks.getNumCities() > 0:
				capital = pTurks.getCapitalCity()
				iCulture = capital.getCulture(iTurks)
				iRequiredCulture = gc.getCultureLevelInfo(iCultureLevel).getSpeedThreshold(gc.getGame().getGameSpeedType())
			
				if (capital.getX(), capital.getY()) in [data.tFirstTurkicCapital, data.tSecondTurkicCapital]:
					aHelp.append(getIcon(False) + localText.getText("TXT_KEY_VICTORY_NO_NEW_CAPITAL", ()))
				else:
					aHelp.append(getIcon(iCulture >= iRequiredCulture) + localText.getText("TXT_KEY_VICTORY_CAPITAL_CULTURE", (capital.getName(), iCulture, iRequiredCulture)))

	elif iPlayer == iArabia:
		if iGoal == 0:
			iMostAdvancedCiv = getBestPlayer(iArabia, playerTechs)
			aHelp.append(getIcon(iMostAdvancedCiv == iArabia) + localText.getText("TXT_KEY_VICTORY_MOST_ADVANCED_CIV", (str(gc.getPlayer(iMostAdvancedCiv).getCivilizationShortDescriptionKey()),)))
		elif iGoal == 1:
			bEgypt = isControlledOrVassalized(iArabia, Areas.getCoreArea(iEgypt, False))
			bMaghreb = isControlledOrVassalized(iArabia, utils.getPlotList(tCarthageTL, tCarthageBR))
			bMesopotamia = isControlledOrVassalized(iArabia, Areas.getCoreArea(iBabylonia, False))
			bPersia = isControlledOrVassalized(iArabia, Areas.getCoreArea(iPersia, False))
			bSpain = isControlledOrVassalized(iArabia, Areas.getNormalArea(iSpain, False))
			aHelp.append(getIcon(bEgypt) + localText.getText("TXT_KEY_CIV_EGYPT_SHORT_DESC", ()) + ' ' + getIcon(bMaghreb) + localText.getText("TXT_KEY_VICTORY_MAGHREB", ()) + ' ' + getIcon(bSpain) + localText.getText("TXT_KEY_CIV_SPAIN_SHORT_DESC", ()))
			aHelp.append(getIcon(bMesopotamia) + localText.getText("TXT_KEY_VICTORY_MESOPOTAMIA", ()) + ' ' + getIcon(bPersia) + localText.getText("TXT_KEY_CIV_PERSIA_SHORT_DESC", ()))
		elif iGoal == 2:
			fReligionPercent = gc.getGame().calculateReligionPercent(iIslam)
			aHelp.append(getIcon(fReligionPercent >= 30.0) + localText.getText("TXT_KEY_VICTORY_SPREAD_RELIGION_PERCENT", (gc.getReligionInfo(iIslam).getTextKey(), str(u"%.2f%%" % fReligionPercent), str(30))))

	elif iPlayer == iTibet:
		if iGoal == 0:
			iNumCities = pTibet.getNumCities()
			aHelp.append(getIcon(iNumCities >= 4) + localText.getText("TXT_KEY_VICTORY_CITIES_ACQUIRED", (iNumCities, 4)))
		elif iGoal == 1:
			fReligionPercent = gc.getGame().calculateReligionPercent(iBuddhism)
			aHelp.append(getIcon(fReligionPercent >= 30.0) + localText.getText("TXT_KEY_VICTORY_SPREAD_RELIGION_PERCENT", (gc.getReligionInfo(iBuddhism).getTextKey(), str(u"%.2f%%" % fReligionPercent), str(30))))
		elif iGoal == 2:
			iCounter = countCitySpecialists(iTibet, Areas.getCapital(iTibet), iSpecialistGreatProphet)
			aHelp.append(getIcon(iCounter >= 5) + localText.getText("TXT_KEY_VICTORY_GREAT_PROPHETS_SETTLED", ("Lhasa", iCounter, 5)))

	elif iPlayer == iIndonesia:
		if iGoal == 0:
			iHighestCiv = getBestPlayer(iIndonesia, playerRealPopulation)
			bHighest = (iHighestCiv == iIndonesia)
			aHelp.append(getIcon(bHighest) + localText.getText("TXT_KEY_VICTORY_HIGHEST_POPULATION_CIV", ()) + localText.getText(str(gc.getPlayer(iHighestCiv).getCivilizationShortDescriptionKey()),()))
		elif iGoal == 1:
			iCounter = countHappinessResources(iIndonesia)
			aHelp.append(getIcon(iCounter >= 10) + localText.getText("TXT_KEY_VICTORY_NUM_HAPPINESS_RESOURCES", (iCounter, 10)))
		elif iGoal == 2:
			popPercent = getPopulationPercent(iIndonesia)
			aHelp.append(getIcon(popPercent >= 9.0) + localText.getText("TXT_KEY_VICTORY_PERCENTAGE_WORLD_POPULATION", (str(u"%.2f%%" % popPercent), str(9))))

	elif iPlayer == iMoors:
		if iGoal == 0:
			bMezquita = data.getWonderBuilder(iMezquita) == iMoors
			iCounter = 0
			iCounter += countCitySpecialists(iMoors, (51, 41), iSpecialistGreatProphet)
			iCounter += countCitySpecialists(iMoors, (51, 41), iSpecialistGreatScientist)
			iCounter += countCitySpecialists(iMoors, (51, 41), iSpecialistGreatEngineer)
			aHelp.append(getIcon(bMezquita) + localText.getText("TXT_KEY_BUILDING_LA_MEZQUITA", ()) + ' ' + getIcon(iCounter >= 3) + localText.getText("TXT_KEY_VICTORY_GREAT_PEOPLE_IN_CITY_MOORS", ("Cordoba", iCounter, 3)))
		elif iGoal == 1:
			iIberia = getNumConqueredCitiesInArea(iMoors, utils.getPlotList(tIberiaTL, tIberiaBR))
			iMaghreb = getNumCitiesInArea(iMoors, utils.getPlotList(tMaghrebTL, tMaghrebBR))
			iWestAfrica = getNumConqueredCitiesInArea(iMoors, utils.getPlotList(tWestAfricaTL, tWestAfricaBR))
			aHelp.append(getIcon(iMaghreb >= 3) + localText.getText("TXT_KEY_VICTORY_MAGHREB_MOORS", (iMaghreb, 3)) + ' ' + getIcon(iIberia >= 2) + localText.getText("TXT_KEY_VICTORY_IBERIA", (iIberia, 2)) + ' ' + getIcon(iWestAfrica >= 2) + localText.getText("TXT_KEY_VICTORY_WEST_AFRICA", (iWestAfrica, 2)))
		elif iGoal == 2:
			iRaidGold = data.iMoorishGold
			aHelp.append(getIcon(iRaidGold >= utils.getTurns(3000)) + localText.getText("TXT_KEY_VICTORY_PIRACY", (iRaidGold, utils.getTurns(3000))))

	elif iPlayer == iSpain:
		if iGoal == 1:
			iNumGold = countResources(iSpain, iGold)
			iNumSilver = countResources(iSpain, iSilver)
			aHelp.append(getIcon(iNumGold + iNumSilver >= 10) + localText.getText("TXT_KEY_VICTORY_GOLD_SILVER_RESOURCES", (iNumGold + iNumSilver, 10)))
		elif iGoal == 2:
			fReligionPercent = gc.getGame().calculateReligionPercent(iCatholicism)
			bNoProtestants = not isStateReligionInArea(iProtestantism, tEuropeTL, tEuropeBR) and not isStateReligionInArea(iProtestantism, tEasternEuropeTL, tEasternEuropeBR)
			aHelp.append(getIcon(fReligionPercent >= 30.0) + localText.getText("TXT_KEY_VICTORY_SPREAD_RELIGION_PERCENT", (gc.getReligionInfo(iCatholicism).getTextKey(), str(u"%.2f%%" % fReligionPercent), str(30))) + ' ' + getIcon(bNoProtestants) + localText.getText("TXT_KEY_VICTORY_NO_PROTESTANTS", ()))

	elif iPlayer == iFrance:
		if iGoal == 0:
			iCulture = getCityCulture(iFrance, (55, 50))
			aHelp.append(getIcon(iCulture >= utils.getTurns(40000)) + localText.getText("TXT_KEY_VICTORY_CITY_CULTURE", ("Paris", iCulture, utils.getTurns(40000))))
		elif iGoal == 1:
			iEurope, iTotalEurope = countControlledTiles(iFrance, tEuropeTL, tEuropeBR, True)
			iEasternEurope, iTotalEasternEurope = countControlledTiles(iFrance, tEasternEuropeTL, tEasternEuropeBR, True)
			iNorthAmerica, iTotalNorthAmerica = countControlledTiles(iFrance, tNorthAmericaTL, tNorthAmericaBR, True)
			fEurope = (iEurope + iEasternEurope) * 100.0 / (iTotalEurope + iTotalEasternEurope)
			fNorthAmerica = iNorthAmerica * 100.0 / iTotalNorthAmerica
			aHelp.append(getIcon(fEurope >= 40.0) + localText.getText("TXT_KEY_VICTORY_EUROPEAN_TERRITORY", (str(u"%.2f%%" % fEurope), str(40))) + ' ' + getIcon(fNorthAmerica >= 40.0) + localText.getText("TXT_KEY_VICTORY_NORTH_AMERICAN_TERRITORY", (str(u"%.2f%%" % fNorthAmerica), str(40))))
		elif iGoal == 2:
			bNotreDame = data.getWonderBuilder(iNotreDame) == iFrance
			bVersailles = data.getWonderBuilder(iVersailles) == iFrance
			bLouvre = data.getWonderBuilder(iLouvre) == iFrance
			bEiffelTower = data.getWonderBuilder(iEiffelTower) == iFrance
			bMetropolitain = data.getWonderBuilder(iMetropolitain) == iFrance
			aHelp.append(getIcon(bNotreDame) + localText.getText("TXT_KEY_BUILDING_NOTRE_DAME", ()) + ' ' + getIcon(bVersailles) + localText.getText("TXT_KEY_BUILDING_VERSAILLES", ()) + ' ' + getIcon(bLouvre) + localText.getText("TXT_KEY_BUILDING_LOUVRE", ()))
			aHelp.append(getIcon(bEiffelTower) + localText.getText("TXT_KEY_BUILDING_EIFFEL_TOWER", ()) + ' ' + getIcon(bMetropolitain) + localText.getText("TXT_KEY_BUILDING_METROPOLITAIN", ()))

	elif iPlayer == iKhmer:
		if iGoal == 0:
			iNumBuddhism = getNumBuildings(iKhmer, iBuddhistMonastery)
			iNumHinduism = getNumBuildings(iKhmer, iHinduMonastery)
			bWatPreahPisnulok = data.getWonderBuilder(iWatPreahPisnulok) == iKhmer
			aHelp.append(getIcon(iNumBuddhism >= 4) + localText.getText("TXT_KEY_VICTORY_NUM_BUDDHIST_MONASTERIES", (iNumBuddhism, 4)) + ' ' + getIcon(iNumHinduism >= 4) + localText.getText("TXT_KEY_VICTORY_NUM_HINDU_MONASTERIES", (iNumHinduism, 4)) + ' ' + getIcon(bWatPreahPisnulok) + localText.getText("TXT_KEY_BUILDING_WAT_PREAH_PISNULOK", ()))
		elif iGoal == 1:
			fPopPerCity = getAverageCitySize(iKhmer)
			aHelp.append(getIcon(fPopPerCity >= 10.0) + localText.getText("TXT_KEY_VICTORY_AVERAGE_CITY_POPULATION", (str(u"%.2f" % fPopPerCity), str(10))))
		elif iGoal == 2:
			iCulture = pKhmer.countTotalCulture()
			aHelp.append(getIcon(iCulture >= utils.getTurns(7000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_CULTURE", (iCulture, utils.getTurns(7000))))

	elif iPlayer == iEngland:
		if iGoal == 0:
			iEnglishNavy = 0
			iEnglishNavy += pEngland.getUnitClassCount(gc.getUnitInfo(iFrigate).getUnitClassType())
			iEnglishNavy += pEngland.getUnitClassCount(gc.getUnitInfo(iShipOfTheLine).getUnitClassType())
			iEnglishSinks = data.iEnglishSinks
			aHelp.append(getIcon(iEnglishNavy >= 25) + localText.getText("TXT_KEY_VICTORY_NAVY_SIZE", (iEnglishNavy, 25)) + ' ' + getIcon(iEnglishSinks >= 50) + localText.getText("TXT_KEY_VICTORY_ENEMY_SHIPS_SUNK", (iEnglishSinks, 50)))
		elif iGoal == 1:
			iIndustrialTechs = countFirstDiscovered(iEngland, iIndustrial)
			aHelp.append(getIcon(iIndustrialTechs >= 10) + localText.getText("TXT_KEY_VICTORY_TECHS_FIRST_DISCOVERED", (gc.getEraInfo(iIndustrial).getText(), iIndustrialTechs, 10)))
		elif iGoal == 2:
			iNAmerica = getNumCitiesInArea(iEngland, utils.getPlotList(tNorthAmericaTL, tNorthAmericaBR))
			iSCAmerica = getNumCitiesInArea(iEngland, utils.getPlotList(tSouthCentralAmericaTL, tSouthCentralAmericaBR))
			iAfrica = getNumCitiesInArea(iEngland, utils.getPlotList(tAfricaTL, tAfricaBR))
			iAsia = getNumCitiesInArea(iEngland, utils.getPlotList(tAsiaTL, tAsiaBR))
			iOceania = getNumCitiesInArea(iEngland, utils.getPlotList(tOceaniaTL, tOceaniaBR))
			aHelp.append(getIcon(iNAmerica >= 5) + localText.getText("TXT_KEY_VICTORY_ENGLAND_CONTROL_NORTH_AMERICA", (iNAmerica, 5)) + ' ' + getIcon(iAsia >= 6) + localText.getText("TXT_KEY_VICTORY_ENGLAND_CONTROL_ASIA", (iAsia, 6)) + ' ' + getIcon(iAfrica >= 4) + localText.getText("TXT_KEY_VICTORY_ENGLAND_CONTROL_AFRICA", (iAfrica, 4)))
			aHelp.append(getIcon(iSCAmerica >= 3) + localText.getText("TXT_KEY_VICTORY_ENGLAND_CONTROL_SOUTH_AMERICA", (iSCAmerica, 3)))
			aHelp.append(getIcon(iOceania >= 6) + localText.getText("TXT_KEY_VICTORY_ENGLAND_CONTROL_OCEANIA", (iOceania, 6)))
			iSunset = iNAmerica + iSCAmerica + iAfrica + iAsia + iOceania
			bSunset = ((iNAmerica >= 5) and (iSCAmerica >= 3) and (iOceania >= 6) and (iAfrica) >= 4 and (iAsia >= 6)) or gc.getGame().getGameTurn() >= getTurnForYear(1860)
			sunsetText = localText.getText("TXT_KEY_VICTORY_ENGLAND_SUNSET", (iSunset, 24))
			if bSunset: sunsetText = localText.getText("TXT_KEY_VICTORY_ENGLAND_SUNSET_COMPLETE", ())
			bCapetoCairo = isConnectedByRailroad(iEngland, (63, 10), lNorthernEgypt)
			aHelp.append(getIcon(bSunset) + sunsetText + ' ' + getIcon(bCapetoCairo) + localText.getText("TXT_KEY_VICTORY_CAPE_TO_CAIRO_RAILWAY", ()))

	elif iPlayer == iHolyRome:
		if iGoal == 0:
			bSaintPeters = data.lHolyRomanShrines[0] or getNumBuildings(iHolyRome, iCatholicShrine) > 0
			bAnastasis = data.lHolyRomanShrines[1] or getNumBuildings(iHolyRome, iOrthodoxShrine) > 0
			bAllSaints = data.lHolyRomanShrines[2] or getNumBuildings(iHolyRome, iProtestantShrine) > 0
			aHelp.append(getIcon(bSaintPeters) + localText.getText("TXT_KEY_BUILDING_CATHOLIC_SHRINE", ()) + ' ' + getIcon(bAnastasis) + localText.getText("TXT_KEY_BUILDING_ORTHODOX_SHRINE", ()) + ' ' + getIcon(bAllSaints) + localText.getText("TXT_KEY_BUILDING_PROTESTANT_SHRINE", ()))
		elif iGoal == 1:
			iNumVassals = countVassals(iHolyRome, lCivGroups[0], iCatholicism)
			aHelp.append(getIcon(iNumVassals >= 3) + localText.getText("TXT_KEY_VICTORY_CATHOLIC_EUROPEAN_VASSALS", (iNumVassals, 3)))
		elif iGoal == 2:
			iGreatArtists = countCitySpecialists(iHolyRome, tVienna, iSpecialistGreatArtist)
			iGreatStatesmen = countCitySpecialists(iHolyRome, tVienna, iSpecialistGreatStatesman)
			iPleasedOrBetterEuropeans = countPlayersWithAttitudeInGroup(iHolyRome, AttitudeTypes.ATTITUDE_PLEASED, lCivGroups[0])
			aHelp.append(getIcon(iGreatArtists + iGreatStatesmen >= 10) + localText.getText("TXT_KEY_VICTORY_GREAT_ARTISTS_AND_STATESMEN_SETTLED", ('Vienna', iGreatArtists + iGreatStatesmen, 10)))
			aHelp.append(getIcon(iPleasedOrBetterEuropeans >= 8) + localText.getText("TXT_KEY_VICTORY_PLEASED_OR_FRIENDLY_EUROPEANS", (iPleasedOrBetterEuropeans, 8)))

	elif iPlayer == iRussia:
		if iGoal == 0:
			iSiberia = getNumFoundedCitiesInArea(iRussia, utils.getPlotList(tSiberiaTL, tSiberiaBR))
			bSiberia = (iSiberia >= 8) or gc.getGame().getGameTurn() >= getTurnForYear(1720)
			siberiaText = localText.getText("TXT_KEY_VICTORY_RUSSIA_CONTROL_SIBERIA", (iSiberia, 8))
			if bSiberia: siberiaText = localText.getText("TXT_KEY_VICTORY_RUSSIA_CONTROL_SIBERIA_COMPLETE", ()) 
			bSiberianRailway = isConnectedByRailroad(iRussia, Areas.getCapital(iRussia), lSiberianCoast)
			aHelp.append(getIcon(bSiberia) + siberiaText + ' ' + getIcon(bSiberianRailway) + localText.getText("TXT_KEY_VICTORY_TRANSSIBERIAN_RAILWAY", ()))
		elif iGoal == 1:
			bManhattanProject = teamRussia.getProjectCount(iManhattanProject) > 0
			bApolloProgram = teamRussia.getProjectCount(iLunarLanding) > 0
			aHelp.append(getIcon(bManhattanProject) + localText.getText("TXT_KEY_PROJECT_MANHATTAN_PROJECT", ()) + ' ' + getIcon(bApolloProgram) + localText.getText("TXT_KEY_PROJECT_LUNAR_LANDING", ()))
		elif iGoal == 2:
			bCommunism = dc.isCommunist(iPlayer)
			iCount = countPlayersWithAttitudeAndCriteria(iPlayer, AttitudeTypes.ATTITUDE_FRIENDLY, dc.isCommunist)
			aHelp.append(getIcon(bCommunism) + localText.getText("TXT_KEY_VICTORY_COMMUNISM", ()) + ' ' + getIcon(iCount >= 6) + localText.getText("TXT_KEY_VICTORY_FRIENDLY_WITH_COMMUNISM", (iCount, 6)))

	elif iPlayer == iMali:
		if iGoal == 1:
			bSankore = False
			iProphets = 0
			for city in utils.getCityList(iMali):
				if city.isHasRealBuilding(iUniversityOfSankore):
					bSankore = True
					iProphets = city.getFreeSpecialistCount(iSpecialistGreatProphet)
					break
			aHelp.append(getIcon(bSankore) + localText.getText("TXT_KEY_BUILDING_UNIVERSITY_OF_SANKORE", ()) + ' ' + getIcon(iProphets >= 1) + localText.getText("TXT_KEY_VICTORY_SANKORE_PROPHETS", (iProphets, 1)))
		elif iGoal == 2:
			iTreasury = pMali.getGold()
			iThreshold = 5000
			if gc.getGame().getGameTurn() > getTurnForYear(1500): iThreshold = 10000
			aHelp.append(getIcon(iTreasury >= utils.getTurns(iThreshold)) + localText.getText("TXT_KEY_VICTORY_TOTAL_GOLD", (iTreasury, utils.getTurns(iThreshold))))
		
	elif iPlayer == iPoland:
		if iGoal == 1:
			lCities = getLargestCities(iPlayer, 4)
			bCity1 = len(lCities) > 0
			bCity2 = len(lCities) > 1
			bCity3 = len(lCities) > 2
			bCity4 = len(lCities) > 3
			if not bCity1: aHelp.append(getIcon(False) + localText.getText("TXT_KEY_VICTORY_NO_CITIES", ()))
			if bCity1: aHelp.append(getIcon(lCities[0].getPopulation() >= 14) + localText.getText("TXT_KEY_VICTORY_CITY_SIZE", (lCities[0].getName(), lCities[0].getPopulation(), 14)))
			if bCity2: aHelp.append(getIcon(lCities[1].getPopulation() >= 14) + localText.getText("TXT_KEY_VICTORY_CITY_SIZE", (lCities[1].getName(), lCities[1].getPopulation(), 14)))
			if bCity3: aHelp.append(getIcon(lCities[2].getPopulation() >= 14) + localText.getText("TXT_KEY_VICTORY_CITY_SIZE", (lCities[2].getName(), lCities[2].getPopulation(), 14)))
			if bCity4: aHelp.append(getIcon(lCities[3].getPopulation() >= 14) + localText.getText("TXT_KEY_VICTORY_CITY_SIZE", (lCities[3].getName(), lCities[3].getPopulation(), 14)))
		elif iGoal == 2:
			iCatholic = getNumBuildings(iPoland, iCatholicCathedral)
			iOrthodox = getNumBuildings(iPoland, iOrthodoxCathedral)
			iProtestant = getNumBuildings(iPoland, iProtestantCathedral)
			iCathedrals = iCatholic + iOrthodox + iProtestant
			aHelp.append(getIcon(iCathedrals >= 3) + localText.getText("TXT_KEY_VICTORY_CHRISTIAN_CATHEDRALS", (iCathedrals, 3)))

	elif iPlayer == iPortugal:
		if iGoal == 0:
			iCount = countOpenBorders(iPortugal)
			aHelp.append(getIcon(iCount >= 15) + localText.getText("TXT_KEY_VICTORY_OPEN_BORDERS", (iCount, 15)))
		elif iGoal == 1:
			iCount = countAcquiredResources(iPortugal, lColonialResources)
			aHelp.append(getIcon(iCount >= 12) + localText.getText("TXT_KEY_VICTORY_COLONIAL_RESOURCES", (iCount, 12)))
		elif iGoal == 2:
			iColonies = getNumCitiesInArea(iPortugal, utils.getPlotList(tBrazilTL, tBrazilBR))
			iColonies += getNumCitiesInRegions(iPortugal, lAfrica)
			iColonies += getNumCitiesInRegions(iPortugal, lAsia)
			aHelp.append(getIcon(iColonies >= 20) + localText.getText("TXT_KEY_VICTORY_EXTRA_EUROPEAN_COLONIES", (iColonies, 20)))

	elif iPlayer == iInca:
		if iGoal == 0:
			bRoad = isRoad(iInca, lAndeanCoast)
			iTambos = getNumBuildings(iInca, iTambo)
			aHelp.append(getIcon(bRoad) + localText.getText("TXT_KEY_VICTORY_ANDEAN_ROAD", ()) + ' ' + getIcon(iTambos >= 5) + localText.getText("TXT_KEY_VICTORY_NUM_TAMBOS", (iTambos, 5)))
		elif iGoal == 1:
			iTreasury = pInca.getGold()
			aHelp.append(getIcon(iTreasury >= utils.getTurns(2500)) + localText.getText("TXT_KEY_VICTORY_TOTAL_GOLD", (iTreasury, utils.getTurns(2500))))
		elif iGoal == 2:
			bSouthAmerica = isAreaOnlyCivs(tSAmericaTL, tSAmericaBR, [iInca])
			aHelp.append(getIcon(bSouthAmerica) + localText.getText("TXT_KEY_VICTORY_NO_FOREIGN_CITIES_SOUTH_AMERICA", ()))

	elif iPlayer == iItaly:
		if iGoal == 0:
			bSanMarcoBasilica = data.getWonderBuilder(iSanMarcoBasilica) == iItaly
			bSistineChapel = data.getWonderBuilder(iSistineChapel) == iItaly
			bSantaMariaDelFiore = data.getWonderBuilder(iSantaMariaDelFiore) == iItaly
			aHelp.append(getIcon(bSanMarcoBasilica) + localText.getText("TXT_KEY_BUILDING_SAN_MARCO_BASILICA", ()) + ' ' + getIcon(bSistineChapel) + localText.getText("TXT_KEY_BUILDING_SISTINE_CHAPEL", ()) + ' ' + getIcon(bSantaMariaDelFiore) + localText.getText("TXT_KEY_BUILDING_SANTA_MARIA_DEL_FIORE", ()))
		elif iGoal == 1:
			iCount = countCitiesWithCultureLevel(iItaly, 5)
			aHelp.append(getIcon(iCount >= 3) + localText.getText("TXT_KEY_VICTORY_NUM_CITIES_INFLUENTIAL_CULTURE", (iCount, 3)))
		elif iGoal == 2:
			iMediterranean, iTotalMediterranean = countControlledTiles(iItaly, tMediterraneanTL, tMediterraneanBR, False, tMediterraneanExceptions, True)
			fMediterranean = iMediterranean * 100.0 / iTotalMediterranean
			aHelp.append(getIcon(fMediterranean >= 75.0) + localText.getText("TXT_KEY_VICTORY_MEDITERRANEAN_TERRITORY", (str(u"%.2f%%" % fMediterranean), str(75))))

	elif iPlayer == iMongolia:
		if iGoal == 1:
			iRazedCities = data.iMongolRazes
			aHelp.append(getIcon(iRazedCities >= 7) + localText.getText("TXT_KEY_VICTORY_NUM_CITIES_RAZED", (iRazedCities, 7)))
		elif iGoal == 2:
			landPercent = getLandPercent(iMongolia)
			aHelp.append(getIcon(landPercent >= 9.995) + localText.getText("TXT_KEY_VICTORY_PERCENTAGE_WORLD_TERRITORY", (str(u"%.2f%%" % landPercent), str(10))))

	elif iPlayer == iMughals:
		if iGoal == 0:
			iNumMosques = getNumBuildings(iMughals, iIslamicCathedral)
			iNumHindu = getNumBuildings(iMughals, iHinduCathedral)
			aHelp.append(getIcon(iNumMosques >= 3) + localText.getText("TXT_KEY_VICTORY_MOSQUES_BUILT", (iNumMosques, 3)) + ' ' + getIcon(iNumHindu >= 1) + localText.getText("TXT_KEY_VICTORY_HINDU_BUILT", (iNumHindu, 1)))
		elif iGoal == 1:
			bRedFort = data.getWonderBuilder(iRedFort) == iMughals
			bShalimarGardens = data.getWonderBuilder(iShalimarGardens) == iMughals
			bTajMahal = data.getWonderBuilder(iTajMahal) == iMughals
			aHelp.append(getIcon(bRedFort) + localText.getText("TXT_KEY_BUILDING_RED_FORT", ()) + ' ' + getIcon(bShalimarGardens) + localText.getText("TXT_KEY_BUILDING_SHALIMAR_GARDENS", ()) + ' ' + getIcon(bTajMahal) + localText.getText("TXT_KEY_BUILDING_TAJ_MAHAL", ()))
		elif iGoal == 2:
			iCulture = pMughals.countTotalCulture()
			aHelp.append(getIcon(iCulture >= utils.getTurns(50000)) + localText.getText("TXT_KEY_VICTORY_TOTAL_CULTURE", (iCulture, utils.getTurns(50000))))

	elif iPlayer == iAztecs:
		if not pAztecs.isReborn():
			if iGoal == 0:
				pBestCity = getBestCity(iAztecs, (18, 37), cityPopulation)
				bBestCity = isBestCity(iAztecs, (18, 37), cityPopulation)
				aHelp.append(getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_MOST_POPULOUS_CITY", (pBestCity.getName(),)))
			elif iGoal == 1:
				iStepPyramids = getNumBuildings(iAztecs, utils.getUniqueBuilding(iAztecs, iPaganTemple))
				iAltars = getNumBuildings(iAztecs, iSacrificialAltar)
				aHelp.append(getIcon(iStepPyramids >= 5) + localText.getText("TXT_KEY_VICTORY_NUM_STEP_PYRAMIDS", (iStepPyramids, 5)) + " " + getIcon(iAltars >= 5) + localText.getText("TXT_KEY_VICTORY_NUM_ALTARS", (iAltars, 5)))
			elif iGoal == 2:
				iEnslavedUnits = data.iAztecSlaves
				aHelp.append(getIcon(iEnslavedUnits >= 20) + localText.getText("TXT_KEY_VICTORY_ENSLAVED_UNITS", (iEnslavedUnits, 20)))
		else:
			if iGoal == 0:
				iNumCathedrals = 0
				iStateReligion = pAztecs.getStateReligion()
				if iStateReligion >= 0:
					iStateReligionCathedral = iCathedral + 4*iStateReligion
					iNumCathedrals = getNumBuildings(iAztecs, iStateReligionCathedral)
				aHelp.append(getIcon(iNumCathedrals >= 3) + localText.getText("TXT_KEY_VICTORY_STATE_RELIGION_CATHEDRALS", (gc.getReligionInfo(iStateReligion).getAdjectiveKey(), iNumCathedrals, 3)))
			elif iGoal == 1:
				iGenerals = data.iMexicanGreatGenerals
				aHelp.append(getIcon(iGenerals >= 3) + localText.getText("TXT_KEY_VICTORY_GREAT_GENERALS", (iGenerals, 3)))
			elif iGoal == 2:
				pBestCity = getBestCity(iAztecs, (18, 37), cityPopulation)
				bBestCity = isBestCity(iAztecs, (18, 37), cityPopulation)
				aHelp.append(getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_MOST_POPULOUS_CITY", (pBestCity.getName(),)))

	elif iPlayer == iOttomans:
		if iGoal == 0:
			capital = pOttomans.getCapitalCity()
			iCounter = countCityWonders(iOttomans, (capital.getX(), capital.getY()), False)
			aHelp.append(getIcon(iCounter >= 4) + localText.getText("TXT_KEY_VICTORY_NUM_WONDERS_CAPITAL", (iCounter, 4)))
		elif iGoal == 1:
			bEasternMediterranean = isCultureControlled(iOttomans, lEasternMediterranean)
			bBlackSea = isCultureControlled(iOttomans, lBlackSea)
			bCairo = controlsCity(iOttomans, tCairo)
			bMecca = controlsCity(iOttomans, tMecca)
			bBaghdad = controlsCity(iOttomans, tBaghdad)
			bVienna = controlsCity(iOttomans, tVienna)
			aHelp.append(getIcon(bEasternMediterranean) + localText.getText("TXT_KEY_VICTORY_EASTERN_MEDITERRANEAN", ()) + ' ' + getIcon(bBlackSea) + localText.getText("TXT_KEY_VICTORY_BLACK_SEA", ()))
			aHelp.append(getIcon(bCairo) + localText.getText("TXT_KEY_VICTORY_CAIRO", ()) + ' ' + getIcon(bMecca) + localText.getText("TXT_KEY_VICTORY_MECCA", ()) + ' ' + getIcon(bBaghdad) + localText.getText("TXT_KEY_VICTORY_BAGHDAD", ()) + ' ' + getIcon(bVienna) + localText.getText("TXT_KEY_VICTORY_VIENNA", ()))
		elif iGoal == 2:
			iOttomanCulture = pOttomans.countTotalCulture()
			iEuropeanCulture = getTotalCulture(lCivGroups[0])
			aHelp.append(getIcon(iOttomanCulture > iEuropeanCulture) + localText.getText("TXT_KEY_VICTORY_TOTAL_CULTURE", (iOttomanCulture, iEuropeanCulture)))

	elif iPlayer == iThailand:
		if iGoal == 0:
			iCount = countOpenBorders(iThailand)
			aHelp.append(getIcon(iCount >= 12) + localText.getText("TXT_KEY_VICTORY_OPEN_BORDERS", (iCount, 12)))
		elif iGoal == 1:
			pBestCity = getBestCity(iThailand, (101, 33), cityPopulation)
			bBestCity = isBestCity(iThailand, (101, 33), cityPopulation)
			if not bBestCity:
				pBestCity = getBestCity(iThailand, (102, 33), cityPopulation)
				bBestCity = isBestCity(iThailand, (102, 33), cityPopulation)
			aHelp.append(getIcon(bBestCity) + localText.getText("TXT_KEY_VICTORY_MOST_POPULOUS_CITY", (pBestCity.getName(),)))
		elif iGoal == 2:
			bSouthAsia = isAreaOnlyCivs(tSouthAsiaTL, tSouthAsiaBR, lSouthAsianCivs)
			aHelp.append(getIcon(bSouthAsia) + localText.getText("TXT_KEY_VICTORY_NO_SOUTH_ASIAN_COLONIES", ()))

	elif iPlayer == iCongo:
		if iGoal == 0:
			fPercent = getApostolicVotePercent(iCongo)
			aHelp.append(getIcon(fPercent >= 15.0) + localText.getText("TXT_KEY_VICTORY_APOSTOLIC_VOTE_PERCENT", (str(u"%.2f%%" % fPercent), str(15))))
		elif iGoal == 1:
			iSlaves = data.iCongoSlaveCounter
			aHelp.append(getIcon(iSlaves >= utils.getTurns(1000)) + localText.getText("TXT_KEY_VICTORY_SLAVES_TRADED", (iSlaves, utils.getTurns(1000))))

	elif iPlayer == iNetherlands:
		if iGoal == 0:
			bStockExchange = False
			capital = pNetherlands.getCapitalCity()
			if capital: bStockExchange = capital.isHasRealBuilding(iStockExchange)
			iCounter = 0
			iCounter += countCitySpecialists(iNetherlands, Areas.getCapital(iNetherlands), iSpecialistGreatMerchant)
			iCounter += countCitySpecialists(iNetherlands, Areas.getCapital(iNetherlands), iSpecialistGreatScientist)
			iCounter += countCitySpecialists(iNetherlands, Areas.getCapital(iNetherlands), iSpecialistGreatArtist)
			aHelp.append(getIcon(bStockExchange) + localText.getText("TXT_KEY_VICTORY_STOCK_EXCHANGE", ()) + ' ' + getIcon(iCounter >= 4) + localText.getText("TXT_KEY_VICTORY_GREAT_PEOPLE_IN_CITY_DUTCH", ("Amsterdam", iCounter, 4)))
		elif iGoal == 1:
			iColonies = data.iDutchColonies
			aHelp.append(getIcon(iColonies >= 5) + localText.getText("TXT_KEY_VICTORY_EUROPEAN_COLONIES_CONQUERED", (iColonies, 5)))
		elif iGoal == 2:
			iNumSpices = pNetherlands.getNumAvailableBonuses(iSpices)
			aHelp.append(getIcon(iNumSpices >= 8) + localText.getText("TXT_KEY_VICTORY_AVAILABLE_SPICE_RESOURCES", (iNumSpices, 8)))

	elif iPlayer == iGermany:
		if iGoal == 0:
			iCounter = 0
			for iSpecialist in lGreatPeople:
				iCounter += countCitySpecialists(iPrussia, Areas.getCapital(iGermany), iSpecialist)
			aHelp.append(getIcon(iCounter >= 7) + localText.getText("TXT_KEY_VICTORY_GREAT_PEOPLE_IN_CITY", ("Berlin", iCounter, 7)))
		elif iGoal == 1:
			bAustria = isControlled(iGermany, utils.getPlotList(tAustriaTL, tAustriaBR))
			bPoland = checkOwnedCiv(iGermany, iPoland)
			bNetherlands = checkOwnedCiv(iGermany, iNetherlands)
			bScandinavia = checkOwnedCiv(iGermany, iVikings)
			bFrance = checkOwnedCiv(iGermany, iFrance)
			bEngland = checkOwnedCiv(iGermany, iEngland)
			bRussia = checkOwnedCiv(iGermany, iRussia)
			aHelp.append(getIcon(bAustria) + localText.getText("TXT_KEY_CIV_AUSTRIA_SHORT_DESC", ()) + ' ' + getIcon(bPoland) + localText.getText("TXT_KEY_CIV_POLAND_SHORT_DESC", ()))
			aHelp.append(getIcon(bNetherlands) + localText.getText("TXT_KEY_CIV_NETHERLANDS_ARTICLE", ()) + ' ' + getIcon(bScandinavia) + localText.getText("TXT_KEY_VICTORY_SCANDINAVIA", ()))
			aHelp.append(getIcon(bFrance) + localText.getText("TXT_KEY_CIV_FRANCE_SHORT_DESC", ()) + ' ' + getIcon(bEngland) + localText.getText("TXT_KEY_CIV_ENGLAND_SHORT_DESC", ()) + ' ' + getIcon(bRussia) + localText.getText("TXT_KEY_CIV_RUSSIA_SHORT_DESC", ()))
		elif iGoal == 2:
			iIndustrialTechs = countFirstDiscovered(iGermany, iIndustrial)
			iGlobalTechs = countFirstDiscovered(iGermany, iGlobal)
			aHelp.append(getIcon(iIndustrialTechs >= 6) + localText.getText("TXT_KEY_VICTORY_TECHS_FIRST_DISCOVERED", (gc.getEraInfo(iIndustrial).getText(), iIndustrialTechs, 6)) + ' ' + getIcon(iGlobalTechs >= 8) + localText.getText("TXT_KEY_VICTORY_TECHS_FIRST_DISCOVERED", (gc.getEraInfo(iGlobal).getText(), iGlobalTechs, 8)))

	elif iPlayer == iAmerica:
		if iGoal == 0:
			bAmericas = isAreaFreeOfCivs(utils.getPlotList(tNCAmericaTL, tNCAmericaBR), lCivGroups[0])
			bMexico = isControlledOrVassalized(iAmerica, Areas.getCoreArea(iAztecs, True))
			aHelp.append(getIcon(bAmericas) + localText.getText("TXT_KEY_VICTORY_NO_NORTH_AMERICAN_COLONIES", ()) + ' ' + getIcon(bMexico) + localText.getText("TXT_KEY_CIV_MEXICO_SHORT_DESC", ()))
		elif iGoal == 1:
			bUnitedNations = data.getWonderBuilder(iUnitedNations) == iAmerica
			bBrooklynBridge = data.getWonderBuilder(iBrooklynBridge) == iAmerica
			bStatueOfLiberty = data.getWonderBuilder(iStatueOfLiberty) == iAmerica
			bGoldenGateBridge = data.getWonderBuilder(iGoldenGateBridge) == iAmerica
			bPentagon = data.getWonderBuilder(iPentagon) == iAmerica
			bEmpireState = data.getWonderBuilder(iEmpireStateBuilding) == iAmerica
			aHelp.append(getIcon(bStatueOfLiberty) + localText.getText("TXT_KEY_BUILDING_STATUE_OF_LIBERTY", ()) + ' ' + getIcon(bBrooklynBridge) + localText.getText("TXT_KEY_BUILDING_BROOKLYN_BRIDGE", ()) + ' ' + getIcon(bEmpireState) + localText.getText("TXT_KEY_BUILDING_EMPIRE_STATE_BUILDING", ()))
			aHelp.append(getIcon(bGoldenGateBridge) + localText.getText("TXT_KEY_BUILDING_GOLDEN_GATE_BRIDGE", ()) + ' ' + getIcon(bPentagon) + localText.getText("TXT_KEY_BUILDING_PENTAGON", ()) + ' ' + getIcon(bUnitedNations) + localText.getText("TXT_KEY_BUILDING_UNITED_NATIONS", ()))
		elif iGoal == 2:
			fAlliedCommercePercent = calculateAlliedCommercePercent(iAmerica)
			fAlliedPowerPercent = calculateAlliedPowerPercent(iAmerica)
			aHelp.append(getIcon(fAlliedCommercePercent >= 75.0) + localText.getText("TXT_KEY_VICTORY_ALLIED_COMMERCE_PERCENT", (str(u"%.2f%%" % fAlliedCommercePercent), str(75))))
			aHelp.append(getIcon(fAlliedPowerPercent >= 75.0) + localText.getText("TXT_KEY_VICTORY_ALLIED_POWER_PERCENT", (str(u"%.2f%%" % fAlliedPowerPercent), str(75))))

	elif iPlayer == iArgentina:
		if iGoal == 0:
			iTradeGold = data.iArgentineTradeGold / 100
			aHelp.append(getIcon(iTradeGold >= utils.getTurns(6000)) + localText.getText("TXT_KEY_VICTORY_TRADE_GOLD", (iTradeGold, utils.getTurns(6000))))
		elif iGoal == 1:
			iCulture = getCityCulture(iArgentina, Areas.getCapital(iArgentina))
			aHelp.append(getIcon(iCulture >= utils.getTurns(40000)) + localText.getText("TXT_KEY_VICTORY_CITY_CULTURE", ("Buenos Aires", iCulture, utils.getTurns(40000))))
		elif iGoal == 2:
			iGoldenAgeTurns = data.iArgentineGoldenAgeTurns
			aHelp.append(getIcon(iGoldenAgeTurns >= utils.getTurns(48)) + localText.getText("TXT_KEY_VICTORY_GOLDEN_AGES", (iGoldenAgeTurns / utils.getTurns(8), 6)))

	elif iPlayer == iBrazil:
		if iGoal == 0:
			iSlavePlantations = countImprovements(iBrazil, iSlavePlantation)
			iPastures = countImprovements(iBrazil, iPasture)
			aHelp.append(getIcon(iSlavePlantations >= 10) + localText.getText("TXT_KEY_VICTORY_NUM_IMPROVEMENTS", (gc.getImprovementInfo(iSlavePlantation).getText(), iSlavePlantations, 10)) + ' ' + getIcon(iPastures >= 4) + localText.getText("TXT_KEY_VICTORY_NUM_IMPROVEMENTS", (gc.getImprovementInfo(iPasture).getText(), iPastures, 4)))
		elif iGoal == 1:
			bWembley = data.getWonderBuilder(iWembley) == iBrazil
			bCristoRedentor = data.getWonderBuilder(iCristoRedentor) == iBrazil
			bItaipuDam = data.getWonderBuilder(iItaipuDam) == iBrazil
			aHelp.append(getIcon(bWembley) + localText.getText("TXT_KEY_BUILDING_WEMBLEY", ()) + ' ' + getIcon(bCristoRedentor) + localText.getText("TXT_KEY_BUILDING_CRISTO_REDENTOR", ()) + ' ' + getIcon(bItaipuDam) + localText.getText("TXT_KEY_BUILDING_ITAIPU_DAM", ()))
		elif iGoal == 2:
			iForestPreserves = countImprovements(iBrazil, iForestPreserve)
			bNationalPark = False
			capital = pBrazil.getCapitalCity()
			if capital: bNationalPark = capital.isHasRealBuilding(iNationalPark)
			aHelp.append(getIcon(iForestPreserves >= 20) + localText.getText("TXT_KEY_VICTORY_NUM_IMPROVEMENTS", (gc.getImprovementInfo(iForestPreserve).getText(), iForestPreserves, 20)) + ' ' + getIcon(bNationalPark) + localText.getText("TXT_KEY_VICTORY_NATIONAL_PARK_CAPITAL", ()))

	elif iPlayer == iCanada:
		if iGoal == 0:
			capital = pCanada.getCapitalCity()
			bAtlantic = capital and isConnectedByRailroad(iCanada, (capital.getX(), capital.getY()), lAtlanticCoast)
			bPacific = capital and isConnectedByRailroad(iCanada, (capital.getX(), capital.getY()), lPacificCoast)
			aHelp.append(getIcon(bAtlantic) + localText.getText("TXT_KEY_VICTORY_ATLANTIC_RAILROAD", ()) + ' ' + getIcon(bPacific) + localText.getText("TXT_KEY_VICTORY_PACIFIC_RAILROAD", ()))
		elif iGoal == 1:
			iCanadaWest, iTotalCanadaWest = countControlledTiles(iCanada, tCanadaWestTL, tCanadaWestBR, False, tCanadaWestExceptions)
			iCanadaEast, iTotalCanadaEast = countControlledTiles(iCanada, tCanadaEastTL, tCanadaEastBR, False, tCanadaEastExceptions)
			fCanada = (iCanadaWest + iCanadaEast) * 100.0 / (iTotalCanadaWest + iTotalCanadaEast)
			bAllCities = controlsAllCities(iCanada, tCanadaWestTL, tCanadaWestBR, tCanadaWestExceptions) and controlsAllCities(iCanada, tCanadaEastTL, tCanadaEastBR, tCanadaEastExceptions)
			aHelp.append(getIcon(fCanada >= 95.0) + localText.getText("TXT_KEY_VICTORY_CONTROL_CANADA", (str(u"%.2f%%" % fCanada), str(95))) + ' ' + getIcon(bAllCities) + localText.getText("TXT_KEY_VICTORY_CONTROL_CANADA_CITIES", ()))
		elif iGoal == 2:
			iPeaceDeals = data.iCanadianPeaceDeals
			aHelp.append(getIcon(iPeaceDeals >= 12) + localText.getText("TXT_KEY_VICTORY_CANADIAN_PEACE_DEALS", (iPeaceDeals, 12)))
			
	return aHelp