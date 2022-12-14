//mandatory DetektorConstruction Class

#include "myDetectorConstruction.hh"
#include "myECalSD.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Polycone.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4NistManager.hh" 
#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
 
#include "G4GDMLParser.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSNofSecondary.hh"
#include "G4PSTrackLength.hh"
#include "G4PSNofStep.hh"
#include "G4PSMinKinEAtGeneration.hh"
#include "G4VSDFilter.hh"
#include "G4SDParticleFilter.hh"

#include "G4ios.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4UserLimits.hh"




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myDetectorConstruction::myDetectorConstruction(G4double _dCluster1, G4double _dCluster2, G4double _tiltCluster2)
:constructed(false),
 logicWorld(0), physicWorld(0),
 hWorldLength(0), chamberOriginPos(0), targetTilt(55*deg),
 dCluster1(_dCluster1), dCluster2(_dCluster2), tiltCluster2(_tiltCluster2)

{	
	InitializeDetectorParameters();
	G4cout << "--> Detector Construction initialized" << G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
myDetectorConstruction::~myDetectorConstruction()
{
	  DestroyMaterials();
}


void myDetectorConstruction::InitializeDetectorParameters()
{
	//energy resolution is defined in myEventAction.cc, make sure to change these as well
	//collimator for tripleClusters can be adjusted in SetupGeometry()
	NbOfClusters = 5;
	
	dCluster3 = 0.*mm; //shift in z for target holder
	
	nameCluster[0] = "Can60";
	NbOfCrystals[0] = 1;
	ClusterDistance[0] = 19.23*cm;
	ClusterAngle[0] = -139.1*deg;
	ConstructBGO[0] = false; //not implemented
	hPGe60OuterCtLayerThickness = 0.3 * mm; 	// Outer Contactlayer Thickness of HPGe60 (Deadlayer)
	hPGe60InnerCtLayerThickness = 0.0005 * mm;	// Inner Contactlayer Thickness of HPGe60 (Deadlayer)
	
	nameCluster[1] = "MB1";
	NbOfCrystals[1] = 3;
	ClusterDistance[1] = 43.*cm;
	ClusterAngle[1] = 117.3*deg;
	ConstructBGO[1] = true;
	dDeadLayerCap[1][0] = 0.2*mm, dDeadLayerCap[1][1] = 0.3*mm, dDeadLayerCap[1][2] = 0.2*mm;
	hDeadLayerCone[1][0] = 2.1*cm, hDeadLayerCone[1][1] = 2.3*cm, hDeadLayerCone[1][2] = 2.2*cm;
	
	nameCluster[2] = "MB2";
	NbOfCrystals[2] = 3;
	ClusterDistance[2] = 41.*cm;
	ClusterAngle[2] = 55.1*deg;
	ConstructBGO[2] = true;	
	dDeadLayerCap[2][0] = 0.05*mm, dDeadLayerCap[2][1] = 0.15*mm, dDeadLayerCap[2][2] = 0.25*mm;
	hDeadLayerCone[2][0] = 1.9*cm, hDeadLayerCone[2][1] = 2.*cm, hDeadLayerCone[2][2] = 2.5*cm;		
	
	nameCluster[3] = "EB18";
	NbOfCrystals[3] = 7;
	ClusterDistance[3] = 41.*cm + tiltCluster2;
	ClusterAngle[3] = -25.*deg;
	ConstructBGO[3] = true; //not implemented
	dDeadLayerCap[3][0] = 0.2*mm, dDeadLayerCap[3][1] = 0.2*mm, dDeadLayerCap[3][2] = 0.4*mm;
	dDeadLayerCap[3][3] = 0.35*mm, dDeadLayerCap[3][4] = 0.05*mm, dDeadLayerCap[3][5] = 0.4*mm, dDeadLayerCap[3][6] = 0.5*mm;
	hDeadLayerCone[3][0] = 2.5*cm, hDeadLayerCone[3][1] = 2.2*cm, hDeadLayerCone[3][2] = 3.4*cm;
	hDeadLayerCone[3][3] = 2.8*cm, hDeadLayerCone[3][4] = 2.2*cm, hDeadLayerCone[3][5] = 3.1*cm, hDeadLayerCone[3][6] = 3.2*cm;	
		
	nameCluster[4] = "EB17";
	NbOfCrystals[4] = 7;
	ClusterDistance[4] = 10.9*cm + tiltCluster2;
	//ClusterDistance[4] = 0.*cm;
	ClusterAngle[4] = -90.*deg;	
	ConstructBGO[4] = false; //not implemented
	dDeadLayerCap[4][0] = 0.2*mm, dDeadLayerCap[4][1] = 0.65*mm, dDeadLayerCap[4][2] = 0.1*mm;
	dDeadLayerCap[4][3] = 0.15*mm, dDeadLayerCap[4][4] = 0.01*mm, dDeadLayerCap[4][5] = 0.2*mm, dDeadLayerCap[4][6] = 0.2*mm;
	hDeadLayerCone[4][0] = 3.3*cm, hDeadLayerCone[4][1] = 2.5*cm, hDeadLayerCone[4][2] = 2.4*cm;
	hDeadLayerCone[4][3] = 2.6*cm, hDeadLayerCone[4][4] = 2.4*cm, hDeadLayerCone[4][5] = 2.3*cm, hDeadLayerCone[4][6] = 2.2*cm;
}	 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* myDetectorConstruction::Construct()
{
	
	if(!constructed)
	{ 
		//		Cleanup old geometry
		G4GeometryManager::GetInstance()->OpenGeometry();
		G4PhysicalVolumeStore::GetInstance()->Clean();
		G4LogicalVolumeStore::GetInstance()->Clean();
		G4SolidStore::GetInstance()->Clean();
		
		constructed = true;

		DefineMaterials();
		InitializeVisAtt();
		SetupGeometry();
		ConstructSD(ConstructBGO);
	} 
	G4cout << "--> Geometry constructed" << G4endl;
	return physicWorld;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myDetectorConstruction::DefineMaterials()
{
	//--------- Material definition ---------
	// Use NIST database for elements and materials whereever possible.

	G4double abundance, a, ia, iz, density, pressure, temperature;
	G4int nel, ncomponents, nAtoms;						// Number of Elementes, Isotops and Atoms in Formula
	G4String symbol;												
	auto nistManager = G4NistManager::Instance();					// NIST Materialdatabank-manager
	nistManager->SetVerbose(0);
	
	Air = nistManager->FindOrBuildMaterial("G4_AIR");   

	Vacuum = new G4Material("Vacuum", density= 1.205e-7 *mg/cm3, nel=1, 
			kStateGas, temperature=300*kelvin, pressure=1.0e-2*pascal);
	Vacuum->AddMaterial(Air,1);
	
	//~ G4Element* Ti = nistManager->FindOrBuildElement("Ti");			// Titan
	//~ G4Element* N = nistManager->FindOrBuildElement("N");			// Nitrogen
		
	//~ TiN = new G4Material("TiN", density=5.22*g/cm3, nel=2);		// Titannitrid
	//~ TiN->AddElement(Ti, nAtoms= 1);
	//~ TiN->AddElement(N, nAtoms= 1);
	 	
	G4Element* Fe = nistManager->FindOrBuildElement("Fe");			// Iron
	G4Element* C = nistManager->FindOrBuildElement("C");			// Carbon
	G4Element* Cr = nistManager->FindOrBuildElement("Cr");			// Chrom
	G4Element* Ni = nistManager->FindOrBuildElement("Ni");			// Nickle
		
	Steel = new G4Material("Steel", density=7.86*g/cm3, nel=4, kStateSolid);
	Steel->AddElement(Fe, 70*perCent);
	Steel->AddElement(C, 1*perCent);
	Steel->AddElement(Cr, 18*perCent);
	Steel->AddElement(Ni, 11*perCent);
	
	C_mat = nistManager->FindOrBuildMaterial("G4_C");				// Carbon
	Cu = nistManager->FindOrBuildMaterial("G4_Cu");   				// Copper
	Al = nistManager->FindOrBuildMaterial("G4_Al");   				// Aluminium
	Ta = nistManager->FindOrBuildMaterial("G4_Ta"); 				// Tantal
	//~ Polyethylene = nistManager->FindOrBuildMaterial("G4_POLYETHYLENE");
	//~ B = nistManager->FindOrBuildMaterial("G4_B");					// Boron
	BGO  =  nistManager ->FindOrBuildMaterial("G4_BGO");			// Bismutgermanat	
	//~ Li =  nistManager ->FindOrBuildMaterial("G4_Li");				// Lithium
	Pb =  nistManager ->FindOrBuildMaterial("G4_Pb");				// Lead

	//~ G4Element* Si = nistManager->FindOrBuildElement("Si");			// Silicon
	//~ G4Element* O = nistManager->FindOrBuildElement("O");			// Oxygen
	
	//~ Quartz = new G4Material("Quartz", 2.65*g/cm3, nel=2, kStateSolid);
	//~ Quartz-> AddElement(Si, 1);
	//~ Quartz-> AddElement(O, 2);
	
	Water = nistManager->FindOrBuildMaterial("G4_WATER");  
	
	// Germanium	
	a = 69.92*g/mole;
	G4Isotope* isGe70 = new G4Isotope("isGe70", iz=32, ia=70, a);
	a = 71.92*g/mole;
	G4Isotope* isGe72 = new G4Isotope("isGe72", iz=32, ia=72, a);
	a = 72.92*g/mole;
	G4Isotope* isGe73 = new G4Isotope("isGe73", iz=32, ia=73, a);
	a = 73.92*g/mole;
	G4Isotope* isGe74 = new G4Isotope("isGe74", iz=32, ia=74, a);
	a = 75.92*g/mole;
	G4Isotope* isGe76 = new G4Isotope("isGe76", iz=32, ia=76, a);
	
	G4Element* elGer = new G4Element("elGer",symbol="Ge",ncomponents=5);
	elGer->AddIsotope(isGe70, abundance=20.57*perCent);
	elGer->AddIsotope(isGe72, abundance=27.45*perCent);
	elGer->AddIsotope(isGe73, abundance=7.75*perCent);
	elGer->AddIsotope(isGe74, abundance=36.50*perCent);
	elGer->AddIsotope(isGe76, abundance=7.73*perCent);
	
	density = 5.323*g/cm3;
	Ge = new G4Material("GermaniumDef", density, nel=1);
	Ge->AddElement(elGer, 1.);
	
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myDetectorConstruction::SetupGeometry()
{
	//version = 2;

	targetTilt = (55. / 180. * pi) *rad;		// Tilt of Target according beamaxis in radiants, for 55deg target chamber
	pSurfChk = false;
	hWorldLength = 4 * m;						// Half length of experimental Hall
	targetInPlace = false;						// Switch for Calibration-Source or Target

	// experimental hall (world volume)
	G4Box* solidWorld= new G4Box("world",hWorldLength,hWorldLength,hWorldLength);
	logicWorld= new G4LogicalVolume(solidWorld, Air, "World");
	
	//  Must place the World Physical volume unrotated at (0,0,0).
	physicWorld = new G4PVPlacement(0,                // no rotation
									G4ThreeVector(), // at (0,0,0)
									logicWorld,      // its logical volume
									"World",         // its name
									0,               // its mother  volume
									false,           // no boolean operations
									0,              // copy number
									pSurfChk);          //overlaps checking
									
	logicWorld ->SetVisAttributes(G4VisAttributes::Invisible); 
	
	//construct target chamber
	//TargetChamberGeometry(); //55deg target chamber
	//YTargetChamberGeometryWaterCooled(); //y target chamber
	YTargetChamberGeometryLN2Cooled(); //y target chamber
	//GastargetGeometry(); //early blueprint
	
	
	////////////////////////////////////////////////////////////
	//Triple Cluster
	////////////////////////////////////////////////////////////
	//lengths of triple cluster for positioning, logical volume consists of two parts, front cone scales with collimator size
	dCollimator = 3.*cm; //3cm or 7cm
	dLeadRingsTotal = 236.*mm;
	dLeadRing1BGOHousing = 2.*cm; //first lead ring is 2cm protruding from BGOHousing
	dFrontCone = dLeadRingsTotal+dCollimator-dLeadRing1BGOHousing;
    dCluster = 700.*mm; //purposefully longer, so there is enough space to move back the detector inside the shielding
    zPositionCluster = dFrontCone/2.;
	//G4double hEndcapLength = 16.35*mm;
	//G4double hTargetChamber = 35 * mm; //70mm for gastarget, 35mm for Y and 55deg chamber
	G4Transform3D positionTripleCluster[maxNbOfClusters];
	G4RotationMatrix TripleClusterRotMatrix[maxNbOfClusters];
	G4double totalDistanceTripleCluster[maxNbOfClusters];
	G4Cons* collimatorCluster[maxNbOfClusters];
	G4LogicalVolume* collimatorCluster_logic[maxNbOfClusters];
	G4PVPlacement* collimatorCluster_physical[maxNbOfClusters];
	//G4Transform3D positioncollimatorCluster[maxNbOfClusters];
	for (G4int i = 0; i < NbOfClusters; i++) {
		if (NbOfCrystals[i] == 3) {	
			TripleClusterRotMatrix[i].rotateX(ClusterAngle[i]);
			G4cout << "distance " << nameCluster[i] << " = " << ClusterDistance[i]/cm << "cm" << G4endl;
			G4cout << "angle " << nameCluster[i] << " = " << ClusterAngle[i]/deg << "deg" << G4endl;
			totalDistanceTripleCluster[i] = zPositionCluster/2. + ClusterDistance[i];
			positionTripleCluster[i] = G4Transform3D(TripleClusterRotMatrix[i],G4ThreeVector(0*mm, -std::sin(ClusterAngle[i])*totalDistanceTripleCluster[i], std::cos(ClusterAngle[i]) * totalDistanceTripleCluster[i]));
			G4double distCollimator = totalDistanceTripleCluster[i] - 760./2.*mm - dCollimator/2.;
			//positioncollimatorCluster[i] = G4Transform3D(TripleClusterRotMatrix[i],G4ThreeVector(0*mm, -std::sin(ClusterAngle[i])*distCollimator, std::cos(ClusterAngle[i]) * distCollimator));
			TripleClusterGeometry(i, positionTripleCluster[i], ConstructBGO[i]);
		}
	}

	////////////////////////////////////////////////////////////
	//Septuple Cluster
	////////////////////////////////////////////////////////////		
	G4RotationMatrix rotationSeptupleCluster[maxNbOfClusters];
	G4ThreeVector vectorSept[maxNbOfClusters];
	G4Transform3D posSept[maxNbOfClusters];
	//G4double dSeptCluster = 140.*mm + 200.*mm;
	//G4double gdmlCorrection = -99.875*mm;
	G4double distanceSeptupleCluster[maxNbOfClusters];
	for (G4int i = 0; i < NbOfClusters; i++) {
		if (NbOfCrystals[i] == 7) {
			//ClusterAngle[i] = ClusterAngle[i]; //weirdly defined in the gdml
			//rotationSeptupleCluster[i].rotateZ(90.*deg); //for gdml housing
			rotationSeptupleCluster[i].rotateX(ClusterAngle[i]);
			G4cout << "distance " << nameCluster[i] << " = " << ClusterDistance[i]/cm << "cm" << G4endl;
			G4cout << "angle " << nameCluster[i] << " = " << ClusterAngle[i]/deg << "deg" << G4endl;
			//distanceSeptupleCluster[i] = -(dSeptCluster/2.+gdmlCorrection+ClusterDistance[i]);//for gdml housing
			distanceSeptupleCluster[i] = (1.*cm+ClusterDistance[i]);//1cm from sphere on top of detector
			vectorSept[i] = G4ThreeVector(0,-std::sin(ClusterAngle[i])*distanceSeptupleCluster[i],std::cos(ClusterAngle[i])*distanceSeptupleCluster[i]);
			posSept[i] = G4Transform3D(rotationSeptupleCluster[i],vectorSept[i]);	
			SeptupleClusterGeometry(i, posSept[i], ConstructBGO[i]);
		}
	}

	////////////////////////////////////////////////////////////
	//Monolithic
	////////////////////////////////////////////////////////////
	G4RotationMatrix rotationMono[maxNbOfClusters];
	G4ThreeVector vectorMono[maxNbOfClusters];
	G4Transform3D posMono[maxNbOfClusters];
	G4double distanceMono[maxNbOfClusters];
	G4double hHPGe60CasingLength =  50. * mm;
	for (G4int i = 0; i < NbOfClusters; i++) {
		if (NbOfCrystals[i] == 1) {
			//rotationMono[i].rotateZ(90.*deg);
			rotationMono[i].rotateX(ClusterAngle[i]);
			distanceMono[i] = ClusterDistance[i] + hHPGe60CasingLength;
			vectorMono[i] = G4ThreeVector(0,-std::sin(ClusterAngle[i])*distanceMono[i],std::cos(ClusterAngle[i])*distanceMono[i]);
			posMono[i] = G4Transform3D(rotationMono[i],vectorMono[i]);	
			HPGe60Geometry(i, posMono[i], ConstructBGO[i]); 
		}
	}		
	
}

void myDetectorConstruction::TargetChamberGeometry()
{
	//55deg target chamber by Louis, unchanged
	G4double targetRadius = 9. * mm;					// Radius of Target
	G4double backingRadius = 9.5 * mm;					// Radius of Target-Backing
	G4double hBackingLength = 0.11 *mm;					// Half height of Target-Backing
	G4double yEndCapLength =  64. * mm;					// Half y length of Chamber Endcap
	G4double xEndCapLength = 52.5 * mm;					// Half x length of Chamber Endcap
	G4double hEndCapLength = 0.5*16.35 * mm;				// Half height of Chamber Endcap nominal 16.35 * mm
	
	G4double EndCapTotalDiameter = 118.*mm;
	G4double EndCapCutDiameter = 105.*mm;
	G4double EndCapBackHeight = 17.*mm;
	G4double EndCapFrontInnerDiameter = 53.*mm;
	G4double EndCapFrontInnerCutDiameter = 80.5*mm;
	G4double EndCapFrontInnerCutHeight = 7.7*mm; 
	G4double EndCapFrontHeight = 15.*mm;
	G4double EndCapMiddleInnerDiameter = 19.*mm;
	G4double EndCapMiddleOuterDiameter = 79.9*mm;
	G4double EndCapMiddleHeight = 7.5*mm;
	
	G4double hTargetChamberLength = 196. * mm;			// Half height of Target Chamber
	G4double targetChamberRadius = 35. * mm;			// Radius of Target Chamber
	G4double champerPipeThickness = 2.* mm;			// Thickness of Chamber Pipe, nominally 2mm
	G4double hCopperPipeLength = 65. * mm;				// Half height of Copper Pipe
	G4double copperPipeRadius = 11. * mm;				// Radius of Copper Pipe
	G4double copperPipeThickness = 2. * mm;				// Thickness of Copper Pipe
	G4double cuPipeOffSet = 20. * mm;					// Distance of Copper Pipe to Target
	G4double hTargetLength  = (2.0e-3/5.22) * mm;		// Half height of Target
	G4double hTargetSocketLength = 1.5 * mm;			// Half height of Socket surrounding Target
	G4double maxTargetSocketRadius = 24 * mm;			// outer Radius of Socket
	G4double minTargetSocketRadius1 = backingRadius;	// first inner Radius of Socket 
	G4double minTargetSocketRadius2 = 15. * mm;			// second inner Radius of Socket 
	G4double hDiskLength = 0.15 * mm;					// Half height of Calibration-Source Disk
	G4double waterPipeRadius = 5 * mm;				// Radius of watercooling pipe 4.25*mm before
	G4double hTargetCoolingLength = 14.5 * mm;			// Half height of Target cooling	
	G4RotationMatrix TargetRotMatrix;					// Rotationmatrix for target
	TargetRotMatrix.rotateX(targetTilt);				// Rotation according Target tilt
	
	G4cout<< "\n--->Changed geometry for Targetchamber:" << G4endl;
	G4cout << "Target Chamber Thickness: " << champerPipeThickness/mm << " mm\n";
	G4cout << "Endcap Thickness: " << hEndCapLength*2./mm << " mm\n";
	//G4cout<< "    Targetchamber material: Al nominal stainless steel" <<G4endl;

	//------------------- Taget Chamber -------------------------------------
	
	G4Transform3D positionTargetChamber;
	if(targetInPlace) {
	chamberOriginPos = targetChamberRadius * std::tan(targetTilt) 
		+ (2 * hBackingLength + hTargetLength) / std::cos(targetTilt) - 2 * hTargetChamberLength;
	} else {	chamberOriginPos = targetChamberRadius * std::tan(targetTilt) 
		+ (2 * hBackingLength + hDiskLength) / std::cos(targetTilt) - 2 * hTargetChamberLength;
	}
	 
	G4ThreeVector positionCutBox = G4ThreeVector(0.,
		-1. * yEndCapLength * std::sin(targetTilt), 
		hTargetChamberLength - targetChamberRadius * std::tan(targetTilt) + yEndCapLength * std::cos(targetTilt));
	G4ThreeVector positionEndCap = G4ThreeVector(0.,
		-1. * hEndCapLength * std::sin(targetTilt), 
		hTargetChamberLength - targetChamberRadius * std::tan(targetTilt) + hEndCapLength * std::cos(targetTilt));
	
	G4Box* cutBox = new G4Box("Cut Box",yEndCapLength, yEndCapLength , yEndCapLength);
	
	G4Tubs* targetChamberCylinder = new G4Tubs(	"Target Chamber Cylinder",	// Name
									0.,						// Inner radius
									targetChamberRadius,	// Outer radius
									hTargetChamberLength,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians
									
	G4SubtractionSolid* cutTargetChamber = new G4SubtractionSolid("Cutted Target Chamber",				// Name
												targetChamberCylinder,			// Minuend Volume
												cutBox,							// Subtrahend Volume
												G4Transform3D(TargetRotMatrix, 	// rotation of Subtrahend
													positionCutBox));			// position of Subtrahend relativ to Minuend
													
	G4Tubs* uncutEndCapTube = new G4Tubs("End Cap uncut Tube", 0.,EndCapTotalDiameter*0.5, EndCapBackHeight*0.5,0.*deg,360.*deg);
	G4Box* cutForEndCapTube = new G4Box("EndCapTubeCuts",EndCapCutDiameter*0.5,EndCapCutDiameter*0.5,EndCapBackHeight*0.5+1.*mm);
	G4SubtractionSolid* firstEndCapCut = new G4SubtractionSolid("1stEndCapCut", uncutEndCapTube,cutForEndCapTube,G4Transform3D(G4RotationMatrix(),G4ThreeVector(EndCapCutDiameter,0,0)));
	G4SubtractionSolid* secondEndCapCut = new G4SubtractionSolid("2ndEndCapCut", firstEndCapCut,cutForEndCapTube,G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,EndCapCutDiameter,0)));
	G4SubtractionSolid* thirdEndCapCut = new G4SubtractionSolid("3rdEndCapCut", secondEndCapCut,cutForEndCapTube,G4Transform3D(G4RotationMatrix(),G4ThreeVector(-EndCapCutDiameter,0,0)));
	G4SubtractionSolid* endCapBack = new G4SubtractionSolid("EndCapBack", thirdEndCapCut,cutForEndCapTube,G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,-EndCapCutDiameter,0)));
	
	G4Tubs* uncutEndCapFrontTube = new G4Tubs("End Cap Front uncut Tube", EndCapFrontInnerDiameter*0.5,EndCapTotalDiameter*0.5, EndCapFrontHeight*0.5,0.*deg,360.*deg);
	G4Box* cutForEndCapFrontTube = new G4Box("EndCapFontTubeCuts",EndCapCutDiameter*0.5,EndCapCutDiameter*0.5,EndCapFrontHeight*0.5+1.*mm);
	G4Tubs* innerFrontEndCapCut = new G4Tubs("InnerCutForFrontEndCap",0.*mm,EndCapFrontInnerCutDiameter*0.5,EndCapFrontInnerCutHeight*0.5,0.*deg,360.*deg);
	G4SubtractionSolid* onceCutEndCapFrontTube = new G4SubtractionSolid("FrontTubeWithInnerCut",uncutEndCapFrontTube,innerFrontEndCapCut,G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,(EndCapFrontHeight-EndCapFrontInnerCutHeight)*0.5)));
	
	G4SubtractionSolid* firstFrontEndCapCut = new G4SubtractionSolid("1stEndCapCut", onceCutEndCapFrontTube,cutForEndCapFrontTube,G4Transform3D(G4RotationMatrix(),G4ThreeVector(EndCapCutDiameter,0,0)));
	G4SubtractionSolid* endCapFront = new G4SubtractionSolid("EndCapFront", firstFrontEndCapCut,cutForEndCapFrontTube,G4Transform3D(G4RotationMatrix(),G4ThreeVector(-EndCapCutDiameter,0,0)));

	G4Tubs* endCapMiddle = new G4Tubs("middleEndCap",EndCapMiddleInnerDiameter*0.5,EndCapMiddleOuterDiameter*0.5,EndCapMiddleHeight*0.5,0.*deg,360.*deg);
	G4UnionSolid* endCapBackPart = new G4UnionSolid("endCapBackPart",endCapBack,endCapMiddle,G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,(-EndCapMiddleHeight-EndCapBackHeight)*0.5)));
	
	G4UnionSolid* endCapUNCUT = new G4UnionSolid("EndCapUNCUT",endCapFront,endCapBackPart,G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,(EndCapFrontHeight+EndCapBackHeight)*0.5))); 	
	

	// Water cooling pipe
	
	G4RotationMatrix coolingRotMatrix;
	coolingRotMatrix.rotateX(90.*deg);
	G4ThreeVector positionCoolingPipe = G4ThreeVector(0.,0., hTargetCoolingLength - waterPipeRadius - 2 * mm);
	G4ThreeVector positionCooling = G4ThreeVector(0.,
		-1. * hTargetCoolingLength * std::sin(targetTilt), 
		hTargetChamberLength - targetChamberRadius * std::tan(targetTilt) + hTargetCoolingLength * std::cos(targetTilt));
	
	G4Tubs* waterPipe = new G4Tubs(	"Water_cooling_pipe",		// Name
									0.,							// Inner radius
									waterPipeRadius,			// Outer radius
									yEndCapLength-0.5*mm,				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians
									
	G4Tubs* targetCooling = new G4Tubs("Target_cooling",	// Name                   
									0.,							// Inner radius
									minTargetSocketRadius1,			// Outer radius
									hTargetCoolingLength,				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians
	G4UnionSolid* solidCooling = new G4UnionSolid("Chamber_Cooling",					// Name
										targetCooling,				// Summand Volume 1
										waterPipe,							// Summand Volume 2
										G4Transform3D(coolingRotMatrix, 	// rotation of Summand 2
										positionCoolingPipe));				// position of Summand 2 relativ to Summand 1
										
	G4SubtractionSolid* endCap = new G4SubtractionSolid("EndCap",endCapUNCUT,solidCooling,G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,hTargetCoolingLength-0.5*EndCapFrontHeight)));

	G4UnionSolid* solidTargetChamber = new G4UnionSolid("Target Chamber",				// Name
										cutTargetChamber,				// Summand Volume 1
										endCap,							// Summand Volume 2
										G4Transform3D(TargetRotMatrix, 	// rotation of Summand 2
										positionEndCap));				// position of Summand 2 relativ to Summand 1
										//~ 
	G4LogicalVolume* logicTargetChamber = new G4LogicalVolume(solidTargetChamber, Steel, "Target Chamber_log", 0,0,0);
	
	G4RotationMatrix targetChamberRotMatrix;
	
	//~ if(configb) {
		//~ positionTargetChamber = G4Transform3D(targetChamberRotMatrix, G4ThreeVector(0.,0.,
															//~ chamberOriginPos + hTargetChamberLength));
	//~ }
	//~ else {
		targetChamberRotMatrix.rotateZ(-90*deg);
		positionTargetChamber = G4Transform3D(targetChamberRotMatrix, G4ThreeVector(0.,0.,
															chamberOriginPos + hTargetChamberLength));
	//~ }	
	physicTargetChamber = new G4PVPlacement(positionTargetChamber,	// Position
									logicTargetChamber,		// its logical volume				  
									"Target_Chamber_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 

	logicTargetChamber ->SetVisAttributes(steelVisAtt);

	
	G4LogicalVolume* logicCooling = new G4LogicalVolume(solidCooling, Water, "Chamber_Cooling", 0,0,0);
	logicCooling ->SetVisAttributes(coolingVisAtt);
	physicCooling = new G4PVPlacement(G4Transform3D(TargetRotMatrix, 	// rotation of Summand 2
										positionCooling),		// at (x,y,z)
									logicCooling,			// its logical volume				  
									"Chamber_Cooling",		// its name
									logicTargetChamber,		// its mother  volume
									false,					// no boolean operations
									0,pSurfChk);						// copy number 

							

	// Chamber Vacuum
	G4Tubs* innerChamberCylinder = new G4Tubs(	"Inner_Chamber_Cylinder",		// Name
									0.,											// Inner radius
									targetChamberRadius - champerPipeThickness,	// Outer radius
									hTargetChamberLength,						// Half length in z 
									0.,											// Starting phi angle in radians
									twopi);									// Angle of the segment in radians

	G4SubtractionSolid* solidChamberVac = new G4SubtractionSolid(	"Chamber_Vacuum",				// Name
												innerChamberCylinder,			// Minuend Volume
												cutBox,							// Subtrahend Volume
												G4Transform3D(TargetRotMatrix, 	// rotation of Subtrahend
													positionCutBox));			// position of Subtrahend relativ to Minuend
	
	G4LogicalVolume* logicChamberVac = new G4LogicalVolume(solidChamberVac, Vacuum, "Chamber_Vacuum", 0,0,0);
	logicChamberVac	->SetVisAttributes(vacVisAtt);
	physicChamberVac = new G4PVPlacement(0,					// no rotation
									G4ThreeVector(),		// at (x,y,z)
									logicChamberVac,		// its logical volume				  
									"Chamber_Vacuum",		// its name
									logicTargetChamber,		// its mother  volume
									false,					// no boolean operations
									0);						// copy number 

	// Target Socket
	G4ThreeVector positionTargetSocket = G4ThreeVector(0., 
		hTargetSocketLength * std::sin(targetTilt),
		hTargetChamberLength - targetChamberRadius * std::tan(targetTilt) - hTargetSocketLength * std::cos(targetTilt));

	G4Cons* solidTargetSocket = new G4Cons(	"Target_Socket",		// Name
									minTargetSocketRadius2,	// inside radius at -pDz
									maxTargetSocketRadius,	// outside radius at -pDz
									minTargetSocketRadius1,	// inside radius at +pDz
									maxTargetSocketRadius,	// outside radius at +pDz
									hTargetSocketLength,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians
										
	G4LogicalVolume* logicTargetSocket = new G4LogicalVolume(solidTargetSocket, Steel, "Target_Socket", 0,0,0);
	physicTargetSocket = new G4PVPlacement(G4Transform3D(TargetRotMatrix, 	// rotation of daughter volume
													positionTargetSocket),	// position in mother frame
									logicTargetSocket,		// its logical volume				  
									"Target_Socket",		// its name
									logicChamberVac,		// its mother  volume
									false,					// no boolean operations
									0);						// copy number
	logicTargetSocket ->SetVisAttributes(steelVisAtt);
 

	// Copper Pipe
	G4ThreeVector positionCopperPipe = G4ThreeVector(0.,0.,
		hTargetChamberLength - hCopperPipeLength - (targetChamberRadius - copperPipeRadius) * std::tan(targetTilt) - cuPipeOffSet);
	positionCutBox = G4ThreeVector(0.,
		-1. * yEndCapLength * std::sin(targetTilt), 
		hCopperPipeLength - copperPipeRadius * std::tan(targetTilt) + yEndCapLength * std::cos(targetTilt));


	G4Tubs* copperPipeCylinder = new G4Tubs("Copper_Pipe",			// Name
									copperPipeRadius - copperPipeThickness,	// Inner radius
									copperPipeRadius,		// Outer radius
									hCopperPipeLength,		// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians

	G4SubtractionSolid* solidCopperPipe = new G4SubtractionSolid("Copper_Pipe",				// Name
											copperPipeCylinder,			// Minuend Volume
											cutBox,							// Subtrahend Volume
											G4Transform3D(TargetRotMatrix, 	// rotation of Subtrahend
													positionCutBox));			// position of Subtrahend relativ to Minuend
	
	G4LogicalVolume* logicCopperPipe = new G4LogicalVolume(solidCopperPipe, Cu, "Copper_Pipe", 0,0,0);
	logicCopperPipe	->SetVisAttributes(cuVisAtt);
	physicCopperPipe = new G4PVPlacement(0,				// no rotation
									positionCopperPipe,		// at (x,y,z)
									logicCopperPipe,		// its logical volume				  
									"Copper_Pipe",			// its name
									logicChamberVac,		// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	
	//Target Backing
	G4ThreeVector positionBacking = G4ThreeVector(0.,
		hBackingLength * std::sin(targetTilt),
		hTargetChamberLength - targetChamberRadius * std::tan(targetTilt) 
		- hBackingLength * std::cos(targetTilt));

	 G4Tubs* solidBacking = new G4Tubs("Target_Backing",	// Name
							0.,					// Inner radius
							backingRadius,		// Outer radius
							hBackingLength,		// Half length in z 
							0.,					// Starting phi angle in radians
							twopi);				// Angle of the segment in radians		
	G4LogicalVolume* logicBacking = new G4LogicalVolume(solidBacking, Ta, "Target_Backing", 0,0,0);
	logicBacking ->SetVisAttributes(backingVisAtt);
	physicBacking = new G4PVPlacement(G4Transform3D(TargetRotMatrix, 	// rotation of daughter volume
													positionBacking),	// position in mother frame
										logicBacking,		// its logical volume				  
										"Target_Backing",	// its name
										logicChamberVac,	// its mother  volume
										false,				// no boolean operations
										0);					// copy number 				
	// Target
	if(targetInPlace) 
	{
		G4ThreeVector positionTarget = G4ThreeVector(0.,
			(2 * hBackingLength + hTargetLength) * std::sin(targetTilt),
			hTargetChamberLength - targetChamberRadius * std::tan(targetTilt)
			- (2 * hBackingLength + hTargetLength) * std::cos(targetTilt));
		
		G4Tubs* solidTarget = new G4Tubs("TiN-Taget",	// Name    
									0.,					// Inner radius
									targetRadius,		// Outer radius
									hTargetLength,		// Half length in z
									0.,					// Starting phi angle in radians
									twopi);			// Angle of the segment in radians
		G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget,TiN,"TiN-Target",0,0,0);
		logicTarget	->SetVisAttributes(targetVisAtt);
		physicTarget = new G4PVPlacement(G4Transform3D(TargetRotMatrix, 	// rotation of daughter volume
														positionTarget),	// position in mother frame
										logicTarget,			// its logical volume				  
										"TiN-Target",			// its name
										logicChamberVac,		// its mother  volume
										false,					// no boolean operations
										0);						// copy number	

	} else {
		// Calibration-Source Disk
	
		G4ThreeVector positionDisk = G4ThreeVector(0.,
			(2 * hBackingLength + hDiskLength) * std::sin(targetTilt),
			hTargetChamberLength - targetChamberRadius * std::tan(targetTilt) 
			- (2 * hBackingLength + hDiskLength) * std::cos(targetTilt));
		
		G4Tubs* solidDisk = new G4Tubs("Source_Disk",	// Name    
									0.,					// Inner radius
									backingRadius,		// Outer radius
									hDiskLength,		// Half length in z
									0.,					// Starting phi angle in radians
									twopi);			// Angle of the segment in radians
		logicDisk = new G4LogicalVolume(solidDisk,Polyethylene,"Source_Disk",0,0,0);
		logicDisk ->SetVisAttributes(diskVisAtt);
		physicDisk = new G4PVPlacement(G4Transform3D(TargetRotMatrix, 	// rotation of daughter volume
														positionDisk),	// position in mother frame
										logicDisk,				// its logical volume				  
										"Source_Disk",			// its name
										logicChamberVac,		// its mother  volume
										false,					// no boolean operations
										0);						// copy number 
	}
	
}

void myDetectorConstruction::YTargetChamberGeometryLN2Cooled()
{
	//y target with LN2 cooled target, 55deg cut copper tube for electron suppression
	G4double distCopperTubeTarget = 5.14*mm; //distance from electron suppression tube to target
	G4double hYBasePipeLength = 300./2.*mm;
	G4double rYBasePipeThickness = 2.*mm;
	G4double rYBasePipe = 35.*mm;
	G4double hYAttachedPipeLength = 130./2.*mm;
	G4double rYAttachedPipeThickness = 2.*mm;
	G4double rYAttachedPipe = 20.5*mm;
	G4double offsetAttachment = 105.*mm;	
	G4double zPositionYPipe = hYBasePipeLength/2. - offsetAttachment;
	
	//Chamber Steel
	G4Tubs* yBasePipeFull = new G4Tubs("YBasePipeFull", rYBasePipe-rYBasePipeThickness,rYBasePipe, hYBasePipeLength,0.*deg,360.*deg);
	G4MultiUnion* yPipe = new G4MultiUnion("yPipeFull");
	G4Transform3D none = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,0));
	yPipe->AddNode(*yBasePipeFull,none);	 
	
	G4Tubs* yBasePipeVac = new G4Tubs("YBasePipeVac", 0,rYBasePipe-rYBasePipeThickness, hYBasePipeLength,0.*deg,360.*deg);
	G4Tubs* yAttachedPipeFull = new G4Tubs("AttachedPipeFull", rYAttachedPipe-rYAttachedPipeThickness,rYAttachedPipe, hYAttachedPipeLength,0.*deg,360.*deg);	
	G4RotationMatrix attachedMatrix;
	G4double angleAttached = -35.5*deg;
	attachedMatrix.rotateY(angleAttached);
	G4Transform3D attached = G4Transform3D(attachedMatrix, G4ThreeVector(-std::sin(angleAttached)*hYAttachedPipeLength, 0.*mm, -std::cos(angleAttached) * hYAttachedPipeLength-zPositionYPipe));
	G4RotationMatrix cutMatrix;
	cutMatrix.rotateY(-angleAttached);
	G4Transform3D cutVector = G4Transform3D(cutMatrix, G4ThreeVector(10.*mm + std::sin(angleAttached)*hYAttachedPipeLength, 0.*mm, std::cos(angleAttached) * hYAttachedPipeLength+zPositionYPipe)); //10*mm in the x coordinate are arbitrary, was too low without for some reason
	G4SubtractionSolid* yAttachedPipeCut = new G4SubtractionSolid("yAttachedPipeCut", yAttachedPipeFull, yBasePipeVac, cutVector);
	yPipe-> AddNode(*yAttachedPipeCut,attached);
	yPipe->Voxelize();	
	
	G4LogicalVolume* logicYPipe = new G4LogicalVolume(yPipe, Steel, "YPipe", 0,0,0);
	physicYTargetChamberLN2 = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe)),	// Position
									logicYPipe,		// its logical volume				  
									"YPipe_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicYPipe->SetVisAttributes(steelVisAtt);
		
	//Chamber Vacuum inside
	//G4Tubs* yBasePipeVac = new G4Tubs("YBasePipeVac", 0,rYBasePipe-rYBasePipeThickness, hYBasePipeLength,0.*deg,360.*deg);
	G4MultiUnion* yPipeVac = new G4MultiUnion("yPipeVac");
	yPipeVac->AddNode(*yBasePipeVac,none);	 
	
	G4Tubs* yAttachedPipeVac = new G4Tubs("YAttachedPipeVac", 0,rYAttachedPipe-rYAttachedPipeThickness, hYAttachedPipeLength,0.*deg,360.*deg);	
	yPipeVac-> AddNode(*yAttachedPipeVac,attached);
	yPipeVac->Voxelize();
	
	G4LogicalVolume* logicYPipeVac = new G4LogicalVolume(yPipeVac, Air, "YPipeVac", 0,0,0);
	G4VPhysicalVolume* physicYPipeVac = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe)),	// Position
									logicYPipeVac,		// its logical volume				  
									"YPipeVac_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicYPipeVac->SetVisAttributes(vacVisAtt);
	
	//connector target to cold trap
	G4double hConnectorTub = 175./2.*mm;
	G4double rConnectorTub = 35.*mm;
	G4double rConnectorTubThickness = 2.*mm;	
	G4Tubs* ConnectorTubSolid = new G4Tubs("ConnectorTubSolid", rConnectorTub-rConnectorTubThickness,rConnectorTub, hConnectorTub,0.*deg,360.*deg);	
	G4Tubs* ConnectorVacTub = new G4Tubs("ConnectorVacTub", 0,rConnectorTub-rConnectorTubThickness, hConnectorTub,0.*deg,360.*deg);	
	G4RotationMatrix connectorMatrix;
	connectorMatrix.rotateY(90.*deg);
	G4SubtractionSolid* ConnectorTub = new G4SubtractionSolid("ConnectorTub", ConnectorTubSolid, ConnectorVacTub, G4Transform3D(connectorMatrix, G4ThreeVector(0,0,0)));	
	G4LogicalVolume* logicConnector = new G4LogicalVolume(ConnectorTub, Steel, "logicConnector", 0,0,0);
	G4VPhysicalVolume* physicConnector = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe + hYBasePipeLength + hConnectorTub)),	// Position
									logicConnector,		// its logical volume				  
									"Connector_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicConnector->SetVisAttributes(steelVisAtt);	
	G4LogicalVolume* logicConnector2 = new G4LogicalVolume(ConnectorTub, Steel, "logicConnector2", 0,0,0);
	G4VPhysicalVolume* physicConnector2 = new G4PVPlacement(G4Transform3D(connectorMatrix,G4ThreeVector(0,0,zPositionYPipe + hYBasePipeLength + hConnectorTub)),	// Position
									logicConnector2,		// its logical volume				  
									"Connector_phys2",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicConnector2->SetVisAttributes(steelVisAtt);	
	
	//Connector Vacuum
	G4UnionSolid* ConnectorVac = new G4UnionSolid("ConnectorVac", ConnectorVacTub, ConnectorVacTub, G4Transform3D(connectorMatrix, G4ThreeVector(0,0,0)));
	G4LogicalVolume* logicConnectorVac = new G4LogicalVolume(ConnectorVac, Air, "logicConnectorVac", 0,0,0);
	G4VPhysicalVolume* physicConnectorVac = new G4PVPlacement(G4Transform3D(connectorMatrix,G4ThreeVector(0,0,zPositionYPipe + hYBasePipeLength + hConnectorTub)),	// Position
									logicConnectorVac,		// its logical volume				  
									"ConnectorVac_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicConnectorVac->SetVisAttributes(vacVisAtt);
			
	//Cold Trap Steel
	G4double hColdTub = 35./2.*mm;
	G4double rColdTub = 35.*mm;
	G4double rColdTubThickness = 2.*mm;
	G4double rColdConThickness = 2.*mm;
	G4double rColdConLow = 35.*mm;
	G4double rColdConHigh = 80.*mm;
	G4double hColdCon = 20./2.*mm;
	G4double hColdTrapTub = 200./2.*mm;
	G4double rColdTrapTub = 155./2.*mm;
	G4double rColdTrapTubThickness = 2.*mm;	
		
	G4MultiUnion* ColdTrap = new G4MultiUnion("ColdTrap");
	G4Tubs* ColdTub = new G4Tubs("ColdTub", rColdTub-rColdTubThickness,rColdTub, hColdTub,0.*deg,360.*deg);
	ColdTrap->AddNode(*ColdTub, none);
	G4Cons* ColdCon = new G4Cons("ColdCon", rColdConLow-rColdConThickness, rColdConLow, rColdConHigh-rColdConThickness, rColdConHigh, hColdCon, 0., 360.*deg);
	G4Transform3D ColdConVector = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,hColdTub+hColdCon));
	ColdTrap->AddNode(*ColdCon, ColdConVector);
	G4Tubs* ColdTrapTub = new G4Tubs("ColdTub", rColdTrapTub-rColdTrapTubThickness,rColdTrapTub, hColdTrapTub,0.*deg,360.*deg);	
	G4Transform3D ColdTrapTubVector = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,hColdTub + hColdCon*2 + hColdTrapTub));
	ColdTrap->AddNode(*ColdTrapTub, ColdTrapTubVector);
	ColdTrap->Voxelize();
	
	G4RotationMatrix coldTrapMatrix;
	coldTrapMatrix.rotateY(90.*deg);
	G4LogicalVolume* logicColdTrap = new G4LogicalVolume(ColdTrap, Steel, "logicColdTrap", 0,0,0);
	G4VPhysicalVolume* physicColdTrap = new G4PVPlacement(G4Transform3D(coldTrapMatrix,G4ThreeVector(hConnectorTub + hColdTub,0,zPositionYPipe + hYBasePipeLength + hConnectorTub)),	// Position
									logicColdTrap,		// its logical volume				  
									"ColdTrap_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicColdTrap->SetVisAttributes(steelVisAtt);		
	
	//flange connection
	//flange wide part at top
	G4double hFlangeWide = 28./2.*mm;
	G4double rFlangeWide = rYBasePipe+12.5*mm;
	G4double tFlangeWide = 2.3*mm;
	G4Tubs* FlangeWideTub = new G4Tubs("FlangeWideTub", rFlangeWide-tFlangeWide,rFlangeWide, hFlangeWide,0.*deg,360.*deg);
	
	//flange thinner part below
	G4double hFlangeThin = 23.8/2.*mm;
	G4double rFlangeThin = rYBasePipe;
	G4double tFlangeThin = 10.2*mm;
	G4Tubs* FlangeThinTub = new G4Tubs("FlangeThinTub", rFlangeThin,rFlangeThin+tFlangeThin, hFlangeThin,0.*deg,360.*deg);	
	
	G4UnionSolid* Flange = new G4UnionSolid("Flange", FlangeWideTub, FlangeThinTub, G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,0)));
	G4Tubs* Cap = new G4Tubs("FlangeCap", 0,rYBasePipe, rYBasePipeThickness/2.,0.*deg,360.*deg);
	
	//minimalistic flange holders
	G4double rFlangeHolder = 11.*mm;
	G4double hFlangeHolder = 60./2.*mm;
	G4Tubs* FlangeHolderBody = new G4Tubs("FlangeHolderBody", 0,rFlangeHolder, hFlangeHolder,0.*deg,360.*deg);
	G4MultiUnion* FlangeHolders = new G4MultiUnion("FlangeHolders");
	const G4int nFlangeHolder = 4;
	G4ThreeVector vectorHolder[nFlangeHolder];
	G4double angleHolder[nFlangeHolder] = {60.*deg, 135.*deg, 210.*deg, 300.*deg};
	G4RotationMatrix rotHolder[nFlangeHolder];
	G4Transform3D positionHolder[nFlangeHolder];
	//G4cout << "VEKTOREN HALTER" << G4endl;
	for (G4int nHolder = 0; nHolder < nFlangeHolder; nHolder++) {
		vectorHolder[nHolder].setRThetaPhi(rFlangeWide+rFlangeHolder/2.-3.*mm, 90*deg, angleHolder[nHolder]);
		//G4cout << vectorHolder[nHolder].getX() << " " << vectorHolder[nHolder].getY() << " " << vectorHolder[nHolder].getZ() << " " << G4endl;
		rotHolder[nHolder].rotateZ(angleHolder[nHolder]);
		positionHolder[nHolder] = G4Transform3D(rotHolder[nHolder], vectorHolder[nHolder]);
		FlangeHolders->AddNode(*FlangeHolderBody, positionHolder[nHolder]);
	}
	FlangeHolders->Voxelize();

	//better results without holders
	//~ G4LogicalVolume* logicFlangeHolder = new G4LogicalVolume(FlangeHolders, Steel, "logicFlangeHolder", 0,0,0);
	//~ G4VPhysicalVolume* physicFlangeHolder = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe + hYBasePipeLength + hFlangeWide/2.+1.*cm)),	// Position
									//~ logicFlangeHolder,		// its logical volume				  
									//~ "FlangeHolder_phys",		// its name
									//~ logicWorld,				// its mother  volume
									//~ false,					// no boolean operations
									//~ 0);						// copy number 
	//~ logicFlangeHolder->SetVisAttributes(steelVisAtt);		
	
	G4UnionSolid* ClosedFlange = new G4UnionSolid("ClosedFlange", Flange, Cap, G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,hFlangeWide)));
	G4LogicalVolume* logicFlange1 = new G4LogicalVolume(Flange, Steel, "logicFlange1", 0,0,0);
	G4VPhysicalVolume* physicFlange1 = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe - hYBasePipeLength + hFlangeWide/2.)),	// Position
									logicFlange1,		// its logical volume				  
									"Flange1_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicFlange1->SetVisAttributes(steelVisAtt);
	G4LogicalVolume* logicFlange2 = new G4LogicalVolume(Flange, Steel, "logicFlange2", 0,0,0);
	G4VPhysicalVolume* physicFlange2 = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe + hYBasePipeLength + hFlangeWide/2.)),	// Position
									logicFlange2,		// its logical volume				  
									"Flange2_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicFlange2->SetVisAttributes(steelVisAtt);
		
	G4LogicalVolume* logicFlange3 = new G4LogicalVolume(ClosedFlange, Steel, "logicFlange3", 0,0,0);
	G4VPhysicalVolume* physicFlange3 = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,zPositionYPipe + hYBasePipeLength + hConnectorTub*2 + hFlangeWide/2.)),	// Position
									logicFlange3,		// its logical volume				  
									"Flange3_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicFlange3->SetVisAttributes(steelVisAtt);	
	G4RotationMatrix flangeMatrix;
	flangeMatrix.rotateY(-90.*deg);
	G4LogicalVolume* logicFlange4 = new G4LogicalVolume(ClosedFlange, Steel, "logicFlange4", 0,0,0);	
	G4VPhysicalVolume* physicFlange4 = new G4PVPlacement(G4Transform3D(flangeMatrix,G4ThreeVector(-hConnectorTub-hFlangeWide/2.,0,zPositionYPipe + hYBasePipeLength + hConnectorTub)),	// Position
									logicFlange4,		// its logical volume				  
									"Flange4_phys",		// its name
									logicWorld,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicFlange4->SetVisAttributes(steelVisAtt);
	
	//target cooler copper
	G4double rCopperHead = 47./2.*mm-dCluster1;
	G4double hCopperHead = (12.+2.5)/2.*mm;
	G4double rInnerTargetHolder = 19./2.*mm;
	G4double rOuterTargetHolder = 15.*mm;
	G4double hTargetHolder = 3.5/2.*mm;
	G4double rHole = 1.5*mm;
	G4double hHole = 25./2.*mm;
	G4double hHolePerpend = 12.5/2.*mm;
	G4double rColdFinger = 22./2.*mm;
	G4double hColdFinger = 43.5*mm + hConnectorTub/2.;
	G4double hBackplate = 6./2.*mm;
	
	G4double copperAngle = dCluster2;
	G4double totalLength = (hCopperHead + hTargetHolder + hColdFinger)*2.;
	G4double yRot = std::sin(copperAngle) * totalLength;
	G4double zRot = 2.* totalLength * std::sin(copperAngle/2.)*std::cos(180.*deg-2.*copperAngle);
	G4Tubs* CopperHeadTub = new G4Tubs("CopperHeadTub", 0,rCopperHead, hCopperHead,0.*deg,360.*deg);
	G4Tubs* TargetHolderTub = new G4Tubs("TargetHolderTub", rOuterTargetHolder,rCopperHead, hTargetHolder,0.*deg,360.*deg);
	G4Cons* TargetHolderCon = new G4Cons("TargetHolderCon", rInnerTargetHolder, rOuterTargetHolder, rOuterTargetHolder, rOuterTargetHolder, hTargetHolder, 0., 360.*deg);
	G4Tubs* ColdFingerTub = new G4Tubs("ColdFingerTub", 0,rColdFinger, hColdFinger,0.*deg,360.*deg);
	G4MultiUnion* CopperCoolingFull = new G4MultiUnion("CopperCoolingFull");
	CopperCoolingFull->AddNode(*TargetHolderTub, none);
	CopperCoolingFull->AddNode(*TargetHolderCon, none);
	G4Transform3D CopperHeadVector = G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,-hTargetHolder-hCopperHead));
	CopperCoolingFull->AddNode(*CopperHeadTub, CopperHeadVector);
	G4Transform3D ColdFingerVector = G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,-hTargetHolder-2*hCopperHead-hColdFinger));
	CopperCoolingFull->AddNode(*ColdFingerTub, ColdFingerVector);
	CopperCoolingFull->Voxelize();
	G4LogicalVolume* logicCopperCooling = new G4LogicalVolume(CopperCoolingFull, Cu, "logicCopperCooling", 0,0,0);	
	G4RotationMatrix copperMatrix;
	copperMatrix.rotateX(180*deg+copperAngle);
	G4ThreeVector copperVector = G4ThreeVector(0,0,-zPositionYPipe-hTargetHolder+dCluster3);
	//copperVector.rotateX(copperAngle);
	copperVector.setY(copperVector.getY()+yRot);
	copperVector.setZ(copperVector.getZ()+zRot);
	G4VPhysicalVolume* physicCopperCooling = new G4PVPlacement(G4Transform3D(copperMatrix,copperVector),	// Position
									logicCopperCooling,		// its logical volume				  
									"CopperCooling_phys",		// its name
									logicYPipeVac,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicCopperCooling->SetVisAttributes(cuVisAtt);
	G4RotationMatrix backplateMatrix;
	backplateMatrix.rotateX(copperAngle);
	G4ThreeVector backplateVector = G4ThreeVector(0,0,-zPositionYPipe+2.*hCopperHead+hBackplate + dCluster3);
	backplateVector.rotateX(copperAngle);
	backplateVector.setY(backplateVector.getY()+yRot);
	backplateVector.setZ(backplateVector.getZ()+zRot);
	G4Tubs* tubSteelBackplate = new G4Tubs("Backplate", rColdFinger, rCopperHead, hBackplate,0.*deg,360.*deg);
	G4LogicalVolume* logicSteelBackplate = new G4LogicalVolume(tubSteelBackplate, Steel, "logicSteelBackplate", 0,0,0);	
	G4VPhysicalVolume* physicSteelBackplate = new G4PVPlacement(G4Transform3D(backplateMatrix, backplateVector),	// Position
									logicSteelBackplate,		// its logical volume				  
									"SteelBackplate_phys",		// its name
									logicYPipeVac,				// its mother  volume
									false,					// no boolean operations
									0);						// copy number 
	logicSteelBackplate->SetVisAttributes(aluVisAtt);	
	
	// Copper Pipe for e suppression
	G4double hCopperPipeLength = 65. * mm;				// Half height of Copper Pipe
	G4double copperPipeRadius = 11. * mm;				// Radius of Copper Pipe
	G4double copperPipeThickness = 2. * mm;				// Thickness of Copper Pipe	
	G4double cutAngle = -55.*deg;
	G4RotationMatrix cutCopperMatrix;
	cutCopperMatrix.rotateY(cutAngle);
	G4double hBox = 5.*cm;
	G4Box* cutBox = new G4Box("Cut Box",hBox, hBox, hBox);
	G4ThreeVector positionCopperPipe = G4ThreeVector(0.,0.,-zPositionYPipe-distCopperTubeTarget-hCopperPipeLength);
	G4ThreeVector positionCutBox = G4ThreeVector(0.,0., hCopperPipeLength+hBox+2.*std::cos(cutAngle/deg)*hBox);


	G4Tubs* copperPipeCylinder = new G4Tubs("Copper_Pipe",			// Name
									copperPipeRadius - copperPipeThickness,	// Inner radius
									copperPipeRadius,		// Outer radius
									hCopperPipeLength,		// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians

	G4SubtractionSolid* solidCopperPipe = new G4SubtractionSolid("Copper_Pipe",				// Name
											copperPipeCylinder,			// Minuend Volume
											cutBox,							// Subtrahend Volume
											G4Transform3D(cutCopperMatrix, 	// rotation of Subtrahend
													positionCutBox));			// position of Subtrahend relativ to Minuend
	
	G4LogicalVolume* logicCopperPipe = new G4LogicalVolume(solidCopperPipe, Cu, "Copper_Pipe", 0,0,0);
	logicCopperPipe	->SetVisAttributes(cuVisAtt);
	physicCopperPipe = new G4PVPlacement(0,				// no rotation
									positionCopperPipe,		// at (x,y,z)
									logicCopperPipe,		// its logical volume				  
									"Copper_Pipe",			// its name
									logicYPipeVac,		// its mother  volume
									false,					// no boolean operations
									0);						// copy number 	

	G4double targetRadius = 9.5 * mm;					
	G4double hTargetLength = 0.11 *mm;					
	G4Tubs* solidTarget = new G4Tubs("SolidTarget", 0, targetRadius, hTargetLength,0.*deg,360.*deg);
	G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, Ta, "Copper_Pipe", 0,0,0);
	logicCopperPipe	->SetVisAttributes(targetVisAtt);
	G4RotationMatrix targetMatrix;
	targetMatrix.rotateX(copperAngle);
	G4ThreeVector targetVector = G4ThreeVector(0,0,-zPositionYPipe+dCluster3);
	//targetVector.rotateX(copperAngle);
	targetVector.setY(targetVector.getY()+yRot);
	targetVector.setZ(targetVector.getZ()+zRot);
	G4cout << "TARGET POSITION: " << yRot << " " << targetVector.getY() << " " << zRot << " " << targetVector.getZ()+zPositionYPipe << G4endl;
	G4cout << "Total Length: " << totalLength << G4endl;
	physicTarget= new G4PVPlacement(G4Transform3D(targetMatrix, targetVector),		// at (x,y,z)
									logicTarget,		// its logical volume				  
									"Target",			// its name
									logicYPipeVac,		// its mother  volume
									false,					// no boolean operations
									0);						// copy number 																			
}


void myDetectorConstruction::YTargetChamberGeometryWaterCooled()
{
	//y target with water cooling
	//particles get stuck a lot in the gdml parts. needs to be double checked or switched out with geant4 geometry
	G4GDMLParser parser;
	parser.Read("gdml/y-Target/y-Target.gdml", false); // shema validation true/false via network url in gdml file    	
	Ytarget_logical = parser.GetWorldVolume()->GetLogicalVolume();
	
	Ytarget_logical -> SetVisAttributes(steelVisAtt);
	
	G4RotationMatrix* YtargetRotMatrix = new G4RotationMatrix();
	// as the target-chamber faces in the wrong direction it has to be rotated
	YtargetRotMatrix->rotateY(180*deg);
	YtargetRotMatrix->rotateX(180*deg);

	Ytarget_physical = new G4PVPlacement(YtargetRotMatrix, G4ThreeVector(0,0,0.34),
                                                    Ytarget_logical,"y-Target",logicWorld,false,0,pSurfChk);
	
	Ytarget_CU_tube_logical = Ytarget_logical->GetDaughter(0)->GetLogicalVolume();
	Ytarget_CU_tube_logical->SetVisAttributes(cuVisAtt);
	Ytarget_ES_sampleholder_logical = Ytarget_logical->GetDaughter(1)->GetLogicalVolume();
	Ytarget_ES_sampleholder_logical->SetVisAttributes(steelVisAtt);
	Ytarget_ES_ychamber_logical = Ytarget_logical->GetDaughter(2)->GetLogicalVolume();
	Ytarget_ES_ychamber_logical->SetVisAttributes(steelVisAtt);
	Ytarget_ES_smallEndCap_logical = Ytarget_logical->GetDaughter(3)->GetLogicalVolume();
	Ytarget_ES_smallEndCap_logical->SetVisAttributes(steelVisAtt);
	Ytarget_ES_coolingBlock_logical = Ytarget_logical->GetDaughter(4)->GetLogicalVolume();
	Ytarget_ES_coolingBlock_logical->SetVisAttributes(steelVisAtt);
	Ytarget_coolingWater_logical = Ytarget_logical->GetDaughter(5)->GetLogicalVolume();
	Ytarget_coolingWater_logical->SetVisAttributes(coolingVisAtt);
	Ytarget_target_logical = Ytarget_logical->GetDaughter(6)->GetLogicalVolume();
	Ytarget_target_logical->SetVisAttributes(steelVisAtt);	
}

void myDetectorConstruction::GastargetGeometry()
{
	//gas target taken from early blueprint
	pSurfChk = true;
	
	G4LogicalVolume* Gastarget_logical;
	G4LogicalVolume* GastargetCollimator_logical;
	
	G4GDMLParser parser;
	parser.Read("gdml/Gastarget/gastarget.gdml", false);		// shema validation true/false via network url in gdml file    	
	Gastarget_logical = parser.GetWorldVolume()->GetLogicalVolume();
	
	Gastarget_logical->SetVisAttributes(steelVisAtt);
	
	G4RotationMatrix* GastargetRotMatrix = new G4RotationMatrix();
	GastargetRotMatrix->rotateZ(90*deg);
	
	G4VPhysicalVolume* Gastarget_physical = new G4PVPlacement(GastargetRotMatrix,
										G4ThreeVector(0,0,0),  //TripleClusterRotMatrix,G4ThreeVector(0*mm, totalDistanceTripleCluster, 0*mm),					// position in mother frame
										Gastarget_logical,		// its logical volume is the world volume of gdml file			  
										"Gastarget",					// its name
										logicWorld,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	
	GastargetCollimator_logical = Gastarget_logical->GetDaughter(0)->GetLogicalVolume();
	GastargetCollimator_logical->SetVisAttributes(cuVisAtt);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myDetectorConstruction::CrystalwDeadLayersGeometry(G4int nbCluster, G4int nbCrystal, G4VPhysicalVolume* motherVolume_phys, G4double dCap, G4double hCone)
{
	//takes the original full crystals from a gdml file and cuts out dead layers
	G4cout << "DeadLayer: " << nbCluster << " " << nbCrystal << " " << dCap/mm << "mm " << hCone/mm << "mm\n";
	G4LogicalVolume* motherVolume = motherVolume_phys->GetLogicalVolume();
	//no dead layer does not work at the moment
	if (dCap == 0 && hCone == 0) {
		ActiveCrystal_logical[nbCluster][nbCrystal] = motherVolume;
		ActiveCrystal_logical[nbCluster][nbCrystal]->SetMaterial(Ge);
		ActiveCrystal_logical[nbCluster][nbCrystal]->SetVisAttributes(geVisAtt);
		ActiveCrystal_physical[nbCluster][nbCrystal] = motherVolume_phys;
		ActiveCrystal_physical[nbCluster][nbCrystal]->SetCopyNo(nbCrystal);	
		G4cout << "no DeadLayer, take original crystal" << G4endl;
		return;
	}
	G4VSolid* solidCrystal = motherVolume->GetSolid();
	//crystal parameters
	G4double rCap = 35.*mm; //radius top of the crystal (doesn't matter as long as its bigger as the crystal, because only intersection/subtraction matters)
	G4double rHole = 5.*mm; //radius hole
	G4double dHole = 63*mm; //depth hole
	G4double dCrystal = 78.*mm;
	G4double rCrystal = 70./2.*mm;
	//position Hole
	G4double zPositionHole = -dCrystal + dHole/2. + hCone/2.;		
	G4double zPositionUnion = dHole/2. - dCrystal;
	G4double zPositionSphere = -dCrystal + dHole;
	G4double zPositionCone = -dCrystal + hCone/2.;

	G4Transform3D positionHole(G4RotationMatrix(),G4ThreeVector(0,0,zPositionHole));
	G4Transform3D positionSphere(G4RotationMatrix(),G4ThreeVector(0,0,zPositionSphere));
	G4Transform3D positionUnion(G4RotationMatrix(),G4ThreeVector(0,0,zPositionUnion));
	G4Transform3D positionCone(G4RotationMatrix(),G4ThreeVector(0,0,zPositionCone));
	G4Transform3D none(G4RotationMatrix(),G4ThreeVector(0,0,0));

	//front dead layer
	G4Tubs* tubDeadLayerCap = new G4Tubs("TubDeadLayerCap", 0, rCap + 10.*mm, dCap, 0, 360.*deg); //thickness is doubled, but one half will get cut off by IntersectionSolid			
	//cone dead layer in the back
	G4Cons* coneDeadLayer = new G4Cons("ConeDeadLayer", 1.*mm, rCrystal, 1.*mm, rHole, hCone/2., 0., 360.*deg);
	//union of dead layers
	G4UnionSolid* solidDeadLayerUncut = new G4UnionSolid("UnionDeadLayerHoleComplete", tubDeadLayerCap, coneDeadLayer, positionCone);
	//cut dead layer from crystal	
	G4IntersectionSolid* solidDeadLayer = new G4IntersectionSolid("Solid_Dead_Layer_Hole", solidCrystal, solidDeadLayerUncut, none);
	G4SubtractionSolid* solidActiveCrystal = new G4SubtractionSolid("Solid_Active_Crystal", solidCrystal, solidDeadLayerUncut, none);
	
	//logical and physical volumes active crystal
	ActiveCrystal_logical[nbCluster][nbCrystal] = new G4LogicalVolume(solidActiveCrystal, Ge, "ActiveCrystal_logic"+nameCluster[nbCluster]+std::to_string(nbCrystal),0,0,0);
	ActiveCrystal_logical[nbCluster][nbCrystal]->SetVisAttributes(geVisAtt);
	ActiveCrystal_physical[nbCluster][nbCrystal] = new G4PVPlacement(none,	// position in mother frame
										ActiveCrystal_logical[nbCluster][nbCrystal],		// its logical volume				  
										"ActiveCrystal_phys"+nameCluster[nbCluster]+std::to_string(nbCrystal),	// its name
										motherVolume,	// its mother  volume
										false,				// no boolean operations
										nbCrystal);					// copy number 
	//logical and physical volumes top dead layer
	G4LogicalVolume* DeadLayerCap_logical = new G4LogicalVolume(solidDeadLayer, Ge, "DeadLayer_logic",0,0,0);
	DeadLayerCap_logical->SetVisAttributes(ContactLayerVisAtt);
	G4PVPlacement* DeadLayerCap_physical = new G4PVPlacement(none,	// position in mother frame
										DeadLayerCap_logical,		// its logical volume				  
										"DeadLayer_phys",	// its name
										motherVolume,	// its mother  volume
										false,				// no boolean operations
										0);					// copy number 						
}

void myDetectorConstruction::ClusterBGOGeometry(G4int nbCluster)
{
	//BGO Crystals for TripleClusters by Louis
	G4double BGOtilt = 10.33*deg;	// tilt of BGO crystals towards beamaxis
	G4double distCenterBGOTip = 71.*mm; 	// distance of BGO front tip to center
 
	G4double  cRmin1 = 106*mm;		// inside radius at -cDz
	G4double  cRmax1 = 150.*mm;		// outside radius at -cDz
	G4double  cRmin2 = 65.*mm;		// inside radius at +cDz
	G4double  cRmax2 = 110.5*mm;	// outside radius at +cDz
	G4double  cDz = 114.75*mm;		// half length in z 
	G4double  cSPhi = 0.*deg;		// starting angle of the segment in radians
	G4double  cDPhi = 360.*deg;		// the angle of the segment in radians 
 
	G4double  tRMin = 203.*mm/2.;		// Inner radius 
	G4double  tRMax = 200.*mm;		// Outer radius 
	G4double  tDz = 25.*mm/2.;			// Half length in z 
	G4double  tSPhi= 0.*deg;		// Starting phi angle in radians 
	G4double  tDPhi = 360.*deg;		// Angle of the segment in radians

	G4double housThick = 1.*mm;		// Thickness of the BGO housing
	
	G4Cons* housingBGOvac_cone = new G4Cons("HousingBGOvac_cone"+nameCluster[nbCluster],
										cRmin1+housThick,		// inside radius at -cDz
										cRmax1-housThick,		// outside radius at -cDz
										cRmin2+housThick,		// inside radius at +cDz
										cRmax2-housThick,		// outside radius at +cDz
										cDz-housThick,		// half length in z 
										cSPhi,		// starting angle of the segment in radians
										cDPhi);		// the angle of the segment in radians 

	G4Tubs* housingBGOvac_tub = new G4Tubs("HousingBGOvac_tub"+nameCluster[nbCluster],
										   tRMin + 5.*mm,		// Inner radius 
										   cRmax1 + 1.*mm,		// Outer radius 
										   tDz-housThick,			// Half length in z 
										   tSPhi,		// Starting phi angle in radians 
										   tDPhi);		// Angle of the segment in radians

	G4UnionSolid* housingBGOvac_solid = new G4UnionSolid("HousingBGOvac_solid"+nameCluster[nbCluster],	// Name
										housingBGOvac_cone,				// Summand Volume 1
										housingBGOvac_tub,				// Summand Volume 2
										0, 	// rotation of Summand 2
										G4ThreeVector(0., 0., -1. * cDz));	// position of Summand 2 relativ to Summand 1
										
	G4LogicalVolume* housingBGOvac_logic = new G4LogicalVolume(housingBGOvac_solid, Vacuum, "housingBGOvac_logic"+nameCluster[nbCluster],0,0,0);

    housingBGOvac_logic->SetVisAttributes(vacVisAtt);
	G4RotationMatrix vacRotation = G4RotationMatrix();
	vacRotation.rotateY(-90.*deg);
	G4Transform3D positionVac = G4Transform3D(vacRotation, G4ThreeVector(cDz+dOffsetBGO+housThick,0,0));
	
	housingBGOvac_physical[nbCluster] = new G4PVPlacement(positionVac,		// position in mother frame
								housingBGOvac_logic,			// its logical volume 		  
								"HousingBGOvac_physic"+nameCluster[nbCluster],			// its name
								BGOHousing_logical[nbCluster],				// its mother  volume
								false,							// no boolean operations
								0,								// copy number
								pSurfChk);						//surface check



	G4double  lDz = 234.*mm /2.;   	// Half z length 
	G4double lTheta = 0.*deg;	// Polar angle of the line joining the centres of the faces at -/+lDz 
	G4double  lPhi = 0.*deg;	// Azimuthal angle of the line joining the centre of the face at -lDz to the centre of the face at +lDz
	G4double lDy1 = 24.25*mm /2.;	// Half y length at -lDz 
	G4double  lDx1 = 91.*mm /2.;  	// Half x length of the side at y=-lDy1 of the face at -lDz 
	G4double lDx2 = 111*mm / 2.;	// Half x length of the side at y=+lDy1 of the face at -lDz 
	G4double  lAlp1 = 0.*deg; 	// Angle with respect to the y axis from the centre of the side (lower endcap) 
	G4double lDy2 = 24.25*mm /2.;	// Half y length at +lDz 
	G4double  lDx3 = 57.0*mm /2.;  	// Half x length of the side at y=-lDy2 of the face at +lDz 
	G4double lDx4 = 77.*mm /2.;	// Half x length of the side at y=+lDy2 of the face at +lDz 
	G4double  lAlp2 = 0.*deg;	// Angle with respect to the y axis from the centre of the side (upper endcap) 

	G4Trap* lowerBGOCrystal = new G4Trap("lowerBGOcrystal_trap"+nameCluster[nbCluster],
										lDz,   	// Half z length 
										lTheta,	// Polar angle of the line joining the centres of the faces at -/+lDz 
										lPhi,	// Azimuthal angle of the line joining the centre of the face at -lDz to the centre of the face at +lDz
										lDy1,	// Half y length at -lDz 
										lDx1,  	// Half x length of the side at y=-lDy1 of the face at -lDz 
										lDx2,	// Half x length of the side at y=+lDy1 of the face at -lDz 
										lAlp1, 	// Angle with respect to the y axis from the centre of the side (lower endcap) 
										lDy2,	// Half y length at +lDz 
										lDx3,  	// Half x length of the side at y=-lDy2 of the face at +lDz 
										lDx4,	// Half x length of the side at y=+lDy2 of the face at +lDz 
										lAlp2);	// Angle with respect to the y axis from the centre of the side (upper endcap) 

	G4Box* cutBox1 = new G4Box("CutBox1",lDx2*1.1, lDy1 *3 , lDy1);
	
	G4RotationMatrix BGOCutRotMatrix;					// Rotationmatrix for detector
	BGOCutRotMatrix.rotateX(-1.*BGOtilt);
	G4Transform3D positionCutBox1 = G4Transform3D(BGOCutRotMatrix, G4ThreeVector(0., 
	lDy1 * std::sin(BGOtilt),
	 lDz + lDy1 * std::cos(BGOtilt) - lDy1 * std::tan(BGOtilt)));

	G4SubtractionSolid* lowerBGOCrystalCut = new G4SubtractionSolid("lowerBGOCrystalCut"+nameCluster[nbCluster],	// Name
											lowerBGOCrystal,			// Minuend Volume
											cutBox1,							// Subtrahend Volume
											positionCutBox1);			// position of Subtrahend relativ to Minuend


	G4double  uDz = 234.*mm /2.;   	// Half z length 
	G4double uTheta = 0.*deg;	// Polar angle of the line joining the centres of the faces at -/+uDz 
	G4double  uPhi = 0.*deg;	// Azimuthal angle of the line joining the centre of the face at -uDz to the centre of the face at +uDz
	G4double uDy1 = 9.25*mm /2.;	// Half y length at -uDz 
	G4double  uDx1 = 111.*mm /2.;  	// Half x length of the side at y=-uDy1 of the face at -uDz 
	G4double uDx2 = 17.01*mm;	// Half x length of the side at y=+uDy1 of the face at -uDz 
	G4double  uAlp1 = 0.*deg; 	// Angle with respect to the y axis from the centre of the side (lower endcap) 
	G4double uDy2 = 9.25*mm /2.;	// Half y length at +uDz 
	G4double  uDx3 = 77.*mm /2.;  	// Half x length of the side at y=-uDy2 of the face at +uDz 
	G4double uDx4 = 0.01*mm;	// Half x length of the side at y=+uDy2 of the face at +uDz 
	G4double  uAlp2 = 0.*deg;	// Angle with respect to the y axis from the centre of the side (upper endcap) 

	G4Trap* upperBGOCrystal = new G4Trap("uperBGOcrystal_trap"+nameCluster[nbCluster],
										uDz,   	// Half z length 
										uTheta,	// Polar angle of the line joining the centres of the faces at -/+uDz 
										uPhi,	// Azimuthal angle of the line joining the centre of the face at -uDz to the centre of the face at +uDz
										uDy1,	// Half y length at -uDz 
										uDx1,  	// Half x length of the side at y=-uDy1 of the face at -uDz 
										uDx2,	// Half x length of the side at y=+uDy1 of the face at -uDz 
										uAlp1, 	// Angle with respect to the y axis from the centre of the side (lower endcap) 
										uDy2,	// Half y length at +uDz 
										uDx3,  	// Half x length of the side at y=-uDy2 of the face at +uDz 
										uDx4,	// Half x length of the side at y=+uDy2 of the face at +uDz 
										uAlp2);	// Angle with respect to the y axis from the centre of the side (upper endcap) 

	G4Box* cutBox2 = new G4Box("CutBox2",lDx2*1.1, (uDy1+lDy1)*3 , (uDy1+lDy1));
	
	G4Transform3D positionCutBox2 = G4Transform3D(BGOCutRotMatrix, G4ThreeVector(0., 
	(uDy1+lDy1)*std::sin(BGOtilt) - lDy1,
	 uDz + (uDy1+lDy1)*std::cos(BGOtilt) - (uDy1+lDy1)*std::tan(BGOtilt)));

	G4SubtractionSolid* upperBGOCrystalCut = new G4SubtractionSolid("upperBGOCrystalCut"+nameCluster[nbCluster],	// Name
											upperBGOCrystal,			// Minuend Volume
											cutBox2,							// Subtrahend Volume
											positionCutBox2);			// position of Subtrahend relativ to Minuend
										
	G4UnionSolid* fullBGOCrystal_solid = new G4UnionSolid("fullBGOCrystal_solid"+nameCluster[nbCluster],				// Name
										lowerBGOCrystalCut,				// Summand Volume 1
										upperBGOCrystalCut,				// Summand Volume 2
										0, 	// rotation of Summand 2
										G4ThreeVector(0., lDy1 + uDy1 - 10 * nm, 0.));	// position of Summand 2 relativ to Summand 1
											
	BGOCrystal_logic[nbCluster] = new G4LogicalVolume(fullBGOCrystal_solid, BGO, "BGOCrystal_logic"+nameCluster[nbCluster],0,0,0);
	BGOCrystal_logic[nbCluster]->SetVisAttributes(BGOVisAtt);
	
	G4Transform3D positionBGOCrystal[8];
	G4RotationMatrix BGORotMatrix;					// Rotationmatrix for detector
	BGORotMatrix.rotateX(BGOtilt);
	G4double distBGOCenter = distCenterBGOTip + lDz * std::sin(BGOtilt) + lDy1 * std::cos(BGOtilt);
	
	for (G4int i=0;i<8;i++)
	{
		positionBGOCrystal[i] = G4Transform3D(BGORotMatrix, 
							G4ThreeVector(distBGOCenter * std::sin(45*deg * i),
							distBGOCenter * std::cos(45*deg * i), 
							cDz -housThick - 8.*mm -1 * (uDz * std::cos(BGOtilt) - lDy1 * std::sin(BGOtilt))));
		G4String crystalName = "BGOcrystal_physic_" + G4UIcommand::ConvertToString(i) + +nameCluster[nbCluster];
		
		BGOCrystal_physical[nbCluster][i] = new G4PVPlacement(positionBGOCrystal[i],					// position in mother frame
									BGOCrystal_logic[nbCluster],		// its logical volume 		  
									crystalName,					// its name
									housingBGOvac_logic,							// its mother  volume
									false,								// no boolean operations
									i,									// copy number
									pSurfChk);							//surface check
		BGORotMatrix.rotateZ(-45*deg);
	}

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myDetectorConstruction::TripleClusterGeometry(G4int nbCluster, G4Transform3D positionTripleCluster, G4bool shieldingOption = 0)
{
	//TripleCluster imported from gdml by Toni K??gler
	if (NbOfCrystals[nbCluster] != 3) {
		G4cout << "Change Number of Crystals to 3 for the Triple Cluster" << G4endl;
		return;
	}
	const G4int NumComp  = 100;						// Number of components
	G4VPhysicalVolume* TripleClusterComp_physical[NumComp];	// The components of 1 cluster detector
	G4LogicalVolume* TripleClusterComp_logical[NumComp];
	pSurfChk = true;

   dBGOHousing = 254.*mm; //taken from CAD
   G4double rBackTub = 220.*mm; //20mm bigger than highest radius of the parts
   G4double rBGOHousingFront = 65.*mm; //taken from CAD
   G4double dTripleCluster = dBGOHousing + 596.*mm;
   dOffsetBGO = 66.*mm; //resulting from CAD file
   G4double dOffsetLeadRings = 45.*mm; 
   G4double dDetectorOffset = 8.*mm - dLeadRingsTotal/2. +dLeadRing1BGOHousing/2.;  
   
	//collimator
   	G4double dColli1 = dLeadRing1BGOHousing; //part inside the protruding lead ring
	G4double dColli2 = dCollimator - dLeadRing1BGOHousing; //part outside the lead ring
	//Geometry following the slope of the BGOhousing, values taken from CAD
	G4double x1housing = 65.*mm, x2housing = 101.*mm;
	G4double z1housing = 66.*mm, z2housing = 264.*mm;
	G4double mSlope = (x2housing - x1housing)/(z2housing-z1housing);
	G4double x0 = x1housing - mSlope * z1housing;
	G4double rColli1 = mSlope * dColli1 + x0;
	G4double rColli2 = mSlope * dColli2 + x0;
	
	//leadrings numbered from small to big
	G4double r1LeadRing3 = 155.*mm, r2LeadRing3 = 180.*mm, dLeadRing3 = 79.*mm;
	G4double r1LeadRing23 = 145.*mm, r2LeadRing23 = 155.*mm, dLeadRing23 = 20.*mm;
	G4double r1LeadRing2 = 145.*mm, r2LeadRing2 = 170.*mm, dLeadRing2 = 79.*mm;
	G4double r1LeadRing12 = 135.*mm, r2LeadRing12 = 145.*mm, dLeadRing12 = 20.*mm;
	G4double r1LeadRing1 = 135.*mm, r2LeadRing1 = 160.*mm, dLeadRing1 = 79.*mm;
	
   //mother volume for triple cluster
   G4Cons* frontShielding = new G4Cons("Shielding", 0.*mm, r2LeadRing1, 0.*mm, rBackTub, dFrontCone/2., 0.*deg, 360.*deg);
   G4Tubs* BackTub = new G4Tubs("BackofDetector", 0*mm, rBackTub, dCluster/2., 0.*deg, 360.*deg);
   G4double zPositionBack = dFrontCone/2. + dCluster/2.;
   
   G4Transform3D positionBack(G4RotationMatrix(), G4ThreeVector(0, 0, zPositionBack));   
   G4UnionSolid* solidTripleClusterVolume = new G4UnionSolid("TripleClusterVolume", frontShielding, BackTub, positionBack);
   
	TripleClusterDetector_logical[nbCluster] = new G4LogicalVolume(solidTripleClusterVolume, Air, "TripleClusterDetector_logic"+std::to_string(nbCluster),0,0,0);
	TripleClusterDetector_logical[nbCluster]->SetVisAttributes(vacVisAtt);
	TripleClusterDetector_physical[nbCluster] = new G4PVPlacement(positionTripleCluster,	// position in mother frame
										TripleClusterDetector_logical[nbCluster],		// its logical volume				  
										"TripleClusterDetector_phys"+std::to_string(nbCluster),	// its name
										logicWorld,	// its mother  volume
										false,				// no boolean operations
										0,					// copy number
										pSurfChk);					 							
	
	//leadRings
	G4Tubs* tubLeadRing1 = new G4Tubs("LeadRing1", r1LeadRing1, r2LeadRing1, dLeadRing1/2., 0.*deg, 360.*deg);
	G4Tubs* tubLeadRing12 = new G4Tubs("LeadRing12", r1LeadRing12, r2LeadRing12, dLeadRing12/2., 0.*deg, 360.*deg);
	G4Tubs* tubLeadRing2 = new G4Tubs("LeadRing2", r1LeadRing2, r2LeadRing2, dLeadRing2/2., 0.*deg, 360.*deg);
	G4Tubs* tubLeadRing23 = new G4Tubs("LeadRing23", r1LeadRing23, r2LeadRing23, dLeadRing23/2., 0.*deg, 360.*deg);
	G4Tubs* tubLeadRing3 = new G4Tubs("LeadRing3", r1LeadRing3, r2LeadRing3, dLeadRing3/2., 0.*deg, 360.*deg);
	G4MultiUnion* unionLeadRings = new G4MultiUnion("unionLeadRings");
	G4Transform3D none = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,0));
	unionLeadRings-> AddNode(*tubLeadRing1,none);
	G4Transform3D transLeadRing12 = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,dLeadRing1/2. - dLeadRing12/2.));
	unionLeadRings-> AddNode(*tubLeadRing12,transLeadRing12);
	G4Transform3D transLeadRing2 = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,dLeadRing1/2. + dLeadRing2/2.));
	unionLeadRings-> AddNode(*tubLeadRing2, transLeadRing2);
	G4Transform3D transLeadRing23 = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,dLeadRing1/2. + dLeadRing2 - dLeadRing23/2.));
	unionLeadRings-> AddNode(*tubLeadRing23, transLeadRing23);
	G4Transform3D transLeadRing3 = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0,0,dLeadRing1/2. + dLeadRing2 + dLeadRing3/2.));
	unionLeadRings-> AddNode(*tubLeadRing3, transLeadRing3);
	unionLeadRings->Voxelize();
	G4LogicalVolume* leadRings_logical = new G4LogicalVolume(unionLeadRings, Pb, "leadRings_logic"+std::to_string(nbCluster),0,0,0);
	leadRings_logical->SetVisAttributes(pbVisAtt);
	G4RotationMatrix rotationLeadRings = G4RotationMatrix();
	G4PVPlacement* leadRings_physical = new G4PVPlacement(G4Transform3D(rotationLeadRings,G4ThreeVector(0,0,-dFrontCone/2. + dColli2 + dLeadRing1/2.)),	// position in mother frame
										leadRings_logical,		// its logical volume				  
										"leadRings_phys"+std::to_string(nbCluster),	// its name
										TripleClusterDetector_logical[nbCluster],	// its mother  volume
										false,				// no boolean operations
										0,					// copy number
										pSurfChk);									

	//collimator
	//outer part
    G4Cons* solidCollimatorIn = new G4Cons("solidCollimatorIn", rColli1, r1LeadRing1, rColli2, r1LeadRing1, dColli1/2., 0.*deg, 360.*deg);							
    G4Cons* solidCollimatorOut = new G4Cons("solidCollimatorOut", rColli2, r2LeadRing1, rBGOHousingFront, r2LeadRing1, dColli2/2., 0.*deg, 360.*deg);
    G4UnionSolid* solidCollimator = new G4UnionSolid("solidCollimator", solidCollimatorIn, solidCollimatorOut, G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,-(dColli1 + dColli2)/2.)));
	G4LogicalVolume* Collimator_logical = new G4LogicalVolume(solidCollimator, Pb, "Collimator_logic"+std::to_string(nbCluster),0,0,0);
	G4PVPlacement* Collimator_physical = new G4PVPlacement(G4Transform3D(G4RotationMatrix(),G4ThreeVector(0,0,-dFrontCone/2. + dColli1/2. + dColli2)),	// position in mother frame
										Collimator_logical,		// its logical volume				  
										"Collimator_phys"+std::to_string(nbCluster),	// its name
										TripleClusterDetector_logical[nbCluster],	// its mother  volume
										false,				// no boolean operations
										0,					// copy number 
										pSurfChk);					
	Collimator_logical->SetVisAttributes(pbVisAtt);
										
	G4GDMLParser parser3;	
	G4RotationMatrix rotationTripleCluster = G4RotationMatrix();
	rotationTripleCluster.rotateY(-180.*deg);
	//MB1 and MB2 different geometry and crystal ordering
	if (strcmp("MB1", nameCluster[nbCluster]) == 0) {
		rotationTripleCluster.rotateZ(-30.*deg);
		parser3.Read("gdml/TripleCluster/TripleClusterMB1.gdml",false);
	}
	else if (strcmp("MB2", nameCluster[nbCluster]) == 0) {
		rotationTripleCluster.rotateZ(30.*deg);
		parser3.Read("gdml/TripleCluster/TripleClusterMB2.gdml",false);									
	}
	
	G4LogicalVolume* TripleCluster_logical = parser3.GetWorldVolume()->GetLogicalVolume();
	TripleCluster_logical->SetMaterial(Air);
	TripleCluster_logical->SetVisAttributes(vacVisAtt);
	TripleCluster_logical->SetName(TripleCluster_logical->GetName()+std::to_string(nbCluster));

	//positioning inside mother volume	
	G4ThreeVector vTriple;
	vTriple = G4ThreeVector(0,0,-dFrontCone/2.-dDetectorOffset + dColli2 + dColli1);
	G4PVPlacement* TripleCluster_physical = new G4PVPlacement(G4Transform3D(rotationTripleCluster,vTriple),
										TripleCluster_logical,		// its logical volume				  
										"TripleCluster_phys"+std::to_string(nbCluster),	// its name
										TripleClusterDetector_logical[nbCluster],	// its mother  volume
										false,				// no boolean operations
										0,
										pSurfChk);					// copy number	

	G4cout << "DeadLayers TripleCluster:\n";
	G4double NumDaughters = TripleCluster_logical->GetNoDaughters();
	if(NumDaughters > NumComp)
	{
		G4cout<< "\n--->Number of detector components in gdml file: " << TripleClusterDetector_logical[nbCluster]->GetNoDaughters() << 
		" larger then assumed component number: " << NumComp << G4endl;
		G4cout<< "\n Tripple Cluster not propper implemented!"<< G4endl;
		return;
	}
	
	for (G4int i=0; i < NumDaughters; i++)  
	{
		TripleClusterComp_physical[i] = TripleCluster_logical->GetDaughter(i);
		G4String CompName = TripleClusterComp_physical[i]->GetName();
		TripleClusterComp_physical[i]->SetName(CompName + nameCluster[nbCluster]);
		TripleClusterComp_logical[i] = TripleClusterComp_physical[i]->GetLogicalVolume();
		CompName = TripleClusterComp_logical[i]->GetName();
		TripleClusterComp_logical[i]->SetName(CompName + nameCluster[nbCluster]);
		G4cout<< CompName << G4endl;
		if(CompName.contains("Crystal"))
		{
			TripleClusterComp_physical[i]->GetLogicalVolume()->SetVisAttributes(vacVisAtt);
			for(G4int nbCrystal=0; nbCrystal<NbOfCrystals[nbCluster]; nbCrystal++) 
			{
				if(CompName.contains(G4UIcommand::ConvertToString(nbCrystal+1)))
				{
					TripleClusterCrystal_physical[nbCluster][nbCrystal] = TripleClusterComp_physical[i];
					TripleClusterCrystal_logical[nbCluster][nbCrystal] = TripleClusterCrystal_physical[nbCluster][nbCrystal]->GetLogicalVolume();
					CrystalwDeadLayersGeometry(nbCluster, nbCrystal, TripleClusterCrystal_physical[nbCluster][nbCrystal], dDeadLayerCap[nbCluster][nbCrystal], hDeadLayerCone[nbCluster][nbCrystal]);			
				}
			}
		} else if (CompName.contains("Cap"))
		{
			TripleClusterComp_physical[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt);
			
		} else if (CompName.contains("Dewar"))
		{
			TripleClusterComp_physical[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt);
			
		} else if (CompName.contains("PreAmp"))
		{
			TripleClusterComp_physical[i]->GetLogicalVolume()->SetVisAttributes(PreampVisAtt);
			
		} else if (CompName.contains("ColdFinger"))
		{
			TripleClusterComp_physical[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt);
		}
	}
	G4GDMLParser parser;
	parser.Read("gdml/TripleCluster/BGOhousing.gdml",false);
	BGOHousing_logical[nbCluster] = parser.GetWorldVolume()->GetLogicalVolume();
	BGOHousing_logical[nbCluster]->SetMaterial(Al);
	BGOHousing_logical[nbCluster]->SetVisAttributes(aluVisAtt);
	BGOHousing_logical[nbCluster]->SetName(BGOHousing_logical[nbCluster]->GetName()+std::to_string(nbCluster));
	G4RotationMatrix rotationBGOHousing = G4RotationMatrix();
	rotationBGOHousing.rotateY(-90.*deg);
	G4PVPlacement* BGOHousing_physical = new G4PVPlacement(G4Transform3D(rotationBGOHousing,G4ThreeVector(0,0,-dFrontCone/2. -dOffsetBGO + dColli2 + dColli1)),	// position in mother frame
										BGOHousing_logical[nbCluster],		// its logical volume				  
										"BGOHousing_phys"+std::to_string(nbCluster),	// its name
										TripleClusterDetector_logical[nbCluster],	// its mother  volume
										false,				// no boolean operations
										0,					// copy number
										pSurfChk);		
	ClusterBGOGeometry(nbCluster);
	
}

void myDetectorConstruction::SeptupleClusterHousing(G4int nbCluster, G4Transform3D positionSeptupleCluster, G4bool shieldingOption = 0){
	//Septuple Housing Felix, not displayed correctly in the standard geant4 viewer
	//shape is hexagon, divide it into two triangles and cut out the triangles at the top
	G4int nSides = 3;
	const G4int zN = 2;
	G4double scaleBack = 1.25;
	G4double lengthTriangle = 225.*mm;
	G4double rFront = sqrt(3.)/6.*lengthTriangle; //outer radius of triangle with sidelength 225.*mm
	G4double rFrontHeight = lengthTriangle/sqrt(3.);
	G4double rBackHeight = scaleBack * rFrontHeight; 
	G4double rBack = scaleBack * rFront;
	G4double z[zN] = {0, 119.*mm};
	G4double r0[zN] = {0,0};
	G4double rMax[zN] = {rFront, rBack};
	G4double heightDome = 10*mm;
	G4double widthDome = 2.*67.*mm;
	G4double thickness = 1.*mm;
	G4double sideCutTriangleFront = 40.*mm;
	G4double sideCutTriangleBack = scaleBack * sideCutTriangleFront;
	G4double heightCutFront = sqrt(3.)/2.*sideCutTriangleFront;
	G4double heightCutBack = sqrt(3.)/2.*sideCutTriangleBack;	
	
	//create two triangles + sphere for base solid
	G4double rDome = (4.*heightDome*heightDome + widthDome*widthDome)/8./heightDome; //calculate radius from circle segment
	G4cerr << rDome << G4endl;
	G4Sphere* Dome = new G4Sphere("Dome",rDome-thickness,rDome, 0.*deg, 360.*deg, 162.*deg, 180.*deg);
	G4Polyhedra* triangle = new G4Polyhedra("triangle", 0, 360.*deg, nSides, zN, z, r0, rMax);
	
	/*
	//cut edges of triangle
	G4TessellatedSolid* tessel = new G4TessellatedSolid("Cut");

	G4double midVector = 0;// - heightCutFront;
	G4ThreeVector topleftFront = G4ThreeVector(rFrontHeight - heightCutFront, -sideCutTriangleFront/2., z[0]-1.*mm);
	G4ThreeVector toprightFront = G4ThreeVector(rFrontHeight - heightCutFront, sideCutTriangleFront/2., z[0]-1.*mm);
	G4ThreeVector bottomleftFront = G4ThreeVector((rBackHeight - heightCutBack), -sideCutTriangleBack/2., z[1]);
	G4ThreeVector bottomrightFront = G4ThreeVector((rBackHeight - heightCutBack), sideCutTriangleBack/2., z[1]);	
	G4ThreeVector topleftBack = G4ThreeVector(5.* (rFrontHeight - heightCutFront), -5.*sideCutTriangleFront/2., 2.*z[0]-2.*mm);
	G4ThreeVector toprightBack = G4ThreeVector(5.* (rFrontHeight - heightCutFront), 5.*sideCutTriangleFront/2., 2.*z[0]-2.*mm);
	G4ThreeVector bottomleftBack = G4ThreeVector(5.*(rBackHeight - heightCutBack), -5.*sideCutTriangleBack/2. ,2.*z[1]);
	G4ThreeVector bottomrightBack = G4ThreeVector(5.*(rBackHeight - heightCutBack), 5.*sideCutTriangleBack/2., 2.*z[1]);	
	G4VFacet* faceFront = new G4QuadrangularFacet(topleftFront, bottomleftFront, bottomrightFront, toprightFront, ABSOLUTE);
	G4cout << "front:\n";
	tessel->AddFacet(faceFront);
	G4VFacet* faceTop = new G4QuadrangularFacet(topleftFront, toprightFront, toprightBack, topleftBack, ABSOLUTE);
	G4cout << "top:\n";
	tessel->AddFacet(faceTop);
	G4VFacet* faceLeft = new G4QuadrangularFacet(topleftFront, topleftBack, bottomleftBack, bottomleftFront, ABSOLUTE);
	G4cout << "left:\n";
	tessel->AddFacet(faceLeft);
	G4VFacet* faceRight = new G4QuadrangularFacet(toprightFront, bottomrightFront, bottomrightBack, toprightBack, ABSOLUTE);
	G4cout << "right:\n";
	tessel->AddFacet(faceRight);
	G4VFacet* faceBottom = new G4QuadrangularFacet(bottomleftFront, bottomleftBack, bottomrightBack, bottomrightFront, ABSOLUTE);
	G4cout << "bottom:\n";
	tessel->AddFacet(faceBottom);
	G4VFacet* faceBack = new G4QuadrangularFacet(topleftBack, toprightBack, bottomrightBack, bottomleftBack, ABSOLUTE);
	G4cout << "back:\n";
	tessel->AddFacet(faceBack);
	tessel->SetSolidClosed(true);
	*/
	
	G4MultiUnion* cutBase = new G4MultiUnion("cutBase");
	G4Box* cutBox = new G4Box("cutBox", sideCutTriangleBack, heightCutBack ,z[1]);
	G4double angleCut = 60.*deg;
	G4double angleTilt = atan((rBackHeight-rFrontHeight)/z[1])*rad;
	G4ThreeVector cutVector[6];
	G4Transform3D positionCut[6];
	G4RotationMatrix cutRot[6];
	for (G4int i = 0; i < 6; i++) {
		cutVector[i] = G4ThreeVector(sideCutTriangleBack + rFrontHeight - heightCutFront,0,0);
		cutVector[i].rotateZ(angleCut * i);
		cutVector[i].setZ(0);
		cutRot[i].rotateY(angleTilt);
		cutRot[i].rotateZ(angleCut * i);
		positionCut[i] = G4Transform3D(cutRot[i],cutVector[i]);
		cutBase->AddNode(*cutBox, positionCut[i]);
	}
	cutBase->Voxelize();
	
	//G4SubtractionSolid* BaseFinal = new G4SubtractionSolid("BaseFinal", triangle, cutBase, G4Transform3D());	
	
	G4MultiUnion* Base = new G4MultiUnion("Base");
	G4Transform3D positionTriangle1(G4RotationMatrix(), G4ThreeVector(0, 0, 0));
	G4RotationMatrix rotTriangle2;
	rotTriangle2.rotateZ(180.*deg);
	G4Transform3D positionTriangle2(rotTriangle2, G4ThreeVector(0, 0, 0));
	G4Transform3D positionDome(G4RotationMatrix(), G4ThreeVector(0, 0, rDome-heightDome));
	Base->AddNode(*triangle, positionTriangle1);
	Base->AddNode(*triangle, positionTriangle2);
	Base->AddNode(*Dome, positionDome);
    Base->Voxelize();
    
    G4SubtractionSolid* BaseFinal = new G4SubtractionSolid("BaseFinal", Base, cutBase, G4Transform3D());	

	//same stuff for vacuum with 1mm distance
	//create two triangles + sphere for base solid
	G4Sphere* DomeVac = new G4Sphere("DomeVac",rDome - 2.*heightDome,rDome-thickness, 0.*deg, 360.*deg, 162.*deg, 180.*deg);
	G4double zVac[zN] = {0+thickness, 119.*mm-thickness};
	G4double r0Vac[zN] = {0,0};
	G4double rMaxVac[zN] = {rFront-thickness, rBack-thickness};
	G4Polyhedra* triangleVac = new G4Polyhedra("triangleVac", 0, 360.*deg, nSides, zN, zVac, r0Vac, rMaxVac);
	
	G4MultiUnion* cutBaseVac = new G4MultiUnion("cutBaseVac");
	G4ThreeVector cutVectorVac[6];
	G4Transform3D positionCutVac[6];
	G4RotationMatrix cutRotVac[6];
	for (G4int i = 0; i < 6; i++) {
		cutVectorVac[i] = G4ThreeVector(sideCutTriangleBack + rFrontHeight - heightCutFront-thickness,0,0);
		cutVectorVac[i].rotateZ(angleCut * i);
		cutVectorVac[i].setZ(thickness);
		cutRotVac[i].rotateY(angleTilt);
		cutRotVac[i].rotateZ(angleCut * i);
		positionCutVac[i] = G4Transform3D(cutRotVac[i],cutVectorVac[i]);
		cutBaseVac->AddNode(*cutBox, positionCutVac[i]);	
	}
	cutBaseVac->Voxelize();
	
	G4MultiUnion* BaseVac = new G4MultiUnion("BaseVac");
	G4Transform3D positionDomeVac(G4RotationMatrix(), G4ThreeVector(0, 0, rDome-2.*heightDome+2.*thickness));
	BaseVac->AddNode(*triangleVac, positionTriangle1);
	BaseVac->AddNode(*triangleVac, positionTriangle2);
	BaseVac->AddNode(*DomeVac, positionDomeVac);
    BaseVac->Voxelize();	
    
    G4SubtractionSolid* BaseFinalVac = new G4SubtractionSolid("BaseFinalVac", BaseVac, cutBaseVac, G4Transform3D());
    											
	SeptupleClusterHousing_logical[nbCluster] = new G4LogicalVolume(BaseFinal, Al, "Housing"+nameCluster[nbCluster],0,0,0);
	SeptupleClusterHousing_logical[nbCluster]->SetVisAttributes(aluVisAtt);
	SeptupleClusterHousing_phys[nbCluster] = new G4PVPlacement(positionSeptupleCluster,	// position in mother frame
										SeptupleClusterHousing_logical[nbCluster],		// its logical volume				  
										"housing_phys"+nameCluster[nbCluster],	// its name
										logicWorld,	// its mother  volume
										false,				// no boolean operations
										0,					// copy number
										pSurfChk);
											
	SeptupleClusterVacuum_logical[nbCluster] = new G4LogicalVolume(BaseFinalVac, Vacuum, "HousingVac"+nameCluster[nbCluster],0,0,0);
	SeptupleClusterVacuum_logical[nbCluster]->SetVisAttributes(vacVisAtt);
	SeptupleClusterVacuum_phys[nbCluster] = new G4PVPlacement(G4Transform3D(),	// position in mother frame
										SeptupleClusterVacuum_logical[nbCluster],		// its logical volume				  
										"housingVac_phys"+nameCluster[nbCluster],	// its name
										SeptupleClusterHousing_logical[nbCluster],	// its mother  volume
										false,				// no boolean operations
										0,					// copy number
										pSurfChk);
}

void myDetectorConstruction::SeptupleClusterGeometry(G4int nbCluster, G4Transform3D positionSeptupleCluster, G4bool shieldingOption = 0)
{
	//Shielding not implemented yet
	//~ if (shieldingOption)
		//~ SeptupleClusterBGOGeometry(nbCluster, positionSeptupleCluster);
	
	if (nameCluster[nbCluster].contains("EB17")) {
		G4ThreeVector vector = positionSeptupleCluster.getTranslation();
		vector.setZ(vector.getZ());
		G4RotationMatrix matrix = positionSeptupleCluster.getRotation();
		//matrix.rotateX(-tiltCluster2);
		positionSeptupleCluster = G4Transform3D(matrix, vector);	
	}
	if (nameCluster[nbCluster].contains("EB18")) {
		G4ThreeVector vector = positionSeptupleCluster.getTranslation();
		G4RotationMatrix matrix = positionSeptupleCluster.getRotation();
		//matrix.rotateX(-dCluster2);
		positionSeptupleCluster = G4Transform3D(matrix, vector);	
	}		
	//housing
	
	SeptupleClusterHousing(nbCluster, positionSeptupleCluster, shieldingOption);
	if (ConstructBGO[nbCluster])
		SeptupleClusterBGOGeometry(nbCluster, positionSeptupleCluster);
	
	
	
	//~ G4GDMLParser parser2;
	//~ parser2.Read("gdml/SeptupleCluster/Housingv.gdml",false); //by Guang-Shun Li NIMA 890 (2018) 148???154
	//~ SeptupleClusterVacuum_logical[nbCluster] = parser2.GetWorldVolume()->GetLogicalVolume();
	//~ SeptupleClusterVacuum_logical[nbCluster]->SetName("SeptupleVacuum"+nameCluster[nbCluster]+"_log");
	//~ SeptupleClusterVacuum_logical[nbCluster]->SetVisAttributes(vacVisAtt);
	//~ SeptupleClusterVacuum_logical[nbCluster]->SetMaterial(Vacuum);
	//~ SeptupleClusterVacuum_phys[nbCluster] = new G4PVPlacement(G4Transform3D(positionSeptupleCluster),	// position in mother frame
										//~ SeptupleClusterVacuum_logical[nbCluster],		// its logical volume				  
										//~ "SeptupleClusterVacuum_phys"+nameCluster[nbCluster],	// its name
										//~ logicWorld,	// its mother  volume
										//~ false,				// no boolean operations
										//~ 0,					// copy number
										//~ pSurfChk);		
 	const G4int maxNumComp  = 16;
 	//~ G4int NumDaughters = SeptupleClusterVacuum_logical[nbCluster]->GetNoDaughters();
 	//~ G4VPhysicalVolume* HousingComp_physical[maxNumComp];	
	//~ for (G4int i=0; i < NumDaughters; i++) {
		//~ HousingComp_physical[i] = SeptupleClusterVacuum_logical[nbCluster]->GetDaughter(i);
		//~ G4String CompName = HousingComp_physical[i]->GetName();
		//~ HousingComp_physical[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt);
		//~ HousingComp_physical[i]->GetLogicalVolume()->SetMaterial(Al);
		//~ HousingComp_physical[i]->SetName(CompName+nameCluster[nbCluster]);
		//~ HousingComp_physical[i]->GetLogicalVolume()->SetName(CompName+nameCluster[nbCluster]+"_log");
		//~ if (CompName.contains("Housing")) {
			//~ SeptupleClusterHousing_logical[nbCluster] = HousingComp_physical[i]->GetLogicalVolume();
		//~ }	
	//~ }
	
	//capsules with crystals inside
	if (NbOfCrystals[nbCluster] != 7) {
		G4cout << "Change Number of Crystals to 7 for the Septuple Cluster" << G4endl;
		return;
	}
						// Number of components
	G4VPhysicalVolume* SeptupleClusterComp_physical[maxNbOfCrystals];	// The components of 1 cluster detector
	G4LogicalVolume* SeptupleClusterComp_logical[maxNbOfCrystals];
	pSurfChk = true;
	//~ G4Region* TripleClusterHPGeRegion = new G4Region("TripleClusterHPGeReg");
	G4Region* ClusterHPGeRegion[maxNbOfClusters][maxNbOfCrystals];
		
	G4VPhysicalVolume* Capsule_physical[maxNbOfCrystals][maxNumComp];	// The components of 1 capsule detector
	G4LogicalVolume* Capsule_logical[maxNbOfCrystals][maxNumComp];	
	G4GDMLParser parser[maxNbOfCrystals];

	for (G4int nbCrystal = 0; nbCrystal < NbOfCrystals[nbCluster]; nbCrystal++) {
		//separate files for each crystal capsule, because sensitive detectors won't work otherwise
		parser[nbCrystal].Read("gdml/SeptupleCluster/Capsule"+G4UIcommand::ConvertToString(nbCrystal)+".gdml", false);		// shema validation true/false via network url in gdml file
		SeptupleClusterComp_logical[nbCrystal] = parser[nbCrystal].GetWorldVolume()->GetLogicalVolume();
		SeptupleClusterComp_logical[nbCrystal]->SetName("Capsule"+nameCluster[nbCluster]+G4UIcommand::ConvertToString(nbCrystal));
		SeptupleClusterComp_logical[nbCrystal]->SetVisAttributes(aluVisAtt);
		G4int middleCapsule = 6;
		G4RotationMatrix rotCapsule;
		rotCapsule.rotateX(180.*deg);
		if (nbCrystal == middleCapsule)
			rotCapsule.rotateY(0.*deg);
		else
			rotCapsule.rotateY(-12.*deg);
						
		G4RotationMatrix rotCapsuleZ;
		if (strcmp("EB17", nameCluster[nbCluster]) == 0) {
			rotCapsule.rotateZ((nbCrystal+6)*-60.*deg);
			rotCapsuleZ.rotateZ((nbCrystal+6)*-60.*deg);
		}
		if (strcmp("EB18", nameCluster[nbCluster]) == 0) {
			rotCapsule.rotateZ((nbCrystal+4)*-60.*deg);
			rotCapsuleZ.rotateZ((nbCrystal+4)*-60.*deg);		
			
		}
		G4double hHeightCapsule = 31.4*mm; //adjusted with the chinese gdml
		G4Transform3D positionCapsule;
		G4double zCorrection = -5.2*mm; //middle capsule needs to be moved back
		G4double offsetHousing = -15.39*mm;
		G4ThreeVector vectorFirstRot(-hHeightCapsule * rotCapsuleZ.xx(), -hHeightCapsule * rotCapsuleZ.yx(), -0.5*hHeightCapsule*rotCapsuleZ.zx());
		G4ThreeVector vectorSecondRot(-hHeightCapsule * rotCapsule.xx(), -hHeightCapsule * rotCapsule.yx(), -0.5*hHeightCapsule * rotCapsule.zx());
		G4ThreeVector vectorCapsule;
		vectorCapsule = vectorFirstRot + vectorSecondRot + G4ThreeVector(0,0,-offsetHousing);
		if (nbCrystal == middleCapsule)
			positionCapsule = G4Transform3D(rotCapsule, G4ThreeVector(0*mm, 0*mm, -offsetHousing-0.5*zCorrection));
		else
			positionCapsule = G4Transform3D(rotCapsule, vectorCapsule);
		SeptupleClusterComp_physical[nbCrystal] = new G4PVPlacement(positionCapsule,// position in mother frame
											SeptupleClusterComp_logical[nbCrystal],		// its logical volume is the world volume of gdml file			  
											"Capsule"+nameCluster[nbCluster]+G4UIcommand::ConvertToString(nbCrystal)+"_phys",					// its name
											SeptupleClusterVacuum_logical[nbCluster],							// its mother  volume
											false,								// no boolean operations
											0,									// copy number
											pSurfChk);							//surface check
		SeptupleClusterComp_physical[nbCrystal]->CheckOverlaps(100000, 0,true);									
	
		
		//G4int k = 0;
	}

	for (G4int nbCrystal = 0; nbCrystal < NbOfCrystals[nbCluster]; nbCrystal++) {
		G4int NumDaughters = SeptupleClusterComp_logical[nbCrystal]->GetNoDaughters();
		//G4cerr << nbCrystal << " " << nbCluster << " " << NumDaughters << G4endl;
		for (G4int nbDaughter=0; nbDaughter < NumDaughters; nbDaughter++)  
		{
			G4String CompName = SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->GetName();	
			//G4cerr << CompName << G4endl;	
			if(CompName.contains("Crystal"))
			{
				SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->SetName("Crystal"+ G4UIcommand::ConvertToString(nbCrystal)+nameCluster[nbCluster]);
				SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->GetLogicalVolume()->SetName("Crystal"+ G4UIcommand::ConvertToString(nbCrystal)+nameCluster[nbCluster]+"_log");
				//create dead layers
				CrystalwDeadLayersGeometry(nbCluster, nbCrystal, SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter), dDeadLayerCap[nbCluster][nbCrystal], hDeadLayerCone[nbCluster][nbCrystal]); 									
			} else if (CompName.contains("PreAmp"))
			{
				SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->GetLogicalVolume()->SetVisAttributes(ContactLayerVisAtt);				
			}
		}		
	}
}

/*void myDetectorConstruction::SeptupleClusterGeometry(G4int nbCluster, G4Transform3D positionSeptupleCluster, G4bool shieldingOption = 0)
{
	//housing gdml version, too large to fit into bgo
	G4GDMLParser parser2;
	parser2.Read("gdml/SeptupleCluster/Housing.gdml",false);
	SeptupleClusterVacuum_logical[nbCluster] = parser2.GetWorldVolume()->GetLogicalVolume();
	SeptupleClusterVacuum_logical[nbCluster]->SetName("SeptupleVacuum"+nameCluster[nbCluster]+"_log");
	SeptupleClusterVacuum_logical[nbCluster]->SetVisAttributes(vacVisAtt);
	SeptupleClusterVacuum_logical[nbCluster]->SetMaterial(Vacuum);
	SeptupleClusterVacuum_phys[nbCluster] = new G4PVPlacement(G4Transform3D(positionSeptupleCluster),	// position in mother frame
										SeptupleClusterVacuum_logical[nbCluster],		// its logical volume				  
										"SeptupleClusterVacuum_phys"+nameCluster[nbCluster],	// its name
										logicWorld,	// its mother  volume
										false,				// no boolean operations
										0,					// copy number
										pSurfChk);		
 	const G4int maxNumComp  = 16;
 	G4int NumDaughters = SeptupleClusterVacuum_logical[nbCluster]->GetNoDaughters();
 	G4VPhysicalVolume* HousingComp_physical[maxNumComp];	
	for (G4int i=0; i < NumDaughters; i++) {
		HousingComp_physical[i] = SeptupleClusterVacuum_logical[nbCluster]->GetDaughter(i);
		G4String CompName = HousingComp_physical[i]->GetName();
		HousingComp_physical[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt);
		HousingComp_physical[i]->GetLogicalVolume()->SetMaterial(Al);
		HousingComp_physical[i]->SetName(CompName+nameCluster[nbCluster]);
		HousingComp_physical[i]->GetLogicalVolume()->SetName(CompName+nameCluster[nbCluster]+"_log");
		if (CompName.contains("Housing")) {
			SeptupleClusterHousing_logical[nbCluster] = HousingComp_physical[i]->GetLogicalVolume();
		}	
	}
	
	//capsules with crystals inside
	if (NbOfCrystals[nbCluster] != 7) {
		G4cout << "Change Number of Crystals to 7 for the Septuple Cluster" << G4endl;
		return;
	}
						// Number of components
	G4VPhysicalVolume* SeptupleClusterComp_physical[maxNbOfCrystals];	// The components of 1 cluster detector
	G4LogicalVolume* SeptupleClusterComp_logical[maxNbOfCrystals];
	pSurfChk = true;
	//~ G4Region* TripleClusterHPGeRegion = new G4Region("TripleClusterHPGeReg");
	G4Region* ClusterHPGeRegion[maxNbOfClusters][maxNbOfCrystals];
		
	G4VPhysicalVolume* Capsule_physical[maxNbOfCrystals][maxNumComp];	// The components of 1 capsule detector
	G4LogicalVolume* Capsule_logical[maxNbOfCrystals][maxNumComp];	
	G4GDMLParser parser[maxNbOfCrystals];
	
	for (G4int nbCrystal = 0; nbCrystal < NbOfCrystals[nbCluster]; nbCrystal++) {
		//separate files for each crystal capsule, because sensitive detectors won't work otherwise
		parser[nbCrystal].Read("gdml/SeptupleCluster/Capsule"+G4UIcommand::ConvertToString(nbCrystal)+".gdml", false);		// shema validation true/false via network url in gdml file
		SeptupleClusterComp_logical[nbCrystal] = parser[nbCrystal].GetWorldVolume()->GetLogicalVolume();
		//SeptupleClusterComp_logical[nbCluster]->SetVisAttributes(BGOVisAtt);
		SeptupleClusterComp_logical[nbCrystal]->SetName("Capsule"+G4UIcommand::ConvertToString(nbCrystal)+nameCluster[nbCluster]);
		SeptupleClusterComp_logical[nbCrystal]->SetVisAttributes(aluVisAtt);
		//G4cerr << nbCrystal << " " << SeptupleClusterComp_logical[nbCrystal]->GetNoDaughters() << G4endl;
		//ClusterHPGeRegion[nbCluster][nbCrystal] = new G4Region("ClusterHPGeReg_" + nameCluster[nbCluster] + G4UIcommand::ConvertToString(nbCrystal));
		G4int middleCapsule = 6;
		G4RotationMatrix rotCapsule;
		if (nbCrystal == middleCapsule)
			rotCapsule.rotateY(0.*deg);
		else
			rotCapsule.rotateY(12.*deg);			
		rotCapsule.rotateZ((nbCrystal+6)*60.*deg-90.*deg);
		
		G4RotationMatrix rotCapsuleZ;
		rotCapsuleZ.rotateZ((nbCrystal+6)*60.*deg-90.*deg);

		G4double hHeightCapsule = 31.4*mm; //adjusted with the chinese gdml
		G4Transform3D positionCapsule;
		G4double zCorrection = 2.39*mm; //middle capsule needs to be moved back
		G4double offsetHousing = -49.4*mm;
		G4ThreeVector vectorFirstRot(-hHeightCapsule * rotCapsuleZ.xx(), -hHeightCapsule * rotCapsuleZ.yx(), -0.5*hHeightCapsule*rotCapsuleZ.zx());
		G4ThreeVector vectorSecondRot(-hHeightCapsule * rotCapsule.xx(), -hHeightCapsule * rotCapsule.yx(), -0.5*hHeightCapsule * rotCapsule.zx());
		G4ThreeVector vectorCapsule;
		vectorCapsule = vectorFirstRot + vectorSecondRot + G4ThreeVector(0,0,-offsetHousing);
		if (nbCrystal == middleCapsule)
			positionCapsule = G4Transform3D(rotCapsule, G4ThreeVector(0*mm, 0*mm, -offsetHousing-0.5*zCorrection));
		else
			positionCapsule = G4Transform3D(rotCapsule, vectorCapsule);
		SeptupleClusterComp_physical[nbCrystal] = new G4PVPlacement(positionCapsule,// position in mother frame
											SeptupleClusterComp_logical[nbCrystal],		// its logical volume is the world volume of gdml file			  
											"Capsule"+G4UIcommand::ConvertToString(nbCrystal)+"_phys",					// its name
											SeptupleClusterVacuum_logical[nbCluster],							// its mother  volume
											false,								// no boolean operations
											0,									// copy number
											pSurfChk);							//surface check
	
		
		//G4int k = 0;
	}
		
	for (G4int nbCrystal = 0; nbCrystal < NbOfCrystals[nbCluster]; nbCrystal++) {
		NumDaughters = SeptupleClusterComp_logical[nbCrystal]->GetNoDaughters();
		//G4cerr << nbCrystal << " " << nbCluster << " " << NumDaughters << G4endl;
		for (G4int nbDaughter=0; nbDaughter < NumDaughters; nbDaughter++)  
		{
			G4String CompName = SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->GetName();	
			//G4cerr << CompName << G4endl;	
			if(CompName.contains("Crystal"))
			{
				SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->SetName("Crystal"+ G4UIcommand::ConvertToString(nbCrystal)+nameCluster[nbCluster]);
				SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->GetLogicalVolume()->SetName("Crystal"+ G4UIcommand::ConvertToString(nbCrystal)+nameCluster[nbCluster]+"_log");
				//create dead layers
				CrystalwDeadLayersGeometry(nbCluster, nbCrystal, SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter), dDeadLayerCap[nbCluster][nbCrystal], hDeadLayerCone[nbCluster][nbCrystal]);
			} else if (CompName.contains("PreAmp"))
			{
				SeptupleClusterComp_logical[nbCrystal]->GetDaughter(nbDaughter)->GetLogicalVolume()->SetVisAttributes(ContactLayerVisAtt);				
			}
		}		
	}
}*/

void myDetectorConstruction::SeptupleClusterBGOGeometry(G4int nbCluster, G4Transform3D positionSeptupleCluster)
{
	//BGO geometry based on CAD by Simon
	G4ThreeVector vector = positionSeptupleCluster.getTranslation();
	G4RotationMatrix matrix;// = positionSeptupleCluster.getRotation();	
	matrix.rotateZ(30.*deg);
	matrix.rotateX(ClusterAngle[nbCluster]);
	vector.setR(vector.getR()+254.*mm-119.*mm);//+36.7*mm);
	G4Transform3D position = G4Transform3D(matrix, vector);
    G4GDMLParser parser;
    parser.Read("gdml/SeptupleClusterBGO/BGOhousing.gdml", false);
	SeptupleClusterBGO_logical[nbCluster] = parser.GetWorldVolume()->GetLogicalVolume();
	SeptupleClusterBGO_logical[nbCluster]->SetVisAttributes(aluVisAtt);
	SeptupleClusterBGO_phys[nbCluster] = new G4PVPlacement(position,					// position in mother frame
										SeptupleClusterBGO_logical[nbCluster],		// its logical volume is the world volume of gdml file			  
										"BGOShielding"+nameCluster[nbCluster],					// its name
										logicWorld,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	
	G4int NumDaughters = SeptupleClusterBGO_logical[nbCluster]->GetNoDaughters();
	G4VPhysicalVolume* BGOComp_phys[5];  
	for (G4int i=0; i < NumDaughters; i++)  
	{
		BGOComp_phys[i] = SeptupleClusterBGO_logical[nbCluster]->GetDaughter(i);
		G4String CompName = BGOComp_phys[i]->GetName();
		G4cout<< CompName << G4endl;
		if(CompName.contains("Aluminum"))
		{
			BGOComp_phys[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt); 
		} else if (CompName.contains("BGO"))
		{
			BGOCrystal_logic[nbCluster] = BGOComp_phys[i]->GetLogicalVolume();
			BGOCrystal_logic[nbCluster]->SetVisAttributes(BGOVisAtt);
			BGOCrystal_logic[nbCluster]->SetName(CompName+nameCluster[nbCluster]);
		}
	}		  	
}

void myDetectorConstruction::MonolithicShieldingGeometry(G4Transform3D positionShielding)
{
	const G4int NumComp  = 4;						// Number of components
	pSurfChk = true;
	//G4Region* ClusterHPGeRegion[NbOfCrystals[nbCluster]];

    G4GDMLParser parser;
    parser.Read("gdml/Shielding90/90_shielding_kolli.gdml", false);		// shema validation true/false via network url in gdml file
	
	shieldingCone_log = parser.GetWorldVolume()->GetLogicalVolume();
	shieldingCone_log->SetVisAttributes(ContactLayerVisAtt);
	
	shieldingCone_phys = new G4PVPlacement(positionShielding,					// position in mother frame
										shieldingCone_log,		// its logical volume is the world volume of gdml file			  
										"monolithicShielding",					// its name
										logicWorld,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	
	G4int NumDaughters = shieldingCone_log->GetNoDaughters();
	if(NumDaughters > NumComp)
	{
		G4cout<< "\n--->Number of detector components in gdml file: " << shieldingCone_log->GetNoDaughters() << 
		" larger then assumed component number: " << NumComp << G4endl;
		G4cout<< "\n Tripple Cluster not properly implemented!"<< G4endl;
		return;
	}
	
	G4VPhysicalVolume* shieldingComp_phys[NumComp];
	
	for (G4int i=0; i < NumDaughters; i++)  
	{
		shieldingComp_phys[i] = shieldingCone_log->GetDaughter(i);
		G4String CompName = shieldingComp_phys[i]->GetName();
		G4cout<< CompName << G4endl;
		if(CompName.contains("Aluminum"))
		{
			shieldingComp_phys[i]->GetLogicalVolume()->SetVisAttributes(aluVisAtt); 
		} else if (CompName.contains("BGO"))
		{
			logicHPGe90BGO = shieldingComp_phys[i]->GetLogicalVolume();
			logicHPGe90BGO->SetVisAttributes(BGOVisAtt);
		} else if (CompName.contains("lead"))
		{
			shieldingComp_phys[i]->GetLogicalVolume()->SetVisAttributes(pbVisAtt);
		}
	}	
}	
	
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myDetectorConstruction::DestroyMaterials()
{
  size_t i;
  G4MaterialTable* matTable = (G4MaterialTable*)G4Material::GetMaterialTable();
  for(i=0;i<matTable->size();i++)
    { delete (*(matTable))[i]; }
  matTable->clear();
  G4ElementTable* elemTable = (G4ElementTable*)G4Element::GetElementTable();
  for(i=0;i<elemTable->size();i++)
    { delete (*(elemTable))[i]; }
  elemTable->clear();
}


void myDetectorConstruction::HPGe60Geometry(G4int nbCluster, G4Transform3D positionHPGe60, G4bool Shielding)
{
// -------------- HPGe 60 Detector -------------------

	G4double distTargetHPGe60front = 100. * mm;				// Distance of HPGe60 Detector endcap to Target nominal 145.*mm
	
	G4double hHPGe60CasingLength =  50. * mm;			// Half height of HPGe60 Casing
	G4double hPGe60CasingRadius = 44.5 * mm;			// Radius of HPGe60 Casing
	G4double hPGe60CasingThickness =  1.5 * mm;			// Thickness of HPGe60 Casing
	G4double distCrystalWindow = 5. * mm;				// Distance of Crystal to Endcap-Window nominal 5.*mm
	G4double hHPGe60GeLength = 29.5 * mm;					// Half height of HPGe60 Crystal nominal 29.5*mm
	G4double hPGe60GeRadius = 35. * mm;				// Radius of HPGe60 Crystal nominal 35*mm
	G4double hHPGe60HoleLength = 24. * mm;				// Half height of HPGe60 Hole nominal 24.*mm
	G4double hPGe60HoleRadius = 5. * mm;				// Radius of HPGe60 Hole
	G4double hHPGe60HolderLength = 35.8 * mm;			// Half height of Crystal Holder
	G4double hPGe60HolderInnerRadius = hPGe60GeRadius + hPGe60OuterCtLayerThickness + 0.5 * mm; // Inner radius of Holder
	G4double hPGe60HolderThickness = 0.8 * mm;			// Thickness of Crystal Holder
	G4double hHPGe60HolderRingLength = 4.3 * mm;		// Half height of Ring surrounding Holder
	G4double hPGe60HolderRingThickness = 2.7 * mm;		// Thickness of Holder Ring 
	//~ G4double rotAngleY = 0 * deg;						// offset rotation Y-axis
	
	//~ G4cout<< "\n--->Changed geometry for HPGe60:"<< G4endl;
	//~ G4cout<<   "    distance Target-HPGe60: "<< G4BestUnit(distTargetHPGe60a,"Length") << " nominal 14.5cm"<< G4endl;
	//~ G4cout<<   "    distance Crystal-Window:  "<< G4BestUnit(distCrystalWindow,"Length") << " nominal 5 mm"<< G4endl;
	//~ G4cout<<   "    Length Ge-crystal: "<< G4BestUnit(2 * hHPGe60GeLength,"Length") << " nominal 5.9 cm"<< G4endl;
	//~ G4cout<<   "    Length crystal hole: "<< G4BestUnit(2 * hHPGe60HoleLength,"Length") << " nominal 4.8 cm"<< G4endl;
	//~ G4cout<<   "    Radius Ge-crystal: "<< G4BestUnit(hPGe60GeRadius,"Length") << " nominal 3.5 cm"<< G4endl;
	//~ G4cout<<   "    Thickness outer contact-layer: "<< G4BestUnit(hPGe60OuterCtLayerThickness,"Length") << " nominal 0.5 mm"<< G4endl;
	
	// Casing
	
	G4Tubs* solidHPGe60Casing = new G4Tubs("HPGe60Casing",	// Name
							0.,									// Inner radius
							hPGe60CasingRadius,					// Outer radius
							hHPGe60CasingLength,				// Half length in z 
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians	

	
	G4LogicalVolume* logicHPGe60Casing = new G4LogicalVolume(solidHPGe60Casing, Al, "HPGe60Casing_log", 0,0,0);
	physicHPGe60Casing = new G4PVPlacement(positionHPGe60,					// position in mother frame
										logicHPGe60Casing,					// its logical volume				  
										"HPGe60Casing",					// its name
										logicWorld,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	
	logicHPGe60Casing ->SetVisAttributes(aluVisAtt);
								
	// Vacuum
	G4ThreeVector positionHPGe60Vac = G4ThreeVector(0.,0.,0.);
	
	G4Tubs* solidHPGe60Vac = new G4Tubs("HPGe60Vacuum",				// Name
							0.,											// Inner radius
							hPGe60CasingRadius - hPGe60CasingThickness,	// Outer radius
							hHPGe60CasingLength -hPGe60CasingThickness,	// Half length in z 
							0.,											// Starting phi angle in radians
							twopi);									// Angle of the segment in radians		
	G4LogicalVolume* logicHPGe60Vac = new G4LogicalVolume(solidHPGe60Vac, Vacuum, "HPGe60Vacuum", 0,0,0);
	physicHPGe60Vac = new G4PVPlacement(0, 						// rotation of daughter volume
										positionHPGe60Vac,		// position in mother frame
										logicHPGe60Vac,			// its logical volume				  
										"HPGe60Vacuum",		// its name
										logicHPGe60Casing,		// its mother  volume
										false,					// no boolean operations
										0,						// copy number
										pSurfChk);				//surface check

	logicHPGe60Vac ->SetVisAttributes(vacVisAtt);
								
	// Ge Crystal
	
	G4ThreeVector positionHPGe60GeCrystal = G4ThreeVector(0.,0., hPGe60CasingThickness 
		+ distCrystalWindow + 1.3 * mm + hHPGe60GeLength - hHPGe60CasingLength);
	G4ThreeVector positionHPGe60Hole = 	G4ThreeVector(0.,0., hHPGe60GeLength - hHPGe60HoleLength);

	G4Tubs* hPGe60GeCylinder = new G4Tubs("HPGe60GeCylinder",	// Name
							0.,									// Inner radius
							hPGe60GeRadius,						// Outer radius
							hHPGe60GeLength,					// Half length in z 
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians		

	G4Tubs* hPGe60Hole = new G4Tubs("HPGe60Hole",	// Name
							0.,						// Inner radius
							hPGe60HoleRadius,		// Outer radius
							hHPGe60HoleLength,		// Half length in z 
							0.,						// Starting phi angle in radians
							twopi);					// Angle of the segment in radians
							//~ 
	G4SubtractionSolid* solidHPGe60GeCrystal = new G4SubtractionSolid("HPGe60Crystal",	// Name
							hPGe60GeCylinder,			// Minuend Volume
							hPGe60Hole,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							positionHPGe60Hole);		// position of Subtrahend relativ to Minuend

	ActiveCrystal_logical[nbCluster][0] = new G4LogicalVolume(solidHPGe60GeCrystal, Ge, "HPGe60_Crystal", 0,0,0);
	physicHPGe60GeCrystal = new G4PVPlacement(0, 		// rotation of daughter volume
							positionHPGe60GeCrystal,	// position in mother frame
							ActiveCrystal_logical[nbCluster][0],		// its logical volume				  
							"ActiveCrystal_logic"+nameCluster[nbCluster],			// its name
							logicHPGe60Vac,				// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check

	ActiveCrystal_logical[nbCluster][0]->SetVisAttributes(geVisAtt);
							
	//HPGe60 Contactlayers
	G4Tubs* solidHPGe60InnerCtLayer = new G4Tubs("HPGe60InnerContactlayer",
							hPGe60HoleRadius,				// Inner radius
							hPGe60HoleRadius + hPGe60InnerCtLayerThickness,	// Outer radius
							hHPGe60HoleLength,				// Half length in z 
							0.,								// Starting phi angle in radians
							twopi);						// Angle of the segment in radians	

	G4LogicalVolume* logicHPGe60InnerCtLayer = new G4LogicalVolume(solidHPGe60InnerCtLayer,Ge,
					     "HPGe60InnerContactlayer",0,0,0);
	physicHPGe60InnerCtLayer = new G4PVPlacement(0, 	// rotation of daughter volume
							positionHPGe60Hole,			// position in mother frame
							logicHPGe60InnerCtLayer,	// its logical volume				  
							"HPGe60innercontact",			// its name
							ActiveCrystal_logical[nbCluster][0],		// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check

	G4Tubs* solidCtTub = new G4Tubs("HPGe60OuterContactlayerTubus",
							hPGe60GeRadius,				// Inner radius
							hPGe60GeRadius + hPGe60OuterCtLayerThickness,	// Outer radius
							hHPGe60GeLength + hPGe60OuterCtLayerThickness,	// Half length in z 
							0.,								// Starting phi angle in radians
							twopi);						// Angle of the segment in radians	

	G4Tubs* solidCtplate = new G4Tubs("HPGe60OuterContactlayerPlate",
							0,				// Inner radius
							hPGe60GeRadius + hPGe60OuterCtLayerThickness,	// Outer radius
							hPGe60OuterCtLayerThickness / 2.,				// Half length in z 
							0.,								// Starting phi angle in radians
							twopi);						// Angle of the segment in radians	

	G4UnionSolid* solidHPGe60OuterCtLayer = new G4UnionSolid("HPGe60OuterContactlayer",	// Name
							solidCtTub,				// Summand Volume 1
							solidCtplate,				// Summand Volume 2
							0, 	// rotation of Summand 2
							G4ThreeVector(0,0,-1.*hHPGe60GeLength - hPGe60OuterCtLayerThickness / 2.));// position of Summand 2 relativ to Summand 1

	G4LogicalVolume* logicHPGe60OuterCtLayer = new G4LogicalVolume(solidHPGe60OuterCtLayer,Ge,
					     "HPGe60OuterContactlayer",0,0,0);
	physicHPGe60OuterCtLayer = new G4PVPlacement(0, 	// rotation of daughter volume
							positionHPGe60GeCrystal,	// position in mother frame
							logicHPGe60OuterCtLayer,	// its logical volume				  
							"HPGe60outercontact",			// its name
							logicHPGe60Vac,				// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check
							
	logicHPGe60InnerCtLayer->SetVisAttributes(ContactLayerVisAtt);
	logicHPGe60OuterCtLayer->SetVisAttributes(ContactLayerVisAtt);


	// Crystal Holder
	G4ThreeVector positionHPGe60Holder = G4ThreeVector(0.,0., hPGe60CasingThickness 
		+ distCrystalWindow + hHPGe60HolderLength - hHPGe60CasingLength);
	G4ThreeVector positionHPGe60HolderHole = G4ThreeVector(0.,0., -3. * mm);
	G4ThreeVector positionHPGe60HolderRing1 = G4ThreeVector(0.,0., 14.  * mm + hHPGe60HolderRingLength - hHPGe60HolderLength);
	G4ThreeVector positionHPGe60HolderRing2 = G4ThreeVector(0.,0., 42. * mm + hHPGe60HolderRingLength - hHPGe60HolderLength);
	
	G4Tubs* hPGe60GHolderCylinder = new G4Tubs("HPGe60HolderCylinder",	// Name
							0.,									// Inner radius
							hPGe60HolderInnerRadius + hPGe60HolderThickness,	// Outer radius
							hHPGe60HolderLength,					// Half length in z 
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians		
							
	G4Tubs* hPGe60GHolderRing = new G4Tubs("HPGe60HolderRing",	// Name
							0.,									// Inner radius
							hPGe60HolderInnerRadius + hPGe60HolderRingThickness,	// Outer radius
							hHPGe60HolderRingLength,			// Half length in z 
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians		

							
	G4UnionSolid* hPGe60GHolder1Ring = new G4UnionSolid("HPGe60Holder+Ring",				// Name
										hPGe60GHolderCylinder,				// Summand Volume 1
										hPGe60GHolderRing,							// Summand Volume 2
										0, 								// rotation of Summand 2
										positionHPGe60HolderRing1);				// position of Summand 2 relativ to Summand 1

	G4UnionSolid* hPGe60GHolder2Rings = new G4UnionSolid("HPGe60Holder+2Rings",				// Name
										hPGe60GHolder1Ring,				// Summand Volume 1
										hPGe60GHolderRing,							// Summand Volume 2
										0, 								// rotation of Summand 2
										positionHPGe60HolderRing2);				// position of Summand 2 relativ to Summand 1
	
	G4Tubs* hPGe60HolderHole = new G4Tubs("HPGe60HolderHole",	// Name
							0.,						// Inner radius
							hPGe60HolderInnerRadius,		// Outer radius
							hHPGe60HolderLength,		// Half length in z 
							0.,						// Starting phi angle in radians
							twopi);				// Angle of the segment in radians
							
	G4SubtractionSolid* solidHPGe60Holder = new G4SubtractionSolid("HPGe60Holder",	// Name
							hPGe60GHolder2Rings,			// Minuend Volume
							hPGe60HolderHole,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							positionHPGe60HolderHole);		// position of Subtrahend relativ to Minuend

	G4LogicalVolume* logicHPGe60Holder = new G4LogicalVolume(solidHPGe60Holder, Al, "HPGe60Holder", 0,0,0);
	physicHPGe60Holder = new G4PVPlacement(0, 			// rotation of daughter volume
							positionHPGe60Holder,		// position in mother frame
							logicHPGe60Holder,			// its logical volume
							"HPGe60Holder",			// its name
							logicHPGe60Vac,				// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check

	logicHPGe60Casing ->SetVisAttributes(aluVisAtt);

	logicHPGe60Holder ->SetVisAttributes(aluVisAtt);

	//G4Transform3D positionShielding;
	//G4RotationMatrix ShieldingRotMatrix;					// Rotationmatrix for detector
	//ShieldingRotMatrix.rotateX(180.*deg);
	//lengthCollimator = 70.*mm;
	//clusterVolLength = (760. + lengthCollimator) *mm/2.; 			// 760=coldfinger+detector, 70 = collimator
	//G4double distFrontCapCenter = 352.5*mm;
	//G4double distCapBGOfront = 54.*mm;				// only needed if BGO front should be at z=0
	//G4double distCapLeadfront = 200*mm;
	//G4double distCapSource = _distCapSource;			// distance of detctor frontcap to source of particles
	//positionShielding = G4Transform3D(ShieldingRotMatrix,G4ThreeVector(50*mm, 50*mm, -3*(distFrontCapCenter)));
	if (Shielding == true)
		MonolithicShieldingGeometry(positionHPGe60);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void myDetectorConstruction::HPGe90Geometry(G4Transform3D positionHPGe90)
{
// -------------- HPGe 90 Detector ------------------- by Louis

	G4double distTargetHPGe90front = 100. * mm;				// Distance of HPGe90 Detector endcap to Target nominal 145.*mm
	
	G4double hHPGe90CasingLength =  191./2. * mm;			// Half height of HPGe90 Casing
	G4double hPGe90CasingRadius = 44.5 * mm;			// Radius of HPGe90 Casing
	G4double hPGe90CasingThickness =  0.76 * mm;			// Thickness of HPGe90 Casing
	G4double distCrystalWindow = 4. * mm;				// Distance of Crystal to Endcap-Window nominal 5.*mm
	G4double hHPGe90GeLength = 77.6/2. * mm;					// Half height of HPGe90 Crystal nominal 29.5*mm
	G4double hPGe90GeRadius = 79.9/2. * mm;				// Radius of HPGe90 Crystal nominal 35*mm
	G4double hPGe90OuterCtLayerThickness = 0.7 * mm; 	// Outer Contactlayer Thickness of HPGe90 (Deadlayer)
	G4double hPGe90InnerCtLayerThickness = 0.0007 * mm;	// Inner Contactlayer Thickness of HPGe90 (Deadlayer)
	G4double hHPGe90HoleLength = 24. * mm;				// Half height of HPGe90 Hole nominal 24.*mm
	G4double hPGe90HoleRadius = 5. * mm;				// Radius of HPGe90 Hole
	//~ G4double hHPGe90HolderLength = 35.8 * mm;			// Half height of Crystal Holder
	//~ G4double hPGe90HolderInnerRadius = hPGe90GeRadius + hPGe90OuterCtLayerThickness + 0.5 * mm; // Inner radius of Holder
	//~ G4double hPGe90HolderThickness = 0.8 * mm;			// Thickness of Crystal Holder
	//~ G4double hHPGe90HolderRingLength = 4.3 * mm;		// Half height of Ring surrounding Holder
	//~ G4double hPGe90HolderRingThickness = 2.7 * mm;		// Thickness of Holder Ring 
	//~ G4double rotAngleY = 0 * deg;						// offset rotation Y-axis
	
	//~ G4cout<< "\n--->Changed geometry for HPGe90:"<< G4endl;
	//~ G4cout<<   "    distance Target-HPGe90: "<< G4BestUnit(distTargetHPGe90a,"Length") << " nominal 14.5cm"<< G4endl;
	//~ G4cout<<   "    distance Crystal-Window:  "<< G4BestUnit(distCrystalWindow,"Length") << " nominal 5 mm"<< G4endl;
	//~ G4cout<<   "    Length Ge-crystal: "<< G4BestUnit(2 * hHPGe90GeLength,"Length") << " nominal 5.9 cm"<< G4endl;
	//~ G4cout<<   "    Length crystal hole: "<< G4BestUnit(2 * hHPGe90HoleLength,"Length") << " nominal 4.8 cm"<< G4endl;
	//~ G4cout<<   "    Radius Ge-crystal: "<< G4BestUnit(hPGe90GeRadius,"Length") << " nominal 3.5 cm"<< G4endl;
	//~ G4cout<<   "    Thickness outer contact-layer: "<< G4BestUnit(hPGe90OuterCtLayerThickness,"Length") << " nominal 0.5 mm"<< G4endl;

	// Casing
	
	G4Tubs* solidHPGe90Casing = new G4Tubs("HPGe90Casing",	// Name
							0.,									// Inner radius
							hPGe90CasingRadius,					// Outer radius
							hHPGe90CasingLength,				// Half length in z 
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians	

	
	G4LogicalVolume* logicHPGe90Casing = new G4LogicalVolume(solidHPGe90Casing, C_mat, "HPGe90Casing_log", 0,0,0);
	physicHPGe90Casing = new G4PVPlacement(positionHPGe90,					// position in mother frame
										logicHPGe90Casing,					// its logical volume				  
										"HPGe90Casing",					// its name
										shieldingCone_log,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	
	logicHPGe90Casing ->SetVisAttributes(aluVisAtt);
								
	// Vacuum
	G4ThreeVector positionHPGe90Vac = G4ThreeVector(0.,0.,0.);
	
	G4Tubs* solidHPGe90Vac = new G4Tubs("HPGe90Vacuum",				// Name
							0.,											// Inner radius
							hPGe90CasingRadius - hPGe90CasingThickness,	// Outer radius
							hHPGe90CasingLength -hPGe90CasingThickness,	// Half length in z 
							0.,											// Starting phi angle in radians
							twopi);									// Angle of the segment in radians		
	G4LogicalVolume* logicHPGe90Vac = new G4LogicalVolume(solidHPGe90Vac, Vacuum, "HPGe90Vacuum", 0,0,0);
	physicHPGe90Vac = new G4PVPlacement(0, 						// rotation of daughter volume
										positionHPGe90Vac,		// position in mother frame
										logicHPGe90Vac,			// its logical volume				  
										"HPGe90Vacuum",		// its name
										logicHPGe90Casing,		// its mother  volume
										false,					// no boolean operations
										0,						// copy number
										pSurfChk);				//surface check

	logicHPGe90Vac ->SetVisAttributes(vacVisAtt);
								
	// Ge Crystal
	
	G4ThreeVector positionHPGe90GeCrystal = G4ThreeVector(0.,0., hPGe90CasingThickness 
		+ distCrystalWindow + 1.3 * mm + hHPGe90GeLength - hHPGe90CasingLength);
	G4ThreeVector positionHPGe90Hole = 	G4ThreeVector(0.,0., hHPGe90GeLength - hHPGe90HoleLength);

	G4Tubs* hPGe90GeCylinder = new G4Tubs("HPGe90GeCylinder",	// Name
							0.,									// Inner radius
							hPGe90GeRadius,						// Outer radius
							hHPGe90GeLength,					// Half length in z 
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians		

	G4Tubs* hPGe90Hole = new G4Tubs("HPGe90Hole",	// Name
							0.,						// Inner radius
							hPGe90HoleRadius,		// Outer radius
							hHPGe90HoleLength,		// Half length in z 
							0.,						// Starting phi angle in radians
							twopi);					// Angle of the segment in radians
							//~ 
	G4SubtractionSolid* solidHPGe90GeCrystal = new G4SubtractionSolid("HPGe90Crystal",	// Name
							hPGe90GeCylinder,			// Minuend Volume
							hPGe90Hole,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							positionHPGe90Hole);		// position of Subtrahend relativ to Minuend

	logicHPGe90GeCrystal = new G4LogicalVolume(solidHPGe90GeCrystal, Ge, "HPGe90_Crystal", 0,0,0);
	physicHPGe90GeCrystal = new G4PVPlacement(0, 		// rotation of daughter volume
							positionHPGe90GeCrystal,	// position in mother frame
							logicHPGe90GeCrystal,		// its logical volume				  
							"HPGe90_Crystal",			// its name
							logicHPGe90Vac,				// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check

	logicHPGe90GeCrystal	->SetVisAttributes(geVisAtt);
							
	//HPGe90 Contactlayers
	G4Tubs* solidHPGe90InnerCtLayer = new G4Tubs("HPGe90InnerContactlayer",
							hPGe90HoleRadius,				// Inner radius
							hPGe90HoleRadius + hPGe90InnerCtLayerThickness,	// Outer radius
							hHPGe90HoleLength,				// Half length in z 
							0.,								// Starting phi angle in radians
							twopi);						// Angle of the segment in radians	

	G4LogicalVolume* logicHPGe90InnerCtLayer = new G4LogicalVolume(solidHPGe90InnerCtLayer,Ge,
					     "HPGe90InnerContactlayer",0,0,0);
	physicHPGe90InnerCtLayer = new G4PVPlacement(0, 	// rotation of daughter volume
							positionHPGe90Hole,			// position in mother frame
							logicHPGe90InnerCtLayer,	// its logical volume				  
							"HPGe90innercontact",			// its name
							logicHPGe90GeCrystal,		// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check

	G4Tubs* solidCtTub = new G4Tubs("HPGe90OuterContactlayerTubus",
							hPGe90GeRadius,				// Inner radius
							hPGe90GeRadius + hPGe90OuterCtLayerThickness,	// Outer radius
							hHPGe90GeLength + hPGe90OuterCtLayerThickness,	// Half length in z 
							0.,								// Starting phi angle in radians
							twopi);						// Angle of the segment in radians	

	G4Tubs* solidCtplate = new G4Tubs("HPGe90OuterContactlayerPlate",
							0,				// Inner radius
							hPGe90GeRadius + hPGe90OuterCtLayerThickness,	// Outer radius
							hPGe90OuterCtLayerThickness / 2.,				// Half length in z 
							0.,								// Starting phi angle in radians
							twopi);						// Angle of the segment in radians	

	G4UnionSolid* solidHPGe90OuterCtLayer = new G4UnionSolid("HPGe90OuterContactlayer",	// Name
							solidCtTub,				// Summand Volume 1
							solidCtplate,				// Summand Volume 2
							0, 	// rotation of Summand 2
							G4ThreeVector(0,0,-1.*hHPGe90GeLength - hPGe90OuterCtLayerThickness / 2.));// position of Summand 2 relativ to Summand 1

	G4LogicalVolume* logicHPGe90OuterCtLayer = new G4LogicalVolume(solidHPGe90OuterCtLayer,Ge,
					     "HPGe90OuterContactlayer",0,0,0);
	physicHPGe90OuterCtLayer = new G4PVPlacement(0, 	// rotation of daughter volume
							positionHPGe90GeCrystal,	// position in mother frame
							logicHPGe90OuterCtLayer,	// its logical volume				  
							"HPGe90outercontact",			// its name
							logicHPGe90Vac,				// its mother  volume
							false,						// no boolean operations
							0,							// copy number
							pSurfChk);					//surface check
							
	logicHPGe90InnerCtLayer->SetVisAttributes(ContactLayerVisAtt);
	logicHPGe90OuterCtLayer->SetVisAttributes(ContactLayerVisAtt);


	// Crystal Holder
	//~ G4ThreeVector positionHPGe90Holder = G4ThreeVector(0.,0., hPGe90CasingThickness 
		//~ + distCrystalWindow + hHPGe90HolderLength - hHPGe90CasingLength);
	//~ G4ThreeVector positionHPGe90HolderHole = G4ThreeVector(0.,0., -3. * mm);
	//~ G4ThreeVector positionHPGe90HolderRing1 = G4ThreeVector(0.,0., 14.  * mm + hHPGe90HolderRingLength - hHPGe90HolderLength);
	//~ G4ThreeVector positionHPGe90HolderRing2 = G4ThreeVector(0.,0., 42. * mm + hHPGe90HolderRingLength - hHPGe90HolderLength);
	//~ 
	//~ G4Tubs* hPGe90GHolderCylinder = new G4Tubs("HPGe90HolderCylinder",	// Name
							//~ 0.,									// Inner radius
							//~ hPGe90HolderInnerRadius + hPGe90HolderThickness,	// Outer radius
							//~ hHPGe90HolderLength,					// Half length in z 
							//~ 0.,									// Starting phi angle in radians
							//~ twopi);							// Angle of the segment in radians		
							//~ 
	//~ G4Tubs* hPGe90GHolderRing = new G4Tubs("HPGe90HolderRing",	// Name
							//~ 0.,									// Inner radius
							//~ hPGe90HolderInnerRadius + hPGe90HolderRingThickness,	// Outer radius
							//~ hHPGe90HolderRingLength,			// Half length in z 
							//~ 0.,									// Starting phi angle in radians
							//~ twopi);							// Angle of the segment in radians		
//~ 
							//~ 
	//~ G4UnionSolid* hPGe90GHolder1Ring = new G4UnionSolid("HPGe90Holder+Ring",				// Name
										//~ hPGe90GHolderCylinder,				// Summand Volume 1
										//~ hPGe90GHolderRing,							// Summand Volume 2
										//~ 0, 								// rotation of Summand 2
										//~ positionHPGe90HolderRing1);				// position of Summand 2 relativ to Summand 1
//~ 
	//~ G4UnionSolid* hPGe90GHolder2Rings = new G4UnionSolid("HPGe90Holder+2Rings",				// Name
										//~ hPGe90GHolder1Ring,				// Summand Volume 1
										//~ hPGe90GHolderRing,							// Summand Volume 2
										//~ 0, 								// rotation of Summand 2
										//~ positionHPGe90HolderRing2);				// position of Summand 2 relativ to Summand 1
	//~ 
	//~ G4Tubs* hPGe90HolderHole = new G4Tubs("HPGe90HolderHole",	// Name
							//~ 0.,						// Inner radius
							//~ hPGe90HolderInnerRadius,		// Outer radius
							//~ hHPGe90HolderLength,		// Half length in z 
							//~ 0.,						// Starting phi angle in radians
							//~ twopi);				// Angle of the segment in radians
							//~ 
	//~ G4SubtractionSolid* solidHPGe90Holder = new G4SubtractionSolid("HPGe90Holder",	// Name
							//~ hPGe90GHolder2Rings,			// Minuend Volume
							//~ hPGe90HolderHole,					// Subtrahend Volume
							//~ 0, 							// rotation of Subtrahend
							//~ positionHPGe90HolderHole);		// position of Subtrahend relativ to Minuend
//~ 
	//~ G4LogicalVolume* logicHPGe90Holder = new G4LogicalVolume(solidHPGe90Holder, Al, "HPGe90Holder", 0,0,0);
	//~ physicHPGe90Holder = new G4PVPlacement(0, 			// rotation of daughter volume
							//~ positionHPGe90Holder,		// position in mother frame
							//~ logicHPGe90Holder,			// its logical volume
							//~ "HPGe90Holder",			// its name
							//~ logicHPGe90Vac,				// its mother  volume
							//~ false,						// no boolean operations
							//~ 0,							// copy number
							//~ pSurfChk);					//surface check

	//~ logicHPGe90Casing ->SetVisAttributes(holderVisAtt);
//~ 
	//~ logicHPGe90Holder ->SetVisAttributes(aluVisAtt);

	//G4Transform3D positionShielding;
	//G4RotationMatrix ShieldingRotMatrix;					// Rotationmatrix for detector
	//ShieldingRotMatrix.rotateX(180.*deg);
	//lengthCollimator = 70.*mm;
	//clusterVolLength = (790. + lengthCollimator) *mm/2.; 			// 790=coldfinger+detector, 70 = collimator
	//G4double distFrontCapCenter = 352.5*mm;
	//G4double distCapBGOfront = 54.*mm;				// only needed if BGO front should be at z=0
	//G4double distCapLeadfront = 200*mm;
	//G4double distCapSource = _distCapSource;			// distance of detctor frontcap to source of particles
	//positionShielding = G4Transform3D(ShieldingRotMatrix,G4ThreeVector(50*mm, 50*mm, -3*(distFrontCapCenter)));
}


void myDetectorConstruction::HPGe100Geometry()
{
	//--------------- HPGe 100 detector by Ralph--------------------------
	G4double distTargetHPGe100front = 43. * mm;			// Distance a of HPGe100 Detector front to Target
	G4double fHPGe100CrystalLenght = 90. * mm;			// full lenght of Ge crystal nominal 90*mm
	//~ G4double offsetZ = 0. * mm;						// Parallel deviation to Target-Detector-Axis in x-z plane
	//~ G4double offsetY = 0. * mm;						// Parallel deviation to Target-Detector-Axis in x-y plane
	G4double rotAngleY = 35.*deg;						// Y rotation of HPGe100
	G4cout<< "\n--->Changed geometry for HPGe100:"<< G4endl;
	G4cout<<   "    distance Target-HPGe100: "<< G4BestUnit(distTargetHPGe100front,"Length") << G4endl;
	G4cout<<   "    Ge crystal length: "<< G4BestUnit(fHPGe100CrystalLenght,"Length") << " nominal 9cm"<< G4endl;
	//~ G4cout<<   "    offset in xz plane B only: "<< G4BestUnit(offsetZ,"Length") << " nominal none"<< G4endl;
	//~ G4cout<<   "    offset in xy plane B only: "<< G4BestUnit(offsetY,"Length") << " nominal none"<< G4endl;
	G4cout<<   "    angle to z-axis: "<< rotAngleY / deg << G4endl;
	
	G4RotationMatrix rotSetup;
	rotSetup.rotateZ(pi*.5);
	
	G4double hSetupLength = 0.41 * m / 2.;
	G4double det1 = 0.01*m + 0.089825*m - hSetupLength;
	//Distance between target and DET
	G4double distTargetHPGe100  = -0.05895 * m - det1;
	G4cout<<   "    Dist Target Det100%%: "<< distTargetHPGe100 << G4endl;

	
	G4RotationMatrix* rotdet = new G4RotationMatrix;
	rotdet -> rotateX (-pi*1.5);
		
	//----------------------------------------------------------------
	
	//Detector 1 41-N31587A1
	
	// Hole HPGe100 setup
	G4Box* solidHPGe100Setup = new G4Box("HPGe100+BGO", 0.120*m, hSetupLength, 0.120*m);
	G4LogicalVolume* logicHPGe100Setup = new G4LogicalVolume(solidHPGe100Setup, 
										 Air, "HPGe100+BGO",0,0,0); //
	
	G4Transform3D postitionHPGe100;
		rotSetup.rotateY(rotAngleY);
		distTargetHPGe100 = distTargetHPGe100 + distTargetHPGe100front + 37.5 * mm + 70. * mm + 5 * mm;
		postitionHPGe100 = G4Transform3D(rotSetup,G4ThreeVector(-1.* std::sin(90*deg - rotAngleY) 
			* distTargetHPGe100, 0, std::cos(90*deg - rotAngleY) * distTargetHPGe100));

	physicHPGe100Setup = new G4PVPlacement(postitionHPGe100, logicHPGe100Setup,"HPGe100+BGO", logicWorld,false,0,pSurfChk);

	logicHPGe100Setup->SetVisAttributes(setupVisAtt); 
	
	//DET1BGO        
	//~ const G4double zPlaneDET1BGO[] = {0*m,		0.03*m,		0.0300001*m,0.075*m,	0.26*m,	0.268*m};
	//~ const G4double rInnerDET1BGO[] = {0.032*m,	0.039*m,	0.058*m,	0.058*m,	0.058*m,0.058*m};
	//~ const G4double rOuterDET1BGO[] = {0.0718*m,	0.07908*m,	0.07908*m,	0.09*m,		0.09*m,	0.05800001*m};
  
    const G4double BGOInnerR[] = {32 * mm, 49.5 * mm, 58. * mm};
  	const G4double BGOOuterR[] = {71.8 * mm, 90. * mm};
	const G4double BGOLength[] = {37.5 * mm, 22.5 * mm, 92.5 * mm, 4 * mm};
	
	G4ThreeVector positionTubBGO1 = G4ThreeVector(0., 0., BGOLength[0] - BGOLength[1] + 1*mm);
	G4ThreeVector positionConBGO2 = G4ThreeVector(0., 0., BGOLength[2] + BGOLength[3]);
	G4ThreeVector positionUnionTubBGO = G4ThreeVector(0., 0., BGOLength[0] + BGOLength[2]);
	G4ThreeVector positionHPGe100BGO = G4ThreeVector(0, det1 + 0.023175*m + BGOLength[0],0);

	G4Cons* conBGO1 = new G4Cons(	"BGO Con 1",		// Name
									BGOInnerR[0],	// inside radius at -pDz
									BGOOuterR[0],	// outside radius at -pDz
									BGOInnerR[1],	// inside radius at +pDz
									BGOOuterR[1],	// outside radius at +pDz
									BGOLength[0],	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians

	G4Tubs* tubBGO1 = new G4Tubs(	"BGO Tub1",		// Name
									0,							// Inner radius
									BGOInnerR[2],			// Outer radius
									BGOLength[1] + 1*mm,				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians
									

	G4SubtractionSolid* subConBGO = new G4SubtractionSolid("BGO Part 1",	// Name
							conBGO1,			// Minuend Volume
							tubBGO1,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							positionTubBGO1);		// position of Subtrahend relativ to Minuend

	G4Tubs* tubBGO2 = new G4Tubs(	"BGO Tub2",		// Name
									BGOInnerR[2],							// Inner radius
									BGOOuterR[1],			// Outer radius
									BGOLength[2],				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians

	G4Cons* conBGO2 = new G4Cons(	"BGO Con 2",		// Name
									BGOInnerR[2],	// inside radius at -pDz
									BGOOuterR[1],	// outside radius at -pDz
									BGOInnerR[2],	// inside radius at +pDz
									BGOInnerR[2] + 0.25*mm,	// outside radius at +pDz
									BGOLength[3],	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians


	G4UnionSolid* unionTubBGO = new G4UnionSolid("BGO Part 2",				// Name
										tubBGO2,				// Summand Volume 1
										conBGO2,				// Summand Volume 2
										0, 	// rotation of Summand 2
										positionConBGO2);				// position of Summand 2 relativ to Summand 1

	G4UnionSolid* solidHPGe100BGO = new G4UnionSolid("HPGe100 BGO",				// Name
										subConBGO,				// Summand Volume 1
										unionTubBGO,				// Summand Volume 2
										0, 	// rotation of Summand 2
										positionUnionTubBGO);				// position of Summand 2 relativ to Summand 1

  //~ G4Polycone* solidHPGe100BGO = new G4Polycone("DET1BGO_sol",0,twopi,6,
						 //~ zPlaneDET1BGO,rInnerDET1BGO,rOuterDET1BGO);
  DET1BGO_logical = new G4LogicalVolume(solidHPGe100BGO,BGO,
					  "DET1BGO_log",0,0,0);
  DET1BGO_physical = new G4PVPlacement(rotdet,positionHPGe100BGO,
					 DET1BGO_logical,"DET1BGO_phys",
					 logicHPGe100Setup,false,0,pSurfChk);
  DET1BGO_logical->SetVisAttributes(BGOVisAtt);					 

  
  const G4double QuartzLength = 4. * mm;
  const G4double QuartzInnerR[] = {90. * mm, 58. * mm};
  const G4double QuartzOuterR[] = {90.001 * mm, 58.001 * mm};
  
  	G4Cons* solidHPGe100Quartz = new G4Cons("HPGe100 Quartz",		// Name
									QuartzInnerR[0],	// inside radius at -pDz
									QuartzOuterR[0],	// outside radius at -pDz
									QuartzInnerR[1],	// inside radius at +pDz
									QuartzOuterR[1],	// outside radius at +pDz
									QuartzLength,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians

  //~ G4Polycone* solidHPGe100Quartz = new G4Polycone("DET1Quartz_sol",0,twopi,2,
						 //~ zPlaneDET1Quartz,rInnerDET1Quartz,rOuterDET1Quartz);
  G4LogicalVolume* DET1Quartz_logical = new G4LogicalVolume(solidHPGe100Quartz,Quartz,
					  "DET1Quartz_log",0,0,0);
  DET1Quartz_logical->SetVisAttributes(quartzVisAtt);						  
  DET1Quartz_physical = new G4PVPlacement(rotdet,G4ThreeVector(0,det1+0.283276*m + QuartzLength,0),
					 DET1Quartz_logical,"DET1Quartz_phys",
					 logicHPGe100Setup,false,0,pSurfChk);

  //DET1HPGE
  const G4double zPlaneDET1HPGE[] = {0*m,1.0/std::sqrt(2.)*0.008*m,0.008*m,0.0095*m,fHPGe100CrystalLenght};
  const G4double rInnerDET1HPGE[] = {0*m,0*m,0*m,0.00625*m,0.00625*m};
  const G4double rOuterDET1HPGE[] = {0.03155*m,0.03155*m+1.0/std::sqrt(2.)*0.008*m,0.03955*m,0.03955*m,0.03955*m};
  G4Polycone* DET1HPGE_solid = new G4Polycone("DET1HPGE_sol",0,twopi,5,
						 zPlaneDET1HPGE,rInnerDET1HPGE,rOuterDET1HPGE);
  DET1HPGE_logical = new G4LogicalVolume(DET1HPGE_solid,Ge,
				      "DET1HPGE_log",0,0,0);
  DET1HPGE_physical = new G4PVPlacement(rotdet,G4ThreeVector(0,det1+0.063475*m,0),
				     DET1HPGE_logical,"DET1HPGE_phys",logicHPGe100Setup,false,0,pSurfChk);

  DET1HPGE_logical->SetVisAttributes(geVisAtt);

  //DET1Contactlayer
  G4Tubs* DET1Contactlayer_solid = new G4Tubs("DET1Contactlayer_sol",0.00575*m,0.00625*m,
					  0.04025*m,0,twopi);
  G4LogicalVolume* DET1Contactlayer_logical = new G4LogicalVolume(DET1Contactlayer_solid,Ge,
					     "DET1Contactlayer_log",0,0,0);
  DET1Contactlayer_physical = new G4PVPlacement(rotdet,G4ThreeVector(0,det1+0.113225*m,0),
					    DET1Contactlayer_logical,"DET1Contactlayer_phys",
					    logicHPGe100Setup,false,0,pSurfChk);
  DET1Contactlayer_logical->SetVisAttributes(ContactLayerVisAtt);

  //DET1Coolingfinger
  G4Tubs* DET1Coolingfinger_solid = new G4Tubs("DET1Coolingfinger_sol",0*m,0.007*m,
					   0.0725*m,0,twopi);
  G4LogicalVolume* DET1Coolingfinger_logical = new G4LogicalVolume(DET1Coolingfinger_solid,Cu,
					      "DET1Coolingfinger_log",0,0,0);
  DET1Coolingfinger_physical = new G4PVPlacement(rotdet,G4ThreeVector(0,det1+0.225975*m,0),
					     DET1Coolingfinger_logical,"DET1Coolingfinger_phys",
					     logicHPGe100Setup,false,0,pSurfChk);
  DET1Coolingfinger_logical->SetVisAttributes(aluVisAtt);

  //DET1Alhousing
  
  	const G4double housingAInnerR[] = {23.5 * mm,	47.74 * mm};
  	const G4double housingAOuterR[] = {53. * mm,	50. * mm};
	const G4double housingALength[] = {8. * mm,		84.525 * mm};
	
	G4ThreeVector positionTubA3 = G4ThreeVector(0., 0., housingALength[1] + housingALength[0] - 1*mm);
	G4ThreeVector positionHousingA = G4ThreeVector(0, det1 + 0.05895*m + housingALength[1] + 1*mm,0);

	G4Tubs* tubA1 = new G4Tubs(	"Housing A Tub1",		// Name
									0,							// Inner radius
									housingAOuterR[1],			// Outer radius
									housingALength[1] + 1*mm,				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians
									
	G4Tubs* tubA2 = new G4Tubs(	"Housing A Tub2",		// Name
									0,							// Inner radius
									housingAInnerR[1],			// Outer radius
									housingALength[1] + 1*mm,				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians

	G4SubtractionSolid* subTubA = new G4SubtractionSolid("Housing A Part 1",	// Name
							tubA1,			// Minuend Volume
							tubA2,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							G4ThreeVector(0,0,1.05*mm));		// position of Subtrahend relativ to Minuend

	G4Tubs* tubA3 = new G4Tubs(	"Housing A Part 2",		// Name
									housingAInnerR[0],							// Inner radius
									housingAOuterR[0],			// Outer radius
									housingALength[0],				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians


	G4UnionSolid* solidHPGe100HousingA = new G4UnionSolid("solid HPGe100 Housing A",				// Name
										subTubA,				// Summand Volume 1
										tubA3,				// Summand Volume 2
										0, 	// rotation of Summand 2
										positionTubA3);				// position of Summand 2 relativ to Summand 1

	//~ const G4double zPlaneDET1Alhousinga[] = {0*m,		0.0010499*m,	0.00105*m,	0.16905*m,	0.169051*m,	0.18505*m};
	//~ const G4double rInnerDET1Alhousinga[] = {0*m,		0*m,			0.04774*m,	0.04774*m,	0.0235*m,	0.0235*m};
	//~ const G4double rOuterDET1Alhousinga[] = {0.05*m,	0.05*m,			0.05*m,		0.05*m,		0.053*m,	0.053*m};
	//~ G4Polycone* solidHPGe100HousingA = new G4Polycone("DET1Alhousinga_sol",0,twopi,6,
						 //~ zPlaneDET1Alhousinga,rInnerDET1Alhousinga,rOuterDET1Alhousinga);
						 
	
	G4LogicalVolume* DET1Alhousinga_logical = new G4LogicalVolume(solidHPGe100HousingA,Al,
					  "DET1Alhousinga_log",0,0,0);
	DET1Alhousinga_physical = new G4PVPlacement(rotdet,positionHousingA,
					 DET1Alhousinga_logical,"DET1Alhousinga_phys",
					 logicHPGe100Setup,false,0,pSurfChk);
	DET1Alhousinga_logical->SetVisAttributes(aluVisAtt);
	
	//DET1Alhousingbgo
	
	//~ const G4double zPlaneDET1Alhousingb[] = {0*m,		 0.034*m, 0.034001*m,	0.035*m, 0.035001*m,	0.272*m};
	//~ const G4double rInnerDET1Alhousingb[] = {0.0275*m, 0.036*m, 0.036*m,		0.036*m, 0.055*m,		0.055*m};
	//~ const G4double rOuterDET1Alhousingb[] = {0.0285*m, 0.037*m, 0.056*m,		0.056*m, 0.056*m,		0.056*m};
	//~ G4Polycone* DET1Alhousingb_solid = new G4Polycone("DET1Alhousingb_sol",0,twopi,6,
						 //~ zPlaneDET1Alhousingb,rInnerDET1Alhousingb,rOuterDET1Alhousingb);
  
    const G4double housingBInnerR[] = {27.5 * mm, 36. * mm, 55. * mm};
	const G4double housingBOuterR[] = {28.5 * mm, 37. * mm, 56. * mm};
	const G4double housingBLength[] = {17. * mm, 1. * mm, 118.5 * mm}; 
	
	G4ThreeVector positionSubTubB = G4ThreeVector(0., 0., housingBLength[0] + housingBLength[2] - 0.25*mm);
	G4ThreeVector positionHousingB = G4ThreeVector(0, det1 + 0.021175*m + housingBLength[0] + 0.25*mm,0);

	G4Cons* conB = new G4Cons(	"Housing B Part 1",		// Name
									housingBInnerR[0],	// inside radius at -pDz
									housingBOuterR[0],	// outside radius at -pDz
									housingBInnerR[1] + 0.5*mm,	// inside radius at +pDz
									housingBOuterR[1],	// outside radius at +pDz
									housingBLength[0] + 0.25*mm,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians
									
	G4Tubs* tubB1 = new G4Tubs(	"Housing B Tub1",		// Name
									housingBInnerR[1],							// Inner radius
									housingBOuterR[2],			// Outer radius
									housingBLength[2],				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians

	G4Tubs* tubB2= new G4Tubs(	"Housing B Tub2",		// Name
									0,							// Inner radius
									housingBInnerR[2],			// Outer radius
									housingBLength[2],				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians

	G4SubtractionSolid* subTubB = new G4SubtractionSolid("Housing B Part 2",	// Name
							tubB1,			// Minuend Volume
							tubB2,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							G4ThreeVector(0,0,housingBLength[1]));		// position of Subtrahend relativ to Minuend



	G4UnionSolid* solidHPGe100HousingB = new G4UnionSolid("solid HPGe100 Housing B",				// Name
										conB,				// Summand Volume 1
										subTubB,				// Summand Volume 2
										0, 	// rotation of Summand 2
										positionSubTubB);				// position of Summand 2 relativ to Summand 1

	G4LogicalVolume* DET1Alhousingb_logical = new G4LogicalVolume(solidHPGe100HousingB,Al,
					  "DET1Alhousingb_log",0,0,0);
	DET1Alhousingb_physical = new G4PVPlacement(rotdet,positionHousingB,
					 DET1Alhousingb_logical,"DET1Alhousingb_phys",
					 logicHPGe100Setup,false,0,pSurfChk);
	DET1Alhousingb_logical->SetVisAttributes(aluVisAtt);
	
	//~ const G4double zPlaneDET1Alhousingc[] = {0*m,		 0.008*m,	0.008001*m,	0.068*m, 0.28*m,	0.28001*m,	0.29*m};
	//~ const G4double rInnerDET1Alhousingc[] = {0.0275*m, 0.0275*m,	0.077*m,	0.092*m, 0.092*m,	0.0245*m,	0.0245*m};
	//~ const G4double rOuterDET1Alhousingc[] = {0.077*m,	 0.079*m,	0.079*m,	0.094*m, 0.094*m,	0.1*m,		0.1*m};
	//~ G4Polycone* DET1Alhousingc_solid = new G4Polycone("DET1Alhousingc_sol",0,twopi,7,
						 //~ zPlaneDET1Alhousingc,rInnerDET1Alhousingc,rOuterDET1Alhousingc);
						 
	const G4double housingCInnerR[] = {27.5 * mm, 92. * mm, 24.5 * mm};
	const G4double housingCOuterR[] = {77.0 * mm, 94. * mm, 100. * mm};
	const G4double housingCLength[] = {34. * mm, 30. * mm, 106. * mm, 5. * mm}; 
	
	G4ThreeVector positionConC2 = G4ThreeVector(0., 0., housingCLength[0] - housingCLength[1] + 0.5*mm);
	G4ThreeVector positionTubC2 = G4ThreeVector(0., 0., housingCLength[2] + housingCLength[3] - 1.*mm);
	G4ThreeVector positionUnionTubC = G4ThreeVector(0., 0., housingCLength[0] + housingCLength[2]);	
	G4ThreeVector positionHousingC = G4ThreeVector(0, det1 + 0.013175*m + housingCLength[0], 0);

	G4Cons* conC1 = new G4Cons(	"Housing C Cons",		// Name
									housingCInnerR[0],	// inside radius at -pDz
									housingCOuterR[0],	// outside radius at -pDz
									housingCInnerR[0],	// inside radius at +pDz
									housingCOuterR[1],	// outside radius at +pDz
									housingCLength[0],	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians
									
	G4Cons* conC2 = new G4Cons(	"Housing C Cons",		// Name
									0,								// inside radius at -pDz
									housingCOuterR[0],	// outside radius at -pDz
									0,								// inside radius at +pDz
									housingCInnerR[1],	// outside radius at +pDz
									housingCLength[1] +0.5*mm,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians

	G4SubtractionSolid* subConC = new G4SubtractionSolid("Housing C Part 1",	// Name
							conC1,			// Minuend Volume
							conC2,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							positionConC2);		// position of Subtrahend relativ to Minuend

	G4Tubs* tubC1= new G4Tubs(	"Housing C Tub1",		// Name
									housingCInnerR[1],							// Inner radius
									housingCOuterR[1],			// Outer radius
									housingCLength[2],				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians

	G4Tubs* tubC2= new G4Tubs(	"Housing C Tub2",		// Name
									housingCInnerR[2],							// Inner radius
									housingCOuterR[2],			// Outer radius
									housingCLength[3],				// Half length in z 
									0.,							// Starting phi angle in radians
									twopi);						// Angle of the segment in radians

	G4UnionSolid* unionTubC = new G4UnionSolid("Housing C Part 2",				// Name
										tubC1,				// Summand Volume 1
										tubC2,				// Summand Volume 2
										0, 	// rotation of Summand 2
										positionTubC2);				// position of Summand 2 relativ to Summand 1



	G4UnionSolid* solidHPGe100HousingC = new G4UnionSolid("solid HPGe100 Housing C",				// Name
										subConC,				// Summand Volume 1
										unionTubC,				// Summand Volume 2
										0, 	// rotation of Summand 2
										positionUnionTubC);				// position of Summand 2 relativ to Summand 1

	G4LogicalVolume* DET1Alhousingc_logical = new G4LogicalVolume(solidHPGe100HousingC,Al,
					  "DET1Alhousingc_log",0,0,0);
	DET1Alhousingc_physical = new G4PVPlacement(rotdet,positionHousingC,
					 DET1Alhousingc_logical,"DET1Alhousingc_phys",
					 logicHPGe100Setup,false,0,pSurfChk);
	DET1Alhousingc_logical->SetVisAttributes(aluVisAtt);
	
	//DET1Alring
	G4Tubs* DET1Alring_solid = new G4Tubs("DET1Alring_sol",0.026*m,0.054*m,
				       0.0225*m,0,twopi);
	G4LogicalVolume* DET1Alring_logical = new G4LogicalVolume(DET1Alring_solid,Al,
					  "DET1Alring_log",0,0,0);
	DET1Alring_physical = new G4PVPlacement(rotdet,G4ThreeVector(0,det1+0.268175*m,0),
					 DET1Alring_logical,"DET1Alring_phys",
					 logicHPGe100Setup,false,0,pSurfChk);
	DET1Alring_logical->SetVisAttributes(aluVisAtt);
	
	//~ //Pb shielding	
	
	G4double kollInnerRadius[] = {15. * mm, 25. * mm};
	G4double kollOuterRadius[] = {59. * mm, 76.5 * mm};
	G4double hKollLength = 35. * mm;
	G4ThreeVector positionHPGe100Kollimator = positionHousingC - G4ThreeVector (0., housingCLength[0] + hKollLength,0.);

	G4Cons* solidHPGe100Kollimator = new G4Cons(	"solid HPGe100-Kollimator",		// Name
									kollInnerRadius[0],	// inside radius at -pDz
									kollOuterRadius[0],	// outside radius at -pDz
									kollInnerRadius[1],	// inside radius at +pDz
									kollOuterRadius[1],	// outside radius at +pDz
									hKollLength,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians
	G4LogicalVolume* logicHPGe100Kollimator = new G4LogicalVolume(solidHPGe100Kollimator, Pb, "logic HPGe100-Kollimator", 0,0,0);
	physicHPGe100Kollimator = new G4PVPlacement(rotdet,positionHPGe100Kollimator,					// position in mother frame
										logicHPGe100Kollimator,					// its logical volume
										"physic HPGe100-Kollimator",					// its name
										logicHPGe100Setup,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check

	G4double kollimatorPlatexyz[] = {92.5 * mm, 100. * mm, 10. * mm};
	G4ThreeVector positionHPGe100KollimatorPlate = positionHPGe100Kollimator - G4ThreeVector (0., hKollLength - 50. * mm + 0.25*mm,0.);

	G4Box* plateBox= new G4Box("plain Plate",kollimatorPlatexyz[0],kollimatorPlatexyz[1],kollimatorPlatexyz[2]);
	
	G4Cons* plateCon = new G4Cons(	"Plate hole",		// Name
									0,	// inside radius at -pDz
									kollOuterRadius[0],	// outside radius at -pDz
									0,	// inside radius at +pDz
									kollOuterRadius[1],	// outside radius at +pDz
									hKollLength,	// Half length in z 
									0.,						// Starting phi angle in radians
									twopi);				// Angle of the segment in radians
									
	G4SubtractionSolid* solidKolliPlate = new G4SubtractionSolid("solid Kollimatorholder",	// Name
							plateBox,			// Minuend Volume
							plateCon,					// Subtrahend Volume
							0, 							// rotation of Subtrahend
							G4ThreeVector (0.,0., hKollLength - 50. * mm));		// position of Subtrahend relativ to Minuend

	G4LogicalVolume* logicKolliPlate = new G4LogicalVolume(solidKolliPlate, Al, "logic Kollimatorholder", 0,0,0);
	physicHPGe100KolliPlate = new G4PVPlacement(rotdet,positionHPGe100KollimatorPlate,					// position in mother frame
										logicKolliPlate,					// its logical volume
										"physic Kollimatorholder",					// its name
										logicHPGe100Setup,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	
	logicKolliPlate ->SetVisAttributes(aluVisAtt);


	G4double hPGe100PbShieldThickness = 20. * mm;				// Thickness of lead shield
	G4double hPGe100PbShieldRadius = 96. * mm;					// inner radius of lead shield
	G4double hHPGe100PbShieldLength = 120. * mm;		// half Length of lead shield
	G4ThreeVector positionHPGe100PbShield = positionHPGe100KollimatorPlate + G4ThreeVector (0., hHPGe100PbShieldLength + 11. * mm,0.);
									
	G4Tubs* solidHPGe100PbShield = new G4Tubs("solid HPGe100 Pb-Shield",	// Name
							hPGe100PbShieldRadius ,	// Inner radius
							hPGe100PbShieldRadius + hPGe100PbShieldThickness,					// Outer radius
							hHPGe100PbShieldLength,				// Half length in z
							0.,									// Starting phi angle in radians
							twopi);							// Angle of the segment in radians

	G4LogicalVolume* logicHPGe100PbShield = new G4LogicalVolume(solidHPGe100PbShield, Pb, "logic HPGe100 PbShield", 0,0,0);
	physicHPGe100PbShield = new G4PVPlacement(rotdet,positionHPGe100PbShield,					// position in mother frame
										logicHPGe100PbShield,					// its logical volume
										"physic HPGe100 PbShield",					// its name
										logicHPGe100Setup,							// its mother  volume
										false,								// no boolean operations
										0,									// copy number
										pSurfChk);							//surface check
	logicHPGe100Kollimator ->SetVisAttributes(pbVisAtt);
	logicHPGe100PbShield ->SetVisAttributes(pbVisAtt);

}















//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myDetectorConstruction::ConstructSD(G4bool* withBGO)
{
	//================================================
	// Sensitive detectors : MultiFunctionalDetector
	//================================================
	//
	//  Sensitive Detector Manager.
	pSDman = G4SDManager::GetSDMpointer();
	pSDman->SetVerboseLevel(1);
	//

	//------------------------
	// Sensitive detectors
	//------------------------
	// Sensitive Detector Names 
	for (G4int nbCluster = 0; nbCluster < NbOfClusters; nbCluster++) {
		G4String HPGeECalSDname = "ActiveCrystals_SD"+nameCluster[nbCluster];
		G4String BGOECalSDname = "TripleClusterBGOSD"+nameCluster[nbCluster];
				
		// Check if sensitive detector has already been created
		ActiveCrystals_SD[nbCluster] = (myECalSD*)pSDman->FindSensitiveDetector(HPGeECalSDname, false);
		tripleClusterBGOSD[nbCluster] = (myECalSD*)pSDman->FindSensitiveDetector(BGOECalSDname, false);
		
		if (!ActiveCrystals_SD[nbCluster]) 
		{
			G4cout << " Creating sensitive detector: " << HPGeECalSDname << G4endl;
			// Define own MultiFunctional SD  with name.
			ActiveCrystals_SD[nbCluster] = new myECalSD(HPGeECalSDname, "TripleClusterHPGe_HitsCollection"+nameCluster[nbCluster], NbOfCrystals[nbCluster]);
			if (ActiveCrystals_SD[nbCluster]) {
				pSDman->AddNewDetector(ActiveCrystals_SD[nbCluster]);                // Register SD to SDManager.
				for (G4int nbCrystal=0; nbCrystal<NbOfCrystals[nbCluster]; nbCrystal++)
				{
					ActiveCrystal_logical[nbCluster][nbCrystal]->SetSensitiveDetector(ActiveCrystals_SD[nbCluster]);	// Assign SD to the logical volumes.
				}
			}
		}
		if (withBGO[nbCluster]) {
			if (!tripleClusterBGOSD[nbCluster]) 
			{
				G4cout << " Creating sensitive detector: " << BGOECalSDname << G4endl;
				// Define own MultiFunctional SD  with name.
				tripleClusterBGOSD[nbCluster] = new myECalSD(BGOECalSDname, "TripleClusterBGO_HitsCollection"+nameCluster[nbCluster], 8);
				if (tripleClusterBGOSD[nbCluster]) {
					pSDman->AddNewDetector(tripleClusterBGOSD[nbCluster]);                	// Register SD to SDManager.
					BGOCrystal_logic[nbCluster]->SetSensitiveDetector(tripleClusterBGOSD[nbCluster]);	// Assign SD to the logical volume.
				}
			}
		}
	}
}	

void myDetectorConstruction::InitializeVisAtt()
{
	G4bool visGe = true, visBGO = true, visAlu = true, visSteel = true, visContactLayer = true, visWater = true, visSetup = true;
	G4bool visPb = true, visDisk = true, visTarget = true, visBacking = true, visCu = true, visVac = true, visQuartz = true;
	G4Colour myRed(86./255.,180./233.,233./255.);	// for deadlayers
	G4Colour myYellow(240./255.,228./255.,66./255.); //preamp
	G4Colour myGEcolor(230./255.,159./255.,0);	// green for Ge crystals
	G4Colour myBGOcolor(0,158./255.,115./255.); // for BGOs
	G4Colour myGray(0.5,0.5,0.5);	// for steel and Alu
	G4Colour myBlue(0,0,1.0);	// for Ge crystals
	G4Colour myGreen(0,1.0,0); // for coolingfingers
	G4Colour myOrange(1.0,0.5,0); // for Pb shields and copper
	G4Colour myLightBlue(0,0.7,0.7,0.4); // for water
	
	//geant4 optics stuff
	geVisAtt = new G4VisAttributes(myGEcolor);
	geVisAtt ->SetVisibility(visGe);
	geVisAtt ->SetForceSolid(true);
	//~ geVisAtt ->SetForceAuxEdgeVisible(true);

	ContactLayerVisAtt = new G4VisAttributes(myRed); 
	ContactLayerVisAtt->SetVisibility(visContactLayer);
	ContactLayerVisAtt->SetForceSolid(true);
	//ContactLayerVisAtt ->SetForceAuxEdgeVisible(true);
	
	PreampVisAtt = new G4VisAttributes(myYellow); 
	PreampVisAtt->SetVisibility(visContactLayer);
	PreampVisAtt->SetForceSolid(true);
	//ContactLayerVisAtt ->SetForceAuxEdgeVisible(true);
		
	pbVisAtt = new G4VisAttributes(myOrange);
	pbVisAtt ->SetVisibility(visPb);
	//~ pbVisAtt->SetForceWireframe(true);
	pbVisAtt->SetForceSolid(true);
	
	steelVisAtt= new G4VisAttributes(myGray);
	steelVisAtt ->SetVisibility(visSteel);
	steelVisAtt ->SetForceSolid(true);
	//~ steelVisAtt ->SetForceWireframe(true);
	//~ steelVisAtt ->SetForceAuxEdgeVisible(true);
	
	diskVisAtt= new G4VisAttributes(G4Colour(1.,1.,1.));	// white
	diskVisAtt->SetVisibility(visDisk);
	diskVisAtt ->SetForceSolid(true);

	targetVisAtt= new G4VisAttributes(myRed);
	targetVisAtt->SetVisibility(visTarget);
	targetVisAtt ->SetForceSolid(true);
	
	backingVisAtt= new G4VisAttributes(myGreen) ; 
	backingVisAtt->SetVisibility(visBacking);
	backingVisAtt->SetForceSolid(true);	
		
	cuVisAtt= new G4VisAttributes(myOrange);
	cuVisAtt->SetVisibility(visCu);
	cuVisAtt ->SetForceAuxEdgeVisible(true);
	//cuVisAtt->SetForceSolid(true);
	
	vacVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));	//white
	vacVisAtt ->SetVisibility(visVac);
	vacVisAtt ->SetForceWireframe(true);
	
	coolingVisAtt= new G4VisAttributes(myLightBlue);
	coolingVisAtt->SetVisibility(visWater);
	coolingVisAtt ->SetForceSolid(true);

	aluVisAtt = new G4VisAttributes(myGray);
	aluVisAtt ->SetVisibility(visAlu);
	aluVisAtt->SetForceSolid(true);
	aluVisAtt ->SetForceAuxEdgeVisible(true);
	
	BGOVisAtt = new G4VisAttributes(myBGOcolor);
	BGOVisAtt ->SetVisibility(visBGO);
	//~ BGOVisAtt->SetForceWireframe(true);
	//BGOVisAtt ->SetForceAuxEdgeVisible(true);
	BGOVisAtt ->SetForceSolid(true);

	setupVisAtt = new G4VisAttributes(myGray);
	setupVisAtt->SetVisibility(visSetup);
	setupVisAtt->SetForceWireframe(true);
	
	quartzVisAtt = new G4VisAttributes(G4Colour(1.0,0.2,0.0)); //lighter red
	quartzVisAtt->SetVisibility(visQuartz);
	quartzVisAtt->SetForceWireframe(true);
	quartzVisAtt ->SetForceAuxEdgeVisible(true);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
