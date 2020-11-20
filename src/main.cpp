#include <Titan/sim.h>
#include <lib3mf_implicit.hpp>
#include "3MFutils.h"

void loadLatticeFrom3MF(string inputLattice, Lib3MF::PBeamLattice lattice) {
	return;
}

int main(int argc, char *argv[]) {
	Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
	string inFile = "";
	string outFile = "";
	double dt = 0;
	double T = 0;

	if (argc == 1) {
		cerr << "OOPS: You need to provide the arguments for the input file, dt, and T!\n";
	}
	else if (argc > 3) {
		inFile = argv[1];
		dt = atof(argv[2]);
		T = atof(argv[3]);
	}

	std::cout << "Starting performance analysis with: " << inFile << " dt: "<< dt << " T: " << T << std::endl;
    
	Lib3MF::PBeamLattice lattice;
	loadLatticeFrom3MF(inFile,lattice);

    return 1;
}

/*
void Simulator::write3MF(const QString &outputFile) {
    QFileInfo info(outputFile);
    QString output = info.path() + "/" + info.completeBaseName() + ".3mf";

    Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
    
    // adding material
   /* Lib3MF::PBaseMaterialGroup materials = Lib3MF::CBaseMaterialGroup();
    string name = "Titanium";
    fRed = 1; fGreen = 1; fBlue = 1; fAlpha = 1;
    Lib3MF::sColor color = FloatRGBAToColor(fRed,fGreen,fBlue,fAlpha)
    materialsAddMaterial(name,color)


    Lib3MF::PModel model = wrapper->CreateModel();
    model->SetUnit(eModelUnit(5));
    Lib3MF::PMeshObject meshObject = model->AddMeshObject();
    meshObject->SetName("Beamlattice");

    std::vector<sLib3MFPosition> vertices(sim->masses.size());
    std::vector<sLib3MFBeam> beams;
    std::vector<sLib3MFTriangle> triangles(0);

    for (int m = 0; m < sim->masses.size(); m++) {
        vertices[m] = fnCreateVertex(sim->masses[m]->origpos[0],sim->masses[m]->origpos[1],sim->masses[m]->origpos[2]);
    }
    for (int s = 0; s < sim->springs.size(); s++) {
        if (sim->springs[s]->_k != 0) {
            beams.push_back(fnCreateBeam(sim->springs[s]->_left->index,sim->springs[s]->_right->index,sim->springs[s]->_diam/2,sim->springs[s]->_diam/2,eBeamLatticeCapMode::Sphere,eBeamLatticeCapMode::Sphere));
        }
    } 
   // Set beamlattice geometry and metadata
    meshObject->SetGeometry(vertices, triangles);
    
    Lib3MF::PBeamLattice beamLattice = meshObject->BeamLattice();
    beamLattice->SetBeams(beams);
    beamLattice->SetMinLength(0.000001);

    model->AddBuildItem(meshObject.get(), wrapper->GetIdentityTransform());

    Lib3MF::PWriter writer = model->QueryWriter("3mf");
    writer->WriteToFile(output.toStdString());
}*/