// plot.cpp

#include "CvGameCoreDLL.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvUnit.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "CvGameAI.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLPlotBuilderIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLFlagEntityIFaceBase.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvRandom.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "CvArtFileMgr.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvEventReporter.h"
#include "CvRhyes.h" //Rhye

#include <Psapi.h>

#define STANDARD_MINIMAP_ALPHA		(0.6f)
#define STANDARD_MINIMAP_ALPHA_TRANSPARENT	(0.4f)


// Public Functions...

CvPlot::CvPlot()
{
	m_aiYield = new short[NUM_YIELD_TYPES];

	m_abCore = new bool[NUM_MAJOR_PLAYERS];
	m_aiSettlerValue = new int[NUM_MAJOR_PLAYERS];
	m_aiWarValue = new int[NUM_MAJOR_PLAYERS];
	m_aiReligionSpreadFactor = new int[NUM_RELIGIONS];

	m_aiReligionInfluence = new int[NUM_RELIGIONS];

	m_aiCulture = NULL;
	m_aiFoundValue = NULL;
	m_aiPlayerCityRadiusCount = NULL;
	m_aiPlotGroup = NULL;
	m_aiVisibilityCount = NULL;
	m_aiStolenVisibilityCount = NULL;
	m_aiBlockadedCount = NULL;
	m_aiRevealedOwner = NULL;
	m_abRiverCrossing = NULL;
	m_abRevealed = NULL;
	m_aeRevealedImprovementType = NULL;
	m_aeRevealedRouteType = NULL;
	m_paiBuildProgress = NULL;
	m_apaiCultureRangeCities = NULL;
	m_apaiInvisibleVisibilityCount = NULL;

	m_pFeatureSymbol = NULL;
	m_pPlotBuilder = NULL;
	m_pRouteSymbol = NULL;
	m_pRiverSymbol = NULL;
	m_pFlagSymbol = NULL;
	m_pFlagSymbolOffset = NULL;
	m_pCenterUnit = NULL;

	m_szScriptData = NULL;

	// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
	m_apaiPlayerDangerCache = NULL;
	// Sanguo Mod Performance, end

	reset(0, 0, true);
}


CvPlot::~CvPlot()
{
	// Leoreth: graphics paging
	pageGraphicsOut();

	uninit();

	SAFE_DELETE_ARRAY(m_aiYield);

	// Leoreth
	SAFE_DELETE_ARRAY(m_abCore);
	SAFE_DELETE_ARRAY(m_aiSettlerValue);
	SAFE_DELETE_ARRAY(m_aiWarValue);
	SAFE_DELETE_ARRAY(m_aiReligionSpreadFactor);

	SAFE_DELETE_ARRAY(m_aiReligionInfluence);
}

void CvPlot::init(int iX, int iY)
{
	//--------------------------------
	// Init saved data
	reset(iX, iY);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
}


void CvPlot::uninit()
{
	SAFE_DELETE_ARRAY(m_szScriptData);

	gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
	if(m_pPlotBuilder)
	{
		gDLL->getPlotBuilderIFace()->destroy(m_pPlotBuilder);
	}
	gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
	gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
	gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
	m_pCenterUnit = NULL;

	deleteAllSymbols();

	SAFE_DELETE_ARRAY(m_aiCulture);
	SAFE_DELETE_ARRAY(m_aiFoundValue);
	SAFE_DELETE_ARRAY(m_aiPlayerCityRadiusCount);
	SAFE_DELETE_ARRAY(m_aiPlotGroup);

	SAFE_DELETE_ARRAY(m_aiVisibilityCount);
	SAFE_DELETE_ARRAY(m_aiStolenVisibilityCount);
	SAFE_DELETE_ARRAY(m_aiBlockadedCount);
	SAFE_DELETE_ARRAY(m_aiRevealedOwner);

	SAFE_DELETE_ARRAY(m_abRiverCrossing);
	SAFE_DELETE_ARRAY(m_abRevealed);

	SAFE_DELETE_ARRAY(m_aeRevealedImprovementType);
	SAFE_DELETE_ARRAY(m_aeRevealedRouteType);

	SAFE_DELETE_ARRAY(m_paiBuildProgress);

	if (NULL != m_apaiCultureRangeCities)
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiCultureRangeCities[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiCultureRangeCities);
	}

	if (NULL != m_apaiInvisibleVisibilityCount)
	{
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount);
	}

	// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
	if (NULL != m_apaiPlayerDangerCache)
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiPlayerDangerCache[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiPlayerDangerCache);
	}
	// Sanguo Mod Performance, end

	m_units.clear();
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlot::reset(int iX, int iY, bool bConstructorCall)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_iX = iX;
	m_iY = iY;
	m_iArea = FFreeList::INVALID_INDEX;
	m_pPlotArea = NULL;
	m_iFeatureVariety = 0;
	m_iOwnershipDuration = 0;
	m_iImprovementDuration = 0;
	m_iUpgradeProgress = 0;
	m_iForceUnownedTimer = 0;
	m_iCityRadiusCount = 0;
	m_iRiverID = -1;
	m_iMinOriginalStartDist = -1;
	m_iReconCount = 0;
	m_iRiverCrossingCount = 0;

	// Leoreth
	//m_iRegionID = -1;

	// Leoreth: graphics paging
	m_iGraphicsPageIndex = -1;

	m_bStartingPlot = false;
	m_bHills = false;
	m_bNOfRiver = false;
	m_bWOfRiver = false;
	m_bIrrigated = false;
	m_bPotentialCityWork = false;
	m_bShowCitySymbols = false;
	m_bFlagDirty = false;
	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_bWithinGreatWall = false;

	m_eOwner = NO_PLAYER;
	m_ePlotType = PLOT_OCEAN;
	m_eTerrainType = NO_TERRAIN;
	m_eFeatureType = NO_FEATURE;
	m_eBonusType = NO_BONUS;
	m_eImprovementType = NO_IMPROVEMENT;
	m_eRouteType = NO_ROUTE;
	m_eRiverNSDirection = NO_CARDINALDIRECTION;
	m_eRiverWEDirection = NO_CARDINALDIRECTION;

	m_plotCity.reset();
	m_workingCity.reset();
	m_workingCityOverride.reset();

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		m_aiYield[iI] = 0;
	}

	for (iI = 0; iI < NUM_MAJOR_PLAYERS; ++iI)
	{
		m_abCore[iI] = false;
		m_aiSettlerValue[iI] = 0;
		m_aiWarValue[iI] = 0;
	}

	for (iI = 0; iI < NUM_RELIGIONS; ++iI)
	{
		m_aiReligionSpreadFactor[iI] = -1;
		m_aiReligionInfluence[iI] = 0;
	}
}

typedef struct
{
	CvPlot*	pPlot;
	int		iSeq;
} GraphicsPagingInfo;

static int iNumPagedInPlots = 0;
static int iPageTableSize = 0;
static int iRenderStartSeq = 0;
static int iCurrentSeq = 0;
static int iOldestSearchSeqHint = 0;
static GraphicsPagingInfo* pagingTable = NULL;

#define	MAX_VALID_PAGING_INDEX (65534)

int findFreePagingTableSlot()
{
	static int iSearchStartHintIndex = 0;

	for(int iI = 0; iI < iPageTableSize; iI++)
	{
		int iIndex = iSearchStartHintIndex++;

		if ( iSearchStartHintIndex >= iPageTableSize )
		{
			iSearchStartHintIndex = 0;
		}

		if ( pagingTable[iIndex].pPlot == NULL )
		{
			return iIndex;
		}
	}

	return -1;
}

int allocateNewPagingEntry()
{
	if ( iPageTableSize <= iNumPagedInPlots++ )
	{
		int iNewSize = std::max(64, iPageTableSize + std::min(iPageTableSize,512));

		GraphicsPagingInfo* newTable = new GraphicsPagingInfo[iNewSize];

		if ( iPageTableSize > 0 )
		{
			memcpy(newTable, pagingTable, iPageTableSize*sizeof(GraphicsPagingInfo));
			SAFE_DELETE_ARRAY(pagingTable);
		}
		
		pagingTable = newTable;
		iPageTableSize = iNewSize;
	}

	return findFreePagingTableSlot();
}

int findOldestEvictablePagingEntry()
{
	int iOldest = MAX_INT;
	int iResult = -1;

	for(int iI = 0; iI < iNumPagedInPlots; iI++)
	{
		if ( pagingTable[iI].pPlot != NULL && pagingTable[iI].iSeq < iRenderStartSeq )
		{
			if ( pagingTable[iI].iSeq < iOldest )
			{
				iOldest = pagingTable[iI].iSeq;
				iResult = iI;
			}
		}
	}

	return iResult;
}

#define DEFAULT_MAX_WORKING_SET_THRESHOLD_BEFORE_EVICTION ((size_t)1024*(size_t)1024*(size_t)1024*(size_t)2)
#define DEFAULT_OS_MEMORY_ALLOWANCE ((size_t)1024*(size_t)1024*(size_t)512)

bool NeedToFreeMemory()
{
	PROCESS_MEMORY_COUNTERS pmc;
	static unsigned int uiMaxMem = 0xFFFFFFFF;

	if ( uiMaxMem == 0xFFFFFFFF )
	{
		MEMORYSTATUSEX memoryStatus;

		memoryStatus.dwLength = sizeof(memoryStatus);
		GlobalMemoryStatusEx(&memoryStatus);

		uiMaxMem = DEFAULT_MAX_WORKING_SET_THRESHOLD_BEFORE_EVICTION;

		DWORDLONG usableMemory = memoryStatus.ullTotalPhys - (DWORDLONG)DEFAULT_OS_MEMORY_ALLOWANCE;
		if ( usableMemory < uiMaxMem )
		{
			uiMaxMem = (unsigned int)usableMemory;
		}
	}

	GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc));
	
	if ( pmc.WorkingSetSize > uiMaxMem )
	{
		OutputDebugString(CvString::format("Found need to free memory: %d used vs %d target\n", pmc.WorkingSetSize, uiMaxMem).c_str());
		return true;
	}
	else
	{
		return false;
	}
}

static bool bFoundEvictable = true;

void CvPlot::EvictGraphicsIfNecessary()
{
	while(bFoundEvictable && NeedToFreeMemory())
	{
		int iEvictionIndex = findOldestEvictablePagingEntry();

		if ( iEvictionIndex == -1 )
		{
			bFoundEvictable = false;
			break;
		}

		pagingTable[iEvictionIndex].pPlot->setShouldHaveFullGraphics(false);
	}
}

void CvPlot::pageGraphicsOut()
{
	bFoundEvictable = true;

	if ( m_iGraphicsPageIndex != -1 )
	{
		pagingTable[m_iGraphicsPageIndex].pPlot = NULL;

		if ( --iNumPagedInPlots == 0 )
		{
			SAFE_DELETE_ARRAY(pagingTable);

			iPageTableSize = 0;
		}
	}

	m_iGraphicsPageIndex = -1;
}

void CvPlot::notePageRenderStart(int iRenderArea)
{
	iRenderStartSeq = std::max(0,iCurrentSeq-iRenderArea);
}

void CvPlot::setShouldHaveFullGraphics(bool bShouldHaveFullGraphics)
{
	bool bChanged;

	if ( !GC.getGraphicalDetailPagingEnabled() )
	{
		bChanged = true;

		m_iGraphicsPageIndex = -1;

		if ( pagingTable != NULL )
		{
			SAFE_DELETE_ARRAY(pagingTable);

			iNumPagedInPlots = 0;
			iPageTableSize = 0;
		}
	}
	else
	{
		//	A set to false is only ever used to switch out paged in graphics
		//	or when truning the entire mechanism on from it previously being off.
		//	In both of these cases we want to treat it as a change
		bChanged = (!bShouldHaveFullGraphics || ((m_iGraphicsPageIndex != -1) != bShouldHaveFullGraphics));

		if ( bShouldHaveFullGraphics )
		{
			if ( m_iGraphicsPageIndex == -1 )
			{
				m_iGraphicsPageIndex = allocateNewPagingEntry();
			}

			GraphicsPagingInfo* pPagingInfo = &pagingTable[m_iGraphicsPageIndex];

			pPagingInfo->iSeq = ++iCurrentSeq;
			pPagingInfo->pPlot = this;

			if ( iCurrentSeq == MAX_INT )
			{
				iCurrentSeq = 0;
			}

			EvictGraphicsIfNecessary();
		}
		else
		{
			pageGraphicsOut();
		}
	}

	if ( bChanged )
	{
		setLayoutDirty(true);
		if ( getPlotCity() != NULL )
		{
			getPlotCity()->setLayoutDirty(true);
		}

		if ( bShouldHaveFullGraphics )
		{
			//gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(),false,true);
			updateSymbols();
			updateFeatureSymbol();
			updateRiverSymbol();
			updateRouteSymbol();
		}
		else
		{
			destroyGraphics();
		}

		updateCenterUnit();
	}
}

bool CvPlot::shouldHaveFullGraphics(void) const
{
	return (!GC.getGraphicalDetailPagingEnabled() || m_iGraphicsPageIndex != -1) && GC.IsGraphicsInitialized()/* && shouldHaveGraphics()*/;
}

/*bool CvPlot::shouldHaveGraphics(void) const
{
	return GC.IsGraphicsInitialized() && isInViewport(); // && isRevealed(GC.getGame().getActiveTeam(), false);
}*/

void CvPlot::destroyGraphics()
{
#ifdef MULTI_FEATURE_MOD
	destroyFeatureSymbols();
#else
	gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
#endif
	if(m_pPlotBuilder)
	{
		gDLL->getPlotBuilderIFace()->destroy(m_pPlotBuilder);
		m_pPlotBuilder = NULL;
	}
	gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
	gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
	gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
	m_pCenterUnit = NULL;
	m_pFeatureSymbol = NULL;
	m_pRouteSymbol = NULL;
	m_pRiverSymbol = NULL;
	m_pFlagSymbol = NULL;
	m_pFlagSymbolOffset = NULL;

	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_bFlagDirty = false;

	deleteAllSymbols();
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlot::setupGraphical()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	updateSymbols();
	updateFeatureSymbol();
	updateRiverSymbol();
	updateMinimapColor();

	updateVisibility();
}

void CvPlot::updateGraphicEra()
{
	if(m_pRouteSymbol != NULL)
		gDLL->getRouteIFace()->updateGraphicEra(m_pRouteSymbol);

	if(m_pFlagSymbol != NULL)
		gDLL->getFlagEntityIFace()->updateGraphicEra(m_pFlagSymbol);
}

void CvPlot::erase()
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	CLinkList<IDInfo> oldUnits;

	// kill units
	oldUnits.clear();

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = oldUnits.next(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->kill(false);
		}
	}

	// kill cities
	pCity = getPlotCity();
	if (pCity != NULL)
	{
		pCity->kill(false);
	}

	setBonusType(NO_BONUS);
	setImprovementType(NO_IMPROVEMENT);
	setRouteType(NO_ROUTE, false);
	setFeatureType(NO_FEATURE);

	// disable rivers
	setNOfRiver(false, NO_CARDINALDIRECTION);
	setWOfRiver(false, NO_CARDINALDIRECTION);
	setRiverID(-1);
}

//Rhye - start
void CvPlot::eraseAIDevelopment()
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	CLinkList<IDInfo> oldUnits;

	// kill units
	oldUnits.clear();

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = oldUnits.next(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->kill(false);
		}
	}

	// kill cities
	pCity = getPlotCity();
	if (pCity != NULL)
	{
		pCity->kill(false);
	}

	setImprovementType(NO_IMPROVEMENT);
}
//Rhye - end


float CvPlot::getPointX() const
{
	return GC.getMapINLINE().plotXToPointX(getX_INLINE());
}


float CvPlot::getPointY() const
{
	return GC.getMapINLINE().plotYToPointY(getY_INLINE());
}


NiPoint3 CvPlot::getPoint() const
{
	NiPoint3 pt3Point;

	pt3Point.x = getPointX();
	pt3Point.y = getPointY();
	pt3Point.z = 0.0f;

	pt3Point.z = gDLL->getEngineIFace()->GetHeightmapZ(pt3Point);

	return pt3Point;
}


float CvPlot::getSymbolSize() const
{
	if (isVisibleWorked())
	{
		if (isShowCitySymbols())
		{
			return 1.6f;
		}
		else
		{
			return 1.2f;
		}
	}
	else
	{
		if (isShowCitySymbols())
		{
			return 1.2f;
		}
		else
		{
			return 0.8f;
		}
	}
}


float CvPlot::getSymbolOffsetX(int iOffset) const
{
	return ((40.0f + (((float)iOffset) * 28.0f * getSymbolSize())) - (GC.getPLOT_SIZE() / 2.0f));
}


float CvPlot::getSymbolOffsetY(int iOffset) const
{
	return (-(GC.getPLOT_SIZE() / 2.0f) + 50.0f);
}


TeamTypes CvPlot::getTeam() const
{
	if (isOwned())
	{
		return GET_PLAYER(getOwnerINLINE()).getTeam();
	}
	else
	{
		return NO_TEAM;
	}
}


void CvPlot::doTurn()
{
	PROFILE_FUNC();

	if (getForceUnownedTimer() > 0)
	{
		changeForceUnownedTimer(-1);
	}

	if (isOwned())
	{
		changeOwnershipDuration(1);
	}

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		changeImprovementDuration(1);
	}

	doFeature();

	doCulture();

	verifyUnitValidPlot();

	// Leoreth: Great Wall effect
	if (isWithinGreatWall() && isOwned())
	{
		if (GET_PLAYER(getOwnerINLINE()).isHasBuildingEffect((BuildingTypes)GREAT_WALL))
		{
			for (int iI = 0; iI < getNumUnits(); iI++)
			{
				if (getUnitByIndex(iI)->getOwnerINLINE() == BARBARIAN)
				{
					getUnitByIndex(iI)->changeDamage(10, getOwnerINLINE());
				}
			}
		}
	}

	/*
	if (!isOwned())
	{
		doImprovementUpgrade();
	}
	*/

	// XXX
#ifdef _DEBUG
	{
		CLLNode<IDInfo>* pUnitNode;
		CvUnit* pLoopUnit;

		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			FAssertMsg(pLoopUnit->atPlot(this), "pLoopUnit is expected to be at the current plot instance");
		}
	}
#endif
	// XXX
}


void CvPlot::doImprovement()
{
	PROFILE_FUNC();

	CvCity* pCity;
	CvWString szBuffer;
	int iI;

	FAssert(isBeingWorked() && isOwned());

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		if (getBonusType() == NO_BONUS)
		{
			FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::doImprovement");
			for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes) iI).getTechReveal())))
				{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                                jdog5000      */
/*                                                                                              */
/* Gamespeed scaling                                                                            */
/************************************************************************************************/
/* original bts code
					if (GC.getImprovementInfo(getImprovementType()).getImprovementBonusDiscoverRand(iI) > 0)
					{
						if (GC.getGameINLINE().getSorenRandNum(GC.getImprovementInfo(getImprovementType()).getImprovementBonusDiscoverRand(iI), "Bonus Discovery") == 0)
						{
*/
					int iOdds = GC.getImprovementInfo(getImprovementType()).getImprovementBonusDiscoverRand(iI);

					if( iOdds > 0 )
					{
						iOdds *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getVictoryDelayPercent();
						iOdds /= 100;

						if( GC.getGameINLINE().getSorenRandNum(iOdds, "Bonus Discovery") == 0)
						{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
							setBonusType((BonusTypes)iI);

							pCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), getOwnerINLINE(), NO_TEAM, false);

							if (pCity != NULL)
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE", GC.getBonusInfo((BonusTypes) iI).getTextKeyWide(), pCity->getNameKey());
								gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DISCOVERBONUS", MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo((BonusTypes) iI).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
							}

							break;
						}
					}
				}
			}
		}
	}

	doImprovementUpgrade();
}

void CvPlot::doImprovementUpgrade()
{
	if (getImprovementType() != NO_IMPROVEMENT)
	{
		ImprovementTypes eImprovementUpdrade = (ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getImprovementUpgrade();
		if (eImprovementUpdrade != NO_IMPROVEMENT)
		{
			if (isBeingWorked() || GC.getImprovementInfo(eImprovementUpdrade).isOutsideBorders())
			{
				changeUpgradeProgress(GET_PLAYER(getOwnerINLINE()).getImprovementUpgradeRate());

				if (getUpgradeProgress() >= GC.getGameINLINE().getImprovementUpgradeTime(getImprovementType()))
				{
					setImprovementType(eImprovementUpdrade);
				}
			}
		}
	}
}

void CvPlot::updateCulture(bool bBumpUnits, bool bUpdatePlotGroups)
{
	if (!isCity())
	{
		setOwner(calculateCulturalOwner(), bBumpUnits, bUpdatePlotGroups);
	}
}


void CvPlot::updateFog()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	FAssert(GC.getGameINLINE().getActiveTeam() != NO_TEAM);

	if (isRevealed(GC.getGameINLINE().getActiveTeam(), false))
	{
		if (gDLL->getInterfaceIFace()->isBareMapMode())
		{
			gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
		}
		else
		{
			int cityScreenFogEnabled = GC.getDefineINT("CITY_SCREEN_FOG_ENABLED");
			if (cityScreenFogEnabled && gDLL->getInterfaceIFace()->isCityScreenUp() && (gDLL->getInterfaceIFace()->getHeadSelectedCity() != getWorkingCity()))
			{
				gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
			}
			else if (isActiveVisible(false))
			{
				gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
			}
			else
			{
				gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
			}
		}
	}
	else
	{
		gDLL->getEngineIFace()->BlackenVisibility(getFOWIndex());
	}
}


void CvPlot::updateVisibility()
{
	PROFILE("CvPlot::updateVisibility");

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	setLayoutDirty(true);

	updateSymbolVisibility();
	updateFeatureSymbolVisibility();
	updateRouteSymbol();

	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
	{
		pCity->updateVisibility();
	}
}


void CvPlot::updateSymbolDisplay()
{
	PROFILE_FUNC();

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	CvSymbol* pLoopSymbol;
	int iLoop;

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	for (iLoop = 0; iLoop < getNumSymbols(); iLoop++)
	{
		pLoopSymbol = getSymbol(iLoop);

		if (pLoopSymbol != NULL)
		{
			if (isShowCitySymbols())
			{
				gDLL->getSymbolIFace()->setAlpha(pLoopSymbol, (isVisibleWorked()) ? 1.0f : 0.8f);
			}
			else
			{
				gDLL->getSymbolIFace()->setAlpha(pLoopSymbol, (isVisibleWorked()) ? 0.8f : 0.6f);
			}
			gDLL->getSymbolIFace()->setScale(pLoopSymbol, getSymbolSize());
			gDLL->getSymbolIFace()->updatePosition(pLoopSymbol);
		}
	}
}


void CvPlot::updateSymbolVisibility()
{
	PROFILE_FUNC();

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	CvSymbol* pLoopSymbol;
	int iLoop;

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	for (iLoop = 0; iLoop < getNumSymbols(); iLoop++)
	{
		pLoopSymbol = getSymbol(iLoop);

		if (pLoopSymbol != NULL)
		{
			if (isRevealed(GC.getGameINLINE().getActiveTeam(), true) &&
				  (isShowCitySymbols() ||
				   (gDLL->getInterfaceIFace()->isShowYields() && !(gDLL->getInterfaceIFace()->isCityScreenUp()))))
			{
				gDLL->getSymbolIFace()->Hide(pLoopSymbol, false);
			}
			else
			{
				gDLL->getSymbolIFace()->Hide(pLoopSymbol, true);
			}
		}
	}
}


void CvPlot::updateSymbols()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	deleteAllSymbols();

	int yieldAmounts[NUM_YIELD_TYPES];
	int maxYield = 0;
	for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; iYieldType++)
	{
		int iYield = calculateYield(((YieldTypes)iYieldType), true);
		yieldAmounts[iYieldType] = iYield;
		if(iYield>maxYield)
		{
			maxYield = iYield;
		}
	}

	if(maxYield>0)
	{
		int maxYieldStack = GC.getDefineINT("MAX_YIELD_STACK");
		int layers = maxYield /maxYieldStack + 1;

		CvSymbol *pSymbol= NULL;
		for(int i=0;i<layers;i++)
		{
			pSymbol = addSymbol();
			for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; iYieldType++)
			{
				int iYield = yieldAmounts[iYieldType] - (maxYieldStack * i);
				LIMIT_RANGE(0,iYield, maxYieldStack);
				if(yieldAmounts[iYieldType])
				{
					gDLL->getSymbolIFace()->setTypeYield(pSymbol,iYieldType,iYield);
				}
			}
		}
		for(int i=0;i<getNumSymbols();i++)
		{
			SymbolTypes eSymbol  = (SymbolTypes)0;
			pSymbol = getSymbol(i);
			gDLL->getSymbolIFace()->init(pSymbol, gDLL->getSymbolIFace()->getID(pSymbol), i, eSymbol, this);
		}
	}

	updateSymbolDisplay();
	updateSymbolVisibility();
}


void CvPlot::updateMinimapColor()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	gDLL->getInterfaceIFace()->setMinimapColor(MINIMAPMODE_TERRITORY, getX_INLINE(), getY_INLINE(), plotMinimapColor(), (!isOwned() || !isWater()) ? STANDARD_MINIMAP_ALPHA : STANDARD_MINIMAP_ALPHA_TRANSPARENT);
}


void CvPlot::updateCenterUnit()
{
	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	// Leoreth: includes graphics paging
	if (!isActiveVisible(true) || !shouldHaveFullGraphics())
	{
		setCenterUnit(NULL);
		return;
	}

	setCenterUnit(getSelectedUnit());

	if (getCenterUnit() == NULL)
	{
		setCenterUnit(getBestDefender(GC.getGameINLINE().getActivePlayer(), NO_PLAYER, NULL, false, false, true));
	}

	if (getCenterUnit() == NULL)
	{
		setCenterUnit(getBestDefender(GC.getGameINLINE().getActivePlayer()));
	}

	if (getCenterUnit() == NULL)
	{
		setCenterUnit(getBestDefender(NO_PLAYER, GC.getGameINLINE().getActivePlayer(), gDLL->getInterfaceIFace()->getHeadSelectedUnit(), true));
	}

	if (getCenterUnit() == NULL)
	{
		setCenterUnit(getBestDefender(NO_PLAYER, GC.getGameINLINE().getActivePlayer(), gDLL->getInterfaceIFace()->getHeadSelectedUnit()));
	}

	if (getCenterUnit() == NULL)
	{
		setCenterUnit(getBestDefender(NO_PLAYER, GC.getGameINLINE().getActivePlayer()));
	}
}


void CvPlot::verifyUnitValidPlot()
{
	PROFILE_FUNC();

	std::vector<CvUnit*> aUnits;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		if (NULL != pLoopUnit)
		{
			aUnits.push_back(pLoopUnit);
		}
	}

	std::vector<CvUnit*>::iterator it = aUnits.begin();
	while (it != aUnits.end())
	{
		CvUnit* pLoopUnit = *it;
		bool bErased = false;

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->atPlot(this))
			{
				if (!(pLoopUnit->isCargo()))
				{
					if (!(pLoopUnit->isCombat()))
					{
						//Rhye - breaks the auto-play
						//if (!isValidDomainForLocation(*pLoopUnit) || !(pLoopUnit->canEnterArea(getTeam(), area())))
						if (!(pLoopUnit->canEnterArea(getTeam(), area())))
						{
							if (!pLoopUnit->jumpToNearestValidPlot())
							{
								bErased = true;
							}
						}
					}
				}
			}
		}

		if (bErased)
		{
			it = aUnits.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (isOwned())
	{
		it = aUnits.begin();
		while (it != aUnits.end())
		{
			CvUnit* pLoopUnit = *it;
			bool bErased = false;

			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->atPlot(this))
				{
					if (!(pLoopUnit->isCombat()))
					{
						if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
						{
							if (isVisibleEnemyUnit(pLoopUnit))
							{
								if (!(pLoopUnit->isInvisible(getTeam(), false)))
								{
									if (!pLoopUnit->jumpToNearestValidPlot())
									{
										bErased = true;
									}
								}
							}
						}
					}
				}
			}

			if (bErased)
			{
				it = aUnits.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}


void CvPlot::nukeExplosion(int iRange, CvUnit* pNukeUnit)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pLoopCity;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	CLinkList<IDInfo> oldUnits;
	CvWString szBuffer;
	int iNukeDamage;
	int iNukedPopulation;
	int iDX, iDY;
	int iI;

	GC.getGameINLINE().changeNukesExploded(1);

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				// if we remove roads, don't remove them on the city... XXX

				pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity == NULL)
				{
					if (!(pLoopPlot->isWater()) && !(pLoopPlot->isImpassable()))
					{
						if (NO_FEATURE == pLoopPlot->getFeatureType() || !GC.getFeatureInfo(pLoopPlot->getFeatureType()).isNukeImmune())
						{
							if (GC.getGameINLINE().getSorenRandNum(100, "Nuke Fallout") < GC.getDefineINT("NUKE_FALLOUT_PROB"))
							{
								pLoopPlot->setImprovementType(NO_IMPROVEMENT);
								pLoopPlot->setFeatureType((FeatureTypes)(GC.getDefineINT("NUKE_FEATURE")));
							}
						}
					}
				}

				oldUnits.clear();

				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = oldUnits.next(pUnitNode);

					if (pLoopUnit != NULL)
					{
						if (pLoopUnit != pNukeUnit)
						{
							if (!pLoopUnit->isNukeImmune() && !pLoopUnit->isDelayedDeath())
							{
								iNukeDamage = (GC.getDefineINT("NUKE_UNIT_DAMAGE_BASE") + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("NUKE_UNIT_DAMAGE_RAND_1"), "Nuke Damage 1") + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("NUKE_UNIT_DAMAGE_RAND_2"), "Nuke Damage 2"));

								if (pLoopCity != NULL)
								{
									iNukeDamage *= std::max(0, (pLoopCity->getNukeModifier() + 100));
									iNukeDamage /= 100;
								}

								if (pLoopUnit->canFight() || pLoopUnit->airBaseCombatStr() > 0)
								{
									pLoopUnit->changeDamage(iNukeDamage, ((pNukeUnit != NULL) ? pNukeUnit->getOwnerINLINE() : NO_PLAYER));
								}
								else if (iNukeDamage >= GC.getDefineINT("NUKE_NON_COMBAT_DEATH_THRESHOLD"))
								{
									pLoopUnit->kill(false, ((pNukeUnit != NULL) ? pNukeUnit->getOwnerINLINE() : NO_PLAYER));
								}
							}
						}
					}
				}

				if (pLoopCity != NULL)
				{
					for (iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
					{
						if (pLoopCity->getNumRealBuilding((BuildingTypes)iI) > 0)
						{
							if (!(GC.getBuildingInfo((BuildingTypes) iI).isNukeImmune()))
							{
								if (GC.getGameINLINE().getSorenRandNum(100, "Building Nuked") < GC.getDefineINT("NUKE_BUILDING_DESTRUCTION_PROB"))
								{
									pLoopCity->setNumRealBuilding(((BuildingTypes)iI), pLoopCity->getNumRealBuilding((BuildingTypes)iI) - 1);
								}
							}
						}
					}

					iNukedPopulation = ((pLoopCity->getPopulation() * (GC.getDefineINT("NUKE_POPULATION_DEATH_BASE") + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_1"), "Population Nuked 1") + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_2"), "Population Nuked 2"))) / 100);

					iNukedPopulation *= std::max(0, (pLoopCity->getNukeModifier() + 100));
					iNukedPopulation /= 100;

					pLoopCity->changePopulation(-(std::min((pLoopCity->getPopulation() - 1), iNukedPopulation)));
				}
			}
		}
	}

	CvEventReporter::getInstance().nukeExplosion(this, pNukeUnit);
}


bool CvPlot::isConnectedTo(const CvCity* pCity) const
{
	FAssert(isOwned());
	return ((getPlotGroup(getOwnerINLINE()) == pCity->plotGroup(getOwnerINLINE())) || (getPlotGroup(pCity->getOwnerINLINE()) == pCity->plotGroup(pCity->getOwnerINLINE())));
}


bool CvPlot::isConnectedToCapital(PlayerTypes ePlayer) const
{
	CvCity* pCapitalCity;

	if (ePlayer == NO_PLAYER)
	{
		ePlayer = getOwnerINLINE();
	}

	if (ePlayer != NO_PLAYER)
	{
		pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

		if (pCapitalCity != NULL)
		{
			return isConnectedTo(pCapitalCity);
		}
	}

	return false;
}


int CvPlot::getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	CvPlotGroup* pPlotGroup;

	FAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
	FAssertMsg(eBonus != NO_BONUS, "Bonus is not assigned a valid value");

	pPlotGroup = getPlotGroup(ePlayer);

	if (pPlotGroup != NULL)
	{
		return pPlotGroup->getNumBonuses(eBonus);
	}
	else
	{
		return 0;
	}
}


bool CvPlot::isPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	return (getPlotGroupConnectedBonus(ePlayer, eBonus) > 0);
}


bool CvPlot::isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isPlotGroupConnectedBonus(ePlayer, eBonus))
			{
				return true;
			}
		}
	}

	return false;
}


void CvPlot::updatePlotGroupBonus(bool bAdd)
{
	PROFILE_FUNC();

	CvCity* pPlotCity;
	CvPlotGroup* pPlotGroup;
	BonusTypes eNonObsoleteBonus;
	int iI;

	if (!isOwned())
	{
		return;
	}

	pPlotGroup = getPlotGroup(getOwnerINLINE());

	if (pPlotGroup != NULL)
	{
		pPlotCity = getPlotCity();

		if (pPlotCity != NULL)
		{
			for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
			{
				if (!GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)iI))
				{
					pPlotGroup->changeNumBonuses(((BonusTypes)iI), (pPlotCity->getFreeBonus((BonusTypes)iI) * ((bAdd) ? 1 : -1)));
				}
			}

			if (pPlotCity->isCapital())
			{
				for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					pPlotGroup->changeNumBonuses(((BonusTypes)iI), (GET_PLAYER(getOwnerINLINE()).getBonusExport((BonusTypes)iI) * ((bAdd) ? -1 : 1)));
					pPlotGroup->changeNumBonuses(((BonusTypes)iI), (GET_PLAYER(getOwnerINLINE()).getBonusImport((BonusTypes)iI) * ((bAdd) ? 1 : -1)));
				}
			}
		}

		eNonObsoleteBonus = getNonObsoleteBonusType(getTeam());

		if (eNonObsoleteBonus != NO_BONUS)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo(eNonObsoleteBonus).getTechCityTrade())))
			{
				if (isCity(true, getTeam()) ||
					((getImprovementType() != NO_IMPROVEMENT) && GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus)))
				{
					if ((pPlotGroup != NULL) && isBonusNetwork(getTeam()))
					{
						pPlotGroup->changeNumBonuses(eNonObsoleteBonus, ((bAdd) ? 1 : -1));
					}
				}
			}
		}
	}
}


bool CvPlot::isAdjacentToArea(int iAreaID) const
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getArea() == iAreaID)
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::isAdjacentToArea(const CvArea* pArea) const
{
	return isAdjacentToArea(pArea->getID());
}


bool CvPlot::shareAdjacentArea(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	int iCurrArea;
	int iLastArea;
	CvPlot* pAdjacentPlot;
	int iI;

	iLastArea = FFreeList::INVALID_INDEX;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			iCurrArea = pAdjacentPlot->getArea();

			if (iCurrArea != iLastArea)
			{
				if (pPlot->isAdjacentToArea(iCurrArea))
				{
					return true;
				}

				iLastArea = iCurrArea;
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentToLand() const
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (!(pAdjacentPlot->isWater()))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isCoastalLand(int iMinWaterSize) const
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	int iI;

	if (isWater())
	{
		return false;
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			//Rhye - start
			//if (pAdjacentPlot->isWater())
			if (pAdjacentPlot->isWater() && !(pAdjacentPlot->getFeatureType() == 0)) //(ice)  .isImpassable()
			//Rhye - end
			{
				if (pAdjacentPlot->area()->getNumTiles() >= iMinWaterSize)
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isVisibleWorked() const
{
	if (isBeingWorked())
	{
		if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer) const
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				if ((eIgnorePlayer == NO_PLAYER) || (((PlayerTypes)iI) != eIgnorePlayer))
				{
					if (isPlayerCityRadius((PlayerTypes)iI))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlot::isLake() const
{
	CvArea* pArea;

	pArea = area();

	//Rhye - start (salt lake)
	int saltLakePlots[13][2] = {
	{76, 43}, //Van
	{82, 48}, //Aral sea
	{82, 49},
	{83, 48},
	{83, 49},
	{87, 50}, //eastern Balkhash
	{88, 50},
	{88, 48}, //Aydar
	{72, 49}, //Black Sea
	{112, 43}, //Japan
	{113, 44},
	{112, 14}, //Eyre
	{14, 48} //Great Salt Lake
	};

	for (int i = 0; i < 13; i++) {
		if (getX() == saltLakePlots[i][0] && getY() == saltLakePlots[i][1])
			return false;
	}
	//Rhye - end

	if (pArea != NULL)
	{
		return pArea->isLake();
	}

	return false;
}


// XXX if this changes need to call updateIrrigated() and pCity->updateFreshWaterHealth()
// XXX precalculate this???
bool CvPlot::isFreshWater() const
{
	CvPlot* pLoopPlot;
	int iDX, iDY;

	if (isWater())
	{
		return false;
	}

	if (isImpassable())
	{
		return false;
	}

	if (isRiver())
	{
		return true;
	}

	//Leoreth: Great Bath effect
	/*if (isCity())
	{
	    CvCity* pCity = getPlotCity();
	    if (pCity->isHasRealBuilding((BuildingTypes)GREAT_BATH))
	    {
	        return true;
	    }
	}*/

	for (iDX = -1; iDX <= 1; iDX++)
	{
		for (iDY = -1; iDY <= 1; iDY++)
		{
			pLoopPlot	= plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isLake())
				{
					return true;
				}

				if (pLoopPlot->getFeatureType() != NO_FEATURE)
				{
					if (GC.getFeatureInfo(pLoopPlot->getFeatureType()).isAddsFreshWater())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlot::isPotentialIrrigation() const
{
	// Leoreth: all Moorish improvements that give food spread irrigation
	if ((isCity() && !isHills()) || ((getImprovementType() != NO_IMPROVEMENT) && (GC.getImprovementInfo(getImprovementType()).isCarriesIrrigation() || (getTeam() == MOORS && GC.getImprovementInfo(getImprovementType()).getYieldChange(0) > 0))))
	{
		if ((getTeam() != NO_TEAM) && GET_TEAM(getTeam()).isIrrigation())
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::canHavePotentialIrrigation() const
{
	int iI;

	if (isCity() && !isHills())
	{
		return true;
	}

	for (iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iI).isCarriesIrrigation() || (getOwner() == MOORS && GC.getImprovementInfo((ImprovementTypes)iI).getYieldChange(0) > 0))
		{
			if (canHaveImprovement(((ImprovementTypes)iI), NO_TEAM, true))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isIrrigationAvailable(bool bIgnoreSelf) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	if (!bIgnoreSelf && isIrrigated())
	{
		return true;
	}

	if (isFreshWater())
	{
		return true;
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isIrrigated())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isRiverMask() const
{
	CvPlot* pPlot;

	if (isNOfRiver())
	{
		return true;
	}

	if (isWOfRiver())
	{
		return true;
	}

	pPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_EAST);
	if ((pPlot != NULL) && pPlot->isNOfRiver())
	{
		return true;
	}

	pPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_SOUTH);
	if ((pPlot != NULL) && pPlot->isWOfRiver())
	{
		return true;
	}

	return false;
}


bool CvPlot::isRiverCrossingFlowClockwise(DirectionTypes eDirection) const
{
	CvPlot *pPlot;
	switch(eDirection)
	{
	case DIRECTION_NORTH:
		pPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_NORTH);
		if (pPlot != NULL)
		{
			return (pPlot->getRiverWEDirection() == CARDINALDIRECTION_EAST);
		}
		break;
	case DIRECTION_EAST:
		return (getRiverNSDirection() == CARDINALDIRECTION_SOUTH);
		break;
	case DIRECTION_SOUTH:
		return (getRiverWEDirection() == CARDINALDIRECTION_WEST);
		break;
	case DIRECTION_WEST:
		pPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_WEST);
		if(pPlot != NULL)
		{
			return (pPlot->getRiverNSDirection() == CARDINALDIRECTION_NORTH);
		}
		break;
	default:
		FAssert(false);
		break;
	}

	return false;
}


bool CvPlot::isRiverSide() const
{
	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
	{
		pLoopPlot = plotCardinalDirection(getX_INLINE(), getY_INLINE(), ((CardinalDirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if (isRiverCrossing(directionXY(this, pLoopPlot)))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isRiver() const
{
	return (getRiverCrossingCount() > 0);
}


bool CvPlot::isRiverConnection(DirectionTypes eDirection) const
{
	if (eDirection == NO_DIRECTION)
	{
		return false;
	}

	switch (eDirection)
	{
	case DIRECTION_NORTH:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_NORTHEAST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_EAST));
		break;

	case DIRECTION_EAST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_SOUTH));
		break;

	case DIRECTION_SOUTHEAST:
		return (isRiverCrossing(DIRECTION_SOUTH) || isRiverCrossing(DIRECTION_EAST));
		break;

	case DIRECTION_SOUTH:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_SOUTHWEST:
		return (isRiverCrossing(DIRECTION_SOUTH) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_WEST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_SOUTH));
		break;

	case DIRECTION_NORTHWEST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_WEST));
		break;

	default:
		FAssert(false);
		break;
	}

	return false;
}


CvPlot* CvPlot::getNearestLandPlotInternal(int iDistance) const
{
	if (iDistance > GC.getMapINLINE().getGridHeightINLINE() && iDistance > GC.getMapINLINE().getGridWidthINLINE())
	{
		return NULL;
	}

	for (int iDX = -iDistance; iDX <= iDistance; iDX++)
	{
		for (int iDY = -iDistance; iDY <= iDistance; iDY++)
		{
			if (abs(iDX) + abs(iDY) == iDistance)
			{
				CvPlot* pPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);
				if (pPlot != NULL)
				{
					if (!pPlot->isWater())
					{
						return pPlot;
					}
				}
			}
		}
	}
	return getNearestLandPlotInternal(iDistance + 1);
}


int CvPlot::getNearestLandArea() const
{
	CvPlot* pPlot = getNearestLandPlot();
	return pPlot ? pPlot->getArea() : -1;
}


CvPlot* CvPlot::getNearestLandPlot() const
{
	return getNearestLandPlotInternal(0);
}


int CvPlot::seeFromLevel(TeamTypes eTeam) const
{
	int iLevel;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iLevel = GC.getTerrainInfo(getTerrainType()).getSeeFromLevel();

	if (isPeak())
	{
		iLevel += GC.getPEAK_SEE_FROM_CHANGE();
	}

	if (isHills())
	{
		iLevel += GC.getHILLS_SEE_FROM_CHANGE();
	}

	if (isWater())
	{
		iLevel += GC.getSEAWATER_SEE_FROM_CHANGE();

		if (GET_TEAM(eTeam).isExtraWaterSeeFrom())
		{
			iLevel++;
		}
	}

	return iLevel;
}


int CvPlot::seeThroughLevel() const
{
	int iLevel;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iLevel = GC.getTerrainInfo(getTerrainType()).getSeeThroughLevel();

	if (getFeatureType() != NO_FEATURE)
	{
		iLevel += GC.getFeatureInfo(getFeatureType()).getSeeThroughChange();
	}

	if (isPeak())
	{
		iLevel += GC.getPEAK_SEE_THROUGH_CHANGE();
	}

	if (isHills())
	{
		iLevel += GC.getHILLS_SEE_THROUGH_CHANGE();
	}

	if (isWater())
	{
		iLevel += GC.getSEAWATER_SEE_FROM_CHANGE();
	}

	return iLevel;
}



void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, CvUnit* pUnit, bool bUpdatePlotGroups)
{
	bool bAerial = (pUnit != NULL && pUnit->getDomainType() == DOMAIN_AIR);

	DirectionTypes eFacingDirection = NO_DIRECTION;
	if (!bAerial && NULL != pUnit)
	{
		eFacingDirection = pUnit->getFacingDirection(true);
	}

	//fill invisible types
	std::vector<InvisibleTypes> aSeeInvisibleTypes;
	if (NULL != pUnit)
	{
		for(int i=0;i<pUnit->getNumSeeInvisibleTypes();i++)
		{
			aSeeInvisibleTypes.push_back(pUnit->getSeeInvisibleType(i));
		}
	}

	if(aSeeInvisibleTypes.size() == 0)
	{
		aSeeInvisibleTypes.push_back(NO_INVISIBLE);
	}

	//check one extra outer ring
	if (!bAerial)
	{
		iRange++;
	}

	for(int i=0;i<(int)aSeeInvisibleTypes.size();i++)
	{
		for (int dx = -iRange; dx <= iRange; dx++)
		{
			for (int dy = -iRange; dy <= iRange; dy++)
			{
				//check if in facing direction
				if (bAerial || shouldProcessDisplacementPlot(dx, dy, iRange - 1, eFacingDirection))
				{
					bool outerRing = false;
					if ((abs(dx) == iRange) || (abs(dy) == iRange))
					{
						outerRing = true;
					}

					//check if anything blocking the plot
					if (bAerial || canSeeDisplacementPlot(eTeam, dx, dy, dx, dy, true, outerRing))
					{
						CvPlot* pPlot = plotXY(getX_INLINE(), getY_INLINE(), dx, dy);
						if (NULL != pPlot)
						{
							pPlot->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), aSeeInvisibleTypes[i], bUpdatePlotGroups);
						}
					}
				}

				if (eFacingDirection != NO_DIRECTION)
				{
					if((abs(dx) <= 1) && (abs(dy) <= 1)) //always reveal adjacent plots when using line of sight
					{
						CvPlot* pPlot = plotXY(getX_INLINE(), getY_INLINE(), dx, dy);
						if (NULL != pPlot)
						{
							pPlot->changeVisibilityCount(eTeam, 1, aSeeInvisibleTypes[i], bUpdatePlotGroups);
							pPlot->changeVisibilityCount(eTeam, -1, aSeeInvisibleTypes[i], bUpdatePlotGroups);
						}
					}
				}
			}
		}
	}
}

bool CvPlot::canSeePlot(CvPlot *pPlot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const
{
	iRange++;

	if (pPlot == NULL)
	{
		return false;
	}

	//find displacement
	int dx = pPlot->getX() - getX();
	int dy = pPlot->getY() - getY();
	dx = dxWrap(dx); //world wrap
	dy = dyWrap(dy);

	//check if in facing direction
	if (shouldProcessDisplacementPlot(dx, dy, iRange - 1, eFacingDirection))
	{
		bool outerRing = false;
		if ((abs(dx) == iRange) || (abs(dy) == iRange))
		{
			outerRing = true;
		}

		//check if anything blocking the plot
		if (canSeeDisplacementPlot(eTeam, dx, dy, dx, dy, true, outerRing))
		{
			return true;
		}
	}

	return false;
}

bool CvPlot::canSeeDisplacementPlot(TeamTypes eTeam, int dx, int dy, int originalDX, int originalDY, bool firstPlot, bool outerRing) const
{
	CvPlot *pPlot = plotXY(getX_INLINE(), getY_INLINE(), dx, dy);
	if (pPlot != NULL)
	{
		//base case is current plot
		if((dx == 0) && (dy == 0))
		{
			return true;
		}

		//find closest of three points (1, 2, 3) to original line from Start (S) to End (E)
		//The diagonal is computed first as that guarantees a change in position
		// -------------
		// |   | 2 | S |
		// -------------
		// | E | 1 | 3 |
		// -------------

		int displacements[3][2] = {{dx - getSign(dx), dy - getSign(dy)}, {dx - getSign(dx), dy}, {dx, dy - getSign(dy)}};
		int allClosest[3];
		int closest = -1;
		for (int i=0;i<3;i++)
		{
			//int tempClosest = abs(displacements[i][0] * originalDX - displacements[i][1] * originalDY); //more accurate, but less structured on a grid
			allClosest[i] = abs(displacements[i][0] * dy - displacements[i][1] * dx); //cross product
			if((closest == -1) || (allClosest[i] < closest))
			{
				closest = allClosest[i];
			}
		}

		//iterate through all minimum plots to see if any of them are passable
		for(int i=0;i<3;i++)
		{
			int nextDX = displacements[i][0];
			int nextDY = displacements[i][1];
			if((nextDX != dx) || (nextDY != dy)) //make sure we change plots
			{
				if(allClosest[i] == closest)
				{
					if(canSeeDisplacementPlot(eTeam, nextDX, nextDY, originalDX, originalDY, false, false))
					{
						int fromLevel = seeFromLevel(eTeam);
						int throughLevel = pPlot->seeThroughLevel();
						if(outerRing) //check strictly higher level
						{
							CvPlot *passThroughPlot = plotXY(getX_INLINE(), getY_INLINE(), nextDX, nextDY);
							int passThroughLevel = passThroughPlot->seeThroughLevel();
							if (fromLevel >= passThroughLevel)
							{
								if((fromLevel > passThroughLevel) || (pPlot->seeFromLevel(eTeam) > fromLevel)) //either we can see through to it or it is high enough to see from far
								{
									return true;
								}
							}
						}
						else
						{
							if(fromLevel >= throughLevel) //we can clearly see this level
							{
								return true;
							}
							else if(firstPlot) //we can also see it if it is the first plot that is too tall
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool CvPlot::shouldProcessDisplacementPlot(int dx, int dy, int range, DirectionTypes eFacingDirection) const
{
	if(eFacingDirection == NO_DIRECTION)
	{
		return true;
	}
	else if((dx == 0) && (dy == 0)) //always process this plot
	{
		return true;
	}
	else
	{
		//							N		NE		E		SE			S		SW		W			NW
		int displacements[8][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

		int directionX = displacements[eFacingDirection][0];
		int directionY = displacements[eFacingDirection][1];

		//compute angle off of direction
		int crossProduct = directionX * dy - directionY * dx; //cross product
		int dotProduct = directionX * dx + directionY * dy; //dot product

		float theta = atan2((float) crossProduct, (float) dotProduct);
		float spread = 60 * (float) M_PI / 180;
		if((abs(dx) <= 1) && (abs(dy) <= 1)) //close plots use wider spread
		{
			spread = 90 * (float) M_PI / 180;
		}

		if((theta >= -spread / 2) && (theta <= spread / 2))
		{
			return true;
		}
		else
		{
			return false;
		}

		/*
		DirectionTypes leftDirection = GC.getTurnLeftDirection(eFacingDirection);
		DirectionTypes rightDirection = GC.getTurnRightDirection(eFacingDirection);

		//test which sides of the line equation (cross product)
		int leftSide = displacements[leftDirection][0] * dy - displacements[leftDirection][1] * dx;
		int rightSide = displacements[rightDirection][0] * dy - displacements[rightDirection][1] * dx;
		if((leftSide <= 0) && (rightSide >= 0))
			return true;
		else
			return false;
		*/
	}
}

void CvPlot::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
//	CvCity* pHolyCity;
	CvUnit* pLoopUnit;
	int iLoop;
	int iI;

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		// Religion - Disabled with new Espionage System
/*		for (iI = 0; iI < GC.getNumReligionInfos(); ++iI)
		{
			if (pCity->isHasReligion((ReligionTypes)iI))
			{
				pHolyCity = GC.getGameINLINE().getHolyCity((ReligionTypes)iI);

				if (pHolyCity != NULL)
				{
					if (GET_PLAYER(pHolyCity->getOwnerINLINE()).getStateReligion() == iI)
					{
						changeAdjacentSight(pHolyCity->getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
					}
				}
			}
		}*/

		// Vassal
		for (iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
			{
				changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
			}
		}

		// EspionageEffect
		for (iI = 0; iI < MAX_CIV_TEAMS; ++iI)
		{
			if (pCity->getEspionageVisibility((TeamTypes)iI))
			{
				// Passive Effect: enough EPs gives you visibility into someone's cities
				changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
			}
		}
	}

	// Owned
	if (isOwned())
	{
		changeAdjacentSight(getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
	}

	pUnitNode = headUnitNode();

	// Unit
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);


		changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->visibilityRange(), bIncrement, pLoopUnit, bUpdatePlotGroups);
	}

	if (getReconCount() > 0)
	{
		int iRange = GC.getDefineINT("RECON_VISIBILITY_RANGE");
		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
			{
				if (pLoopUnit->getReconPlot() == this)
				{
					changeAdjacentSight(pLoopUnit->getTeam(), iRange, bIncrement, pLoopUnit, bUpdatePlotGroups);
				}
			}
		}
	}
}


void CvPlot::updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;

	int iRange = GC.getDefineINT("UNIT_VISIBILITY_RANGE") + 1;
	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
	{
		iRange += GC.getPromotionInfo((PromotionTypes)iPromotion).getVisibilityChange();
	}

	iRange = std::max(GC.getDefineINT("RECON_VISIBILITY_RANGE") + 1, iRange);

	for (iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (iDY = -iRange; iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				pLoopPlot->updateSight(bIncrement, bUpdatePlotGroups);
			}
		}
	}
}


bool CvPlot::canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eBonus == NO_BONUS)
	{
		return true;
	}

	if (getBonusType() != NO_BONUS)
	{
		return false;
	}

	if (isPeak())
	{
		return false;
	}

	if (getFeatureType() != NO_FEATURE)
	{
		if (!(GC.getBonusInfo(eBonus).isFeature(getFeatureType())))
		{
			return false;
		}

		if (!(GC.getBonusInfo(eBonus).isFeatureTerrain(getTerrainType())))
		{
			return false;
		}
	}
	else
	{
		if (!(GC.getBonusInfo(eBonus).isTerrain(getTerrainType())))
		{
			return false;
		}
	}

	if (isHills())
	{
		if (!(GC.getBonusInfo(eBonus).isHills()))
		{
			return false;
		}
	}
	else if (isFlatlands())
	{
		if (!(GC.getBonusInfo(eBonus).isFlatlands()))
		{
			return false;
		}
	}

	if (GC.getBonusInfo(eBonus).isNoRiverSide())
	{
		if (isRiverSide())
		{
			return false;
		}
	}

	if (GC.getBonusInfo(eBonus).getMinAreaSize() != -1)
	{
		if (area()->getNumTiles() < GC.getBonusInfo(eBonus).getMinAreaSize())
		{
			return false;
		}
	}

	if (!bIgnoreLatitude)
	{
		if (getLatitude() > GC.getBonusInfo(eBonus).getMaxLatitude())
		{
			return false;
		}

		if (getLatitude() < GC.getBonusInfo(eBonus).getMinLatitude())
		{
			return false;
		}
	}

	if (!isPotentialCityWork())
	{
		return false;
	}

	return true;
}


bool CvPlot::canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam, bool bPotential) const
{
	CvPlot* pLoopPlot;
	bool bValid, bMexico;
	int iI;

	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	bValid = false;
	bMexico = false;

	if (isCity())
	{
		return false;
	}

	if (isImpassable())
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isWater() != isWater())
	{
		return false;
	}

	// Leoreth: Mexican UP (Arid Agriculture): can build farms on hills
	if (eTeam == AZTECS && GET_PLAYER((PlayerTypes)AZTECS).isReborn() && eImprovement == GC.getInfoTypeForString("IMPROVEMENT_FARM") && getTerrainType() != GC.getInfoTypeForString("TERRAIN_DESERT"))
	{
		bMexico = true;
	}

	// Leoreth: different fishing boats for different sea levels
	if (GC.getImprovementInfo(eImprovement).isWater())
	{
		if (eImprovement == GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS") && getTerrainType() != GC.getInfoTypeForString("TERRAIN_COAST")) return false;
		if (eImprovement == GC.getInfoTypeForString("IMPROVEMENT_OCEAN_FISHERY") && getTerrainType() != GC.getInfoTypeForString("TERRAIN_OCEAN")) return false;
	}

	if (getFeatureType() != NO_FEATURE)
	{
		// Leoreth: unless the feature makes valid
		if (GC.getFeatureInfo(getFeatureType()).isNoImprovement() && !GC.getImprovementInfo(eImprovement).getFeatureMakesValid(getFeatureType()))
		{
			return false;
		}
	}

	if ((getBonusType(eTeam) != NO_BONUS) && GC.getImprovementInfo(eImprovement).isImprovementBonusMakesValid(getBonusType(eTeam)))
	{
		return true;
	}

	if (GC.getImprovementInfo(eImprovement).isNoFreshWater() && isFreshWater())
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isRequiresFlatlands() && !isFlatlands() && !bMexico) // Mexican UP
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isRequiresFeature() && (getFeatureType() == NO_FEATURE))
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isHillsMakesValid() && isHills())
	{
		bValid = true;
	}

	if (GC.getImprovementInfo(eImprovement).isFreshWaterMakesValid() && isFreshWater())
	{
		bValid = true;
	}

	if (GC.getImprovementInfo(eImprovement).isRiverSideMakesValid() && isRiverSide())
	{
		bValid = true;
	}

	if (GC.getImprovementInfo(eImprovement).getTerrainMakesValid(getTerrainType()))
	{
		bValid = true;
	}

	if ((getFeatureType() != NO_FEATURE) && GC.getImprovementInfo(eImprovement).getFeatureMakesValid(getFeatureType()))
	{
		bValid = true;
	}

	if (!bValid)
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isRequiresRiverSide())
	{
		bValid = false;

		for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotCardinalDirection(getX_INLINE(), getY_INLINE(), ((CardinalDirectionTypes)iI));

			if (pLoopPlot != NULL)
			{
				if (isRiverCrossing(directionXY(this, pLoopPlot)))
				{
					if (pLoopPlot->getImprovementType() != eImprovement)
					{
						bValid = true;
						break;
					}
				}
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (calculateNatureYield(((YieldTypes)iI), eTeam) < GC.getImprovementInfo(eImprovement).getPrereqNatureYield(iI) && !bMexico) // Mexican UP
		{
			return false;
		}
	}

	if ((getTeam() == NO_TEAM) || !(GET_TEAM(getTeam()).isIgnoreIrrigation()))
	{
		if (!bPotential && GC.getImprovementInfo(eImprovement).isRequiresIrrigation() && !isIrrigationAvailable())
		{
			return false;
		}
	}

	return true;
}


bool CvPlot::canBuild(BuildTypes eBuild, PlayerTypes ePlayer, bool bTestVisible) const
{
	ImprovementTypes eImprovement;
	ImprovementTypes eFinalImprovementType;
	RouteTypes eRoute;
	bool bValid;

	if(GC.getUSE_CAN_BUILD_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getX_INLINE());
		argsList.add(getY_INLINE());
		argsList.add((int)eBuild);
		argsList.add((int)ePlayer);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canBuild", argsList.makeFunctionArgs(), &lResult);
		if (lResult >= 1)
		{
			return true;
		}
		else if (lResult == 0)
		{
			return false;
		}
	}

	if (eBuild == NO_BUILD)
	{
		return false;
	}

	bValid = false;

	eImprovement = ((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement()));

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (!canHaveImprovement(eImprovement, GET_PLAYER(ePlayer).getTeam(), bTestVisible))
		{
			return false;
		}

		// Leoreth: instantly built improvements not allowed on features to prevent their instant removal
		if (getFeatureType() != NO_FEATURE && GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
		{
			if (GC.getBuildInfo(eBuild).isKill())
			{
				return false;
			}
		}

		// Leoreth: no adjacent acts as city improvements
		CvPlot* pAdjacentPlot;
		if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isActsAsCity())
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					if (pAdjacentPlot->isCity() || (pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pAdjacentPlot->getImprovementType()).isActsAsCity()))
					{
						return false;
					}
				}
			}
		}

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(getImprovementType()).isPermanent())
			{
				return false;
			}

			if (getImprovementType() == eImprovement)
			{
				return false;
			}

			eFinalImprovementType = finalImprovementUpgrade(getImprovementType());

			if (eFinalImprovementType != NO_IMPROVEMENT)
			{
				if (eFinalImprovementType == finalImprovementUpgrade(eImprovement))
				{
					return false;
				}
			}
		}

		if (!bTestVisible)
		{
			if (GET_PLAYER(ePlayer).getTeam() != getTeam())
			{
				//outside borders can't be built in other's culture
				if (GC.getImprovementInfo(eImprovement).isOutsideBorders())
				{
					if (getTeam() != NO_TEAM)
					{
						return false;
					}
				}
				else //only buildable in own culture
				{
					return false;
				}
			}
		}

		bValid = true;
	}

	eRoute = ((RouteTypes)(GC.getBuildInfo(eBuild).getRoute()));

	if (eRoute != NO_ROUTE)
	{
		if (getRouteType() != NO_ROUTE)
		{
			if (getRouteType() == eRoute)
			{
				return false;
			}

			if (GC.getRouteInfo(getRouteType()).getValue() > GC.getRouteInfo(eRoute).getValue())
			{
				return false;
			}

			if (ePlayer != GC.getGame().getActivePlayer() && GC.getRouteInfo(getRouteType()).getValue() == GC.getRouteInfo(eRoute).getValue())
			{
				return false;
			}
		}

		if (!bTestVisible)
		{
			if (GC.getRouteInfo(eRoute).getPrereqBonus() != NO_BONUS)
			{
				if (!isAdjacentPlotGroupConnectedBonus(ePlayer, ((BonusTypes)(GC.getRouteInfo(eRoute).getPrereqBonus()))))
				{
					return false;
				}
			}

			bool bFoundValid = true;
			for (int i = 0; i < GC.getNUM_ROUTE_PREREQ_OR_BONUSES(); ++i)
			{
				if (NO_BONUS != GC.getRouteInfo(eRoute).getPrereqOrBonus(i))
				{
					bFoundValid = false;

					if (isAdjacentPlotGroupConnectedBonus(ePlayer, ((BonusTypes)(GC.getRouteInfo(eRoute).getPrereqOrBonus(i)))))
					{
						bFoundValid = true;
						break;
					}
				}
			}

			if (!bFoundValid)
			{
				return false;
			}
		}

		bValid = true;
	}

	if (getFeatureType() != NO_FEATURE)
	{
		if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
		{
			if (isOwned() && (GET_PLAYER(ePlayer).getTeam() != getTeam()) && !atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
			{
				return false;
			}

			bValid = true;
		}
	}

	return bValid;
}


int CvPlot::getBuildTime(BuildTypes eBuild) const
{
	int iTime;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iTime = GC.getBuildInfo(eBuild).getTime();

	if (getFeatureType() != NO_FEATURE)
	{
		iTime += GC.getBuildInfo(eBuild).getFeatureTime(getFeatureType());
	}

	iTime *= std::max(0, (GC.getTerrainInfo(getTerrainType()).getBuildModifier() + 100));
	iTime /= 100;

	iTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getBuildPercent();
	iTime /= 100;

	iTime *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getBuildPercent();
	iTime /= 100;

//KNOEDELstart
	if (getFeatureType() != NO_FEATURE)
		if (GC.getGame().getActivePlayer() == KHMER && GC.getBuildInfo(eBuild).getImprovement() == GC.getInfoTypeForString("IMPROVEMENT_FARM") && getFeatureType() == GC.getInfoTypeForString("FEATURE_RAINFOREST"))
			iTime /= 2;
//KNOEDELend

	return iTime;
}


// BUG - Partial Builds - start
int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	int iWorkRate = GET_PLAYER(ePlayer).getWorkRate(eBuild);
	if (iWorkRate > 0)
	{
		return getBuildTurnsLeft(eBuild, iWorkRate, iWorkRate, false);
	}
	else
	{
		return MAX_INT;
	}
}
// BUG - Partial Builds - end

// BUG - Partial Builds - start
int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, int iNowExtra, int iThenExtra, bool bIncludeUnits) const
// BUG - Partial Builds - end
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iNowBuildRate;
	int iThenBuildRate;
	int iBuildLeft;
	int iTurnsLeft;

	iNowBuildRate = iNowExtra;
	iThenBuildRate = iThenExtra;

// BUG - Partial Builds - start
	if (bIncludeUnits)
	{
// BUG - Partial Builds - end
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->getBuildType() == eBuild)
			{
				if (pLoopUnit->canMove())
				{
					iNowBuildRate += pLoopUnit->workRate(false);
				}
				iThenBuildRate += pLoopUnit->workRate(true);
			}
		}
// BUG - Partial Builds - start
	}
// BUG - Partial Builds - end

	if (iThenBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return MAX_INT;
	}

	iBuildLeft = getBuildTime(eBuild);

	iBuildLeft -= getBuildProgress(eBuild);
	iBuildLeft -= iNowBuildRate;

	iBuildLeft = std::max(0, iBuildLeft);

	iTurnsLeft = (iBuildLeft / iThenBuildRate);

	if ((iTurnsLeft * iThenBuildRate) < iBuildLeft)
	{
		iTurnsLeft++;
	}

	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}


int CvPlot::getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CvCity** ppCity) const
{
	int iProduction;

	if (getFeatureType() == NO_FEATURE)
	{
		return 0;
	}

	*ppCity = getWorkingCity();

	if (*ppCity == NULL)
	{
		*ppCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), NO_PLAYER, eTeam, false);
	}

	if (*ppCity == NULL)
	{
		return 0;
	}

	iProduction = (GC.getBuildInfo(eBuild).getFeatureProduction(getFeatureType()) - (std::max(0, (plotDistance(getX_INLINE(), getY_INLINE(), (*ppCity)->getX_INLINE(), (*ppCity)->getY_INLINE()) - 2)) * 5));

	iProduction *= std::max(0, (GET_PLAYER((*ppCity)->getOwnerINLINE()).getFeatureProductionModifier() + 100));
	iProduction /= 100;

	iProduction *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFeatureProductionPercent();
	iProduction /= 100;

	iProduction *= std::min((GC.getDefineINT("BASE_FEATURE_PRODUCTION_PERCENT") + (GC.getDefineINT("FEATURE_PRODUCTION_PERCENT_MULTIPLIER") * (*ppCity)->getPopulation())), 100);
	iProduction /= 100;

	if (getTeam() != eTeam)
	{
		iProduction *= GC.getDefineINT("DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT");
		iProduction /= 100;
	}

	return std::max(0, iProduction);
}


CvUnit* CvPlot::getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;

	pBestUnit = NULL;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwnerINLINE() == eOwner))
		{
			if ((eAttackingPlayer == NO_PLAYER) || !(pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false)))
			{
				if (!bTestAtWar || eAttackingPlayer == NO_PLAYER || pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwnerINLINE()).getTeam(), this)))
				{
					if (!bTestPotentialEnemy || (eAttackingPlayer == NO_PLAYER) ||  pLoopUnit->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isPotentialEnemy(GET_PLAYER(pLoopUnit->getOwnerINLINE()).getTeam(), this)))
					{
						if (!bTestCanMove || (pLoopUnit->canMove() && !(pLoopUnit->isCargo())))
						{
							if ((pAttacker == NULL) || (pAttacker->getDomainType() != DOMAIN_AIR) || (pLoopUnit->getDamage() < pAttacker->airCombatLimit()))
							{
								if (pLoopUnit->isBetterDefenderThan(pBestUnit, pAttacker))
								{
									pBestUnit = pLoopUnit;
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

// returns a sum of the strength (adjusted by firepower) of all the units on a plot
int CvPlot::AI_sumStrength(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, DomainTypes eDomainType, bool bDefensiveBonuses, bool bTestAtWar, bool bTestPotentialEnemy) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int	strSum = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwnerINLINE() == eOwner))
		{
			if ((eAttackingPlayer == NO_PLAYER) || !(pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false)))
			{
				if (!bTestAtWar || (eAttackingPlayer == NO_PLAYER) || atWar(GET_PLAYER(eAttackingPlayer).getTeam(), pLoopUnit->getTeam()))
				{
					if (!bTestPotentialEnemy || (eAttackingPlayer == NO_PLAYER) || pLoopUnit->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this))
					{
						// we may want to be more sophisticated about domains
						// somewhere we need to check to see if this is a city, if so, only land units can defend here, etc
						if (eDomainType == NO_DOMAIN || (pLoopUnit->getDomainType() == eDomainType))
						{
							const CvPlot* pPlot = NULL;

							if (bDefensiveBonuses)
								pPlot = this;

							strSum += pLoopUnit->currEffectiveStr(pPlot, NULL);
						}
					}
				}
			}
		}
	}

	return strSum;
}


CvUnit* CvPlot::getSelectedUnit() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->IsSelected())
		{
			return pLoopUnit;
		}
	}

	return NULL;
}


int CvPlot::getUnitPower(PlayerTypes eOwner) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwnerINLINE() == eOwner))
		{
			iCount += pLoopUnit->getUnitInfo().getPowerValue();
		}
	}

	return iCount;
}


int CvPlot::defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding, bool bHelp) const
{
	CvCity* pCity;
	ImprovementTypes eImprovement;
	int iModifier;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	// Leoreth: feature affects defense only without improvement	//KNOEDEL: nah, let's not do that
//	iModifier = ((getFeatureType() == NO_FEATURE || getImprovementType() != NO_IMPROVEMENT) ? GC.getTerrainInfo(getTerrainType()).getDefenseModifier() : GC.getFeatureInfo(getFeatureType()).getDefenseModifier());
	iModifier = ((getFeatureType() == NO_FEATURE) ? GC.getTerrainInfo(getTerrainType()).getDefenseModifier() : GC.getFeatureInfo(getFeatureType()).getDefenseModifier());

	if (isHills())
	{
		iModifier += GC.getHILLS_EXTRA_DEFENSE();
	}

	if (bHelp)
	{
		eImprovement = getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), false);
	}
	else
	{
		eImprovement = getImprovementType();
	}

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (eDefender != NO_TEAM && (getTeam() == NO_TEAM || GET_TEAM(eDefender).isFriendlyTerritory(getTeam())))
		{
			iModifier += GC.getImprovementInfo(eImprovement).getDefenseModifier();
		}
	}

	if (!bHelp)
	{
		pCity = getPlotCity();

		if (pCity != NULL)
		{
			iModifier += pCity->getDefenseModifier(bIgnoreBuilding);
		}
	}

	return iModifier;
}

//KNOEDELstart
// byFra
bool CvPlot::isCrossRiverMovement(const CvUnit* pUnit, const CvPlot* pFromPlot, bool bDebug) const
{
	if (bDebug)
	{
		CvWString szDebug;
		szDebug.Format(L"isCrossRiverMovement, from (%d,%d) to (%d,%d)", pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), getX_INLINE(), getY_INLINE());
		gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), szDebug, "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
	}

	if (!isRiver() && !pFromPlot->isRiver())
	{
		if (bDebug) gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), L"false 1", "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));

		return false;
	}

	if (isWater() || pFromPlot->isWater())
	{
		if (bDebug) gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), L"false 2", "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
	
		return false;
	}

	if (pFromPlot->isRiverCrossing(directionXY(pFromPlot, this)))
	{
		if (getOwnerINLINE() == NO_PLAYER || pFromPlot->getOwnerINLINE() == NO_PLAYER
			|| getTeam() == NO_TEAM || pFromPlot->getTeam() == NO_TEAM)
		{
			if (bDebug) gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), L"true 3", "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
		
			return true;
		}
		if (getRouteType() != NO_ROUTE && pFromPlot->getRouteType() != NO_ROUTE)
		{
			if ((pUnit->getTeam() == getTeam() || GET_TEAM(pUnit->getTeam()).isOpenBorders(getTeam())) && GET_TEAM(getTeam()).isBridgeBuilding())
			{
				if (getTeam() == pFromPlot->getTeam() || (pUnit->getTeam() == pFromPlot->getTeam() || GET_TEAM(pUnit->getTeam()).isOpenBorders(pFromPlot->getTeam()) && GET_TEAM(pFromPlot->getTeam()).isBridgeBuilding()))
				{
					if (bDebug) gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), L"false 4", "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
				
					return false;
				}
			}
		}

		if (bDebug) gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), L"true 5", "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));

		return true;
	}

	if (bDebug) gDLL->getInterfaceIFace()->addMessage((PlayerTypes)0, false, GC.getEVENT_MESSAGE_TIME(), L"false 6", "AS2D_WELOVEKING", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));

	return false;
}
//KNOEDELend

int CvPlot::movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot) const
{
	int iRegularCost;
	int iRouteCost;
	int iRouteFlatCost;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (pUnit->flatMovementCost() || (pUnit->getDomainType() == DOMAIN_AIR))
	{
		return GC.getMOVE_DENOMINATOR();
	}

	if (pUnit->isHuman())
	{
		if (!isRevealed(pUnit->getTeam(), false))
		{
			return pUnit->maxMoves();
		}
	}

	if (!pFromPlot->isValidDomainForLocation(*pUnit))
	{
		return pUnit->maxMoves();
	}

	// Leoreth: entering enemy waters ends the turn
	if (isWater())
	{
		if (getOwner() != NO_PLAYER)
		{
			if (getOwner() != pUnit->getOwner())
			{
				if (atWar(getTeam(), pUnit->getTeam()))
				{
					if (pFromPlot->getTeam() == NO_TEAM || !atWar(pUnit->getTeam(), pFromPlot->getTeam()))
					{
						return pUnit->maxMoves();
					}
				}
			}
		}
	}

	if (!isValidDomainForAction(*pUnit))
	{
		return GC.getMOVE_DENOMINATOR();
	}

	FAssert(pUnit->getDomainType() != DOMAIN_IMMOBILE);

	if (pUnit->ignoreTerrainCost())
	{
		iRegularCost = 1;
	}
	else
	{
		iRegularCost = ((getFeatureType() == NO_FEATURE) ? GC.getTerrainInfo(getTerrainType()).getMovementCost() : GC.getFeatureInfo(getFeatureType()).getMovementCost());

		if (isHills())
		{
			iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
		}

		// Leoreth: Great Wall effect (+1 movement cost for enemies within the great wall)
		if (isWithinGreatWall() && isOwned())
		{
			if (GET_PLAYER(getOwnerINLINE()).isHasBuildingEffect((BuildingTypes)GREAT_WALL) && GET_TEAM((TeamTypes)getOwnerINLINE()).isAtWar((TeamTypes)pUnit->getOwner()))
			{
				iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
			}
		}

		if (iRegularCost > 0)
		{
			iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));
		}
	}

//KNOEDELstart
	// byFra
	if (!pUnit->ignoreTerrainCost() || !pUnit->isRiver())
	{
		if (isCrossRiverMovement(pUnit, pFromPlot))
		{
			return pUnit->maxMoves();
		}
	}
//KNOEDELend

	bool bHasTerrainCost = (iRegularCost > 1);

	iRegularCost = std::min(iRegularCost, pUnit->baseMoves());

	iRegularCost *= GC.getMOVE_DENOMINATOR();
//KNOEDELstart: I don't think there's a need for oceans speeding up ships when ships now have so much movement anyway
/*	//Rhye - start
	if (getTerrainType() == TERRAIN_OCEAN)
	{
		// Leoreth: reduced movement cost only for units that could enter ocean on their own
		if (!pUnit->getUnitInfo().getTerrainImpassable(getTerrainType()) || (pUnit->getUnitInfo().getTerrainPassableTech(getTerrainType()) != NO_TECH && GET_TEAM(pUnit->getTeam()).isHasTech((TechTypes)pUnit->getUnitInfo().getTerrainPassableTech(getTerrainType()))))
		{
			iRegularCost /= 2;
		}
	}
	//Rhye - end*/
//KNOEDELend
	if (bHasTerrainCost)
	{
		// Leoreth: terrain double move only when there are no hills
		if (((getFeatureType() == NO_FEATURE) ? (pUnit->isTerrainDoubleMove(getTerrainType()) && !isHills()) : pUnit->isFeatureDoubleMove(getFeatureType())) || (isHills() && pUnit->isHillsDoubleMove()))
		{
			iRegularCost /= 2;
		}
	}

	if (pFromPlot->isValidRoute(pUnit) && isValidRoute(pUnit) && ((GET_TEAM(pUnit->getTeam()).isBridgeBuilding() || !(pFromPlot->isRiverCrossing(directionXY(pFromPlot, this))))))
	{
		iRouteCost = std::max((GC.getRouteInfo(pFromPlot->getRouteType()).getMovementCost() + GET_TEAM(pUnit->getTeam()).getRouteChange(pFromPlot->getRouteType())),
			               (GC.getRouteInfo(getRouteType()).getMovementCost() + GET_TEAM(pUnit->getTeam()).getRouteChange(getRouteType())));
		iRouteFlatCost = std::max((GC.getRouteInfo(pFromPlot->getRouteType()).getFlatMovementCost() * pUnit->baseMoves()),
			                   (GC.getRouteInfo(getRouteType()).getFlatMovementCost() * pUnit->baseMoves()));
	}
	else
	{
		iRouteCost = MAX_INT;
		iRouteFlatCost = MAX_INT;
	}

	//Rhye - start UP
	// Leoreth: UP not for Italy
	// Leoreth: UP replaced
	/*if (getOwnerINLINE() == ROME && !GET_PLAYER((PlayerTypes)ROME).isReborn())
	{
		iRouteCost /= 2;
		iRouteFlatCost /= 2;
	}*/
	//Rhye - end UP
//KNOEDELstart: extra movement along rivers
	if ((!pUnit->isEnemy(getTeam(), this) || pUnit->isEnemyRoute()) && !pFromPlot->isRiverCrossing(directionXY(pFromPlot, this)) && pFromPlot->isRiverSide() && isRiverSide() && GET_TEAM(pUnit->getTeam()).isRiverTrade())
    {
        if (pUnit->baseMoves() == 1)
        {
            iRegularCost /= 2;
        }
		else if (pUnit->baseMoves() == 2)
        {
            iRegularCost /= 4;
            iRegularCost *= 3;
        }
        else
        {
            iRegularCost /= 10;
            iRegularCost *= 9;
        }
    }

	return std::max(1, std::min(iRegularCost, std::min(iRouteCost, iRouteFlatCost)));
//KNOEDELend
}

int CvPlot::getExtraMovePathCost() const
{
	return GC.getGameINLINE().getPlotExtraCost(getX_INLINE(), getY_INLINE());
}


void CvPlot::changeExtraMovePathCost(int iChange)
{
	GC.getGameINLINE().changePlotExtraCost(getX_INLINE(), getY_INLINE(), iChange);
}

bool CvPlot::isAdjacentOwned() const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isOwned())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getOwnerINLINE() == ePlayer)
			{
				if (!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentTeam(TeamTypes eTeam, bool bLandOnly) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getTeam() == eTeam)
			{
				if (!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isWithinCultureRange(PlayerTypes ePlayer) const
{
	int iI;

	for (iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
	{
		if (isCultureRangeCity(ePlayer, iI))
		{
			return true;
		}
	}

	return false;
}


int CvPlot::getNumCultureRangeCities(PlayerTypes ePlayer) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
	{
		iCount += getCultureRangeCities(ePlayer, iI);
	}

	return iCount;
}


PlayerTypes CvPlot::calculateCulturalOwner() const
{
	PROFILE("CvPlot::calculateCulturalOwner()")

	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pLoopPlot;
	PlayerTypes eBestPlayer;
	bool bValid;
	int iCulture;
	int iBestCulture;
	int iPriority;
	int iBestPriority;
	int iI;

	if (isForceUnowned())
	{
		return NO_PLAYER;
	}

	iBestCulture = 0;
	eBestPlayer = NO_PLAYER;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCulture = getCulture((PlayerTypes)iI);

			if (iCulture > 0)
			{
				// All major civilizations have easier control over their own core (80% rule)
				if (iI < NUM_MAJOR_PLAYERS) if (isCore((PlayerTypes)iI)) iCulture *= 4;

				// Independents get the same advantage over a civ's core if that civ is dead
				if (iI == INDEPENDENT || iI == INDEPENDENT2)
				{
					for (int iJ = 0; iJ < NUM_MAJOR_PLAYERS; iJ++)
					{
						if (isCore((PlayerTypes)iI) && GC.getGame().getGameTurnYear() > GET_PLAYER((PlayerTypes)iJ).getBirthYear() && !GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							iCulture *= 4;
							break;
						}
					}
				}

				bool bCanCover = false;
				if (getBonusType() >= 0 && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasTech((TechTypes)GC.getBonusInfo(getBonusType()).getTechReveal())) bCanCover = true;

				CvPlot* pTempPlot;
				for (int iJ = 0; iJ < NUM_CARDINALDIRECTION_TYPES; iJ++)
				{
					if (bCanCover) break;
					pTempPlot = plotCardinalDirection(getX(), getY(), (CardinalDirectionTypes)iJ);
					if (pTempPlot != NULL && pTempPlot->isWithinCultureRange((PlayerTypes)iI)) bCanCover = true;
				}

				if (bCanCover && isWithinCultureRange((PlayerTypes)iI))
				{
					if ((iCulture > iBestCulture) || ((iCulture == iBestCulture) && (getOwnerINLINE() == iI)))
					{
						iBestCulture = iCulture;
						eBestPlayer = ((PlayerTypes)iI);
					}
				}
			}
		}
	}

	if (!isCity())
	{
		if (eBestPlayer != NO_PLAYER)
		{
			iBestPriority = MAX_INT;
			pBestCity = NULL;

			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopCity = pLoopPlot->getPlotCity();

					if (pLoopCity != NULL)
					{
						if (pLoopCity->getTeam() == GET_PLAYER(eBestPlayer).getTeam() || GET_TEAM(GET_PLAYER(eBestPlayer).getTeam()).isVassal(pLoopCity->getTeam()))
						{
							if (getCulture(pLoopCity->getOwnerINLINE()) > 0)
							{
								if (isWithinCultureRange(pLoopCity->getOwnerINLINE()))
								{
									iPriority = GC.getCityPlotPriority()[iI];

									if (pLoopCity->getTeam() == GET_PLAYER(eBestPlayer).getTeam())
									{
									    if (pBestCity != NULL)
									    {
										if (abs(pLoopCity->getX() - getX()) > 1 || abs(pLoopCity->getY() - getY()) > 1 || abs(pBestCity->getX() - getX()) == 1 || abs(pBestCity->getY() - getY()) == 1) // Leoreth: spare the first ring around the city to help small civs (except if it's the first ring of a master's city
											iPriority += 5; // priority ranges from 0 to 4 -> give priority to Masters of a Vassal
									    }
										else
									    {
										if (abs(pLoopCity->getX() - getX()) > 1 || abs(pLoopCity->getY() - getY()) > 1)
											iPriority += 5;
									    }
									}

									if ((iPriority < iBestPriority) || ((iPriority == iBestPriority) && (pLoopCity->getOwnerINLINE() == eBestPlayer)))
									{
										iBestPriority = iPriority;
										pBestCity = pLoopCity;
									}
								}
							}
						}
					}
				}
			}

			if (pBestCity != NULL)
			{
				eBestPlayer = pBestCity->getOwnerINLINE();
			}
		}
	}

	if (eBestPlayer == NO_PLAYER)
	{
		bValid = true;

		for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotCardinalDirection(getX_INLINE(), getY_INLINE(), ((CardinalDirectionTypes)iI));

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned())
				{
					if (eBestPlayer == NO_PLAYER)
					{
						eBestPlayer = pLoopPlot->getOwnerINLINE();
					}
					else if (eBestPlayer != pLoopPlot->getOwnerINLINE())
					{
						bValid = false;
						break;
					}
				}
				else
				{
					bValid = false;
					break;
				}
			}
		}

		if (!bValid)
		{
			eBestPlayer = NO_PLAYER;
		}
	}

	return eBestPlayer;
}


void CvPlot::plotAction(PlotUnitFunc func, int iData1, int iData2, PlayerTypes eOwner, TeamTypes eTeam)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwnerINLINE() == eOwner))
		{
			if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
			{
				func(pLoopUnit, iData1, iData2);
			}
		}
	}
}


int CvPlot::plotCount(ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwnerINLINE() == eOwner))
		{
			if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
			{
				if ((funcA == NULL) || funcA(pLoopUnit, iData1A, iData2A))
				{
					if ((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B))
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}


CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwnerINLINE() == eOwner))
		{
			if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
			{
				if (funcA(pLoopUnit, iData1A, iData2A))
				{
					if ((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B))
					{
						return pLoopUnit;
					}
				}
			}
		}
	}

	return NULL;
}


bool CvPlot::isOwned() const
{
	return (getOwnerINLINE() != NO_PLAYER);
}


bool CvPlot::isBarbarian() const
{
	return (getOwnerINLINE() == BARBARIAN_PLAYER);
}


bool CvPlot::isRevealedBarbarian() const
{
	return (getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true) == BARBARIAN_PLAYER);
}


bool CvPlot::isVisible(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return true;
	}
	else
	{
		if (eTeam == NO_TEAM)
		{
			return false;
		}

		return ((getVisibilityCount(eTeam) > 0) || (getStolenVisibilityCount(eTeam) > 0));
	}
}


bool CvPlot::isActiveVisible(bool bDebug) const
{
	return isVisible(GC.getGameINLINE().getActiveTeam(), bDebug);
}


bool CvPlot::isVisibleToCivTeam() const
{
	int iI;

	for (iI = 0; iI < MAX_CIV_TEAMS; ++iI)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (isVisible(((TeamTypes)iI), false))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isVisibleToWatchingHuman() const
{
	int iI;

	for (iI = 0; iI < MAX_CIV_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				if (isVisible(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentVisible(TeamTypes eTeam, bool bDebug) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isVisible(eTeam, bDebug))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::isAdjacentNonvisible(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (!pAdjacentPlot->isVisible(eTeam, false))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isGoody(TeamTypes eTeam) const
{
	if ((eTeam != NO_TEAM) && GET_TEAM(eTeam).isBarbarian())
	{
		return false;
	}

	return ((getImprovementType() == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getImprovementType()).isGoody());
}


bool CvPlot::isRevealedGoody(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
	{
		return isGoody();
	}

	if (GET_TEAM(eTeam).isBarbarian())
	{
		return false;
	}

	return ((getRevealedImprovementType(eTeam, false) == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getRevealedImprovementType(eTeam, false)).isGoody());
}


void CvPlot::removeGoody()
{
	setImprovementType(NO_IMPROVEMENT);
}


bool CvPlot::isCity(bool bCheckImprovement, TeamTypes eForTeam) const
{
	if (bCheckImprovement && NO_IMPROVEMENT != getImprovementType())
	{
		if (GC.getImprovementInfo(getImprovementType()).isActsAsCity())
		{
			if (NO_TEAM == eForTeam || (NO_TEAM == getTeam() && GC.getImprovementInfo(getImprovementType()).isOutsideBorders()) || GET_TEAM(eForTeam).isFriendlyTerritory(getTeam()))
			{
				return true;
			}
		}
	}

	return (getPlotCity() != NULL);
}


bool CvPlot::isFriendlyCity(const CvUnit& kUnit, bool bCheckImprovement) const
{
	if (!isCity(bCheckImprovement, kUnit.getTeam()))
	{
		return false;
	}

	if (isVisibleEnemyUnit(&kUnit))
	{
		return false;
	}

	TeamTypes ePlotTeam = getTeam();

	if (NO_TEAM != ePlotTeam)
	{
		if (kUnit.isEnemy(ePlotTeam))
		{
			return false;
		}

		TeamTypes eTeam = GET_PLAYER(kUnit.getCombatOwner(ePlotTeam, this)).getTeam();

		if (eTeam == ePlotTeam)
		{
			return true;
		}

		if (GET_TEAM(eTeam).isOpenBorders(ePlotTeam))
		{
			return true;
		}

		if (GET_TEAM(ePlotTeam).isVassal(eTeam))
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isEnemyCity(const CvUnit& kUnit) const
{
	CvCity* pCity = getPlotCity();

	if (pCity != NULL)
	{
		return kUnit.isEnemy(pCity->getTeam(), this);
	}

	return false;
}


bool CvPlot::isOccupation() const
{
	CvCity* pCity;

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		return pCity->isOccupation();
	}

	return false;
}


bool CvPlot::isBeingWorked() const
{
	CvCity* pWorkingCity;

	pWorkingCity = getWorkingCity();

	if (pWorkingCity != NULL)
	{
		return pWorkingCity->isWorkingPlot(this);
	}

	return false;
}


bool CvPlot::isUnit() const
{
	return (getNumUnits() > 0);
}


bool CvPlot::isInvestigate(TeamTypes eTeam) const
{
	return (plotCheck(PUF_isInvestigate, -1, -1, NO_PLAYER, eTeam) != NULL);
}


bool CvPlot::isVisibleEnemyDefender(const CvUnit* pUnit) const
{
	return (plotCheck(PUF_canDefendEnemy, pUnit->getOwnerINLINE(), pUnit->isAlwaysHostile(this), NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwnerINLINE()) != NULL);
}


CvUnit *CvPlot::getVisibleEnemyDefender(PlayerTypes ePlayer) const
{
	return plotCheck(PUF_canDefendEnemy, ePlayer, false, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer);
}


int CvPlot::getNumDefenders(PlayerTypes ePlayer) const
{
	return plotCount(PUF_canDefend, -1, -1, ePlayer);
}


int CvPlot::getNumVisibleEnemyDefenders(const CvUnit* pUnit) const
{
	return plotCount(PUF_canDefendEnemy, pUnit->getOwnerINLINE(), pUnit->isAlwaysHostile(this), NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwnerINLINE());
}


int CvPlot::getNumVisiblePotentialEnemyDefenders(const CvUnit* pUnit) const
{
	return plotCount(PUF_canDefendPotentialEnemy, pUnit->getOwnerINLINE(), pUnit->isAlwaysHostile(this), NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwnerINLINE());
}


bool CvPlot::isVisibleEnemyUnit(PlayerTypes ePlayer) const
{
	return (plotCheck(PUF_isEnemy, ePlayer, false, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL);
}

int CvPlot::getNumVisibleUnits(PlayerTypes ePlayer) const
{
	return plotCount(PUF_isVisibleDebug, ePlayer);
}


bool CvPlot::isVisibleEnemyUnit(const CvUnit* pUnit) const
{
	return (plotCheck(PUF_isEnemy, pUnit->getOwnerINLINE(), pUnit->isAlwaysHostile(this), NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwnerINLINE()) != NULL);
}

bool CvPlot::isVisibleOtherUnit(PlayerTypes ePlayer) const
{
	return (plotCheck(PUF_isOtherTeam, ePlayer, -1, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL);
}


bool CvPlot::isFighting() const
{
	return (plotCheck(PUF_isFighting) != NULL);
}


bool CvPlot::canHaveFeature(FeatureTypes eFeature) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eFeature == NO_FEATURE)
	{
		return true;
	}

	if (getFeatureType() != NO_FEATURE)
	{
		return false;
	}

	if (isPeak())
	{
		return false;
	}

	if (isCity())
	{
		return false;
	}

	if (!(GC.getFeatureInfo(eFeature).isTerrain(getTerrainType())))
	{
		return false;
	}

	if (GC.getFeatureInfo(eFeature).isNoCoast() && isCoastalLand())
	{
		return false;
	}

	if (GC.getFeatureInfo(eFeature).isNoRiver() && isRiver())
	{
		return false;
	}

	if (GC.getFeatureInfo(eFeature).isRequiresFlatlands() && isHills())
	{
		return false;
	}

	if (GC.getFeatureInfo(eFeature).isNoAdjacent())
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->getFeatureType() == eFeature)
				{
					return false;
				}
			}
		}
	}

	if (GC.getFeatureInfo(eFeature).isRequiresRiver() && !isRiver())
	{
		return false;
	}

	return true;
}


bool CvPlot::isRoute() const
{
	return (getRouteType() != NO_ROUTE);
}


bool CvPlot::isValidRoute(const CvUnit* pUnit) const
{
	if (isRoute())
	{
		if (!pUnit->isEnemy(getTeam(), this) || pUnit->isEnemyRoute())
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isTradeNetworkImpassable(TeamTypes eTeam) const
{
	return (isImpassable() && !isRiverNetwork(eTeam));
}

bool CvPlot::isRiverNetwork(TeamTypes eTeam) const
{
	if (!isRiver())
	{
		return false;
	}

	if (GET_TEAM(eTeam).isRiverTrade())
	{
		return true;
	}

	if (getTeam() == eTeam)
	{
		return true;
	}

	return false;
}

bool CvPlot::isNetworkTerrain(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (GET_TEAM(eTeam).isTerrainTrade(getTerrainType()))
	{
		return true;
	}

	if (isWater())
	{
		if (getTeam() == eTeam)
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isBonusNetwork(TeamTypes eTeam) const
{
	if (isRoute())
	{
		return true;
	}

	if (isRiverNetwork(eTeam))
	{
		return true;
	}

	if (isNetworkTerrain(eTeam))
	{
		return true;
	}

	return false;
}


bool CvPlot::isTradeNetwork(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");

	if (atWar(eTeam, getTeam()))
	{
		return false;
	}

	if (getBlockadedCount(eTeam) > 0)
	{
		return false;
	}

	if (isTradeNetworkImpassable(eTeam))
	{
		return false;
	}

	if (!isOwned())
	{
		if (!isRevealed(eTeam, false))
		{
			return false;
		}
	}

	return isBonusNetwork(eTeam);
}


bool CvPlot::isTradeNetworkConnected(const CvPlot* pPlot, TeamTypes eTeam) const
{
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");

	if (atWar(eTeam, getTeam()) || atWar(eTeam, pPlot->getTeam()))
	{
		return false;
	}

	if (isTradeNetworkImpassable(eTeam) || pPlot->isTradeNetworkImpassable(eTeam))
	{
		return false;
	}

	if (!isOwned())
	{
		if (!isRevealed(eTeam, false) || !(pPlot->isRevealed(eTeam, false)))
		{
			return false;
		}
	}

	if (isRoute())
	{
		if (pPlot->isRoute())
		{
			return true;
		}
	}

	bool bBonusIsland = (!isWater() && GC.getMap().getArea(getArea())->getNumTiles() <= 3);
	/*for (int iI = 0; iI < NUM_BONUS_ISLANDS; iI++)
	{
		if (getX_INLINE() == bonusIslandsX[iI] && getY_INLINE() == bonusIslandsY[iI])
		{
			bBonusIsland = true;
			break;
		}
	}*/

	if (isCity(true, eTeam) || bBonusIsland)
	{
		if (pPlot->isNetworkTerrain(eTeam))
		{
			return true;
		}
	}

	if (isNetworkTerrain(eTeam))
	{
		if (pPlot->isCity(true, eTeam) || bBonusIsland)
		{
			return true;
		}

		if (pPlot->isNetworkTerrain(eTeam))
		{
			return true;
		}

		if (pPlot->isRiverNetwork(eTeam))
		{
			if (pPlot->isRiverConnection(directionXY(pPlot, this)))
			{
				return true;
			}
		}
	}

	if (isRiverNetwork(eTeam))
	{
		if (pPlot->isNetworkTerrain(eTeam))
		{
			if (isRiverConnection(directionXY(this, pPlot)))
			{
				return true;
			}
		}

		if (isRiverConnection(directionXY(this, pPlot)) || pPlot->isRiverConnection(directionXY(pPlot, this)))
		{
			if (pPlot->isRiverNetwork(eTeam))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isValidDomainForLocation(const CvUnit& unit) const
{
	if (isValidDomainForAction(unit))
	{
		return true;
	}

	return isCity(true, unit.getTeam());
}


bool CvPlot::isValidDomainForAction(const CvUnit& unit) const
{
	switch (unit.getDomainType())
	{
	case DOMAIN_SEA:
		return (isWater() || unit.canMoveAllTerrain());
		break;

	case DOMAIN_AIR:
		return false;
		break;

	case DOMAIN_LAND:
	case DOMAIN_IMMOBILE:
		return (!isWater() || unit.canMoveAllTerrain());
		break;

	default:
		FAssert(false);
		break;
	}

	return false;
}


bool CvPlot::isImpassable() const
{
	if (isPeak())
	{
		return true;
	}

	if (getTerrainType() == NO_TERRAIN)
	{
		return false;
	}

	return ((getFeatureType() == NO_FEATURE) ? GC.getTerrainInfo(getTerrainType()).isImpassable() : GC.getFeatureInfo(getFeatureType()).isImpassable());
}


int CvPlot::getX() const
{
	return m_iX;
}


int CvPlot::getY() const
{
	return m_iY;
}


bool CvPlot::at(int iX, int iY) const
{
	return ((getX_INLINE() == iX) && (getY_INLINE() == iY));
}


// BUG - Lat/Long Coordinates - start
#define MINUTES_PER_DEGREE	60
#define MIN_LONGITUDE		-180
#define MAX_LONGITUDE		180

int CvPlot::calculateMinutes(int iPlotIndex, int iPlotCount, bool bWrap, int iDegreeMin, int iDegreeMax) const
{
	if (!bWrap)
	{
		iPlotCount--;
	}
	return iPlotIndex * (iDegreeMax - iDegreeMin) * MINUTES_PER_DEGREE / iPlotCount + iDegreeMin * MINUTES_PER_DEGREE;
}

int CvPlot::getLongitudeMinutes() const
{
	if (GC.getMapINLINE().isWrapXINLINE())
	{
		// normal and toroidal
		return calculateMinutes(getX_INLINE(), GC.getMapINLINE().getGridWidthINLINE(), true, MIN_LONGITUDE, MAX_LONGITUDE);
	}
	else if (!GC.getMapINLINE().isWrapYINLINE())
	{
		// flat
		return calculateMinutes(getX_INLINE(), GC.getMapINLINE().getGridWidthINLINE(), false, MIN_LONGITUDE, MAX_LONGITUDE);
	}
	else
	{
		// tilted axis
		return calculateMinutes(getY_INLINE(), GC.getMapINLINE().getGridHeightINLINE(), true, MIN_LONGITUDE, MAX_LONGITUDE);
	}
}

int CvPlot::getLatitudeMinutes() const
{
	if (GC.getMapINLINE().isWrapXINLINE())
	{
		// normal and toroidal
		return calculateMinutes(getY_INLINE(), GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapYINLINE(), GC.getMapINLINE().getBottomLatitude(), GC.getMapINLINE().getTopLatitude());
	}
	else if (!GC.getMapINLINE().isWrapYINLINE())
	{
		// flat
		return calculateMinutes(getY_INLINE(), GC.getMapINLINE().getGridHeightINLINE(), false, GC.getMapINLINE().getBottomLatitude(), GC.getMapINLINE().getTopLatitude());
	}
	else
	{
		// tilted axis
		return calculateMinutes(getX_INLINE(), GC.getMapINLINE().getGridWidthINLINE(), false, GC.getMapINLINE().getBottomLatitude(), GC.getMapINLINE().getTopLatitude());
	}
}

int CvPlot::getLatitude() const
{
	return abs(getLatitudeMinutes() / MINUTES_PER_DEGREE);
}
// BUG - Lat/Long Coordinates - end


int CvPlot::getFOWIndex() const
{
	return ((((GC.getMapINLINE().getGridHeight() - 1) - getY_INLINE()) * GC.getMapINLINE().getGridWidth() * LANDSCAPE_FOW_RESOLUTION * LANDSCAPE_FOW_RESOLUTION) + (getX_INLINE() * LANDSCAPE_FOW_RESOLUTION));
}


CvArea* CvPlot::area() const
{
	if(m_pPlotArea == NULL)
	{
		m_pPlotArea = GC.getMapINLINE().getArea(getArea());
	}

	return m_pPlotArea;
}


CvArea* CvPlot::waterArea() const
{
	CvArea* pBestArea;
	CvPlot* pAdjacentPlot;
	int iValue;
	int iBestValue;
	int iI;

	if (isWater())
	{
		return area();
	}

	iBestValue = 0;
	pBestArea = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isWater())
			{
				iValue = pAdjacentPlot->area()->getNumTiles();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pAdjacentPlot->area();
				}
			}
		}
	}

	return pBestArea;
}

CvArea* CvPlot::secondWaterArea() const
{

	CvArea* pWaterArea = waterArea();
	CvArea* pBestArea;
	CvPlot* pAdjacentPlot;
	int iValue;
	int iBestValue;
	int iI;

	FAssert(!isWater());

	iBestValue = 0;
	pBestArea = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isWater() && (pAdjacentPlot->getArea() != pWaterArea->getID()))
			{
				iValue = pAdjacentPlot->area()->getNumTiles();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pAdjacentPlot->area();
				}
			}
		}
	}

	return pBestArea;

}


int CvPlot::getArea() const
{
	return m_iArea;
}


void CvPlot::setArea(int iNewValue)
{
	bool bOldLake;

	if (getArea() != iNewValue)
	{
		bOldLake = isLake();

		if (area() != NULL)
		{
			processArea(area(), -1);
		}

		m_iArea = iNewValue;
		m_pPlotArea = NULL;

		if (area() != NULL)
		{
			processArea(area(), 1);

			updateIrrigated();
			updateYield();
		}
	}
}


int CvPlot::getFeatureVariety() const
{
	FAssert((getFeatureType() == NO_FEATURE) || (m_iFeatureVariety < GC.getFeatureInfo(getFeatureType()).getArtInfo()->getNumVarieties()));
	FAssert(m_iFeatureVariety >= 0);
	return m_iFeatureVariety;
}


int CvPlot::getOwnershipDuration() const
{
	return m_iOwnershipDuration;
}


bool CvPlot::isOwnershipScore() const
{
	return (getOwnershipDuration() >= GC.getDefineINT("OWNERSHIP_SCORE_DURATION_THRESHOLD"));
}


void CvPlot::setOwnershipDuration(int iNewValue)
{
	bool bOldOwnershipScore;

	if (getOwnershipDuration() != iNewValue)
	{
		bOldOwnershipScore = isOwnershipScore();

		m_iOwnershipDuration = iNewValue;
		FAssert(getOwnershipDuration() >= 0);

		if (bOldOwnershipScore != isOwnershipScore())
		{
			if (isOwned())
			{
				if (!isWater())
				{
					GET_PLAYER(getOwnerINLINE()).changeTotalLandScored((isOwnershipScore()) ? 1 : -1);
				}
			}
		}
	}
}


void CvPlot::changeOwnershipDuration(int iChange)
{
	setOwnershipDuration(getOwnershipDuration() + iChange);
}


int CvPlot::getImprovementDuration() const
{
	return m_iImprovementDuration;
}


void CvPlot::setImprovementDuration(int iNewValue)
{
	m_iImprovementDuration = iNewValue;
	FAssert(getImprovementDuration() >= 0);
}


void CvPlot::changeImprovementDuration(int iChange)
{
	setImprovementDuration(getImprovementDuration() + iChange);
}


int CvPlot::getUpgradeProgress() const
{
	return m_iUpgradeProgress;
}


int CvPlot::getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const
{
	int iUpgradeLeft;
	int iUpgradeRate;
	int iTurnsLeft;

	iUpgradeLeft = (GC.getGameINLINE().getImprovementUpgradeTime(eImprovement) - ((getImprovementType() == eImprovement) ? getUpgradeProgress() : 0));

	if (ePlayer == NO_PLAYER)
	{
		return iUpgradeLeft;
	}

	//Rhye - start switch DOESN'T WORK - it's only for display
	/*
	switch (ePlayer)
	{
	case EGYPT:
		iUpgradeLeft *= 133;
		iUpgradeLeft /= 100;
		break;
	case INDIA:
		iUpgradeLeft *= 133;
		iUpgradeLeft /= 100;
		break;
	case CHINA:
		iUpgradeLeft *= 133;
		iUpgradeLeft /= 100;
		break;
	case BABYLONIA:
		iUpgradeLeft *= 133;
		iUpgradeLeft /= 100;
		break;
	case GREECE:
		iUpgradeLeft *= 120;
		iUpgradeLeft /= 100;
		break;
	case PERSIA:
		iUpgradeLeft *= 110;
		iUpgradeLeft /= 100;
		break;
	case PHOENICIA:
		iUpgradeLeft *= 110;
		iUpgradeLeft /= 100;
		break;
	case ROME:
		iUpgradeLeft *= 110;
		iUpgradeLeft /= 100;
		break;
	case JAPAN:
		iUpgradeLeft *= 110;
		iUpgradeLeft /= 100;
		break;
	case ETHIOPIA:
		iUpgradeLeft *= 110;
		iUpgradeLeft /= 100;
		break;
	case MAYA:
		iUpgradeLeft *= 110;
		iUpgradeLeft /= 100;
		break;
	case VIKINGS:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case ARABIA:
		iUpgradeLeft *= 100;
		iUpgradeLeft /= 100;
		break;
	case KHMER:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case SPAIN:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case FRANCE:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case ENGLAND:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case GERMANY:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case RUSSIA:
		iUpgradeLeft *= 90;
		iUpgradeLeft /= 100;
		break;
	case NETHERLANDS:
		iUpgradeLeft *= 80;
		iUpgradeLeft /= 100;
		break;
	case MALI:
		iUpgradeLeft *= 100;
		iUpgradeLeft /= 100;
		break;
	case TURKEY:
		iUpgradeLeft *= 80;
		iUpgradeLeft /= 100;
		break;
	case PORTUGAL:
		iUpgradeLeft *= 80;
		iUpgradeLeft /= 100;
		break;
	case INCA:
		iUpgradeLeft *= 80;
		iUpgradeLeft /= 100;
		break;
	case MONGOLIA:
		iUpgradeLeft *= 80;
		iUpgradeLeft /= 100;
		break;
	case AZTECS:
		iUpgradeLeft *= 80;
		iUpgradeLeft /= 100;
		break;
	case AMERICA:
		iUpgradeLeft *= 70;
		iUpgradeLeft /= 100;
		break;
	default:
		iUpgradeLeft *= 130;
		iUpgradeLeft /= 100;
		break;
	}
	if (!GET_PLAYER((PlayerTypes)EGYPT).isPlayable()) //late start condition
		if (ePlayer < VIKINGS) {
			iUpgradeLeft *= 90;
			iUpgradeLeft /= 100;
		}
		*/
	//Rhye - end switch

	iUpgradeRate = GET_PLAYER(ePlayer).getImprovementUpgradeRate();

	if (iUpgradeRate == 0)
	{
		return iUpgradeLeft;
	}

	iTurnsLeft = (iUpgradeLeft / iUpgradeRate);

	if ((iTurnsLeft * iUpgradeRate) < iUpgradeLeft)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}


void CvPlot::setUpgradeProgress(int iNewValue)
{
	m_iUpgradeProgress = iNewValue;
	FAssert(getUpgradeProgress() >= 0);
}


void CvPlot::changeUpgradeProgress(int iChange)
{
	setUpgradeProgress(getUpgradeProgress() + iChange);
}


int CvPlot::getForceUnownedTimer() const
{
	return m_iForceUnownedTimer;
}


bool CvPlot::isForceUnowned() const
{
	return (getForceUnownedTimer() > 0);
}


void CvPlot::setForceUnownedTimer(int iNewValue)
{
	m_iForceUnownedTimer = iNewValue;
	FAssert(getForceUnownedTimer() >= 0);
}


void CvPlot::changeForceUnownedTimer(int iChange)
{
	setForceUnownedTimer(getForceUnownedTimer() + iChange);
}


int CvPlot::getCityRadiusCount() const
{
	return m_iCityRadiusCount;
}


int CvPlot::isCityRadius() const
{
	return (getCityRadiusCount() > 0);
}


void CvPlot::changeCityRadiusCount(int iChange)
{
	m_iCityRadiusCount = (m_iCityRadiusCount + iChange);
	FAssert(getCityRadiusCount() >= 0);
}


bool CvPlot::isStartingPlot() const
{
	return m_bStartingPlot;
}


void CvPlot::setStartingPlot(bool bNewValue)
{
	m_bStartingPlot = bNewValue;
}


bool CvPlot::isNOfRiver() const
{
	return m_bNOfRiver;
}


void CvPlot::setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir)
{
	CvPlot* pAdjacentPlot;
	int iI;

	if ((isNOfRiver() != bNewValue) || (eRiverDir != m_eRiverWEDirection))
	{
		if (isNOfRiver() != bNewValue)
		{
			updatePlotGroupBonus(false);
			m_bNOfRiver = bNewValue;
			updatePlotGroupBonus(true);

			updateRiverCrossing();
			updateYield();

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
			}

			if (area() != NULL)
			{
				area()->changeNumRiverEdges((isNOfRiver()) ? 1 : -1);
			}
		}

		FAssertMsg(eRiverDir == CARDINALDIRECTION_WEST || eRiverDir == CARDINALDIRECTION_EAST || eRiverDir == NO_CARDINALDIRECTION, "invalid parameter");
		m_eRiverWEDirection = eRiverDir;

		updateRiverSymbol(true, true);
	}
}


bool CvPlot::isWOfRiver() const
{
	return m_bWOfRiver;
}


void CvPlot::setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir)
{
	CvPlot* pAdjacentPlot;
	int iI;

	if ((isWOfRiver() != bNewValue) || (eRiverDir != m_eRiverNSDirection))
	{
		if (isWOfRiver() != bNewValue)
		{
			updatePlotGroupBonus(false);
			m_bWOfRiver = bNewValue;
			updatePlotGroupBonus(true);

			updateRiverCrossing();
			updateYield();

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
			}

			if (area())
			{
				area()->changeNumRiverEdges((isWOfRiver()) ? 1 : -1);
			}
		}

		FAssertMsg(eRiverDir == CARDINALDIRECTION_NORTH || eRiverDir == CARDINALDIRECTION_SOUTH || eRiverDir == NO_CARDINALDIRECTION, "invalid parameter");
		m_eRiverNSDirection = eRiverDir;

		updateRiverSymbol(true, true);
	}
}


CardinalDirectionTypes CvPlot::getRiverNSDirection() const
{
	return (CardinalDirectionTypes)m_eRiverNSDirection;
}


CardinalDirectionTypes CvPlot::getRiverWEDirection() const
{
	return (CardinalDirectionTypes)m_eRiverWEDirection;
}


// This function finds an *inland* corner of this plot at which to place a river.
// It then returns the plot with that corner at its SE.

CvPlot* CvPlot::getInlandCorner() const
{
	CvPlot* pRiverPlot = NULL; // will be a plot through whose SE corner we want the river to run
	int aiShuffle[4];

	shuffleArray(aiShuffle, 4, GC.getGameINLINE().getMapRand());

	for (int iI = 0; iI < 4; ++iI)
	{
		switch (aiShuffle[iI])
		{
		case 0:
			pRiverPlot = GC.getMapINLINE().plotSorenINLINE(getX_INLINE(), getY_INLINE()); break;
		case 1:
			pRiverPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_NORTH); break;
		case 2:
			pRiverPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_NORTHWEST); break;
		case 3:
			pRiverPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_WEST); break;
		}
		if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
		{
			break;
		}
		else
		{
			pRiverPlot = NULL;
		}
	}

	return pRiverPlot;
}


bool CvPlot::hasCoastAtSECorner() const
{
	CvPlot* pAdjacentPlot;

	if (isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_EAST);
	if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_SOUTHEAST);
	if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_SOUTH);
	if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	return false;
}


bool CvPlot::isIrrigated() const
{
	return m_bIrrigated;
}


void CvPlot::setIrrigated(bool bNewValue)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;

	if (isIrrigated() != bNewValue)
	{
		m_bIrrigated = bNewValue;

		for (iDX = -1; iDX <= 1; iDX++)
		{
			for (iDY = -1; iDY <= 1; iDY++)
			{
				pLoopPlot	= plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateYield();
					pLoopPlot->setLayoutDirty(true);
				}
			}
		}
	}
}


void CvPlot::updateIrrigated()
{
	PROFILE("CvPlot::updateIrrigated()");

	CvPlot* pLoopPlot;
	FAStar* pIrrigatedFinder;
	bool bFoundFreshWater;
	bool bIrrigated;
	int iI;

	if (area() == NULL)
	{
		return;
	}

	if (!(GC.getGameINLINE().isFinalInitialized()))
	{
		return;
	}

	pIrrigatedFinder = gDLL->getFAStarIFace()->create();

	if (isIrrigated())
	{
		if (!isPotentialIrrigation())
		{
			setIrrigated(false);

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					bFoundFreshWater = false;
					gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMapINLINE().getGridWidthINLINE(), GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapXINLINE(), GC.getMapINLINE().isWrapYINLINE(), NULL, NULL, NULL, potentialIrrigation, NULL, checkFreshWater, &bFoundFreshWater);
					gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), -1, -1);

					if (!bFoundFreshWater)
					{
						bIrrigated = false;
						gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMapINLINE().getGridWidthINLINE(), GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapXINLINE(), GC.getMapINLINE().isWrapYINLINE(), NULL, NULL, NULL, potentialIrrigation, NULL, changeIrrigated, &bIrrigated);
						gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), -1, -1);
					}
				}
			}
		}
	}
	else
	{
		if (isPotentialIrrigation() && isIrrigationAvailable(true))
		{
			bIrrigated = true;
			gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMapINLINE().getGridWidthINLINE(), GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapXINLINE(), GC.getMapINLINE().isWrapYINLINE(), NULL, NULL, NULL, potentialIrrigation, NULL, changeIrrigated, &bIrrigated);
			gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, getX_INLINE(), getY_INLINE(), -1, -1);
		}
	}

	gDLL->getFAStarIFace()->destroy(pIrrigatedFinder);
}


bool CvPlot::isPotentialCityWork() const
{
	return m_bPotentialCityWork;
}


bool CvPlot::isPotentialCityWorkForArea(CvArea* pArea) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

		if (pLoopPlot != NULL)
		{
			if (!(pLoopPlot->isWater()) || GC.getDefineINT("WATER_POTENTIAL_CITY_WORK_FOR_AREA"))
			{
				if (pLoopPlot->area() == pArea)
				{
					return true;
				}
			}
		}
	}

	return false;
}


void CvPlot::updatePotentialCityWork()
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	bool bValid;
	int iI;

	bValid = false;

	for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

		if (pLoopPlot != NULL)
		{
			if (!(pLoopPlot->isWater()))
			{
				bValid = true;
				break;
			}
		}
	}

	if (isPotentialCityWork() != bValid)
	{
		m_bPotentialCityWork = bValid;

		updateYield();
	}
}


bool CvPlot::isShowCitySymbols() const
{
	return m_bShowCitySymbols;
}


void CvPlot::updateShowCitySymbols()
{
	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	bool bNewShowCitySymbols;
	int iI;

	bNewShowCitySymbols = false;

	for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

		if (pLoopPlot != NULL)
		{
			pLoopCity = pLoopPlot->getPlotCity();

			if (pLoopCity != NULL)
			{
				if (pLoopCity->isCitySelected() && gDLL->getInterfaceIFace()->isCityScreenUp())
				{
					if (pLoopCity->canWork(this))
					{
						bNewShowCitySymbols = true;
						break;
					}
				}
			}
		}
	}

	if (isShowCitySymbols() != bNewShowCitySymbols)
	{
		m_bShowCitySymbols = bNewShowCitySymbols;

		updateSymbolDisplay();
		updateSymbolVisibility();
	}
}


bool CvPlot::isFlagDirty() const
{
	return m_bFlagDirty;
}


void CvPlot::setFlagDirty(bool bNewValue)
{
	m_bFlagDirty = bNewValue;
}


PlayerTypes CvPlot::getOwner() const
{
	return getOwnerINLINE();
}


void CvPlot::setOwner(PlayerTypes eNewValue, bool bCheckUnits, bool bUpdatePlotGroup)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvCity* pOldCity;
	CvCity* pNewCity;
	CvUnit* pLoopUnit;
	CvWString szBuffer;
	UnitTypes eBestUnit;
	int iFreeUnits;
	int iI;

	if (getOwnerINLINE() != eNewValue)
	{
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_PLOT_OWNER_CHANGE, eNewValue, (char*)NULL, getX_INLINE(), getY_INLINE());

		pOldCity = getPlotCity();

		// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
		if (GC.getGameINLINE().isFinalInitialized() && getTeam() != NO_TEAM)
		{
            for (iI = 0; iI < MAX_TEAMS; iI++)
            {
                if (GET_TEAM(getTeam()).isAtWar((TeamTypes)iI))
                {
                    for (int iDX = -DANGER_RANGE; iDX <= DANGER_RANGE; iDX++)
                    {
                        for (int iDY = -DANGER_RANGE; iDY <= DANGER_RANGE; iDY++)
                        {
                            int iIndex = GC.getMapINLINE().plotNumINLINE(getX_INLINE() + iDX, getY_INLINE() + iDY);
                            if (iIndex > -1 && iIndex < GC.getMapINLINE().numPlotsINLINE())
                            {
                                for (int iJ = 0; iJ < GET_TEAM((TeamTypes)iI).getPlayerMemberListSize(); iJ++)
                                {
                                    GET_PLAYER(GET_TEAM((TeamTypes)iI).getPlayerMemberAt(iJ)).AI_invalidatePlotDangerCache(iIndex);
                                }
                            }
                        }
                    }
                }
            }
		}
		// Sanguo Mod Performance, end

		if (pOldCity != NULL)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTED_JOINED", pOldCity->getNameKey(), GET_PLAYER(eNewValue).getCivilizationDescriptionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
			gDLL->getInterfaceIFace()->addMessage(eNewValue, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

			szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTS_JOINS", pOldCity->getNameKey(), GET_PLAYER(eNewValue).getCivilizationDescriptionKey());
			GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwnerINLINE(), szBuffer, getX_INLINE(), getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_ALT_HIGHLIGHT_TEXT"));

			FAssertMsg(pOldCity->getOwnerINLINE() != eNewValue, "pOldCity->getOwnerINLINE() is not expected to be equal with eNewValue");
			GET_PLAYER(eNewValue).acquireCity(pOldCity, false, false, bUpdatePlotGroup); // will delete the pointer
			pOldCity = NULL;
			pNewCity = getPlotCity();
			FAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

			if (pNewCity != NULL)
			{
				CLinkList<IDInfo> oldUnits;

				pUnitNode = headUnitNode();

				while (pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = oldUnits.next(pUnitNode);

					if (pLoopUnit)
					{
						if (pLoopUnit->isEnemy(GET_PLAYER(eNewValue).getTeam(), this))
						{
							FAssert(pLoopUnit->getTeam() != GET_PLAYER(eNewValue).getTeam());
							pLoopUnit->kill(false, eNewValue);
						}
					}
				}

				eBestUnit = pNewCity->AI_bestUnitAI(UNITAI_CITY_DEFENSE);

				if (eBestUnit == NO_UNIT)
				{
					eBestUnit = pNewCity->AI_bestUnitAI(UNITAI_ATTACK);
				}

				if (eBestUnit != NO_UNIT)
				{
					iFreeUnits = (GC.getDefineINT("BASE_REVOLT_FREE_UNITS") + ((pNewCity->getHighestPopulation() * GC.getDefineINT("REVOLT_FREE_UNITS_PERCENT")) / 100));

					for (iI = 0; iI < iFreeUnits; ++iI)
					{
						GET_PLAYER(eNewValue).initUnit(eBestUnit, getX_INLINE(), getY_INLINE(), UNITAI_CITY_DEFENSE);
					}
				}
			}
		}
		else
		{
			setOwnershipDuration(0);

			if (isOwned())
			{
				changeAdjacentSight(getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), false, NULL, bUpdatePlotGroup);

				if (area())
				{
					area()->changeNumOwnedTiles(-1);
				}
				GC.getMapINLINE().changeOwnedPlots(-1);

				if (!isWater())
				{
					GET_PLAYER(getOwnerINLINE()).changeTotalLand(-1);
					GET_TEAM(getTeam()).changeTotalLand(-1);

					if (isOwnershipScore())
					{
						GET_PLAYER(getOwnerINLINE()).changeTotalLandScored(-1);
					}
				}

				if (getImprovementType() != NO_IMPROVEMENT)
				{
					GET_PLAYER(getOwnerINLINE()).changeImprovementCount(getImprovementType(), -1);
				}

				updatePlotGroupBonus(false);
			}

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
				{
					GET_PLAYER(pLoopUnit->getOwnerINLINE()).changeNumOutsideUnits(-1);
				}

				if (pLoopUnit->isBlockading())
				{
					pLoopUnit->setBlockading(false);
					pLoopUnit->getGroup()->clearMissionQueue();
					pLoopUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
			}

			m_eOwner = eNewValue;

			setWorkingCityOverride(NULL);
			updateWorkingCity();

			if (isOwned())
			{
				changeAdjacentSight(getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), true, NULL, bUpdatePlotGroup);

				if (area())
				{
					area()->changeNumOwnedTiles(1);
				}
				GC.getMapINLINE().changeOwnedPlots(1);

				if (!isWater())
				{
					GET_PLAYER(getOwnerINLINE()).changeTotalLand(1);
					GET_TEAM(getTeam()).changeTotalLand(1);

					if (isOwnershipScore())
					{
						GET_PLAYER(getOwnerINLINE()).changeTotalLandScored(1);
					}
				}

				if (getImprovementType() != NO_IMPROVEMENT)
				{
					GET_PLAYER(getOwnerINLINE()).changeImprovementCount(getImprovementType(), 1);
				}

				updatePlotGroupBonus(true);
			}

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
				{
					GET_PLAYER(pLoopUnit->getOwnerINLINE()).changeNumOutsideUnits(1);
				}
			}

			for (iI = 0; iI < MAX_TEAMS; ++iI)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					updateRevealedOwner((TeamTypes)iI);
				}
			}

			updateIrrigated();
			updateYield();

			if (bUpdatePlotGroup)
			{
				updatePlotGroup();
			}

			if (bCheckUnits)
			{
				verifyUnitValidPlot();
			}

			if (isOwned())
			{
				if (isGoody())
				{
					GET_PLAYER(getOwnerINLINE()).doGoody(this, NULL);
				}

				for (iI = 0; iI < MAX_CIV_TEAMS; ++iI)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (isVisible((TeamTypes)iI, false))
						{
							GET_TEAM((TeamTypes)iI).meet(getTeam(), true);
						}
					}
				}
			}

			if (GC.getGameINLINE().isDebugMode())
			{
				updateMinimapColor();

				gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

				gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
			}
		}

		// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
		if (GC.getGameINLINE().isFinalInitialized() && getTeam() != NO_TEAM)
		{
            for (iI = 0; iI < MAX_TEAMS; iI++)
            {
                if (GET_TEAM(getTeam()).isAtWar((TeamTypes)iI))
                {
                    for (int iDX = -DANGER_RANGE; iDX <= DANGER_RANGE; iDX++)
                    {
                        for (int iDY = -DANGER_RANGE; iDY <= DANGER_RANGE; iDY++)
                        {
                            int iIndex = GC.getMapINLINE().plotNumINLINE(getX_INLINE() + iDX, getY_INLINE() + iDY);
                            if (iIndex > -1 && iIndex < GC.getMapINLINE().numPlotsINLINE())
                            {
                                for (int iJ = 0; iJ < GET_TEAM((TeamTypes)iI).getPlayerMemberListSize(); iJ++)
                                {
                                    GET_PLAYER(GET_TEAM((TeamTypes)iI).getPlayerMemberAt(iJ)).AI_invalidatePlotDangerCache(iIndex);
                                }
                            }
                        }
                    }
                }
            }
		}
		// Sanguo Mod Performance, end

		// Leoreth: gain plot control over slave plantation without being able to practice slavery
		if (eNewValue != NO_PLAYER)
		{
			if (getImprovementType() == (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_SLAVE_PLANTATION"))
			{
				if (!GET_PLAYER(eNewValue).canUseSlaves())
				{
					setImprovementType((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_PLANTATION"));
				}
			}
		}

		updateSymbols();
	}
}


PlotTypes CvPlot::getPlotType() const
{
	return (PlotTypes)m_ePlotType;
}


bool CvPlot::isWater() const
{
	return (getPlotType() == PLOT_OCEAN);
}


bool CvPlot::isFlatlands() const
{
	return (getPlotType() == PLOT_LAND);
}


bool CvPlot::isHills() const
{
	return (getPlotType() == PLOT_HILLS);
}


bool CvPlot::isPeak() const
{
	return (getPlotType() == PLOT_PEAK);
}


void CvPlot::setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	CvArea* pNewArea;
	CvArea* pCurrArea;
	CvArea* pLastArea;
	CvPlot* pLoopPlot;
	bool bWasWater;
	bool bRecalculateAreas;
	int iAreaCount;
	int iI;

	if (getPlotType() != eNewValue)
	{
		if ((getPlotType() == PLOT_OCEAN) || (eNewValue == PLOT_OCEAN))
		{
			erase();
		}

		bWasWater = isWater();

		updateSeeFromSight(false, true);

		m_ePlotType = eNewValue;

		updateYield();
		updatePlotGroup();

		updateSeeFromSight(true, true);

		if ((getTerrainType() == NO_TERRAIN) || (GC.getTerrainInfo(getTerrainType()).isWater() != isWater()))
		{
			if (isWater())
			{
				if (isAdjacentToLand())
				{
					setTerrainType(((TerrainTypes)(GC.getDefineINT("SHALLOW_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
				}
				else
				{
					setTerrainType(((TerrainTypes)(GC.getDefineINT("DEEP_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
				}
			}
			else
			{
				setTerrainType(((TerrainTypes)(GC.getDefineINT("LAND_TERRAIN"))), bRecalculate, bRebuildGraphics);
			}
		}

		GC.getMapINLINE().resetPathDistance();

		if (bWasWater != isWater())
		{
			if (bRecalculate)
			{
				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->isWater())
						{
							if (pLoopPlot->isAdjacentToLand())
							{
								pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDefineINT("SHALLOW_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
							}
							else
							{
								pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDefineINT("DEEP_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
							}
						}
					}
				}
			}

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateYield();
					pLoopPlot->updatePlotGroup();
				}
			}

			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updatePotentialCityWork();
				}
			}

			GC.getMapINLINE().changeLandPlots((isWater()) ? -1 : 1);

			if (getBonusType() != NO_BONUS)
			{
				GC.getMapINLINE().changeNumBonusesOnLand(getBonusType(), ((isWater()) ? -1 : 1));
			}

			if (isOwned())
			{
				GET_PLAYER(getOwnerINLINE()).changeTotalLand((isWater()) ? -1 : 1);
				GET_TEAM(getTeam()).changeTotalLand((isWater()) ? -1 : 1);
			}

			if (bRecalculate)
			{
				pNewArea = NULL;
				bRecalculateAreas = false;

				// XXX might want to change this if we allow diagonal water movement...
				if (isWater())
				{
					for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotCardinalDirection(getX_INLINE(), getY_INLINE(), ((CardinalDirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->area()->isWater())
							{
								if (pNewArea == NULL)
								{
									pNewArea = pLoopPlot->area();
								}
								else if (pNewArea != pLoopPlot->area())
								{
									bRecalculateAreas = true;
									break;
								}
							}
						}
					}
				}
				else
				{
					for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							if (!(pLoopPlot->area()->isWater()))
							{
								if (pNewArea == NULL)
								{
									pNewArea = pLoopPlot->area();
								}
								else if (pNewArea != pLoopPlot->area())
								{
									bRecalculateAreas = true;
									break;
								}
							}
						}
					}
				}

				if (!bRecalculateAreas)
				{
					pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)(NUM_DIRECTION_TYPES - 1)));

					if (pLoopPlot != NULL)
					{
						pLastArea = pLoopPlot->area();
					}
					else
					{
						pLastArea = NULL;
					}

					iAreaCount = 0;

					for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							pCurrArea = pLoopPlot->area();
						}
						else
						{
							pCurrArea = NULL;
						}

						if (pCurrArea != pLastArea)
						{
							iAreaCount++;
						}

						pLastArea = pCurrArea;
					}

					if (iAreaCount > 2)
					{
						bRecalculateAreas = true;
					}
				}

				if (bRecalculateAreas)
				{
					GC.getMapINLINE().recalculateAreas();
				}
				else
				{
					setArea(FFreeList::INVALID_INDEX);

					if ((area() != NULL) && (area()->getNumTiles() == 1))
					{
						GC.getMapINLINE().deleteArea(getArea());
					}

					if (pNewArea == NULL)
					{
						pNewArea = GC.getMapINLINE().addArea();
						pNewArea->init(pNewArea->getID(), isWater());
					}

					setArea(pNewArea->getID());
				}
			}
		}

		if (bRebuildGraphics && GC.IsGraphicsInitialized())
		{
			//Update terrain graphical
			gDLL->getEngineIFace()->RebuildPlot(getX_INLINE(), getY_INLINE(), true, true);
			//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
			//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);

			updateFeatureSymbol();
			setLayoutDirty(true);
			updateRouteSymbol(false, true);
			updateRiverSymbol(false, true);
		}
	}
}


TerrainTypes CvPlot::getTerrainType() const
{
	return (TerrainTypes)m_eTerrainType;
}


void CvPlot::setTerrainType(TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	bool bUpdateSight;

	if (getTerrainType() != eNewValue)
	{
		if ((getTerrainType() != NO_TERRAIN) &&
			  (eNewValue != NO_TERRAIN) &&
			  ((GC.getTerrainInfo(getTerrainType()).getSeeFromLevel() != GC.getTerrainInfo(eNewValue).getSeeFromLevel()) ||
				 (GC.getTerrainInfo(getTerrainType()).getSeeThroughLevel() != GC.getTerrainInfo(eNewValue).getSeeThroughLevel())))
		{
			bUpdateSight = true;
		}
		else
		{
			bUpdateSight = false;
		}

		if (bUpdateSight)
		{
			updateSeeFromSight(false, true);
		}

		m_eTerrainType = eNewValue;

		updateYield();
		updatePlotGroup();

		if (bUpdateSight)
		{
			updateSeeFromSight(true, true);
		}

		if (bRebuildGraphics && GC.IsGraphicsInitialized())
		{
			//Update terrain graphics
			gDLL->getEngineIFace()->RebuildPlot(getX_INLINE(), getY_INLINE(),false,true);
			//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
			//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
		}

		if (GC.getTerrainInfo(getTerrainType()).isWater() != isWater())
		{
			setPlotType(((GC.getTerrainInfo(getTerrainType()).isWater()) ? PLOT_OCEAN : PLOT_LAND), bRecalculate, bRebuildGraphics);
		}
	}
}


FeatureTypes CvPlot::getFeatureType() const
{
	return (FeatureTypes)m_eFeatureType;
}


void CvPlot::setFeatureType(FeatureTypes eNewValue, int iVariety)
{
	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	FeatureTypes eOldFeature;
	bool bUpdateSight;
	int iI;

	eOldFeature = getFeatureType();

	if (eNewValue != NO_FEATURE)
	{
		if (iVariety == -1)
		{
			iVariety = ((GC.getFeatureInfo(eNewValue).getArtInfo()->getNumVarieties() * ((getLatitude() * 9) / 8)) / 90);
		}

		iVariety = range(iVariety, 0, (GC.getFeatureInfo(eNewValue).getArtInfo()->getNumVarieties() - 1));
	}
	else
	{
		iVariety = 0;
	}

	if ((eOldFeature != eNewValue) || (m_iFeatureVariety != iVariety))
	{
		if ((eOldFeature == NO_FEATURE) ||
			  (eNewValue == NO_FEATURE) ||
			  (GC.getFeatureInfo(eOldFeature).getSeeThroughChange() != GC.getFeatureInfo(eNewValue).getSeeThroughChange()))
		{
			bUpdateSight = true;
		}
		else
		{
			bUpdateSight = false;
		}

		if (bUpdateSight)
		{
			updateSeeFromSight(false, true);
		}

		m_eFeatureType = eNewValue;
		m_iFeatureVariety = iVariety;

		updateYield();

		if (bUpdateSight)
		{
			updateSeeFromSight(true, true);
		}

		updateFeatureSymbol();

		if (((eOldFeature != NO_FEATURE) && (GC.getFeatureInfo(eOldFeature).getArtInfo()->isRiverArt())) ||
			  ((getFeatureType() != NO_FEATURE) && (GC.getFeatureInfo(getFeatureType()).getArtInfo()->isRiverArt())))
		{
			updateRiverSymbolArt(true);
		}

		for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity != NULL)
				{
					pLoopCity->updateFeatureHealth();
					pLoopCity->updateFeatureHappiness();
				}
			}
		}

		if (getFeatureType() == NO_FEATURE)
		{
			if (getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(getImprovementType()).isRequiresFeature())
				{
					setImprovementType(NO_IMPROVEMENT);
				}
			}
		}
	}

	if (eOldFeature != eNewValue)
	{
		// Leoreth: update culture costs
		CvPlot* pLoopPlot;
		for (int iI = 0; iI < NUM_CITY_PLOTS_3; iI++)
		{
			pLoopPlot = plotCity3(getX(), getY(), iI);
			if (pLoopPlot != NULL && pLoopPlot->isCity()) 
			{
				pLoopPlot->getPlotCity()->updateCultureCosts();
				pLoopPlot->getPlotCity()->updateCoveredPlots(true);
			}
		}
	}
}

void CvPlot::setFeatureDummyVisibility(const char *dummyTag, bool show)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->setDummyVisibility(m_pFeatureSymbol, dummyTag, show);
	}
}

void CvPlot::addFeatureDummyModel(const char *dummyTag, const char *modelTag)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->addDummyModel(m_pFeatureSymbol, dummyTag, modelTag);
	}
}

void CvPlot::setFeatureDummyTexture(const char *dummyTag, const char *textureTag)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->setDummyTexture(m_pFeatureSymbol, dummyTag, textureTag);
	}
}

CvString CvPlot::pickFeatureDummyTag(int mouseX, int mouseY)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		return gDLL->getFeatureIFace()->pickDummyTag(m_pFeatureSymbol, mouseX, mouseY);
	}

	return NULL;
}

void CvPlot::resetFeatureModel()
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->resetModel(m_pFeatureSymbol);
	}
}

BonusTypes CvPlot::getBonusType(TeamTypes eTeam) const
{
	if (eTeam != NO_TEAM)
	{
		if (m_eBonusType != NO_BONUS)
		{
			if (!GET_TEAM(eTeam).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)m_eBonusType).getTechReveal())) && !GET_TEAM(eTeam).isForceRevealedBonus((BonusTypes)m_eBonusType))
			{
				return NO_BONUS;
			}
		}
	}

	return (BonusTypes)m_eBonusType;
}


BonusTypes CvPlot::getNonObsoleteBonusType(TeamTypes eTeam) const
{
	FAssert(eTeam != NO_TEAM);

	BonusTypes eBonus = getBonusType(eTeam);
	if (eBonus != NO_BONUS)
	{
		if (GET_TEAM(eTeam).isBonusObsolete(eBonus))
		{
			return NO_BONUS;
		}
	}

	return eBonus;
}


void CvPlot::setBonusType(BonusTypes eNewValue)
{
	if (getBonusType() != eNewValue)
	{
		if (getBonusType() != NO_BONUS)
		{
			if (area())
			{
				area()->changeNumBonuses(getBonusType(), -1);
			}
			GC.getMapINLINE().changeNumBonuses(getBonusType(), -1);

			if (!isWater())
			{
				GC.getMapINLINE().changeNumBonusesOnLand(getBonusType(), -1);
			}
		}

		updatePlotGroupBonus(false);
		m_eBonusType = eNewValue;
		updatePlotGroupBonus(true);

		if (getBonusType() != NO_BONUS)
		{
			if (area())
			{
				area()->changeNumBonuses(getBonusType(), 1);
			}
			GC.getMapINLINE().changeNumBonuses(getBonusType(), 1);

			if (!isWater())
			{
				GC.getMapINLINE().changeNumBonusesOnLand(getBonusType(), 1);
			}
		}

		updateYield();

		setLayoutDirty(true);

		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

		// Leoreth: update culture costs
		CvPlot* pLoopPlot;
		for (int iI = 0; iI < NUM_CITY_PLOTS_3; iI++)
		{
			pLoopPlot = plotCity3(getX(), getY(), iI);
			if (pLoopPlot != NULL && pLoopPlot->isCity()) 
			{
				pLoopPlot->getPlotCity()->updateCultureCosts();
				pLoopPlot->getPlotCity()->updateCoveredPlots(true);
			}
		}
	}
}


ImprovementTypes CvPlot::getImprovementType() const
{
	return (ImprovementTypes)m_eImprovementType;
}


void CvPlot::setImprovementType(ImprovementTypes eNewValue)
{
	int iI;
	ImprovementTypes eOldImprovement = getImprovementType();

	if (getImprovementType() != eNewValue)
	{
		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (area())
			{
				area()->changeNumImprovements(getImprovementType(), -1);
			}
			if (isOwned())
			{
				GET_PLAYER(getOwnerINLINE()).changeImprovementCount(getImprovementType(), -1);
			}
		}

		updatePlotGroupBonus(false);
		m_eImprovementType = eNewValue;
		updatePlotGroupBonus(true);

		if (getImprovementType() == NO_IMPROVEMENT)
		{
			setImprovementDuration(0);
		}

		setUpgradeProgress(0);

		for (iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (isVisible((TeamTypes)iI, false))
				{
					setRevealedImprovementType((TeamTypes)iI, getImprovementType());
				}
			}
		}

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (area())
			{
				area()->changeNumImprovements(getImprovementType(), 1);
			}
			if (isOwned())
			{
				GET_PLAYER(getOwnerINLINE()).changeImprovementCount(getImprovementType(), 1);
			}
		}

		updateIrrigated();
		updateYield();

		/*for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity != NULL)
				{
					pLoopCity->updateFeatureHappiness();
				}
			}
		}*/

		// Building or removing a fort will now force a plotgroup update to verify resource connections.
		if ( (NO_IMPROVEMENT != getImprovementType() && GC.getImprovementInfo(getImprovementType()).isActsAsCity()) !=
			 (NO_IMPROVEMENT != eOldImprovement && GC.getImprovementInfo(eOldImprovement).isActsAsCity()) )
		{
			updatePlotGroup();

			// Leoreth: update culture costs
			CvPlot* pLoopPlot;
			for (int iI = 0; iI < NUM_CITY_PLOTS_3; iI++)
			{
				pLoopPlot = plotCity3(getX(), getY(), iI);
				if (pLoopPlot != NULL && pLoopPlot->isCity()) 
				{
					pLoopPlot->getPlotCity()->updateCultureCosts();
					pLoopPlot->getPlotCity()->updateCoveredPlots(true);
				}
			}
		}

		if (NO_IMPROVEMENT != eOldImprovement && GC.getImprovementInfo(eOldImprovement).isActsAsCity())
		{
			verifyUnitValidPlot();
		}

		if (GC.getGameINLINE().isDebugMode())
		{
			setLayoutDirty(true);
		}

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			CvEventReporter::getInstance().improvementBuilt(getImprovementType(), getX_INLINE(), getY_INLINE());
		}

		if (getImprovementType() == NO_IMPROVEMENT)
		{
			CvEventReporter::getInstance().improvementDestroyed(eOldImprovement, getOwnerINLINE(), getX_INLINE(), getY_INLINE());
		}

		CvCity* pWorkingCity = getWorkingCity();
		if (NULL != pWorkingCity)
		{
			if (pWorkingCity->isWorkingPlot(this))
			{
				pWorkingCity->updateWorkedImprovement(eOldImprovement, eNewValue);
			}

			if ((NO_IMPROVEMENT != eNewValue && pWorkingCity->getImprovementFreeSpecialists(eNewValue) > 0)	||
				(NO_IMPROVEMENT != eOldImprovement && pWorkingCity->getImprovementFreeSpecialists(eOldImprovement) > 0))
			{
				pWorkingCity->AI_setAssignWorkDirty(true);
			}
		}

		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}


RouteTypes CvPlot::getRouteType() const
{
	return (RouteTypes)m_eRouteType;
}


void CvPlot::setRouteType(RouteTypes eNewValue, bool bUpdatePlotGroups)
{
	bool bOldRoute;
	int iI;

	if (getRouteType() != eNewValue)
	{
		bOldRoute = isRoute(); // XXX is this right???

		updatePlotGroupBonus(false);
		m_eRouteType = eNewValue;
		updatePlotGroupBonus(true);

		for (iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (isVisible((TeamTypes)iI, false))
				{
					setRevealedRouteType((TeamTypes)iI, getRouteType());
				}
			}
		}

		updateYield();

		if (bUpdatePlotGroups)
		{
			if (bOldRoute != isRoute())
			{
				updatePlotGroup();
			}
		}

		if (GC.getGameINLINE().isDebugMode())
		{
			updateRouteSymbol(true, true);
		}

		// Sanguo Mod Performance, start, added by poyuzhe 08.12.09
		{PROFILE_BEGIN("setRouteType::plotdangercache");
		if (GC.getGameINLINE().isFinalInitialized())
		{
			for (int iDX = -DANGER_RANGE; iDX <= DANGER_RANGE; iDX++)
			{
				for (int iDY = -DANGER_RANGE; iDY <= DANGER_RANGE; iDY++)
				{
					int iIndex = GC.getMapINLINE().plotNumINLINE(getX_INLINE() + iDX, getY_INLINE() + iDY);
					if (iIndex > -1 && iIndex < GC.getMapINLINE().numPlotsINLINE())
					{
						CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndex(iIndex);
						FAssert (pLoopPlot != NULL);

						int iDistance = stepDistance(getX_INLINE(), getY_INLINE(), getX_INLINE() + iDX, getY_INLINE() + iDY);

						FAssert (iDistance <= DANGER_RANGE);
						for (iI = 0; iI < MAX_PLAYERS; iI++)
						{
							for (int iJ = 0; iJ < iDistance; iJ++)
							{
								pLoopPlot->invalidatePlayerDangerCache((PlayerTypes)iI, iJ);
							}
						}
					}
				}
			}
		}
		PROFILE_END();}
		// Sanguo Mod Performance, end

		if (getRouteType() != NO_ROUTE)
		{
			CvEventReporter::getInstance().routeBuilt(getRouteType(), getX_INLINE(), getY_INLINE());
		}
	}
}


void CvPlot::updateCityRoute(bool bUpdatePlotGroup)
{
	RouteTypes eCityRoute;

	if (isCity())
	{
		FAssertMsg(isOwned(), "isOwned is expected to be true");

		eCityRoute = GET_PLAYER(getOwnerINLINE()).getBestRoute();

		bool bRomanRoadAround = false;
		for (int iI = std::max(0, getX_INLINE()-1); iI < std::min(getX_INLINE()+1, EARTH_X)+1; iI++)
		{
			for (int iJ = std::max(0, getY_INLINE()-1); iJ < std::min(getY_INLINE()+1, EARTH_X)+1; iJ++)
			{
				if (GC.getMap().plot(iI, iJ)->getRouteType() == (RouteTypes)GC.getInfoTypeForString("ROUTE_ROMAN_ROAD"))
				{
					bRomanRoadAround = true;
					break;
					break;
				}
			}
		}

		//Leoreth: no Roman roads for everyone
		if (getOwnerINLINE() != ROME && !bRomanRoadAround)
		{
			if (eCityRoute == GC.getInfoTypeForString("ROUTE_ROMAN_ROAD"))
			{
				eCityRoute = (RouteTypes)GC.getInfoTypeForString("ROUTE_ROAD");
			}
		}

		if (eCityRoute == NO_ROUTE)
		{
			eCityRoute = ((RouteTypes)(GC.getDefineINT("INITIAL_CITY_ROUTE_TYPE")));
		}

		setRouteType(eCityRoute, bUpdatePlotGroup);
	}
}


CvCity* CvPlot::getPlotCity() const
{
	return getCity(m_plotCity);
}


void CvPlot::setPlotCity(CvCity* pNewValue)
{
	CvPlotGroup* pPlotGroup;
	CvPlot* pLoopPlot;
	int iI;

	if (getPlotCity() != pNewValue)
	{
		if (isCity())
		{
			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(-1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwnerINLINE(), -1);
				}
			}
		}

		updatePlotGroupBonus(false);
		if (isCity())
		{
			pPlotGroup = getPlotGroup(getOwnerINLINE());

			if (pPlotGroup != NULL)
			{
				FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotCity");
				for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					getPlotCity()->changeNumBonuses(((BonusTypes)iI), -(pPlotGroup->getNumBonuses((BonusTypes)iI)));
				}
			}
		}
		if (pNewValue != NULL)
		{
			m_plotCity = pNewValue->getIDInfo();
		}
		else
		{
			m_plotCity.reset();
		}
		if (isCity())
		{
			pPlotGroup = getPlotGroup(getOwnerINLINE());

			if (pPlotGroup != NULL)
			{
				FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotCity");
				for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					getPlotCity()->changeNumBonuses(((BonusTypes)iI), pPlotGroup->getNumBonuses((BonusTypes)iI));
				}
			}
		}
		updatePlotGroupBonus(true);

		if (isCity())
		{
			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwnerINLINE(), 1);
				}
			}
		}

		updateIrrigated();
		updateYield();

		updateMinimapColor();
	}
}


CvCity* CvPlot::getWorkingCity() const
{
	return getCity(m_workingCity);
}


void CvPlot::updateWorkingCity()
{
	CvCity* pOldWorkingCity;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pLoopPlot;
	int iBestPlot;
	int iI;

	pBestCity = getPlotCity();

	if (pBestCity == NULL)
	{
		pBestCity = getWorkingCityOverride();
		FAssertMsg((pBestCity == NULL) || (pBestCity->getOwnerINLINE() == getOwnerINLINE()), "pBest city is expected to either be NULL or the current plot instance's");
	}

	if ((pBestCity == NULL) && isOwned())
	{
		iBestPlot = 0;

		for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity != NULL)
				{
					if (pLoopCity->getOwnerINLINE() == getOwnerINLINE())
					{
						// XXX use getGameTurnAcquired() instead???
						if ((pBestCity == NULL) ||
							  (GC.getCityPlotPriority()[iI] < GC.getCityPlotPriority()[iBestPlot]) ||
							  ((GC.getCityPlotPriority()[iI] == GC.getCityPlotPriority()[iBestPlot]) &&
							   ((pLoopCity->getGameTurnFounded() < pBestCity->getGameTurnFounded()) ||
							    ((pLoopCity->getGameTurnFounded() == pBestCity->getGameTurnFounded()) &&
							     (pLoopCity->getID() < pBestCity->getID())))))
						{
							iBestPlot = iI;
							pBestCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	pOldWorkingCity = getWorkingCity();

	if (pOldWorkingCity != pBestCity)
	{
		if (pOldWorkingCity != NULL)
		{
			pOldWorkingCity->setWorkingPlot(this, false);
		}

		if (pBestCity != NULL)
		{
			FAssertMsg(isOwned(), "isOwned is expected to be true");
			FAssertMsg(!isBeingWorked(), "isBeingWorked did not return false as expected");
			m_workingCity = pBestCity->getIDInfo();
		}
		else
		{
			m_workingCity.reset();
		}

		if (pOldWorkingCity != NULL)
		{
			pOldWorkingCity->AI_setAssignWorkDirty(true);
		}
		if (getWorkingCity() != NULL)
		{
			getWorkingCity()->AI_setAssignWorkDirty(true);
		}

		updateYield();

		updateFog();
		updateShowCitySymbols();

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			if (gDLL->getGraphicOption(GRAPHICOPTION_CITY_RADIUS))
			{
				if (gDLL->getInterfaceIFace()->canSelectionListFound())
				{
					gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
				}
			}
		}
	}
}


CvCity* CvPlot::getWorkingCityOverride() const
{
	return getCity(m_workingCityOverride);
}


void CvPlot::setWorkingCityOverride( const CvCity* pNewValue)
{
	if (getWorkingCityOverride() != pNewValue)
	{
		if (pNewValue != NULL)
		{
			FAssertMsg(pNewValue->getOwnerINLINE() == getOwnerINLINE(), "Argument city pNewValue's owner is expected to be the same as the current instance");
			m_workingCityOverride = pNewValue->getIDInfo();
		}
		else
		{
			m_workingCityOverride.reset();
		}

		updateWorkingCity();
	}
}


int CvPlot::getRiverID() const
{
	return m_iRiverID;
}


void CvPlot::setRiverID(int iNewValue)
{
	m_iRiverID = iNewValue;
}


int CvPlot::getMinOriginalStartDist() const
{
	return m_iMinOriginalStartDist;
}


void CvPlot::setMinOriginalStartDist(int iNewValue)
{
	m_iMinOriginalStartDist = iNewValue;
}


int CvPlot::getReconCount() const
{
	return m_iReconCount;
}


void CvPlot::changeReconCount(int iChange)
{
	m_iReconCount = (m_iReconCount + iChange);
	FAssert(getReconCount() >= 0);
}


int CvPlot::getRiverCrossingCount() const
{
	return m_iRiverCrossingCount;
}


void CvPlot::changeRiverCrossingCount(int iChange)
{
	m_iRiverCrossingCount = (m_iRiverCrossingCount + iChange);
	FAssert(getRiverCrossingCount() >= 0);
}


short* CvPlot::getYield()
{
	return m_aiYield;
}


int CvPlot::getYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiYield[eIndex];
}


int CvPlot::calculateNatureYield(YieldTypes eYield, TeamTypes eTeam, bool bIgnoreFeature) const
{
	BonusTypes eBonus;
	int iYield;


	//Rhye - start UP
	if (isPeak())
	{
		if (eTeam == INCA)
			return 0 + GC.getYieldInfo(eYield).getLakeChange() + GC.getYieldInfo(eYield).getLakeChange() + GC.getYieldInfo(eYield).getHillsChange() + GC.getYieldInfo(eYield).getLakeChange();
		else
			return 0;
	}
	//Rhye - end UP

	if (isImpassable())
	{
		return 0;
	}

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	iYield = GC.getTerrainInfo(getTerrainType()).getYield(eYield);

	if (isHills())
	{
		iYield += GC.getYieldInfo(eYield).getHillsChange();
	}

	if (isPeak())
	{
		iYield += GC.getYieldInfo(eYield).getPeakChange();
	}

	if (isLake())
	{
		iYield += GC.getYieldInfo(eYield).getLakeChange();
	}

	if (eTeam != NO_TEAM)
	{
		eBonus = getBonusType(eTeam);

		if (eBonus != NO_BONUS)
		{
			iYield += GC.getBonusInfo(eBonus).getYieldChange(eYield);
		}
	}

	if (isRiver())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType()).getRiverYieldChange(eYield) : GC.getFeatureInfo(getFeatureType()).getRiverYieldChange(eYield));
	}

	if (isHills())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType()).getHillsYieldChange(eYield) : GC.getFeatureInfo(getFeatureType()).getHillsYieldChange(eYield));
	}

	if (!bIgnoreFeature)
	{
		if (getFeatureType() != NO_FEATURE)
		{
			iYield += GC.getFeatureInfo(getFeatureType()).getYieldChange(eYield);

			//Leoreth: Congo UP: +1 food, +1 production on jungle, rainforest and marsh tiles
			if (getOwnerINLINE() == CONGO)
			{
				if (getFeatureType() == GC.getInfoTypeForString("FEATURE_JUNGLE") ||
					getFeatureType() == GC.getInfoTypeForString("FEATURE_RAINFOREST") ||
					getFeatureType() == GC.getInfoTypeForString("FEATURE_MARSH"))
				{
					if ((int)eYield == 0 || (int)eYield == 1)
					{
						iYield += 1;
					}
				}
			}
		}
	}

	return std::max(0, iYield);
}


int CvPlot::calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const
{
	return std::max(calculateNatureYield(eIndex, eTeam, false), calculateNatureYield(eIndex, eTeam, true));
}


int CvPlot::calculateTotalBestNatureYield(TeamTypes eTeam) const
{
	return (calculateBestNatureYield(YIELD_FOOD, eTeam) + calculateBestNatureYield(YIELD_PRODUCTION, eTeam) + calculateBestNatureYield(YIELD_COMMERCE, eTeam));
}


int CvPlot::calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal) const
{
	PROFILE_FUNC();

	BonusTypes eBonus;
	int iBestYield;
	int iYield;
	int iI;

	iYield = GC.getImprovementInfo(eImprovement).getYieldChange(eYield);

	if (isRiverSide())
	{
		iYield += GC.getImprovementInfo(eImprovement).getRiverSideYieldChange(eYield);
	}

	if (isHills())
	{
		iYield += GC.getImprovementInfo(eImprovement).getHillsYieldChange(eYield);
	}

	if ((bOptimal) ? true : isIrrigationAvailable())
	{
		iYield += GC.getImprovementInfo(eImprovement).getIrrigatedYieldChange(eYield);
	}

	if (bOptimal)
	{
		iBestYield = 0;

		for (iI = 0; iI < GC.getNumRouteInfos(); ++iI)
		{
			iBestYield = std::max(iBestYield, GC.getImprovementInfo(eImprovement).getRouteYieldChanges(iI, eYield));
		}

		iYield += iBestYield;
	}
	else
	{
		if (getRouteType() != NO_ROUTE)
		{
			iYield += GC.getImprovementInfo(eImprovement).getRouteYieldChanges(getRouteType(), eYield);
		}
	}

	if (bOptimal || ePlayer == NO_PLAYER)
	{
		for (iI = 0; iI < GC.getNumTechInfos(); ++iI)
		{
			iYield += GC.getImprovementInfo(eImprovement).getTechYieldChanges(iI, eYield);
		}

		for (iI = 0; iI < GC.getNumCivicInfos(); ++iI)
		{
			iYield += GC.getCivicInfo((CivicTypes) iI).getImprovementYieldChanges(eImprovement, eYield);
		}
	}
	else
	{
		iYield += GET_PLAYER(ePlayer).getImprovementYieldChange(eImprovement, eYield);
		iYield += GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getImprovementYieldChange(eImprovement, eYield);
		if (!isWater() && !isPeak()) iYield -= GET_PLAYER(ePlayer).getUnimprovedTileYield(eYield);
	}

	if (ePlayer != NO_PLAYER)
	{
		eBonus = getBonusType(GET_PLAYER(ePlayer).getTeam());

		if (eBonus != NO_BONUS)
		{
			iYield += GC.getImprovementInfo(eImprovement).getImprovementBonusYield(eBonus, eYield);
		}
	}
//KNOEDELstart
	// Mayan UP: Cottages give 1 extra Food and Production
	if (eYield != YIELD_COMMERCE && ePlayer == MAYA && GET_PLAYER(ePlayer).getCurrentEra() < ERA_RENAISSANCE)
	{
		if (eImprovement == GC.getInfoTypeForString("IMPROVEMENT_COTTAGE") ||
			eImprovement == GC.getInfoTypeForString("IMPROVEMENT_VILLAGE") ||
			eImprovement == GC.getInfoTypeForString("IMPROVEMENT_HAMLET") ||
			eImprovement == GC.getInfoTypeForString("IMPROVEMENT_TOWN"))
		{
			iYield += 1;
		}
	}
//KNOEDELend
	// Leoreth: Moorish UP: +1 food on plains for all improvements that add food until the Renaissance
	if (ePlayer == MOORS && GET_PLAYER(ePlayer).getCurrentEra() < ERA_RENAISSANCE)
	{
		if ((int)eYield == 0 && iYield > 0 && getTerrainType() == GC.getInfoTypeForString("TERRAIN_PLAINS"))
		{
			iYield += 1;
		}
	}

	return iYield;
}


int CvPlot::calculateYield(YieldTypes eYield, bool bDisplay) const
{
	CvCity* pCity;
	CvCity* pWorkingCity;
	ImprovementTypes eImprovement;
	RouteTypes eRoute;
	PlayerTypes ePlayer;
	bool bCity;
	int iYield;

	if (bDisplay && GC.getGameINLINE().isDebugMode())
	{
		return getYield(eYield);
	}

	if (getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}

	if (!isPotentialCityWork())
	{
		return 0;
	}

	bCity = false;

	if (bDisplay)
	{
		ePlayer = getRevealedOwner(GC.getGameINLINE().getActiveTeam(), false);
		eImprovement = getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), false);
		eRoute = getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), false);

		if (ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGameINLINE().getActivePlayer();
		}
	}
	else
	{
		ePlayer = getOwnerINLINE();
		eImprovement = getImprovementType();
		eRoute = getRouteType();
	}

	iYield = calculateNatureYield(eYield, ((ePlayer != NO_PLAYER) ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM));

	if (eImprovement != NO_IMPROVEMENT)
	{
		iYield += calculateImprovementYieldChange(eImprovement, eYield, ePlayer);
	}

	if (eRoute != NO_ROUTE)
	{
		iYield += GC.getRouteInfo(eRoute).getYieldChange(eYield);
	}

	if (ePlayer != NO_PLAYER)
	{
		pCity = getPlotCity();

		if (pCity != NULL)
		{
			if (!bDisplay || pCity->isRevealed(GC.getGameINLINE().getActiveTeam(), false))
			{
				iYield += GC.getYieldInfo(eYield).getCityChange();
				if (GC.getYieldInfo(eYield).getPopulationChangeDivisor() != 0)
				{
					iYield += ((pCity->getPopulation() + GC.getYieldInfo(eYield).getPopulationChangeOffset()) / GC.getYieldInfo(eYield).getPopulationChangeDivisor());
				}
				bCity = true;
			}
		}

		if (isWater())
		{
			if (!isImpassable())
			{
				iYield += GET_PLAYER(ePlayer).getSeaPlotYield(eYield);

				pWorkingCity = getWorkingCity();

				if (pWorkingCity != NULL)
				{
					if (!bDisplay || pWorkingCity->isRevealed(GC.getGameINLINE().getActiveTeam(), false))
					{
						iYield += pWorkingCity->getSeaPlotYield(eYield);
					}
				}
			}
		}

		if (isRiver())
		{
			if (!isImpassable())
			{
				pWorkingCity = getWorkingCity();

				if (NULL != pWorkingCity)
				{
					if (!bDisplay || pWorkingCity->isRevealed(GC.getGameINLINE().getActiveTeam(), false))
					{
						iYield += pWorkingCity->getRiverPlotYield(eYield);
					}
				}
			}
		}

		// Leoreth: additional yield from certain improved bonuses from buildings
		if (getBonusType() != NO_BONUS && getImprovementType() != NO_IMPROVEMENT)
		{
			if (!isImpassable())
			{
				pWorkingCity = getWorkingCity();

				if (NULL != pWorkingCity)
				{
					if (GC.getImprovementInfo(getImprovementType()).isImprovementBonusMakesValid(getBonusType()))
					{
						if (!bDisplay || pWorkingCity->isRevealed(GC.getGameINLINE().getActiveTeam(), false))
						{
							iYield += pWorkingCity->getBonusYield(getBonusType(), eYield);
						}
					}
				}
			}
		}

		// Leoreth: unimproved land tiles by civic effect
		// yield is subtracted again in calculateImprovementYieldChange()
		if (!isWater() && !isImpassable())
		{
			iYield += GET_PLAYER(ePlayer).getUnimprovedTileYield(eYield);
		}
	}

	if (bCity)
	{
		iYield = std::max(iYield, GC.getYieldInfo(eYield).getMinCity());
		int iAppliedImprovement = -1;

		// Leoreth (edead): city counts as correct improvement wrt. bonus yields on small islands, except food
		if (GC.getMap().getArea(getArea())->getNumTiles() <= 5)
		{
			if (getBonusType(GET_PLAYER(ePlayer).getTeam()) != NO_BONUS && eYield != (YieldTypes)0)
			{
				for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
				{
					if (GC.getImprovementInfo((ImprovementTypes)iImprovement).isImprovementBonusMakesValid(getBonusType(GET_PLAYER(ePlayer).getTeam())))
					{
						for (int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
						{
							if (GC.getBuildInfo((BuildTypes)iBuild).getImprovement() == iImprovement && GET_TEAM((TeamTypes)ePlayer).isHasTech((TechTypes)GC.getBuildInfo((BuildTypes)iBuild).getTechPrereq()))
							{
								if (!GC.getBuildInfo((BuildTypes)iBuild).isKill())
								{
									iAppliedImprovement = iImprovement;
									break;
									break;
								}
							}
						}
					}
				}
			}
		}

		if (iAppliedImprovement != -1)
		{
			iYield += calculateImprovementYieldChange((ImprovementTypes)iAppliedImprovement, eYield, ePlayer);
		}
	}

	iYield += GC.getGameINLINE().getPlotExtraYield(m_iX, m_iY, eYield);

	if (ePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield) > 0)
		{
			if (iYield >= GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield))
			{
				iYield += GC.getDefineINT("EXTRA_YIELD");
			}
		}

		//Rhye - start UP (not shown in debug mode)
		if (ePlayer == MALI)
		{
			//if (getYield((YieldTypes)2) == 1)
			if (!isWater())
			{
				iYield += GC.getBonusInfo((BonusTypes)21).getYieldChange(eYield); //+1 commerce, same as fur
			}
		}
		//Rhye - end UP

		// Leoreth: Tamil UP
		if (ePlayer == TAMILS)
		{
			if (isWater())
			{
				iYield += GC.getBonusInfo((BonusTypes)21).getYieldChange(eYield);
			}
		}

		// Leoreth: Moai Statues effect
		/*if (isWater() && GET_PLAYER(ePlayer).isHasBuildingEffect((BuildingTypes)MOAI_STATUES))
		{
			if (eYield == YIELD_PRODUCTION && getImprovementType() != NO_IMPROVEMENT)
			{
				iYield += 2;
			}
		}*/

		// Leoreth: possible mall effect: +1 commerce on cottages
		/*if (ePlayer == AMERICA)
		{
			if (getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_VILLAGE") || getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_HAMLET") || getImprovementType() == GC.getInfoTypeForString("IMPROVEMENT_TOWN"))
			{
				if (getWorkingCity()->isHasRealBuilding((BuildingTypes)GC.getInfoTypeForString("BUILDING_AMERICAN_MALL"))
				{
					iYield += GC.getBonudInfo((BonusTypes)21).getYieldChange(eYield);
				}
			}
		}*/

		if (GET_PLAYER(ePlayer).isGoldenAge())
		{
			if (iYield >= GC.getYieldInfo(eYield).getGoldenAgeYieldThreshold())
			{
				iYield += GC.getYieldInfo(eYield).getGoldenAgeYield();
			}

			// Leoreth: Polish UP: +1 food and commerce during golden ages for every tile that produces at least two
			if (ePlayer == POLAND)
			{
				if (eYield == YIELD_FOOD)
				{
					if (iYield >= 2) iYield += 1;
				}

				if (eYield == YIELD_COMMERCE)
				{
					if (iYield >= 3) iYield += 1; // normal golden age effect has to be accounted for
				}
			}
		}
	}

	return std::max(0, iYield);
}


bool CvPlot::hasYield() const
{
	int iI;

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (getYield((YieldTypes)iI) > 0)
		{
			return true;
		}
	}

	return false;
}


void CvPlot::updateYield()
{
	CvCity* pWorkingCity;
	bool bChange;
	int iNewYield;
	int iOldYield;
	int iI;

	if (area() == NULL)
	{
		return;
	}

	bChange = false;

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		iNewYield = calculateYield((YieldTypes)iI);

		if (getYield((YieldTypes)iI) != iNewYield)
		{
			iOldYield = getYield((YieldTypes)iI);

			m_aiYield[iI] = iNewYield;
			FAssert(getYield((YieldTypes)iI) >= 0);

			pWorkingCity = getWorkingCity();

			if (pWorkingCity != NULL)
			{
				if (isBeingWorked())
				{
					pWorkingCity->changeBaseYieldRate(((YieldTypes)iI), (getYield((YieldTypes)iI) - iOldYield));
					// Leoreth: catch the overflow bug
					if (pWorkingCity->getBaseYieldRate((YieldTypes)iI) < 0 || pWorkingCity->getBaseYieldRate((YieldTypes)iI) > 1000) {
						GC.getGame().logMsg("Overflow in (%d, %d) for CvPlot::updateYield()", pWorkingCity->getX(), pWorkingCity->getY());
						gDLL->getInterfaceIFace()->addMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_OVERFLOW", pWorkingCity->getX(), pWorkingCity->getY(), L"CvPlot::updateYield()"), "", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
						GC.getGame().setAIAutoPlay(0);
						GC.getGame().setAIAutoPlayCatapult(0);
						gDLL->getEngineIFace()->AutoSave();
					}
				}

				pWorkingCity->AI_setAssignWorkDirty(true);
			}

			bChange = true;
		}
	}

	if (bChange)
	{
		updateSymbols();
	}
}


int CvPlot::getCulture(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiCulture)
	{
		return 0;
	}

	if (eIndex == VIKINGS && GC.getGameINLINE().getGameTurnYear() < 1000)
	{
		if (getY_INLINE() == 54 && (getX_INLINE() == 59 || getX_INLINE() == 60 || getX_INLINE() == 61))
		{
			return 0;
		}
	}

	return m_aiCulture[eIndex];
}


int CvPlot::countTotalCulture(bool bIncludeDeadPlayers) const
{
	int iTotalCulture;
	int iI;

	iTotalCulture = 0;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		// Leoreth: consider the culture of dead civilizations
		if (bIncludeDeadPlayers || GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCulture += getCulture((PlayerTypes)iI);
		}
	}

	return iTotalCulture;
}


TeamTypes CvPlot::findHighestCultureTeam() const
{
	PlayerTypes eBestPlayer = findHighestCulturePlayer();

	if (NO_PLAYER == eBestPlayer)
	{
		return NO_TEAM;
	}

	return GET_PLAYER(eBestPlayer).getTeam();
}


PlayerTypes CvPlot::findHighestCulturePlayer() const
{
	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iValue = getCulture((PlayerTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = (PlayerTypes)iI;
			}
		}
	}

	return eBestPlayer;
}


int CvPlot::calculateCulturePercent(PlayerTypes eIndex) const
{
	int iTotalCulture;

	iTotalCulture = countTotalCulture();

	if (iTotalCulture > 0)
	{
		return ((getCulture(eIndex) * 100) / iTotalCulture);
	}

	return 0;
}

int CvPlot::calculateOverallCulturePercent(PlayerTypes eIndex) const
{
	int iTotalCulture;

	iTotalCulture = countTotalCulture(true);

	if (iTotalCulture > 0)
	{
		return ((getCulture(eIndex) * 100) / iTotalCulture);
	}

	return 0;
}

int CvPlot::calculateTeamCulturePercent(TeamTypes eIndex) const
{
	int iTeamCulturePercent;
	int iI;

	iTeamCulturePercent = 0;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex)
			{
				iTeamCulturePercent += calculateCulturePercent((PlayerTypes)iI);
			}
		}
	}

	return iTeamCulturePercent;
}


void CvPlot::setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CvCity* pCity;

	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");

	if (getCulture(eIndex) != iNewValue)
	{
		if(NULL == m_aiCulture)
		{
			m_aiCulture = new int[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiCulture[iI] = 0;
			}
		}

		m_aiCulture[eIndex] = iNewValue;
		FAssert(getCulture(eIndex) >= 0);

		if (bUpdate)
		{
			updateCulture(true, bUpdatePlotGroups);
		}

		pCity = getPlotCity();

		if (pCity != NULL)
		{
			pCity->AI_setAssignWorkDirty(true);
		}
	}
}


void CvPlot::changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate)
{
	if (0 != iChange)
	{
		setCulture(eIndex, (getCulture(eIndex) + iChange), bUpdate, true);
	}
}


int CvPlot::getFoundValue(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (eIndex == KOREA && getX_INLINE() == 108 && getY_INLINE() == 48)
	{
		return 82393;
	}

	if ((getX_INLINE() == 101 && getY_INLINE() == 37) || (getSettlerValue(eIndex) >= 800))
	{
		int iValue = GET_PLAYER(eIndex).AI_foundValue(getX_INLINE(), getY_INLINE(), -1, false);
		if (iValue > area()->getBestFoundValue(eIndex))
		{
			area()->setBestFoundValue(eIndex, iValue);
		}

		return iValue;
	}

	if (NULL == m_aiFoundValue)
	{
		return 0;
	}

	if (m_aiFoundValue[eIndex] == -1 || eIndex == NETHERLANDS)
	{
		/*long lResult=-1;
		if(GC.getUSE_GET_CITY_FOUND_VALUE_CALLBACK())
		{
			CyArgsList argsList;
			argsList.add((int)eIndex);
			argsList.add(getX());
			argsList.add(getY());
			gDLL->getPythonIFace()->callFunction(PYGameModule, "getCityFoundValue", argsList.makeFunctionArgs(), &lResult);
		}*/

		//if (lResult == -1)
		//{
		m_aiFoundValue[eIndex] = GET_PLAYER(eIndex).AI_foundValue(getX_INLINE(), getY_INLINE(), -1, true);
		//}

		if (eIndex == NETHERLANDS)
			m_aiFoundValue[eIndex] = abs(m_aiFoundValue[eIndex]);

		if (m_aiFoundValue[eIndex] > area()->getBestFoundValue(eIndex))
		{
			area()->setBestFoundValue(eIndex, m_aiFoundValue[eIndex]);
		}
	}

	return m_aiFoundValue[eIndex];
}


bool CvPlot::isBestAdjacentFound(PlayerTypes eIndex)
{
	CvPlot* pAdjacentPlot;
	int iI;

	int iPlotValue = GET_PLAYER(eIndex).AI_foundValue(getX_INLINE(), getY_INLINE());

	if (iPlotValue == 0)
	{
		return false;
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if ((pAdjacentPlot != NULL) && pAdjacentPlot->isRevealed(GET_PLAYER(eIndex).getTeam(), false))
		{
			//if (pAdjacentPlot->getFoundValue(eIndex) >= getFoundValue(eIndex))
			if (GET_PLAYER(eIndex).AI_foundValue(pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE()) > iPlotValue)
			{
				return false;
			}
		}
	}

	return true;
}


void CvPlot::setFoundValue(PlayerTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssert(iNewValue >= -1);

	if (NULL == m_aiFoundValue && 0 != iNewValue)
	{
		m_aiFoundValue = new short[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_aiFoundValue[iI] = 0;
		}
	}

	if (NULL != m_aiFoundValue)
	{
		m_aiFoundValue[eIndex] = iNewValue;
	}
}


int CvPlot::getPlayerCityRadiusCount(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiPlayerCityRadiusCount)
	{
		return 0;
	}

	return m_aiPlayerCityRadiusCount[eIndex];
}


bool CvPlot::isPlayerCityRadius(PlayerTypes eIndex) const
{
	return (getPlayerCityRadiusCount(eIndex) > 0);
}


void CvPlot::changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (0 != iChange)
	{
		if (NULL == m_aiPlayerCityRadiusCount)
		{
			m_aiPlayerCityRadiusCount = new char[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiPlayerCityRadiusCount[iI] = 0;
			}
		}

		m_aiPlayerCityRadiusCount[eIndex] += iChange;
		FAssert(getPlayerCityRadiusCount(eIndex) >= 0);
	}
}


CvPlotGroup* CvPlot::getPlotGroup(PlayerTypes ePlayer) const
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiPlotGroup)
	{
		return GET_PLAYER(ePlayer).getPlotGroup(FFreeList::INVALID_INDEX);
	}

	return GET_PLAYER(ePlayer).getPlotGroup(m_aiPlotGroup[ePlayer]);
}


CvPlotGroup* CvPlot::getOwnerPlotGroup() const
{
	if (getOwnerINLINE() == NO_PLAYER)
	{
		return NULL;
	}

	return getPlotGroup(getOwnerINLINE());
}


void CvPlot::setPlotGroup(PlayerTypes ePlayer, CvPlotGroup* pNewValue)
{
	int iI;

	CvPlotGroup* pOldPlotGroup = getPlotGroup(ePlayer);

	if (pOldPlotGroup != pNewValue)
	{
		if (NULL ==  m_aiPlotGroup)
		{
			m_aiPlotGroup = new int[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiPlotGroup[iI] = FFreeList::INVALID_INDEX;
			}
		}

		CvCity* pCity = getPlotCity();

		if (ePlayer == getOwnerINLINE())
		{
			updatePlotGroupBonus(false);
		}

		if (pOldPlotGroup != NULL)
		{
			if (pCity != NULL)
			{
				if (pCity->getOwnerINLINE() == ePlayer)
				{
					FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotGroup");
					for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
					{
						pCity->changeNumBonuses(((BonusTypes)iI), -(pOldPlotGroup->getNumBonuses((BonusTypes)iI)));
					}
				}
			}
		}

		if (pNewValue == NULL)
		{
			m_aiPlotGroup[ePlayer] = FFreeList::INVALID_INDEX;
		}
		else
		{
			m_aiPlotGroup[ePlayer] = pNewValue->getID();
		}

		if (getPlotGroup(ePlayer) != NULL)
		{
			if (pCity != NULL)
			{
				if (pCity->getOwnerINLINE() == ePlayer)
				{
					FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotGroup");
					for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
					{
						pCity->changeNumBonuses(((BonusTypes)iI), getPlotGroup(ePlayer)->getNumBonuses((BonusTypes)iI));
					}
				}
			}
		}
		if (ePlayer == getOwnerINLINE())
		{
			updatePlotGroupBonus(true);
		}
	}
}


void CvPlot::updatePlotGroup()
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			updatePlotGroup((PlayerTypes)iI);
		}
	}
}


void CvPlot::updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate)
{
	PROFILE("CvPlot::updatePlotGroup(Player)");

	CvPlotGroup* pPlotGroup;
	CvPlotGroup* pAdjacentPlotGroup;
	CvPlot* pAdjacentPlot;
	bool bConnected;
	bool bEmpty;
	int iI;

	if (!(GC.getGameINLINE().isFinalInitialized()))
	{
		return;
	}

	pPlotGroup = getPlotGroup(ePlayer);

	if (pPlotGroup != NULL)
	{
		if (bRecalculate)
		{
			bConnected = false;

			if (isTradeNetwork(GET_PLAYER(ePlayer).getTeam()))
			{
				bConnected = true;

				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

					if (pAdjacentPlot != NULL)
					{
						if (pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup)
						{
							if (!isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(ePlayer).getTeam()))
							{
								bConnected = false;
								break;
							}
						}
					}
				}
			}

			if (!bConnected)
			{
				bEmpty = (pPlotGroup->getLengthPlots() == 1);
				FAssertMsg(pPlotGroup->getLengthPlots() > 0, "pPlotGroup should have more than 0 plots");

				pPlotGroup->removePlot(this);

				if (!bEmpty)
				{
					pPlotGroup->recalculatePlots();
				}
			}
		}

		pPlotGroup = getPlotGroup(ePlayer);
	}

	if (isTradeNetwork(GET_PLAYER(ePlayer).getTeam()))
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlotGroup = pAdjacentPlot->getPlotGroup(ePlayer);

				if ((pAdjacentPlotGroup != NULL) && (pAdjacentPlotGroup != pPlotGroup))
				{
					if (isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(ePlayer).getTeam()))
					{
						if (pPlotGroup == NULL)
						{
							pAdjacentPlotGroup->addPlot(this);
							pPlotGroup = pAdjacentPlotGroup;
							FAssertMsg(getPlotGroup(ePlayer) == pPlotGroup, "ePlayer's plot group is expected to equal pPlotGroup");
						}
						else
						{
							FAssertMsg(getPlotGroup(ePlayer) == pPlotGroup, "ePlayer's plot group is expected to equal pPlotGroup");
							GC.getMapINLINE().combinePlotGroups(ePlayer, pPlotGroup, pAdjacentPlotGroup);
							pPlotGroup = getPlotGroup(ePlayer);
							FAssertMsg(pPlotGroup != NULL, "PlotGroup is not assigned a valid value");
						}
					}
				}
			}
		}

		if (pPlotGroup == NULL)
		{
			GET_PLAYER(ePlayer).initPlotGroup(this);
		}
	}
}


int CvPlot::getVisibilityCount(TeamTypes eTeam) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiVisibilityCount)
	{
		return 0;
	}

	return m_aiVisibilityCount[eTeam];
}


void CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bUpdatePlotGroups)
{
	CvCity* pCity;
	CvPlot* pAdjacentPlot;
	bool bOldVisible;
	int iI;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		if (NULL == m_aiVisibilityCount)
		{
			m_aiVisibilityCount = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiVisibilityCount[iI] = 0;
			}
		}

		bOldVisible = isVisible(eTeam, false);

		m_aiVisibilityCount[eTeam] += iChange;
		FAssert(getVisibilityCount(eTeam) >= 0);

		if (eSeeInvisible != NO_INVISIBLE)
		{
			changeInvisibleVisibilityCount(eTeam, eSeeInvisible, iChange);
		}

		if (bOldVisible != isVisible(eTeam, false))
		{
			if (isVisible(eTeam, false))
			{
				setRevealed(eTeam, true, false, NO_TEAM, bUpdatePlotGroups);

				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

					if (pAdjacentPlot != NULL)
					{
						pAdjacentPlot->updateRevealedOwner(eTeam);
					}
				}

				if (getTeam() != NO_TEAM)
				{
					GET_TEAM(getTeam()).meet(eTeam, true);
				}
			}

			pCity = getPlotCity();

			if (pCity != NULL)
			{
				pCity->setInfoDirty(true);
			}

			for (iI = 0; iI < MAX_TEAMS; ++iI)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isStolenVisibility(eTeam))
					{
						changeStolenVisibilityCount(((TeamTypes)iI), ((isVisible(eTeam, false)) ? 1 : -1));
					}
				}
			}

			if (eTeam == GC.getGameINLINE().getActiveTeam())
			{
				updateFog();
				updateMinimapColor();
				updateCenterUnit();
			}
		}
	}
}


int CvPlot::getStolenVisibilityCount(TeamTypes eTeam) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiStolenVisibilityCount)
	{
		return 0;
	}

	return m_aiStolenVisibilityCount[eTeam];
}


void CvPlot::changeStolenVisibilityCount(TeamTypes eTeam, int iChange)
{
	CvCity* pCity;
	bool bOldVisible;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		if (NULL == m_aiStolenVisibilityCount)
		{
			m_aiStolenVisibilityCount = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiStolenVisibilityCount[iI] = 0;
			}
		}

		bOldVisible = isVisible(eTeam, false);

		m_aiStolenVisibilityCount[eTeam] += iChange;
		FAssert(getStolenVisibilityCount(eTeam) >= 0);

		if (bOldVisible != isVisible(eTeam, false))
		{
			if (isVisible(eTeam, false))
			{
				setRevealed(eTeam, true, false, NO_TEAM, true);
			}

			pCity = getPlotCity();

			if (pCity != NULL)
			{
				pCity->setInfoDirty(true);
			}

			if (eTeam == GC.getGameINLINE().getActiveTeam())
			{
				updateFog();
				updateMinimapColor();
				updateCenterUnit();
			}
		}
	}
}


int CvPlot::getBlockadedCount(TeamTypes eTeam) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiBlockadedCount)
	{
		return 0;
	}

	return m_aiBlockadedCount[eTeam];
}

void CvPlot::changeBlockadedCount(TeamTypes eTeam, int iChange)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		if (NULL == m_aiBlockadedCount)
		{
			m_aiBlockadedCount = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiBlockadedCount[iI] = 0;
			}
		}

		m_aiBlockadedCount[eTeam] += iChange;
		FAssert(getBlockadedCount(eTeam) >= 0);
		FAssert(getBlockadedCount(eTeam) == 0 || isWater())

		CvCity* pWorkingCity = getWorkingCity();
		if (NULL != pWorkingCity)
		{
			pWorkingCity->AI_setAssignWorkDirty(true);
		}
	}
}

PlayerTypes CvPlot::getRevealedOwner(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return getOwnerINLINE();
	}
	else
	{
		FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		if (NULL == m_aiRevealedOwner)
		{
			return NO_PLAYER;
		}

		return (PlayerTypes)m_aiRevealedOwner[eTeam];
	}
}


TeamTypes CvPlot::getRevealedTeam(TeamTypes eTeam, bool bDebug) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eRevealedOwner = getRevealedOwner(eTeam, bDebug);

	if (eRevealedOwner != NO_PLAYER)
	{
		return GET_PLAYER(eRevealedOwner).getTeam();
	}
	else
	{
		return NO_TEAM;
	}
}


void CvPlot::setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (getRevealedOwner(eTeam, false) != eNewValue)
	{
		if (NULL == m_aiRevealedOwner)
		{
			m_aiRevealedOwner = new char[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiRevealedOwner[iI] = -1;
			}
		}

		m_aiRevealedOwner[eTeam] = eNewValue;

		if (eTeam == GC.getGameINLINE().getActiveTeam())
		{
			updateMinimapColor();

			if (GC.IsGraphicsInitialized())
			{
				gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

				gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
			}
		}
	}

	FAssert((NULL == m_aiRevealedOwner) || (m_aiRevealedOwner[eTeam] == eNewValue));
}


void CvPlot::updateRevealedOwner(TeamTypes eTeam)
{
	CvPlot* pAdjacentPlot;
	bool bRevealed;
	int iI;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	bRevealed = false;

	if (!bRevealed)
	{
		if (isVisible(eTeam, false))
		{
			bRevealed = true;
		}
	}

	if (!bRevealed)
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->isVisible(eTeam, false))
				{
					bRevealed = true;
					break;
				}
			}
		}
	}

	if (bRevealed)
	{
		setRevealedOwner(eTeam, getOwnerINLINE());
	}
}


bool CvPlot::isRiverCrossing(DirectionTypes eIndex) const
{
	FAssertMsg(eIndex < NUM_DIRECTION_TYPES, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (eIndex == NO_DIRECTION)
	{
		return false;
	}

	if (NULL == m_abRiverCrossing)
	{
		return false;
	}

	return m_abRiverCrossing[eIndex];
}


void CvPlot::updateRiverCrossing(DirectionTypes eIndex)
{
	CvPlot* pNorthEastPlot;
	CvPlot* pSouthEastPlot;
	CvPlot* pSouthWestPlot;
	CvPlot* pNorthWestPlot;
	CvPlot* pCornerPlot;
	CvPlot* pPlot;
	bool bValid;

	FAssertMsg(eIndex >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DIRECTION_TYPES, "eTeam is expected to be within maximum bounds (invalid Index)");

	pCornerPlot = NULL;
	bValid = false;
	pPlot = plotDirection(getX_INLINE(), getY_INLINE(), eIndex);

	if ((NULL == pPlot || !pPlot->isWater()) && !isWater())
	{
		switch (eIndex)
		{
		case DIRECTION_NORTH:
			if (pPlot != NULL)
			{
				bValid = pPlot->isNOfRiver();
			}
			break;

		case DIRECTION_NORTHEAST:
			pCornerPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_NORTH);
			break;

		case DIRECTION_EAST:
			bValid = isWOfRiver();
			break;

		case DIRECTION_SOUTHEAST:
			pCornerPlot = this;
			break;

		case DIRECTION_SOUTH:
			bValid = isNOfRiver();
			break;

		case DIRECTION_SOUTHWEST:
			pCornerPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_WEST);
			break;

		case DIRECTION_WEST:
			if (pPlot != NULL)
			{
				bValid = pPlot->isWOfRiver();
			}
			break;

		case DIRECTION_NORTHWEST:
			pCornerPlot = plotDirection(getX_INLINE(), getY_INLINE(), DIRECTION_NORTHWEST);
			break;

		default:
			FAssert(false);
			break;
		}

		if (pCornerPlot != NULL)
		{
			pNorthEastPlot = plotDirection(pCornerPlot->getX_INLINE(), pCornerPlot->getY_INLINE(), DIRECTION_EAST);
			pSouthEastPlot = plotDirection(pCornerPlot->getX_INLINE(), pCornerPlot->getY_INLINE(), DIRECTION_SOUTHEAST);
			pSouthWestPlot = plotDirection(pCornerPlot->getX_INLINE(), pCornerPlot->getY_INLINE(), DIRECTION_SOUTH);
			pNorthWestPlot = pCornerPlot;

			if (pSouthWestPlot && pNorthWestPlot && pSouthEastPlot && pNorthEastPlot)
			{
				if (pSouthWestPlot->isWOfRiver() && pNorthWestPlot->isWOfRiver())
				{
					bValid = true;
				}
				else if (pNorthEastPlot->isNOfRiver() && pNorthWestPlot->isNOfRiver())
				{
					bValid = true;
				}
				else if ((eIndex == DIRECTION_NORTHEAST) || (eIndex == DIRECTION_SOUTHWEST))
				{
					if (pNorthEastPlot->isNOfRiver() && (pNorthWestPlot->isWOfRiver() || pNorthWestPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pNorthEastPlot->isNOfRiver() || pSouthEastPlot->isWater()) && pNorthWestPlot->isWOfRiver())
					{
						bValid = true;
					}
					else if (pSouthWestPlot->isWOfRiver() && (pNorthWestPlot->isNOfRiver() || pNorthWestPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pSouthWestPlot->isWOfRiver() || pSouthEastPlot->isWater()) && pNorthWestPlot->isNOfRiver())
					{
						bValid = true;
					}
				}
				else
				{
					FAssert((eIndex == DIRECTION_SOUTHEAST) || (eIndex == DIRECTION_NORTHWEST));

					if (pNorthWestPlot->isNOfRiver() && (pNorthWestPlot->isWOfRiver() || pNorthEastPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pNorthWestPlot->isNOfRiver() || pSouthWestPlot->isWater()) && pNorthWestPlot->isWOfRiver())
					{
						bValid = true;
					}
					else if (pNorthEastPlot->isNOfRiver() && (pSouthWestPlot->isWOfRiver() || pSouthWestPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pNorthEastPlot->isNOfRiver() || pNorthEastPlot->isWater()) && pSouthWestPlot->isWOfRiver())
					{
						bValid = true;
					}
				}
			}

		}
	}

	if (isRiverCrossing(eIndex) != bValid)
	{
		if (NULL == m_abRiverCrossing)
		{
			m_abRiverCrossing = new bool[NUM_DIRECTION_TYPES];
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				m_abRiverCrossing[iI] = false;
			}
		}

		m_abRiverCrossing[eIndex] = bValid;

		changeRiverCrossingCount((isRiverCrossing(eIndex)) ? 1 : -1);
	}
}


void CvPlot::updateRiverCrossing()
{
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		updateRiverCrossing((DirectionTypes)iI);
	}
}


bool CvPlot::isRevealed(TeamTypes eTeam, bool bDebug) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return true;
	}

	if (NULL == m_abRevealed)
	{
		return false;
	}

	return m_abRevealed[eTeam];
}


void CvPlot::setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam, bool bUpdatePlotGroup)
{
	CvCity* pCity;
	int iI;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	pCity = getPlotCity();

	if (isRevealed(eTeam, false) != bNewValue)
	{
		if (NULL == m_abRevealed)
		{
			m_abRevealed = new bool[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_abRevealed[iI] = false;
			}
		}

		m_abRevealed[eTeam] = bNewValue;

		if (area())
		{
			area()->changeNumRevealedTiles(eTeam, ((isRevealed(eTeam, false)) ? 1 : -1));
		}

		if (bUpdatePlotGroup)
		{
			for (iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
					{
						updatePlotGroup((PlayerTypes)iI);
					}
				}
			}
		}

		if (eTeam == GC.getGameINLINE().getActiveTeam())
		{
			updateSymbols();
			updateFog();
			updateVisibility();

			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		}

		if (isRevealed(eTeam, false))
		{
/*************************************************************************************************/
/**	SPEEDTWEAK (Block Python) Sephi                                               	            **/
/**	If you want to allow modmodders to enable this Callback, see CvCity::cancreate for example  **/
/*************************************************************************************************/
/**
			// ONEVENT - PlotRevealed
			CvEventReporter::getInstance().plotRevealed(this, eTeam);
/*************************************************************************************************/
/**	END	                                        												**/
/*************************************************************************************************/
		}
	}

	if (!bTerrainOnly)
	{
		if (isRevealed(eTeam, false))
		{
			if (eFromTeam == NO_TEAM)
			{
				setRevealedOwner(eTeam, getOwnerINLINE());
				setRevealedImprovementType(eTeam, getImprovementType());
				setRevealedRouteType(eTeam, getRouteType());

				if (pCity != NULL)
				{
					pCity->setRevealed(eTeam, true);
				}
			}
			else
			{
				if (getRevealedOwner(eFromTeam, false) == getOwnerINLINE())
				{
					setRevealedOwner(eTeam, getRevealedOwner(eFromTeam, false));
				}

				if (getRevealedImprovementType(eFromTeam, false) == getImprovementType())
				{
					setRevealedImprovementType(eTeam, getRevealedImprovementType(eFromTeam, false));
				}

				if (getRevealedRouteType(eFromTeam, false) == getRouteType())
				{
					setRevealedRouteType(eTeam, getRevealedRouteType(eFromTeam, false));
				}

				if (pCity != NULL)
				{
					if (pCity->isRevealed(eFromTeam, false))
					{
						pCity->setRevealed(eTeam, true);
					}
				}
			}
		}
		else
		{
			setRevealedOwner(eTeam, NO_PLAYER);
			setRevealedImprovementType(eTeam, NO_IMPROVEMENT);
			setRevealedRouteType(eTeam, NO_ROUTE);

			if (pCity != NULL)
			{
				pCity->setRevealed(eTeam, false);
			}
		}
	}
}

bool CvPlot::isAdjacentRevealed(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isRevealed(eTeam, false))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::isAdjacentNonrevealed(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (!pAdjacentPlot->isRevealed(eTeam, false))
			{
				return true;
			}
		}
	}

	return false;
}


ImprovementTypes CvPlot::getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return getImprovementType();
	}
	else
	{
		FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		if (NULL == m_aeRevealedImprovementType)
		{
			return NO_IMPROVEMENT;
		}

		return (ImprovementTypes)m_aeRevealedImprovementType[eTeam];
	}
}


void CvPlot::setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (getRevealedImprovementType(eTeam, false) != eNewValue)
	{
		if (NULL == m_aeRevealedImprovementType)
		{
			m_aeRevealedImprovementType = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aeRevealedImprovementType[iI] = NO_IMPROVEMENT;
			}
		}

		m_aeRevealedImprovementType[eTeam] = eNewValue;

		if (eTeam == GC.getGameINLINE().getActiveTeam())
		{
			updateSymbols();
			setLayoutDirty(true);
			//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
		}
	}
}


RouteTypes CvPlot::getRevealedRouteType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return getRouteType();
	}
	else
	{
		FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		if (NULL == m_aeRevealedRouteType)
		{
			return NO_ROUTE;
		}

		return (RouteTypes)m_aeRevealedRouteType[eTeam];
	}
}


void CvPlot::setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (getRevealedRouteType(eTeam, false) != eNewValue)
	{
		if (NULL == m_aeRevealedRouteType)
		{
			m_aeRevealedRouteType = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aeRevealedRouteType[iI] = NO_ROUTE;
			}
		}

		m_aeRevealedRouteType[eTeam] = eNewValue;

		if (eTeam == GC.getGameINLINE().getActiveTeam())
		{
			updateSymbols();
			updateRouteSymbol(true, true);
		}
	}
}


int CvPlot::getBuildProgress(BuildTypes eBuild) const
{
	if (NULL == m_paiBuildProgress)
	{
		return 0;
	}

	return m_paiBuildProgress[eBuild];
}


// Returns true if build finished...
bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, TeamTypes eTeam)
{
	CvCity* pCity;
	CvWString szBuffer;
	int iProduction;
	bool bFinished;

	bFinished = false;

	if (iChange != 0)
	{
		if (NULL == m_paiBuildProgress)
		{
			m_paiBuildProgress = new short[GC.getNumBuildInfos()];
			for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				m_paiBuildProgress[iI] = 0;
			}
		}

		m_paiBuildProgress[eBuild] += iChange;
		FAssert(getBuildProgress(eBuild) >= 0);

		if (getBuildProgress(eBuild) >= getBuildTime(eBuild))
		{
			m_paiBuildProgress[eBuild] = 0;

			if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
			{
				setImprovementType((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement());

				// Leoreth: forts allow to cover cities: if tile unowned, let the building team claim it, otherwise let setImprovementType() handle it
				/*if (GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isActsAsCity())
				{
					if (getOwner() == NO_PLAYER) changeCultureRangeCities(GET_TEAM(eTeam).getLeaderID(), 0, 1, true);
				}*/
			}

			if (GC.getBuildInfo(eBuild).getRoute() != NO_ROUTE)
			{
				setRouteType((RouteTypes)GC.getBuildInfo(eBuild).getRoute(), true);
			}

			if (getFeatureType() != NO_FEATURE)
			{
				if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()) && GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getFeatureTech(getFeatureType())))
				{
					FAssertMsg(eTeam != NO_TEAM, "eTeam should be valid");

					iProduction = getFeatureProduction(eBuild, eTeam, &pCity);

					if (iProduction > 0)
					{
						pCity->changeFeatureProduction(iProduction);

						szBuffer = gDLL->getText("TXT_KEY_MISC_CLEARING_FEATURE_BONUS", GC.getFeatureInfo(getFeatureType()).getTextKeyWide(), iProduction, pCity->getNameKey());
						gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getFeatureInfo(getFeatureType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
					}

					// Python Event
					CvEventReporter::getInstance().plotFeatureRemoved(this, getFeatureType(), pCity);

					setFeatureType(NO_FEATURE);
				}
			}

			bFinished = true;
		}
	}

	return bFinished;
}


// BUG - Partial Builds - start
/*
 * Returns true if the build progress array has been created; false otherwise.
 * A false return value implies that every build has zero progress.
 * A true return value DOES NOT imply that any build has a non-zero progress--just the possibility.
 */
bool CvPlot::hasAnyBuildProgress() const
{
	return NULL != m_paiBuildProgress;
}
// BUG - Partial Builds - end


void CvPlot::updateFeatureSymbolVisibility()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	if (m_pFeatureSymbol != NULL)
	{
		bool bVisible = isRevealed(GC.getGameINLINE().getActiveTeam(), true);
		if(getFeatureType() != NO_FEATURE)
		{
			if(GC.getFeatureInfo(getFeatureType()).isVisibleAlways())
				bVisible = true;
		}

		bool wasVisible = !gDLL->getFeatureIFace()->IsHidden(m_pFeatureSymbol);
		if(wasVisible != bVisible)
		{
			gDLL->getFeatureIFace()->Hide(m_pFeatureSymbol, !bVisible);
			gDLL->getEngineIFace()->MarkPlotTextureAsDirty(m_iX, m_iY);
		}
	}
}


void CvPlot::updateFeatureSymbol(bool bForce)
{
	PROFILE_FUNC();

	FeatureTypes eFeature;

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	eFeature = getFeatureType();

	gDLL->getEngineIFace()->RebuildTileArt(m_iX,m_iY);

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	if ((eFeature == NO_FEATURE) ||
		  (GC.getFeatureInfo(eFeature).getArtInfo()->isRiverArt()) ||
		  (GC.getFeatureInfo(eFeature).getArtInfo()->getTileArtType() != TILE_ART_TYPE_NONE))
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		return;
	}

	if (bForce || (m_pFeatureSymbol == NULL) || (gDLL->getFeatureIFace()->getFeature(m_pFeatureSymbol) != eFeature))
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		m_pFeatureSymbol = gDLL->getFeatureIFace()->createFeature();

		FAssertMsg(m_pFeatureSymbol != NULL, "m_pFeatureSymbol is not expected to be equal with NULL");

		gDLL->getFeatureIFace()->init(m_pFeatureSymbol, 0, 0, eFeature, this);

		updateFeatureSymbolVisibility();
	}
	else
	{
		gDLL->getEntityIFace()->updatePosition((CvEntity*)m_pFeatureSymbol); //update position and contours
	}
}


CvRoute* CvPlot::getRouteSymbol() const
{
	return m_pRouteSymbol;
}


// XXX route symbols don't really exist anymore...
void CvPlot::updateRouteSymbol(bool bForce, bool bAdjacent)
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	RouteTypes eRoute;
	int iI;

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	if (bAdjacent)
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlot->updateRouteSymbol(bForce, false);
				//pAdjacentPlot->setLayoutDirty(true);
			}
		}
	}

	eRoute = getRevealedRouteType(GC.getGameINLINE().getActiveTeam(), true);

	if (eRoute == NO_ROUTE)
	{
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		return;
	}

	if (bForce || (m_pRouteSymbol == NULL) || (gDLL->getRouteIFace()->getRoute(m_pRouteSymbol) != eRoute))
	{
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		m_pRouteSymbol = gDLL->getRouteIFace()->createRoute();
		FAssertMsg(m_pRouteSymbol != NULL, "m_pRouteSymbol is not expected to be equal with NULL");

		gDLL->getRouteIFace()->init(m_pRouteSymbol, 0, 0, eRoute, this);
		setLayoutDirty(true);
	}
	else
	{
		gDLL->getEntityIFace()->updatePosition((CvEntity *)m_pRouteSymbol); //update position and contours
	}
}


CvRiver* CvPlot::getRiverSymbol() const
{
	return m_pRiverSymbol;
}


CvFeature* CvPlot::getFeatureSymbol() const
{
	return m_pFeatureSymbol;
}


void CvPlot::updateRiverSymbol(bool bForce, bool bAdjacent)
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	if (bAdjacent)
	{
		for(int i=0;i<NUM_DIRECTION_TYPES;i++)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)i));
			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlot->updateRiverSymbol(bForce, false);
				//pAdjacentPlot->setLayoutDirty(true);
			}
		}
	}

	if (!isRiverMask())
	{
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		return;
	}

	if (bForce || (m_pRiverSymbol == NULL))
	{
		//create river
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		m_pRiverSymbol = gDLL->getRiverIFace()->createRiver();
		FAssertMsg(m_pRiverSymbol != NULL, "m_pRiverSymbol is not expected to be equal with NULL");
		gDLL->getRiverIFace()->init(m_pRiverSymbol, 0, 0, 0, this);

		//force tree cuts for adjacent plots
		DirectionTypes affectedDirections[] = {NO_DIRECTION, DIRECTION_EAST, DIRECTION_SOUTHEAST, DIRECTION_SOUTH};
		for(int i=0;i<4;i++)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), affectedDirections[i]);
			if (pAdjacentPlot != NULL)
			{
				gDLL->getEngineIFace()->ForceTreeOffsets(pAdjacentPlot->getX(), pAdjacentPlot->getY());
			}
		}

		//cut out canyons
		gDLL->getEngineIFace()->RebuildRiverPlotTile(getX_INLINE(), getY_INLINE(), true, false);

		//recontour adjacent rivers
		for(int i=0;i<NUM_DIRECTION_TYPES;i++)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)i));
			if((pAdjacentPlot != NULL) && (pAdjacentPlot->m_pRiverSymbol != NULL))
			{
				gDLL->getEntityIFace()->updatePosition((CvEntity *)pAdjacentPlot->m_pRiverSymbol); //update position and contours
			}
		}

		// update the symbol
		setLayoutDirty(true);
	}

	//recontour rivers
	gDLL->getEntityIFace()->updatePosition((CvEntity *)m_pRiverSymbol); //update position and contours
}


void CvPlot::updateRiverSymbolArt(bool bAdjacent)
{
	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	//this is used to update floodplain features
	gDLL->getEntityIFace()->setupFloodPlains(m_pRiverSymbol);
	if(bAdjacent)
	{
		for(int i=0;i<NUM_DIRECTION_TYPES;i++)
		{
			CvPlot *pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes) i);
			if((pAdjacentPlot != NULL) && (pAdjacentPlot->m_pRiverSymbol != NULL))
			{
				gDLL->getEntityIFace()->setupFloodPlains(pAdjacentPlot->m_pRiverSymbol);
			}
		}
	}
}


CvFlagEntity* CvPlot::getFlagSymbol() const
{
	return m_pFlagSymbol;
}

CvFlagEntity* CvPlot::getFlagSymbolOffset() const
{
	return m_pFlagSymbolOffset;
}

void CvPlot::updateFlagSymbol()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return;
	}

	PlayerTypes ePlayer = NO_PLAYER;
	PlayerTypes ePlayerOffset = NO_PLAYER;

	CvUnit* pCenterUnit = getCenterUnit();

	//get the plot's unit's flag
	if (pCenterUnit != NULL)
	{
		ePlayer = pCenterUnit->getVisualOwner();
	}

	//get moving unit's flag
	if (gDLL->getInterfaceIFace()->getSingleMoveGotoPlot() == this)
	{
		if(ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGameINLINE().getActivePlayer();
		}
		else
		{
			ePlayerOffset = GC.getGameINLINE().getActivePlayer();
		}
	}

	//don't put two of the same flags
	if(ePlayerOffset == ePlayer)
	{
		ePlayerOffset = NO_PLAYER;
	}

	//destroy old flags
	if (ePlayer == NO_PLAYER)
	{
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	}
	if (ePlayerOffset == NO_PLAYER)
	{
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
	}

	//create and/or update unit's flag
	if (ePlayer != NO_PLAYER)
	{
		if ((m_pFlagSymbol == NULL) || (gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbol) != ePlayer))
		{
			if (m_pFlagSymbol != NULL)
			{
				gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
			}
			m_pFlagSymbol = gDLL->getFlagEntityIFace()->create(ePlayer);
			if (m_pFlagSymbol != NULL)
			{
				gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbol, this, false);
			}
		}

		if (m_pFlagSymbol != NULL)
		{
			gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbol, this, false);
		}
	}

	//create and/or update offset flag
	if (ePlayerOffset != NO_PLAYER)
	{
		if ((m_pFlagSymbolOffset == NULL) || (gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbolOffset) != ePlayerOffset))
		{
			if (m_pFlagSymbolOffset != NULL)
			{
				gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
			}
			m_pFlagSymbolOffset = gDLL->getFlagEntityIFace()->create(ePlayerOffset);
			if (m_pFlagSymbolOffset != NULL)
			{
				gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbolOffset, this, true);
			}
		}

		if (m_pFlagSymbolOffset != NULL)
		{
			gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbolOffset, this, true);
		}
	}
}


CvUnit* CvPlot::getCenterUnit() const
{
	return m_pCenterUnit;
}


CvUnit* CvPlot::getDebugCenterUnit() const
{
	CvUnit* pCenterUnit;

	pCenterUnit = getCenterUnit();

	if (pCenterUnit == NULL)
	{
		if (GC.getGameINLINE().isDebugMode())
		{
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			if(pUnitNode == NULL)
				pCenterUnit = NULL;
			else
				pCenterUnit = ::getUnit(pUnitNode->m_data);
		}
	}

	return pCenterUnit;
}


void CvPlot::setCenterUnit(CvUnit* pNewValue)
{
	CvUnit* pOldValue;

	pOldValue = getCenterUnit();

	if (pOldValue != pNewValue)
	{
		m_pCenterUnit = pNewValue;
		updateMinimapColor();

		setFlagDirty(true);

		if (getCenterUnit() != NULL)
		{
			getCenterUnit()->setInfoBarDirty(true);
		}
	}
}


int CvPlot::getCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex) const
{
	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PLAYERS);
	FAssert(iRangeIndex >= 0);
	FAssert(iRangeIndex < GC.getNumCultureLevelInfos());

	if (NULL == m_apaiCultureRangeCities)
	{
		return 0;
	}
	else if (NULL == m_apaiCultureRangeCities[eOwnerIndex])
	{
		return 0;
	}

	return m_apaiCultureRangeCities[eOwnerIndex][iRangeIndex];
}


bool CvPlot::isCultureRangeCity(PlayerTypes eOwnerIndex, int iRangeIndex) const
{
	return (getCultureRangeCities(eOwnerIndex, iRangeIndex) > 0);
}


void CvPlot::changeCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex, int iChange, bool bUpdatePlotGroups)
{
	bool bOldCultureRangeCities;

	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PLAYERS);
	FAssert(iRangeIndex >= 0);
	FAssert(iRangeIndex < GC.getNumCultureLevelInfos());

	if (0 != iChange)
	{
		bOldCultureRangeCities = isCultureRangeCity(eOwnerIndex, iRangeIndex);

		if (NULL == m_apaiCultureRangeCities)
		{
			m_apaiCultureRangeCities = new char*[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_apaiCultureRangeCities[iI] = NULL;
			}
		}

		if (NULL == m_apaiCultureRangeCities[eOwnerIndex])
		{
			m_apaiCultureRangeCities[eOwnerIndex] = new char[GC.getNumCultureLevelInfos()];
			for (int iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
			{
				m_apaiCultureRangeCities[eOwnerIndex][iI] = 0;
			}
		}

		m_apaiCultureRangeCities[eOwnerIndex][iRangeIndex] += iChange;

		if (bOldCultureRangeCities != isCultureRangeCity(eOwnerIndex, iRangeIndex))
		{
			updateCulture(true, bUpdatePlotGroups);
		}
	}
}


int CvPlot::getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eInvisible >= 0, "eInvisible is expected to be non-negative (invalid Index)");
	FAssertMsg(eInvisible < GC.getNumInvisibleInfos(), "eInvisible is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_apaiInvisibleVisibilityCount)
	{
		return 0;
	}
	else if (NULL == m_apaiInvisibleVisibilityCount[eTeam])
	{
		return 0;
	}

	return m_apaiInvisibleVisibilityCount[eTeam][eInvisible];
}


bool CvPlot::isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible)	const
{
	return (getInvisibleVisibilityCount(eTeam, eInvisible) > 0);
}


void CvPlot::changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange)
{
	bool bOldInvisibleVisible;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eInvisible >= 0, "eInvisible is expected to be non-negative (invalid Index)");
	FAssertMsg(eInvisible < GC.getNumInvisibleInfos(), "eInvisible is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		bOldInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);

		if (NULL == m_apaiInvisibleVisibilityCount)
		{
			m_apaiInvisibleVisibilityCount = new short*[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_apaiInvisibleVisibilityCount[iI] = NULL;
			}
		}

		if (NULL == m_apaiInvisibleVisibilityCount[eTeam])
		{
			m_apaiInvisibleVisibilityCount[eTeam] = new short[GC.getNumInvisibleInfos()];
			for (int iI = 0; iI < GC.getNumInvisibleInfos(); ++iI)
			{
				m_apaiInvisibleVisibilityCount[eTeam][iI] = 0;
			}
		}

		m_apaiInvisibleVisibilityCount[eTeam][eInvisible] += iChange;

		if (bOldInvisibleVisible != isInvisibleVisible(eTeam, eInvisible))
		{
			if (eTeam == GC.getGameINLINE().getActiveTeam())
			{
				updateCenterUnit();
			}
		}
	}
}


int CvPlot::getNumUnits() const
{
	return m_units.getLength();
}


CvUnit* CvPlot::getUnitByIndex(int iIndex) const
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = m_units.nodeNum(iIndex);

	if (pUnitNode != NULL)
	{
		return ::getUnit(pUnitNode->m_data);
	}
	else
	{
		return NULL;
	}
}


void CvPlot::addUnit(CvUnit* pUnit, bool bUpdate)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	FAssertMsg(pUnit->at(getX_INLINE(), getY_INLINE()), "pUnit is expected to be at getX_INLINE and getY_INLINE");

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (!isBeforeUnitCycle(pLoopUnit, pUnit))
		{
			break;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (pUnitNode != NULL)
	{
		m_units.insertBefore(pUnit->getIDInfo(), pUnitNode);
	}
	else
	{
		m_units.insertAtEnd(pUnit->getIDInfo());
	}

	if (bUpdate)
	{
		updateCenterUnit();

		setFlagDirty(true);
	}
}


void CvPlot::removeUnit(CvUnit* pUnit, bool bUpdate)
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			FAssertMsg(::getUnit(pUnitNode->m_data)->at(getX_INLINE(), getY_INLINE()), "The current unit instance is expected to be at getX_INLINE and getY_INLINE");
			m_units.deleteNode(pUnitNode);
			break;
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	if (bUpdate)
	{
		updateCenterUnit();

		setFlagDirty(true);
	}
}


CLLNode<IDInfo>* CvPlot::nextUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.next(pNode);
}


CLLNode<IDInfo>* CvPlot::prevUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.prev(pNode);
}


CLLNode<IDInfo>* CvPlot::headUnitNode() const
{
	return m_units.head();
}


CLLNode<IDInfo>* CvPlot::tailUnitNode() const
{
	return m_units.tail();
}


int CvPlot::getNumSymbols() const
{
	return m_symbols.size();
}


CvSymbol* CvPlot::getSymbol(int iID) const
{
	return m_symbols[iID];
}


CvSymbol* CvPlot::addSymbol()
{
	CvSymbol* pSym=gDLL->getSymbolIFace()->createSymbol();
	m_symbols.push_back(pSym);
	return pSym;
}


void CvPlot::deleteSymbol(int iID)
{
	m_symbols.erase(m_symbols.begin()+iID);
}


void CvPlot::deleteAllSymbols()
{
	int i;
	for(i=0;i<getNumSymbols();i++)
	{
		gDLL->getSymbolIFace()->destroy(m_symbols[i]);
	}
	m_symbols.clear();
}

CvString CvPlot::getScriptData() const
{
	return m_szScriptData;
}

void CvPlot::setScriptData(const char* szNewValue)
{
	SAFE_DELETE_ARRAY(m_szScriptData);
	m_szScriptData = _strdup(szNewValue);
}

// Protected Functions...

void CvPlot::doFeature()
{
	PROFILE("CvPlot::doFeature()")

	CvCity* pCity;
	CvPlot* pLoopPlot;
	CvWString szBuffer;
	int iProbability;
	int iI, iJ;

	if (getFeatureType() != NO_FEATURE)
	{
		iProbability = GC.getFeatureInfo(getFeatureType()).getDisappearanceProbability();

		if (iProbability > 0)
		{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                                jdog5000      */
/*                                                                                              */
/* Gamespeed scaling                                                                            */
/************************************************************************************************/
/* original bts code
			if (GC.getGameINLINE().getSorenRandNum(10000, "Feature Disappearance") < iProbability)
*/
			int iOdds = (10000*GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getVictoryDelayPercent())/100;
			if (GC.getGameINLINE().getSorenRandNum(iOdds, "Feature Disappearance") < iProbability)
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
			{
				setFeatureType(NO_FEATURE);
			}
		}
	}
	else
	{
		if (!isUnit())
		{
			if (getImprovementType() == NO_IMPROVEMENT)
			{
				for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
				{
					if (canHaveFeature((FeatureTypes)iI))
					{
						if ((getBonusType() == NO_BONUS) || (GC.getBonusInfo(getBonusType()).isFeature(iI)))
						{
							iProbability = 0;

							for (iJ = 0; iJ < NUM_CARDINALDIRECTION_TYPES; iJ++)
							{
								pLoopPlot = plotCardinalDirection(getX_INLINE(), getY_INLINE(), ((CardinalDirectionTypes)iJ));

								if (pLoopPlot != NULL)
								{
									if (pLoopPlot->getFeatureType() == ((FeatureTypes)iI))
									{
										if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
										{
											iProbability += GC.getFeatureInfo((FeatureTypes)iI).getGrowthProbability();
										}
										else
										{
											iProbability += GC.getImprovementInfo(pLoopPlot->getImprovementType()).getFeatureGrowthProbability();
										}
									}
								}
							}

							iProbability *= std::max(0, (GC.getFEATURE_GROWTH_MODIFIER() + 100));
							iProbability /= 100;

							if (isRoute())
							{
								iProbability *= std::max(0, (GC.getROUTE_FEATURE_GROWTH_MODIFIER() + 100));
								iProbability /= 100;
							}

							if (iProbability > 0)
							{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                                jdog5000      */
/*                                                                                              */
/* Gamespeed scaling                                                                            */
/************************************************************************************************/
/* original bts code
								if (GC.getGameINLINE().getSorenRandNum(10000, "Feature Growth") < iProbability)
*/
								int iOdds = (10000*GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getVictoryDelayPercent())/100;
								if( GC.getGameINLINE().getSorenRandNum(iOdds, "Feature Growth") < iProbability )
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
								{
									setFeatureType((FeatureTypes)iI);

									pCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), getOwnerINLINE(), NO_TEAM, false);

									if (pCity != NULL)
									{
										// Tell the owner of this city.
										szBuffer = gDLL->getText("TXT_KEY_MISC_FEATURE_GROWN_NEAR_CITY", GC.getFeatureInfo((FeatureTypes) iI).getTextKeyWide(), pCity->getNameKey());
										gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_FEATUREGROWTH", MESSAGE_TYPE_INFO, GC.getFeatureInfo((FeatureTypes) iI).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
									}

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}


void CvPlot::doCulture()
{
	PROFILE("CvPlot::doCulture()")

	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	CvWString szBuffer;
	PlayerTypes eCulturalOwner;
	int iGarrison;
	int iCityStrength;

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		eCulturalOwner = calculateCulturalOwner();

		if (eCulturalOwner != NO_PLAYER)
		{
			if (GET_PLAYER(eCulturalOwner).getTeam() != getTeam())
			{
				if (!(pCity->isOccupation()))
				{
					if (GC.getGameINLINE().getSorenRandNum(100, "Revolt #1") < pCity->getRevoltTestProbability())
					{
						iCityStrength = pCity->cultureStrength(eCulturalOwner);
						iGarrison = pCity->cultureGarrison(eCulturalOwner);

						if ((GC.getGameINLINE().getSorenRandNum(iCityStrength, "Revolt #2") > iGarrison) || pCity->isBarbarian())
						{
							CLinkList<IDInfo> oldUnits;

							pUnitNode = headUnitNode();

							while (pUnitNode != NULL)
							{
								oldUnits.insertAtEnd(pUnitNode->m_data);
								pUnitNode = nextUnitNode(pUnitNode);
							}

							pUnitNode = oldUnits.head();

							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = nextUnitNode(pUnitNode);

								if (pLoopUnit)
								{
									if (pLoopUnit->isBarbarian())
									{
										pLoopUnit->kill(false, eCulturalOwner);
									}
									else if (pLoopUnit->canDefend())
									{
										pLoopUnit->changeDamage((pLoopUnit->currHitPoints() / 2), eCulturalOwner);
									}
								}

							}

							if (pCity->isBarbarian() || (!(GC.getGameINLINE().isOption(GAMEOPTION_NO_CITY_FLIPPING)) && (GC.getGameINLINE().isOption(GAMEOPTION_FLIPPING_AFTER_CONQUEST) || !(pCity->isEverOwned(eCulturalOwner))) && (pCity->getNumRevolts(eCulturalOwner) >= GC.getDefineINT("NUM_WARNING_REVOLTS"))))
							{
								if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(eCulturalOwner).isHuman())
								{
									pCity->kill(true);
								}
								else
								{
									setOwner(eCulturalOwner, true, true); // will delete pCity
								}
								pCity = NULL;
							}
							else
							{
								pCity->changeNumRevolts(eCulturalOwner, 1);
								pCity->changeOccupationTimer(GC.getDefineINT("BASE_REVOLT_OCCUPATION_TURNS") + ((iCityStrength * GC.getDefineINT("REVOLT_OCCUPATION_TURNS_PERCENT")) / 100));

								// XXX announce for all seen cities?
								szBuffer = gDLL->getText("TXT_KEY_MISC_REVOLT_IN_CITY", GET_PLAYER(eCulturalOwner).getCivilizationAdjective(), pCity->getNameKey());
								gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
								gDLL->getInterfaceIFace()->addMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
							}
						}
					}
				}
			}
		}
	}

	updateCulture(true, true);
}


void CvPlot::processArea(CvArea* pArea, int iChange)
{
	CvCity* pCity;
	int iI, iJ;

	// XXX am not updating getBestFoundValue() or getAreaAIType()...

	pArea->changeNumTiles(iChange);

	if (isOwned())
	{
		pArea->changeNumOwnedTiles(iChange);
	}

	if (isNOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}
	if (isWOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}

	if (getBonusType() != NO_BONUS)
	{
		pArea->changeNumBonuses(getBonusType(), iChange);
	}

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		pArea->changeNumImprovements(getImprovementType(), iChange);
	}

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).getStartingPlot() == this)
		{
			pArea->changeNumStartingPlots(iChange);
		}

		pArea->changePower(((PlayerTypes)iI), (getUnitPower((PlayerTypes)iI) * iChange));

		pArea->changeUnitsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isPlayer, iI) * iChange));
		pArea->changeAnimalsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isAnimal, -1, -1, ((PlayerTypes)iI)) * iChange));

		for (iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
		{
			pArea->changeNumAIUnits(((PlayerTypes)iI), ((UnitAITypes)iJ), (plotCount(PUF_isUnitAIType, iJ, -1, ((PlayerTypes)iI)) * iChange));
		}
	}

	for (iI = 0; iI < MAX_TEAMS; ++iI)
	{
		if (isRevealed(((TeamTypes)iI), false))
		{
			pArea->changeNumRevealedTiles(((TeamTypes)iI), iChange);
		}
	}

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		// XXX make sure all of this (esp. the changePower()) syncs up...
		pArea->changePower(pCity->getOwnerINLINE(), (getPopulationPower(pCity->getPopulation()) * iChange));

		pArea->changeCitiesPerPlayer(pCity->getOwnerINLINE(), iChange);
		pArea->changePopulationPerPlayer(pCity->getOwnerINLINE(), (pCity->getPopulation() * iChange));

		for (iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
		{
			if (pCity->getNumActiveBuilding((BuildingTypes)iI) > 0)
			{
				pArea->changePower(pCity->getOwnerINLINE(), (GC.getBuildingInfo((BuildingTypes)iI).getPowerValue() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));

				if (GC.getBuildingInfo((BuildingTypes) iI).getAreaHealth() > 0)
				{
					pArea->changeBuildingGoodHealth(pCity->getOwnerINLINE(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaHealth() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
				}
				else
				{
					pArea->changeBuildingBadHealth(pCity->getOwnerINLINE(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaHealth() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
				}
				pArea->changeBuildingHappiness(pCity->getOwnerINLINE(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaHappiness() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
				pArea->changeFreeSpecialist(pCity->getOwnerINLINE(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaFreeSpecialist() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));

				pArea->changeCleanPowerCount(pCity->getTeam(), ((GC.getBuildingInfo((BuildingTypes)iI).isAreaCleanPower()) ? iChange * pCity->getNumActiveBuilding((BuildingTypes)iI) : 0));

				pArea->changeBorderObstacleCount(pCity->getTeam(), ((GC.getBuildingInfo((BuildingTypes)iI).isAreaBorderObstacle()) ? iChange * pCity->getNumActiveBuilding((BuildingTypes)iI) : 0));

				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					pArea->changeYieldRateModifier(pCity->getOwnerINLINE(), ((YieldTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getAreaYieldModifier(iJ) * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
				}
			}
		}

		for (iI = 0; iI < NUM_UNITAI_TYPES; ++iI)
		{
			pArea->changeNumTrainAIUnits(pCity->getOwnerINLINE(), ((UnitAITypes)iI), (pCity->getNumTrainUnitAI((UnitAITypes)iI) * iChange));
		}

		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (pArea->getTargetCity((PlayerTypes)iI) == pCity)
			{
				pArea->setTargetCity(((PlayerTypes)iI), NULL);
			}
		}
	}
}


ColorTypes CvPlot::plotMinimapColor()
{
	CvUnit* pCenterUnit;

	if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
	{
		CvCity* pCity;

		pCity = getPlotCity();

		if ((pCity != NULL) && pCity->isRevealed(GC.getGameINLINE().getActiveTeam(), true))
		{
			return (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE");
		}

		if (isActiveVisible(true))
		{
			pCenterUnit = getDebugCenterUnit();

			if (pCenterUnit != NULL)
			{
				return ((ColorTypes)(GC.getPlayerColorInfo(GET_PLAYER(pCenterUnit->getVisualOwner()).getPlayerColor()).getColorTypePrimary()));
			}
		}

		//if ((getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true) != NO_PLAYER) && !isRevealedBarbarian())
		if (/*!isWater() &&*/ (getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true) != NO_PLAYER) && !isRevealedBarbarian()) // edead
		{
			return ((ColorTypes)(GC.getPlayerColorInfo(GET_PLAYER(getRevealedOwner(GC.getGameINLINE().getActiveTeam(), true)).getPlayerColor()).getColorTypePrimary()));
		}
	}

	return (ColorTypes)GC.getInfoTypeForString("COLOR_CLEAR");
}

//
// read object from a stream
// used during load
//
void CvPlot::read(FDataStreamBase* pStream)
{
	int iI;
	bool bVal;
	char cCount;
	int iCount;

	// Init saved data
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	pStream->Read(&m_iArea);
	// m_pPlotArea not saved
	pStream->Read(&m_iFeatureVariety);
	pStream->Read(&m_iOwnershipDuration);
	pStream->Read(&m_iImprovementDuration);
	pStream->Read(&m_iUpgradeProgress);
	pStream->Read(&m_iForceUnownedTimer);
	pStream->Read(&m_iCityRadiusCount);
	pStream->Read(&m_iRiverID);
	pStream->Read(&m_iMinOriginalStartDist);
	pStream->Read(&m_iReconCount);
	pStream->Read(&m_iRiverCrossingCount);

	pStream->Read(&bVal);
	m_bStartingPlot = bVal;
	pStream->Read(&bVal);
	m_bHills = bVal;
	pStream->Read(&bVal);
	m_bNOfRiver = bVal;
	pStream->Read(&bVal);
	m_bWOfRiver = bVal;
	pStream->Read(&bVal);
	m_bIrrigated = bVal;
	pStream->Read(&bVal);
	m_bPotentialCityWork = bVal;
	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved
	pStream->Read(&m_bWithinGreatWall); // Leoreth

	pStream->Read(&m_eOwner);
	pStream->Read(&m_ePlotType);
	pStream->Read(&m_eTerrainType);
	pStream->Read(&m_eFeatureType);
	pStream->Read(&m_eBonusType);
	pStream->Read(&m_eImprovementType);
	pStream->Read(&m_eRouteType);
	pStream->Read(&m_eRiverNSDirection);
	pStream->Read(&m_eRiverWEDirection);

	pStream->Read((int*)&m_plotCity.eOwner);
	pStream->Read(&m_plotCity.iID);
	pStream->Read((int*)&m_workingCity.eOwner);
	pStream->Read(&m_workingCity.iID);
	pStream->Read((int*)&m_workingCityOverride.eOwner);
	pStream->Read(&m_workingCityOverride.iID);

	pStream->Read(NUM_YIELD_TYPES, m_aiYield);

	SAFE_DELETE_ARRAY(m_aiCulture);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiCulture = new int[cCount];
		pStream->Read(cCount, m_aiCulture);
	}

	SAFE_DELETE_ARRAY(m_aiFoundValue);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiFoundValue = new short[cCount];
		pStream->Read(cCount, m_aiFoundValue);
	}

	SAFE_DELETE_ARRAY(m_aiPlayerCityRadiusCount);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiPlayerCityRadiusCount = new char[cCount];
		pStream->Read(cCount, m_aiPlayerCityRadiusCount);
	}

	SAFE_DELETE_ARRAY(m_aiPlotGroup);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiPlotGroup = new int[cCount];
		pStream->Read(cCount, m_aiPlotGroup);
	}

	SAFE_DELETE_ARRAY(m_aiVisibilityCount);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiVisibilityCount = new short[cCount];
		pStream->Read(cCount, m_aiVisibilityCount);
	}

	SAFE_DELETE_ARRAY(m_aiStolenVisibilityCount);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiStolenVisibilityCount = new short[cCount];
		pStream->Read(cCount, m_aiStolenVisibilityCount);
	}

	SAFE_DELETE_ARRAY(m_aiBlockadedCount);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiBlockadedCount = new short[cCount];
		pStream->Read(cCount, m_aiBlockadedCount);
	}

	SAFE_DELETE_ARRAY(m_aiRevealedOwner);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aiRevealedOwner = new char[cCount];
		pStream->Read(cCount, m_aiRevealedOwner);
	}

	SAFE_DELETE_ARRAY(m_abRiverCrossing);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_abRiverCrossing = new bool[cCount];
		pStream->Read(cCount, m_abRiverCrossing);
	}

	SAFE_DELETE_ARRAY(m_abRevealed);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_abRevealed = new bool[cCount];
		pStream->Read(cCount, m_abRevealed);
	}

	SAFE_DELETE_ARRAY(m_aeRevealedImprovementType);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aeRevealedImprovementType = new short[cCount];
		pStream->Read(cCount, m_aeRevealedImprovementType);
	}

	SAFE_DELETE_ARRAY(m_aeRevealedRouteType);
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_aeRevealedRouteType = new short[cCount];
		pStream->Read(cCount, m_aeRevealedRouteType);
	}

	m_szScriptData = pStream->ReadString();

	SAFE_DELETE_ARRAY(m_paiBuildProgress);
	pStream->Read(&iCount);
	if (iCount > 0)
	{
		m_paiBuildProgress = new short[iCount];
		pStream->Read(iCount, m_paiBuildProgress);
	}

	if (NULL != m_apaiCultureRangeCities)
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiCultureRangeCities[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiCultureRangeCities);
	}
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_apaiCultureRangeCities = new char*[cCount];
		for (iI = 0; iI < cCount; ++iI)
		{
			pStream->Read(&iCount);
			if (iCount > 0)
			{
				m_apaiCultureRangeCities[iI] = new char[iCount];
				pStream->Read(iCount, m_apaiCultureRangeCities[iI]);
			}
			else
			{
				m_apaiCultureRangeCities[iI] = NULL;
			}
		}
	}

	if (NULL != m_apaiInvisibleVisibilityCount)
	{
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount);
	}
	pStream->Read(&cCount);
	if (cCount > 0)
	{
		m_apaiInvisibleVisibilityCount = new short*[cCount];
		for (iI = 0; iI < cCount; ++iI)
		{
			pStream->Read(&iCount);
			if (iCount > 0)
			{
				m_apaiInvisibleVisibilityCount[iI] = new short[iCount];
				pStream->Read(iCount, m_apaiInvisibleVisibilityCount[iI]);
			}
			else
			{
				m_apaiInvisibleVisibilityCount[iI] = NULL;
			}
		}
	}

	// Leoreth
	pStream->Read(NUM_MAJOR_PLAYERS, m_abCore);
	pStream->Read(NUM_MAJOR_PLAYERS, m_aiSettlerValue);
	pStream->Read(NUM_MAJOR_PLAYERS, m_aiWarValue);
	pStream->Read(NUM_RELIGIONS, m_aiReligionSpreadFactor);
	pStream->Read(NUM_RELIGIONS, m_aiReligionInfluence);
	pStream->Read(&m_iRegionID);

	// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
//	if (NULL != m_apaiPlayerDangerCache)
//	{
//		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
//		{
//			SAFE_DELETE_ARRAY(m_apaiPlayerDangerCache[iI]);
//		}
//		SAFE_DELETE_ARRAY(m_apaiPlayerDangerCache);
//	}
//	pStream->Read(&cCount);
//	if (cCount > 0)
//	{
//		m_apaiPlayerDangerCache = new short*[cCount];
//		for (iI = 0; iI < cCount; ++iI)
//		{
//			pStream->Read(&iCount);
//			if (iCount > 0)
//			{
//				m_apaiPlayerDangerCache[iI] = new short[iCount];
//				pStream->Read(iCount, m_apaiPlayerDangerCache[iI]);
//			}
//			else
//			{
//				m_apaiPlayerDangerCache[iI] = NULL;
//			}
//		}
//	}
	// Sanguo Mod Performance, end

	m_units.Read(pStream);
}

//
// write object to a stream
// used during save
//
void CvPlot::write(FDataStreamBase* pStream)
{
	uint iI;

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iArea);
	// m_pPlotArea not saved
	pStream->Write(m_iFeatureVariety);
	pStream->Write(m_iOwnershipDuration);
	pStream->Write(m_iImprovementDuration);
	pStream->Write(m_iUpgradeProgress);
	pStream->Write(m_iForceUnownedTimer);
	pStream->Write(m_iCityRadiusCount);
	pStream->Write(m_iRiverID);
	pStream->Write(m_iMinOriginalStartDist);
	pStream->Write(m_iReconCount);
	pStream->Write(m_iRiverCrossingCount);

	pStream->Write(m_bStartingPlot);
	pStream->Write(m_bHills);
	pStream->Write(m_bNOfRiver);
	pStream->Write(m_bWOfRiver);
	pStream->Write(m_bIrrigated);
	pStream->Write(m_bPotentialCityWork);
	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved
	pStream->Write(m_bWithinGreatWall);

	pStream->Write(m_eOwner);
	pStream->Write(m_ePlotType);
	pStream->Write(m_eTerrainType);
	pStream->Write(m_eFeatureType);
	pStream->Write(m_eBonusType);
	pStream->Write(m_eImprovementType);
	pStream->Write(m_eRouteType);
	pStream->Write(m_eRiverNSDirection);
	pStream->Write(m_eRiverWEDirection);

	pStream->Write(m_plotCity.eOwner);
	pStream->Write(m_plotCity.iID);
	pStream->Write(m_workingCity.eOwner);
	pStream->Write(m_workingCity.iID);
	pStream->Write(m_workingCityOverride.eOwner);
	pStream->Write(m_workingCityOverride.iID);

	pStream->Write(NUM_YIELD_TYPES, m_aiYield);

	if (NULL == m_aiCulture)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_PLAYERS);
		pStream->Write(MAX_PLAYERS, m_aiCulture);
	}

	if (NULL == m_aiFoundValue)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_PLAYERS);
		pStream->Write(MAX_PLAYERS, m_aiFoundValue);
	}

	if (NULL == m_aiPlayerCityRadiusCount)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_PLAYERS);
		pStream->Write(MAX_PLAYERS, m_aiPlayerCityRadiusCount);
	}

	if (NULL == m_aiPlotGroup)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_PLAYERS);
		pStream->Write(MAX_PLAYERS, m_aiPlotGroup);
	}

	if (NULL == m_aiVisibilityCount)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_aiVisibilityCount);
	}

	if (NULL == m_aiStolenVisibilityCount)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_aiStolenVisibilityCount);
	}

	if (NULL == m_aiBlockadedCount)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_aiBlockadedCount);
	}

	if (NULL == m_aiRevealedOwner)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_aiRevealedOwner);
	}

	if (NULL == m_abRiverCrossing)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)NUM_DIRECTION_TYPES);
		pStream->Write(NUM_DIRECTION_TYPES, m_abRiverCrossing);
	}

	if (NULL == m_abRevealed)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_abRevealed);
	}

	if (NULL == m_aeRevealedImprovementType)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_aeRevealedImprovementType);
	}

	if (NULL == m_aeRevealedRouteType)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		pStream->Write(MAX_TEAMS, m_aeRevealedRouteType);
	}

	pStream->WriteString(m_szScriptData);

	if (NULL == m_paiBuildProgress)
	{
		pStream->Write((int)0);
	}
	else
	{
		pStream->Write((int)GC.getNumBuildInfos());
		pStream->Write(GC.getNumBuildInfos(), m_paiBuildProgress);
	}

	if (NULL == m_apaiCultureRangeCities)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_PLAYERS);
		for (iI=0; iI < MAX_PLAYERS; ++iI)
		{
			if (NULL == m_apaiCultureRangeCities[iI])
			{
				pStream->Write((int)0);
			}
			else
			{
				pStream->Write((int)GC.getNumCultureLevelInfos());
				pStream->Write(GC.getNumCultureLevelInfos(), m_apaiCultureRangeCities[iI]);
			}
		}
	}

	if (NULL == m_apaiInvisibleVisibilityCount)
	{
		pStream->Write((char)0);
	}
	else
	{
		pStream->Write((char)MAX_TEAMS);
		for (iI=0; iI < MAX_TEAMS; ++iI)
		{
			if (NULL == m_apaiInvisibleVisibilityCount[iI])
			{
				pStream->Write((int)0);
			}
			else
			{
				pStream->Write((int)GC.getNumInvisibleInfos());
				pStream->Write(GC.getNumInvisibleInfos(), m_apaiInvisibleVisibilityCount[iI]);
			}
		}
	}

	// Leoreth
	pStream->Write(NUM_MAJOR_PLAYERS, m_abCore);
	pStream->Write(NUM_MAJOR_PLAYERS, m_aiSettlerValue);
	pStream->Write(NUM_MAJOR_PLAYERS, m_aiWarValue);
	pStream->Write(NUM_RELIGIONS, m_aiReligionSpreadFactor);
	pStream->Write(NUM_RELIGIONS, m_aiReligionInfluence);
	pStream->Write(m_iRegionID);

	// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
//	if (NULL == m_apaiPlayerDangerCache)
//	{
//		pStream->Write((char)0);
//	}
//	else
//	{
//		pStream->Write((char)MAX_PLAYERS);
//		for (iI=0; iI < MAX_PLAYERS; ++iI)
//		{
//			if (NULL == m_apaiPlayerDangerCache[iI])
//			{
//				pStream->Write((int)0);
//			}
//			else
//			{
//				pStream->Write(DANGER_RANGE);
//				pStream->Write(DANGER_RANGE, m_apaiPlayerDangerCache[iI]);
//			}
//		}
//	}
	// Sanguo Mod Performance, end

	m_units.Write(pStream);
}

void CvPlot::setLayoutDirty(bool bDirty)
{
	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	if (isLayoutDirty() != bDirty)
	{
		m_bPlotLayoutDirty = bDirty;

		if (isLayoutDirty() && (m_pPlotBuilder == NULL))
		{
			if (!updatePlotBuilder())
			{
				m_bPlotLayoutDirty = false;
			}
		}
	}
}

bool CvPlot::updatePlotBuilder()
{
	// Leoreth: graphics paging
	if ( !shouldHaveFullGraphics() )
	{
		return false;
	}

	if (GC.IsGraphicsInitialized() && shouldUsePlotBuilder())
	{
		if (m_pPlotBuilder == NULL) // we need a plot builder... but it doesn't exist
		{
			m_pPlotBuilder = gDLL->getPlotBuilderIFace()->create();
			gDLL->getPlotBuilderIFace()->init(m_pPlotBuilder, this);
		}

		return true;
	}

	return false;
}

bool CvPlot::isLayoutDirty() const
{
	return m_bPlotLayoutDirty;
}

bool CvPlot::isLayoutStateDifferent() const
{
	bool bSame = true;
	// is worked
	bSame &= m_bLayoutStateWorked == isBeingWorked();

	// done
	return !bSame;
}

void CvPlot::setLayoutStateToCurrent()
{
	m_bLayoutStateWorked = isBeingWorked();
}

//------------------------------------------------------------------------------------------------

void CvPlot::getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked)
{
	eType = NO_IMPROVEMENT;
	bWorked = false;

	if (GC.getGameINLINE().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	eType = getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true);

	if (eType == NO_IMPROVEMENT)
	{
		if (isActiveVisible(true))
		{
			if (isBeingWorked() && !isCity())
			{
				if (isWater())
				{
					eType = ((ImprovementTypes)(GC.getDefineINT("WATER_IMPROVEMENT")));
				}
				else
				{
					eType = ((ImprovementTypes)(GC.getDefineINT("LAND_IMPROVEMENT")));
				}
			}
		}
	}

	// worked state
	if (isActiveVisible(false) && isBeingWorked())
	{
		bWorked = true;
	}
}

void CvPlot::getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked)
{
	eType = NO_BONUS;
	bImproved = false;
	bWorked = false;

	if (GC.getGameINLINE().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	if (GC.getGameINLINE().isDebugMode())
	{
		eType = getBonusType();
	}
	else if (isRevealed(GC.getGameINLINE().getActiveTeam(), false))
	{
		eType = getBonusType(GC.getGameINLINE().getActiveTeam());
	}

	// improved and worked states ...
	if (eType != NO_BONUS)
	{
		ImprovementTypes eRevealedImprovement = getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true);

		if ((eRevealedImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eRevealedImprovement).isImprovementBonusTrade(eType))
		{
			bImproved = true;
			bWorked = isBeingWorked();
		}
	}
}

bool CvPlot::shouldUsePlotBuilder()
{
	bool bBonusImproved; bool bBonusWorked; bool bImprovementWorked;
	BonusTypes eBonusType;
	ImprovementTypes eImprovementType;
	getVisibleBonusState(eBonusType, bBonusImproved, bBonusWorked);
	getVisibleImprovementState(eImprovementType, bImprovementWorked);
	if(eBonusType != NO_BONUS || eImprovementType != NO_IMPROVEMENT)
	{
		return true;
	}
	return false;
}


int CvPlot::calculateMaxYield(YieldTypes eYield) const
{
	if (getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}

	int iMaxYield = calculateNatureYield(eYield, NO_TEAM);

	int iImprovementYield = 0;
	for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
	{
		iImprovementYield = std::max(calculateImprovementYieldChange((ImprovementTypes)iImprovement, eYield, NO_PLAYER, true), iImprovementYield);
	}
	iMaxYield += iImprovementYield;

	int iRouteYield = 0;
	for (int iRoute = 0; iRoute < GC.getNumRouteInfos(); iRoute++)
	{
		iRouteYield = std::max(GC.getRouteInfo((RouteTypes)iRoute).getYieldChange(eYield), iRouteYield);
	}
	iMaxYield += iRouteYield;

	if (isWater() && !isImpassable())
	{
		int iBuildingYield = 0;
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			CvBuildingInfo& building = GC.getBuildingInfo((BuildingTypes)iBuilding);
			iBuildingYield = std::max(building.getSeaPlotYieldChange(eYield) + building.getGlobalSeaPlotYieldChange(eYield), iBuildingYield);
		}
		iMaxYield += iBuildingYield;
	}

	if (isRiver())
	{
		int iBuildingYield = 0;
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			CvBuildingInfo& building = GC.getBuildingInfo((BuildingTypes)iBuilding);
			iBuildingYield = std::max(building.getRiverPlotYieldChange(eYield), iBuildingYield);
		}
		iMaxYield += iBuildingYield;
	}

	int iExtraYieldThreshold = 0;
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); iTrait++)
	{
		CvTraitInfo& trait = GC.getTraitInfo((TraitTypes)iTrait);
		iExtraYieldThreshold  = std::max(trait.getExtraYieldThreshold(eYield), iExtraYieldThreshold);
	}
	if (iExtraYieldThreshold > 0 && iMaxYield > iExtraYieldThreshold)
	{
		iMaxYield += GC.getDefineINT("EXTRA_YIELD");
	}

	return iMaxYield;
}

int CvPlot::getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const
{
	int iYield = 0;

	bool bIgnoreFeature = false;
	if (getFeatureType() != NO_FEATURE)
	{
		if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
		{
			bIgnoreFeature = true;
		}
	}

	iYield += calculateNatureYield(eYield, getTeam(), bIgnoreFeature);

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (bWithUpgrade)
		{
			//in the case that improvements upgrade, use 2 upgrade levels higher for the
			//yield calculations.
			ImprovementTypes eUpgradeImprovement = (ImprovementTypes)GC.getImprovementInfo(eImprovement).getImprovementUpgrade();
			if (eUpgradeImprovement != NO_IMPROVEMENT)
			{
				//unless it's commerce on a low food tile, in which case only use 1 level higher
				if ((eYield != YIELD_COMMERCE) || (getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION()))
				{
					ImprovementTypes eUpgradeImprovement2 = (ImprovementTypes)GC.getImprovementInfo(eUpgradeImprovement).getImprovementUpgrade();
					if (eUpgradeImprovement2 != NO_IMPROVEMENT)
					{
						eUpgradeImprovement = eUpgradeImprovement2;
					}
				}
			}

			if ((eUpgradeImprovement != NO_IMPROVEMENT) && (eUpgradeImprovement != eImprovement))
			{
				eImprovement = eUpgradeImprovement;
			}
		}

		iYield += calculateImprovementYieldChange(eImprovement, eYield, getOwnerINLINE(), false);
	}

	RouteTypes eRoute = (RouteTypes)GC.getBuildInfo(eBuild).getRoute();
	if (eRoute != NO_ROUTE)
	{
		eImprovement = getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iYield += GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, iI);
				if (getRouteType() != NO_ROUTE)
				{
					iYield -= GC.getImprovementInfo(eImprovement).getRouteYieldChanges(getRouteType(), iI);
				}
			}
		}
	}


	return iYield;
}

bool CvPlot::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const
{
	FAssert(::isPlotEventTrigger(eTrigger));

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (kTrigger.isOwnPlot() && getOwnerINLINE() != ePlayer)
	{
		return false;
	}

	if (kTrigger.getPlotType() != NO_PLOT)
	{
		if (getPlotType() != kTrigger.getPlotType())
		{
			return false;
		}
	}

	if (kTrigger.getNumFeaturesRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumFeaturesRequired(); ++i)
		{
			if (kTrigger.getFeatureRequired(i) == getFeatureType())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumTerrainsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumTerrainsRequired(); ++i)
		{
			if (kTrigger.getTerrainRequired(i) == getTerrainType())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumImprovementsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumImprovementsRequired(); ++i)
		{
			if (kTrigger.getImprovementRequired(i) == getImprovementType())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumBonusesRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumBonusesRequired(); ++i)
		{
			if (kTrigger.getBonusRequired(i) == getBonusType(kTrigger.isOwnPlot() ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM))
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumRoutesRequired() > 0)
	{
		bool bFoundValid = false;

		if (NULL == getPlotCity())
		{
			for (int i = 0; i < kTrigger.getNumRoutesRequired(); ++i)
			{
				if (kTrigger.getRouteRequired(i) == getRouteType())
				{
					bFoundValid = true;
					break;
				}
			}

		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.isUnitsOnPlot())
	{
		bool bFoundValid = false;

		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (NULL != pUnitNode)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->getOwnerINLINE() == ePlayer)
			{
				if (-1 != pLoopUnit->getTriggerValue(eTrigger, this, false))
				{
					bFoundValid = true;
					break;
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}


	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); ++iI)
		{
			const EventTriggeredData* pTriggeredData = GET_PLAYER(ePlayer).getEventOccured((EventTypes)kTrigger.getPrereqEvent(iI));
			if (NULL == pTriggeredData || pTriggeredData->m_iPlotX != getX_INLINE() || pTriggeredData->m_iPlotY != getY_INLINE())
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}


	return true;
}

bool CvPlot::canApplyEvent(EventTypes eEvent) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.getFeatureChange() > 0)
	{
		if (NO_FEATURE != kEvent.getFeature())
		{
			if (NO_IMPROVEMENT != getImprovementType() || !canHaveFeature((FeatureTypes)kEvent.getFeature()))
			{
				return false;
			}
		}
	}
	else if (kEvent.getFeatureChange() < 0)
	{
		if (NO_FEATURE == getFeatureType())
		{
			return false;
		}
	}

	if (kEvent.getImprovementChange() > 0)
	{
		if (NO_IMPROVEMENT != kEvent.getImprovement())
		{
			if (!canHaveImprovement((ImprovementTypes)kEvent.getImprovement(), getTeam()))
			{
				return false;
			}
		}
	}
	else if (kEvent.getImprovementChange() < 0)
	{
		if (NO_IMPROVEMENT == getImprovementType())
		{
			return false;
		}
	}

	if (kEvent.getBonusChange() > 0)
	{
		if (NO_BONUS != kEvent.getBonus())
		{
			if (!canHaveBonus((BonusTypes)kEvent.getBonus(), false))
			{
				return false;
			}
		}
	}
	else if (kEvent.getBonusChange() < 0)
	{
		if (NO_BONUS == getBonusType())
		{
			return false;
		}
	}

	if (kEvent.getRouteChange() < 0)
	{
		if (NO_ROUTE == getRouteType())
		{
			return false;
		}

		if (isCity())
		{
			return false;
		}
	}

	return true;
}

void CvPlot::applyEvent(EventTypes eEvent)
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.getFeatureChange() > 0)
	{
		if (NO_FEATURE != kEvent.getFeature())
		{
			setFeatureType((FeatureTypes)kEvent.getFeature());
		}
	}
	else if (kEvent.getFeatureChange() < 0)
	{
		setFeatureType(NO_FEATURE);
	}

	if (kEvent.getImprovementChange() > 0)
	{
		if (NO_IMPROVEMENT != kEvent.getImprovement())
		{
			setImprovementType((ImprovementTypes)kEvent.getImprovement());
		}
	}
	else if (kEvent.getImprovementChange() < 0)
	{
		setImprovementType(NO_IMPROVEMENT);
	}

	if (kEvent.getBonusChange() > 0)
	{
		if (NO_BONUS != kEvent.getBonus())
		{
			setBonusType((BonusTypes)kEvent.getBonus());
		}
	}
	else if (kEvent.getBonusChange() < 0)
	{
		setBonusType(NO_BONUS);
	}

	if (kEvent.getRouteChange() > 0)
	{
		if (NO_ROUTE != kEvent.getRoute())
		{
			setRouteType((RouteTypes)kEvent.getRoute(), true);
		}
	}
	else if (kEvent.getRouteChange() < 0)
	{
		setRouteType(NO_ROUTE, true);
	}

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		int iChange = kEvent.getPlotExtraYield(i);
		if (0 != iChange)
		{
			GC.getGameINLINE().setPlotExtraYield(m_iX, m_iY, (YieldTypes)i, iChange);
		}
	}
}

bool CvPlot::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible) const
{
	CvCity* pCity = getPlotCity();

	if (GC.getUnitInfo(eUnit).isPrereqReligion())
	{
		if (NULL == pCity || pCity->getReligionCount() > 0)
		{
			return false;
		}
	}

	if (GC.getUnitInfo(eUnit).getPrereqReligion() != NO_RELIGION)
	{
		if (NULL == pCity || !pCity->isHasReligion((ReligionTypes)(GC.getUnitInfo(eUnit).getPrereqReligion())))
		{
			return false;
		}
	}

	if (GC.getUnitInfo(eUnit).getPrereqCorporation() != NO_CORPORATION)
	{
		if (NULL == pCity || !pCity->isActiveCorporation((CorporationTypes)(GC.getUnitInfo(eUnit).getPrereqCorporation())))
		{
			return false;
		}
	}

	if (GC.getUnitInfo(eUnit).isPrereqBonuses())
	{
		if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA)
		{
			bool bValid = false;

			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						if (pLoopPlot->area()->getNumTotalBonuses() > 0)
						{
							bValid = true;
							break;
						}
					}
				}
			}

			if (!bValid)
			{
				return false;
			}
		}
		else
		{
			if (area()->getNumTotalBonuses() > 0)
			{
				return false;
			}
		}
	}

	if (isCity())
	{
		if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA)
		{
			if (!isWater() && !isCoastalLand(GC.getUnitInfo(eUnit).getMinAreaSize()))
			{
				return false;
			}
		}
		else
		{
			if (area()->getNumTiles() < GC.getUnitInfo(eUnit).getMinAreaSize())
			{
				return false;
			}
		}
	}
	else
	{
		if (area()->getNumTiles() < GC.getUnitInfo(eUnit).getMinAreaSize())
		{
			return false;
		}

		if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA)
		{
			if (!isWater())
			{
				return false;
			}
		}
		else if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_LAND)
		{
			if (isWater())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if (!bTestVisible)
	{
		if (GC.getUnitInfo(eUnit).getHolyCity() != NO_RELIGION)
		{
			if (NULL == pCity || !pCity->isHolyCity(((ReligionTypes)(GC.getUnitInfo(eUnit).getHolyCity()))))
			{
				return false;
			}
		}

		if (GC.getUnitInfo(eUnit).getPrereqBuilding() != NO_BUILDING)
		{
			if (NULL == pCity)
			{
				return false;
			}

			if (pCity->getNumBuilding((BuildingTypes)(GC.getUnitInfo(eUnit).getPrereqBuilding())) == 0)
			{
				SpecialBuildingTypes eSpecialBuilding = ((SpecialBuildingTypes)(GC.getBuildingInfo((BuildingTypes)(GC.getUnitInfo(eUnit).getPrereqBuilding())).getSpecialBuildingType()));

				if ((eSpecialBuilding == NO_SPECIALBUILDING) || !(GET_PLAYER(getOwnerINLINE()).isSpecialBuildingNotRequired(eSpecialBuilding)))
				{
					return false;
				}
			}
		}

		if (GC.getUnitInfo(eUnit).getPrereqAndBonus() != NO_BONUS)
		{
			if (NULL == pCity)
			{
				if (!isPlotGroupConnectedBonus(getOwnerINLINE(), (BonusTypes)GC.getUnitInfo(eUnit).getPrereqAndBonus()))
				{
					return false;
				}
			}
			else
			{
				if (!pCity->hasBonus((BonusTypes)GC.getUnitInfo(eUnit).getPrereqAndBonus()))
				{
					return false;
				}
			}
		}

		bool bRequiresBonus = false;
		bool bNeedsBonus = true;

		for (int iI = 0; iI < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); ++iI)
		{
			if (GC.getUnitInfo(eUnit).getPrereqOrBonuses(iI) != NO_BONUS)
			{
				bRequiresBonus = true;

				if (NULL == pCity)
				{
					if (isPlotGroupConnectedBonus(getOwnerINLINE(), (BonusTypes)GC.getUnitInfo(eUnit).getPrereqOrBonuses(iI)))
					{
						bNeedsBonus = false;
						break;
					}
				}
				else
				{
					if (pCity->hasBonus((BonusTypes)GC.getUnitInfo(eUnit).getPrereqOrBonuses(iI)))
					{
						bNeedsBonus = false;
						break;
					}
				}
			}
		}

		if (bRequiresBonus && bNeedsBonus)
		{
			return false;
		}
	}

	return true;
}

int CvPlot::countFriendlyCulture(TeamTypes eTeam) const
{
	int iTotalCulture = 0;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			CvTeam& kLoopTeam = GET_TEAM(kLoopPlayer.getTeam());
			if (kLoopPlayer.getTeam() == eTeam || kLoopTeam.isVassal(eTeam) || kLoopTeam.isOpenBorders(eTeam))
			{
				iTotalCulture += getCulture((PlayerTypes)iPlayer);
			}
		}
	}

	return iTotalCulture;
}

int CvPlot::countNumAirUnits(TeamTypes eTeam) const
{
	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (DOMAIN_AIR == pLoopUnit->getDomainType() && !pLoopUnit->isCargo() && pLoopUnit->getTeam() == eTeam)
		{
			iCount += GC.getUnitInfo(pLoopUnit->getUnitType()).getAirUnitCap();
		}
	}

	return iCount;
}

int CvPlot::airUnitSpaceAvailable(TeamTypes eTeam) const
{
	int iMaxUnits = 0;

	CvCity* pCity = getPlotCity();
	if (NULL != pCity)
	{
		iMaxUnits = pCity->getAirUnitCapacity(getTeam());
	}
	else
	{
		iMaxUnits = GC.getDefineINT("CITY_AIR_UNIT_CAPACITY");
	}

	return (iMaxUnits - countNumAirUnits(eTeam));
}


bool CvPlot::isEspionageCounterSpy(TeamTypes eTeam) const
{
	CvCity* pCity = getPlotCity();

	if (NULL != pCity && pCity->getTeam() == eTeam)
	{
		if (pCity->getEspionageDefenseModifier() > 0)
		{
			return true;
		}
	}

	if (plotCount(PUF_isCounterSpy, -1, -1, NO_PLAYER, eTeam) > 0)
	{
		return true;
	}

	return false;
}

int CvPlot::getAreaIdForGreatWall() const
{
	return getArea();
}

int CvPlot::getSoundScriptId() const
{
	int iScriptId = -1;
	if (isActiveVisible(true))
	{
		if (getImprovementType() != NO_IMPROVEMENT)
		{
			iScriptId = GC.getImprovementInfo(getImprovementType()).getWorldSoundscapeScriptId();
		}
		else if (getFeatureType() != NO_FEATURE)
		{
			iScriptId = GC.getFeatureInfo(getFeatureType()).getWorldSoundscapeScriptId();
		}
		else if (getTerrainType() != NO_TERRAIN)
		{
			iScriptId = GC.getTerrainInfo(getTerrainType()).getWorldSoundscapeScriptId();
		}
	}
	return iScriptId;
}

int CvPlot::get3DAudioScriptFootstepIndex(int iFootstepTag) const
{
	if (getFeatureType() != NO_FEATURE)
	{
		return GC.getFeatureInfo(getFeatureType()).get3DAudioScriptFootstepIndex(iFootstepTag);
	}

	if (getTerrainType() != NO_TERRAIN)
	{
		return GC.getTerrainInfo(getTerrainType()).get3DAudioScriptFootstepIndex(iFootstepTag);
	}

	return -1;
}

float CvPlot::getAqueductSourceWeight() const
{
	float fWeight = 0.0f;
	if (isLake() || isPeak() || (getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).isAddsFreshWater()))
	{
		fWeight = 1.0f;
	}
	else if (isHills())
	{
		fWeight = 0.67f;
	}

	return fWeight;
}

bool CvPlot::shouldDisplayBridge(CvPlot* pToPlot, PlayerTypes ePlayer) const
{
	TeamTypes eObservingTeam = GET_PLAYER(ePlayer).getTeam();
	TeamTypes eOurTeam = getRevealedTeam(eObservingTeam, true);
	TeamTypes eOtherTeam = NO_TEAM;
	if (pToPlot != NULL)
	{
		eOtherTeam = pToPlot->getRevealedTeam(eObservingTeam, true);
	}

	if (eOurTeam == eObservingTeam || eOtherTeam == eObservingTeam || (eOurTeam == NO_TEAM && eOtherTeam == NO_TEAM))
	{
		return GET_TEAM(eObservingTeam).isBridgeBuilding();
	}

	if (eOurTeam == NO_TEAM)
	{
		return GET_TEAM(eOtherTeam).isBridgeBuilding();
	}

	if (eOtherTeam == NO_TEAM)
	{
		return GET_TEAM(eOurTeam).isBridgeBuilding();
	}

	return (GET_TEAM(eOurTeam).isBridgeBuilding() && GET_TEAM(eOtherTeam).isBridgeBuilding());
}

bool CvPlot::checkLateEra() const
{
	PlayerTypes ePlayer = getOwnerINLINE();
	if (ePlayer == NO_PLAYER)
	{
		//find largest culture in this plot
		ePlayer = GC.getGameINLINE().getActivePlayer();
		int maxCulture = getCulture(ePlayer);
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			int newCulture = getCulture((PlayerTypes) i);
			if (newCulture > maxCulture)
			{
				maxCulture = newCulture;
				ePlayer = (PlayerTypes) i;
			}
		}
	}

	return (GET_PLAYER(ePlayer).getCurrentEra() > GC.getNumEraInfos() / 2);
}

// Sanguo Mod Performance, start, added by poyuzhe 08.13.09
int CvPlot::getPlayerDangerCache(PlayerTypes ePlayer, int iRange)
{
	if (NULL == m_apaiPlayerDangerCache)
	{
		m_apaiPlayerDangerCache = new short*[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_apaiPlayerDangerCache[iI] = NULL;
		}
	}

	if (NULL == m_apaiPlayerDangerCache[ePlayer])
	{
		m_apaiPlayerDangerCache[ePlayer] = new short[DANGER_RANGE + 1];
		for (int iI = 0; iI < DANGER_RANGE + 1; ++iI)
		{
			m_apaiPlayerDangerCache[ePlayer][iI] = MAX_SHORT;
		}
	}

	return m_apaiPlayerDangerCache[ePlayer][iRange];
}

void CvPlot::setPlayerDangerCache(PlayerTypes ePlayer, int iRange, int iNewValue)
{
	if (NULL == m_apaiPlayerDangerCache)
	{
		m_apaiPlayerDangerCache = new short*[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_apaiPlayerDangerCache[iI] = NULL;
		}
	}

	if (NULL == m_apaiPlayerDangerCache[ePlayer])
	{
		m_apaiPlayerDangerCache[ePlayer] = new short[DANGER_RANGE + 1];
		for (int iI = 0; iI < DANGER_RANGE + 1; ++iI)
		{
			m_apaiPlayerDangerCache[ePlayer][iI] = MAX_SHORT;
		}
	}

	m_apaiPlayerDangerCache[ePlayer][iRange] = iNewValue;
}

void CvPlot::invalidatePlayerDangerCache(PlayerTypes ePlayer, int iRange)
{
	if (NULL == m_apaiPlayerDangerCache)
	{
		m_apaiPlayerDangerCache = new short*[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_apaiPlayerDangerCache[iI] = NULL;
		}
	}

	if (NULL == m_apaiPlayerDangerCache[ePlayer])
	{
		m_apaiPlayerDangerCache[ePlayer] = new short[DANGER_RANGE + 1];
		for (int iI = 0; iI < DANGER_RANGE + 1; ++iI)
		{
			m_apaiPlayerDangerCache[ePlayer][iI] = MAX_SHORT;
		}
	}

	m_apaiPlayerDangerCache[ePlayer][iRange] = MAX_SHORT;
}
// Sanguo Mod Performance, end

int CvPlot::getRegionID() const
{
	return m_iRegionID;
}

void CvPlot::setRegionID(int iNewValue)
{
	m_iRegionID = iNewValue;
}

CvWString CvPlot::getRegionName() const
{
	char szBuffer[20];
	CvWString szResult;
	sprintf(szBuffer, "TXT_KEY_REGION_%d", getRegionID());
	szResult = gDLL->getText(szBuffer);
	return gDLL->getText(szResult);
}


bool CvPlot::isCore(PlayerTypes ePlayer) const
{
	FAssertMsg(ePlayer >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (ePlayer >= NUM_MAJOR_PLAYERS) return false;
	return m_abCore[ePlayer];
}


void CvPlot::setCore(PlayerTypes ePlayer, bool bNewValue)
{
	FAssertMsg(ePlayer >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "eTeam is expected to be within maximum bounds (invalid Index)");

	m_abCore[ePlayer] = bNewValue;
}

// Leoreth
int CvPlot::getSettlerValue(PlayerTypes ePlayer) const
{
	if (ePlayer >= NUM_MAJOR_PLAYERS) return 0;
	return m_aiSettlerValue[ePlayer];
}

void CvPlot::setSettlerValue(PlayerTypes ePlayer, int iNewValue)
{
	m_aiSettlerValue[ePlayer] = iNewValue;
}


int CvPlot::getWarValue(PlayerTypes ePlayer) const
{
	if (ePlayer >= NUM_MAJOR_PLAYERS) return 0;
	return m_aiWarValue[ePlayer];
}

void CvPlot::setWarValue(PlayerTypes ePlayer, int iNewValue)
{
	m_aiWarValue[ePlayer] = iNewValue;
}


int CvPlot::getSpreadFactor(ReligionTypes eReligion) const
{
	int iSpreadFactor = m_aiReligionSpreadFactor[eReligion];

	if (eReligion == JUDAISM)
	{
		if (!GC.getGameINLINE().isReligionFounded(ORTHODOXY))
		{
			return getSpreadFactor(ORTHODOXY);
		}
	}
	
	if (eReligion == ORTHODOXY)
	{
		if (!GC.getGameINLINE().isReligionFounded(ISLAM))
		{
			if (getSpreadFactor(ISLAM) == REGION_SPREAD_CORE)
			{
				return REGION_SPREAD_CORE;
			}
		}

		if (!GC.getGameINLINE().isReligionFounded(CATHOLICISM))
		{
			if (iSpreadFactor < getSpreadFactor(CATHOLICISM))
			{
				return getSpreadFactor(CATHOLICISM);
			}
		}
	}

	if (eReligion == CATHOLICISM)
	{
		if (!GC.getGameINLINE().isReligionFounded(PROTESTANTISM))
		{
			if (iSpreadFactor < getSpreadFactor(PROTESTANTISM))
			{
				return getSpreadFactor(PROTESTANTISM);
			}
		}
	}

	return iSpreadFactor;
}

void CvPlot::setSpreadFactor(ReligionTypes eReligion, int iNewValue)
{
	m_aiReligionSpreadFactor[eReligion] = iNewValue;
}


// Leoreth
bool CvPlot::isWithinGreatWall() const
{
	return m_bWithinGreatWall;
}

// Leoreth
void CvPlot::setWithinGreatWall(bool bNewValue)
{
	m_bWithinGreatWall = bNewValue;
}

// Leoreth
void CvPlot::cameraLookAt()
{
	gDLL->getEngineIFace()->cameraLookAt(getPoint());
}

// Leoreth
int CvPlot::calculateCultureCost() const
{
	int iCost = 0;

	iCost += GC.getTerrainInfo(getTerrainType()).getCultureCostModifier();
	if (getFeatureType() >= 0) iCost += GC.getFeatureInfo(getFeatureType()).getCultureCostModifier();

	if (isHills()) iCost += GC.getDefineINT("CULTURE_COST_HILL");
	if (isPeak()) iCost += GC.getDefineINT("CULTURE_COST_PEAK");

	return iCost;
}

// Leoreth
bool CvPlot::canUseSlave(PlayerTypes ePlayer) const
{
	if (GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian()) return false;

	if (GET_PLAYER(ePlayer).getNumCities() == 0) return false;

	if (GET_PLAYER(ePlayer).getCapitalCity() == NULL) return false;

	int rid = GET_PLAYER(ePlayer).getCapitalCity()->getRegionID();

	switch (getRegionID())
	{
	case REGION_ALASKA:
	case REGION_CANADA:
	case REGION_UNITED_STATES:
	case REGION_CARIBBEAN:
	case REGION_MESOAMERICA:
	case REGION_BRAZIL:
	case REGION_ARGENTINA:
	case REGION_PERU:
	case REGION_COLOMBIA:
		return true;
	case REGION_ETHIOPIA:
	case REGION_WEST_AFRICA:
	case REGION_SOUTH_AFRICA:
		if (rid != REGION_ETHIOPIA && rid != REGION_WEST_AFRICA && rid != REGION_SOUTH_AFRICA) return true;
	default:
		return false;
	}
}

// Leoreth
int CvPlot::getReligionInfluence(ReligionTypes eReligion) const
{
	return m_aiReligionInfluence[eReligion];
}

void CvPlot::setReligionInfluence(ReligionTypes eReligion, int iNewValue)
{
	m_aiReligionInfluence[eReligion] = iNewValue;
}

void CvPlot::changeReligionInfluence(ReligionTypes eReligion, int iChange)
{
	m_aiReligionInfluence[eReligion] += iChange;
}

bool CvPlot::canSpread(ReligionTypes eReligion) const
{
	return getReligionInfluence(eReligion) > 0;
}

bool CvPlot::isPlains() const
{
	return isFlatlands() && (getFeatureType() == NO_FEATURE || GC.getFeatureInfo(getFeatureType()).getDefenseModifier() <= 0) && !isCity(true);
}