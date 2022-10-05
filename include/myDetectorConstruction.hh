//mandatory DetektorConstruction Header

#ifndef myDetectorConstruction_h
#define myDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class myECalSD;
class G4UserLimits;
class G4Material;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class myDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
	
	myDetectorConstruction(G4double, G4double, G4double);
	virtual ~myDetectorConstruction();
	
	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSD(G4bool*);
	
	G4double	GetWorldHalfLength()	{return hWorldLength;}; 
	G4double	GetChamberOriginPos()	{return chamberOriginPos;}; 
	G4bool		GetIfTargetInPlace()	{return targetInPlace;};
	
	static const G4int maxNbOfClusters = 5; //number of cluster detectors
	static const G4int maxNbOfCrystals = 7; // number of crystals per cluster
	G4double ClusterDistance[maxNbOfClusters];
	G4double ClusterAngle[maxNbOfClusters];
	G4int NbOfCrystals[maxNbOfClusters];
	G4int NbOfClusters;
	G4String nameCluster[maxNbOfClusters];
	G4bool ConstructBGO[maxNbOfClusters]; //if Detector is without BGO
	G4Material* Ge;	
	G4Material* BGO;
	G4Material* Al;
	
  private:
	
	void DestroyMaterials();
	void DefineMaterials();
	void InitializeDetectorParameters();
	void SetupGeometry();
	void TargetChamberGeometry();
	void GastargetGeometry();
	void YTargetChamberGeometryWaterCooled();
	void YTargetChamberGeometryLN2Cooled();
	void HPGe60Geometry(G4int, G4Transform3D, G4bool);
	void HPGe90Geometry(G4Transform3D);
	void ClusterBGOGeometry(G4int);
	void CrystalwDeadLayersGeometry(G4int, G4int, G4VPhysicalVolume*, G4double, G4double);
	void TripleClusterGeometry(G4int, G4Transform3D, G4bool);
	void SeptupleClusterHousing(G4int, G4Transform3D, G4bool);
	void SeptupleClusterGeometry(G4int, G4Transform3D, G4bool);
	void SeptupleClusterBGOGeometry(G4int, G4Transform3D);
	void MonolithicShieldingGeometry(G4Transform3D);
	void HPGe100Geometry();
	void InitializeVisAtt();
	
	G4bool constructed;							// check if construction is needed
	G4LogicalVolume* 	logicWorld;				// pointer to the logical envelope
	G4VPhysicalVolume* physicWorld;				// pointer to the physical envelope
	// Target chamber components
	G4VPhysicalVolume* physicTarget;			// pointer to the target
	G4VPhysicalVolume* physicTargetChamber;		// pointers to the taget chamber
	G4VPhysicalVolume* physicBacking; 			// pointers to the backing
	G4VPhysicalVolume* physicChamberVac;		// "Drills" cylinder in chamber for vacuum
	G4VPhysicalVolume* physicCopperPipe;		// pointers to the copper pipe
	G4VPhysicalVolume* physicTargetSocket;		// pointers to cone of target socket 
	G4VPhysicalVolume* physicCooling;			// Target-Cooling

	//Y target chamber
    G4LogicalVolume* Ytarget_logical;	// is read in from a gdml file
	G4LogicalVolume* Ytarget_ES_sampleholder_logical; // each subpart of the file is saved in a seperate logical volume
	G4LogicalVolume* Ytarget_ES_ychamber_logical;
	G4LogicalVolume* Ytarget_CU_tube_logical;
	G4LogicalVolume* Ytarget_ES_smallEndCap_logical;
	G4LogicalVolume* Ytarget_ES_coolingBlock_logical;
	G4LogicalVolume* Ytarget_coolingWater_logical;
	G4LogicalVolume* Ytarget_target_logical;
	G4VPhysicalVolume* Ytarget_physical;
	G4LogicalVolume* logicSample;
	G4VPhysicalVolume* physicSample;
	
	//Y target chamber LN2 cooled
	G4VPhysicalVolume* physicYTargetChamberLN2;

	// Triple cluster detector
	G4LogicalVolume* TripleClusterDetector_logical[maxNbOfClusters];		// the detector world volum from gdml file
	G4LogicalVolume* TripleClusterCrystal_logical[maxNbOfClusters][maxNbOfCrystals];			// logical crystals
	G4VPhysicalVolume* TripleClusterDetector_physical[maxNbOfClusters];	// The whole detector
	G4VPhysicalVolume* TripleClusterCrystal_physical[maxNbOfClusters][maxNbOfCrystals];		// crystals

	//Cluster Crystals
	G4double dDeadLayerCap[maxNbOfClusters][maxNbOfCrystals];
	G4double hDeadLayerCone[maxNbOfClusters][maxNbOfCrystals];
	G4LogicalVolume* ActiveCrystal_logical[maxNbOfClusters][maxNbOfCrystals];
	G4VPhysicalVolume* ActiveCrystal_physical[maxNbOfClusters][maxNbOfCrystals];
	myECalSD* ActiveCrystals_SD[maxNbOfClusters];							// sensitive detector for HPGe crystals
	
	G4double dCluster1;
	G4double dCluster2;
	G4double dCluster3;
	G4double tiltCluster2;
	G4int version;
	//Septuple cluster detector
	G4LogicalVolume* SeptupleClusterVacuum_logical[maxNbOfClusters];
	G4LogicalVolume* SeptupleClusterHousing_logical[maxNbOfClusters];
	G4LogicalVolume* SeptupleClusterBGO_logical[maxNbOfClusters];
	G4VPhysicalVolume* SeptupleClusterVacuum_phys[maxNbOfClusters];
	G4VPhysicalVolume* SeptupleClusterHousing_phys[maxNbOfClusters];
	G4VPhysicalVolume* SeptupleClusterBGO_phys[maxNbOfClusters];
	//Triple cluster BGO
	G4double dBGOHousing;
	G4double dOffsetBGO;
	G4LogicalVolume* BGOCrystal_logic[maxNbOfClusters];				// logical BGO crystals
	G4LogicalVolume* BGOHousing_logical[maxNbOfClusters];			// Housing of BGO (full Al)
	G4VPhysicalVolume* housingBGOvac_physical[maxNbOfClusters];		// Inner vacuum of BGO housing
	G4VPhysicalVolume* BGOCrystal_physical[maxNbOfClusters][8];		// 8 cloned BGO crystals
	myECalSD* tripleClusterBGOSD[maxNbOfClusters];					// sensitive detector BGO crystals
	
	// HPGe60 detector 	components
	G4VPhysicalVolume* physicHPGe60PbShield;	// Lead shield of HPGe60 detector
	G4VPhysicalVolume* physicHPGe60Casing;		// Casing of HPGe60 detector
	G4VPhysicalVolume* physicHPGe60Vac;			// Space for HPGe60 vacuum
	G4VPhysicalVolume* physicHPGe60GeCrystal;	// HPGe crystal
	G4VPhysicalVolume* physicHPGe60InnerCtLayer; // HPGe60 inner contact layer
	G4VPhysicalVolume* physicHPGe60OuterCtLayer; // HPGe60 outer contact layer
	G4VPhysicalVolume* physicHPGe60Holder;		// Holder for crystal
	//dead layers HPGe60
	G4double hPGe60OuterCtLayerThickness; 	// Outer Contactlayer Thickness of HPGe60 (Deadlayer)
	G4double hPGe60InnerCtLayerThickness;

	// HPGe90 detector 	components
	G4VPhysicalVolume* physicHPGe90PbShield;	// Lead shield of HPGe60 detector
	G4VPhysicalVolume* physicHPGe90Casing;		// Casing of HPGe60 detector
	G4VPhysicalVolume* physicHPGe90Vac;			// Space for HPGe60 vacuum
	G4LogicalVolume*   logicHPGe90GeCrystal;	// logical HPGe crystal
	G4VPhysicalVolume* physicHPGe90GeCrystal;	// HPGe crystal
	G4VPhysicalVolume* physicHPGe90InnerCtLayer; // HPGe60 inner contact layer
	G4VPhysicalVolume* physicHPGe90OuterCtLayer; // HPGe60 outer contact layer
	G4VPhysicalVolume* physicHPGe90Holder;		// Holder for crystal
	myECalSD* HPGe90SD;
	myECalSD* HPGe90BGOSD;
	
	//Monolithic Shielding
	G4LogicalVolume* shieldingCone_log;		// the shielding world volum from gdml file
	G4VPhysicalVolume* shieldingCone_phys;	// its physical
	G4LogicalVolume*   logicHPGe90BGO;	// logical HPGe crystal
		
	// 100% detector components
	G4LogicalVolume*   DET1BGO_logical;			// logical BGO Anti-Compton Shield
	G4VPhysicalVolume* DET1BGO_physical;		// BGO Anti-Compton Shield
	G4LogicalVolume*   DET1HPGE_logical;		// logical HPGe 100% detector
	G4VPhysicalVolume* DET1HPGE_physical;		// HPGe 100% detector
	G4VPhysicalVolume* DET1Contactlayer_physical;// HPGe100 contact layer
	G4VPhysicalVolume* DET1Coolingfinger_physical;// HPGe100 Coolingfinger
	G4VPhysicalVolume* DET1Alhousinga_physical;	// HPGe100 Alu housing
	G4VPhysicalVolume* DET1Alhousingb_physical;	// HPGe100 Alu housing
	G4VPhysicalVolume* DET1Alhousingc_physical;	// HPGe100 Alu housing
	G4VPhysicalVolume* DET1Alring_physical;		// HPGe100 Alu ring
	G4VPhysicalVolume* physicHPGe100Kollimator;	// lead kollimator in front of HPGe100
	G4VPhysicalVolume* physicHPGe100KolliPlate;	// kollimator holder HPGe100
	G4VPhysicalVolume* physicHPGe100PbShield;	// lead shield around HPGe100
	G4VPhysicalVolume* DET1Quartz_physical;		// HPGe100 Quartz layer
	G4VPhysicalVolume* physicHPGe100Setup;		// Hole HPGe100 + BGO setup
	// calibration component
	G4LogicalVolume* logicDisk;					// logical Calibration source disk
	G4VPhysicalVolume* physicDisk;				// Calibration source disk
	
	G4bool targetInPlace;				// Switch for calibration-source or target
	G4bool pSurfChk;					// Switch for geometrical overlap check (time-consuming)
	G4bool solidGe;						// general switch for ge visibility
	G4double hWorldLength;				// Half length of the world volume
	G4double chamberOriginPos;			// Origin of taget chamber
	G4double targetTilt;				// Tilt of target according beamaxis in degree
	G4double dFrontCone;
    G4double dCluster;
    G4double zPositionCluster;
    G4double dLeadRingsTotal;
    G4double dLeadRing1BGOHousing;
	G4double dCollimator;			// length Collimator

	G4SDManager* pSDman;						// sensitive detector manager
	G4MultiFunctionalDetector* sourceSD;		// sensitive detector for source
	
	G4Material* Air;
	G4Material* TiN;
	G4Material* Steel;
	G4Material* Cu;
	G4Material* Vacuum;
	G4Material* Ta;
	G4Material* Polyethylene;
	G4Material* B;
	G4Material* Li;
	G4Material* Pb;
	G4Material* Quartz;
	G4Material* Water;
	G4Material* C_mat;
	
	G4VisAttributes* ContactLayerVisAtt;
	G4VisAttributes* PreampVisAtt;
	G4VisAttributes* geVisAtt;
	G4VisAttributes* aluVisAtt;
	G4VisAttributes* pbVisAtt;
	G4VisAttributes* steelVisAtt;
	G4VisAttributes* diskVisAtt;
	G4VisAttributes* targetVisAtt;
	G4VisAttributes* backingVisAtt;
	G4VisAttributes* cuVisAtt;
	G4VisAttributes* vacVisAtt;
	G4VisAttributes* coolingVisAtt;
	G4VisAttributes* quartzVisAtt;
	G4VisAttributes* setupVisAtt;
	G4VisAttributes* BGOVisAtt;
};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
