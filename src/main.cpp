#include <Titan/sim.h>
#include <lib3mf_implicit.hpp>
#include <chrono> 

using namespace Lib3MF;

float getScaleFromUnit(eModelUnit units) {
    float scale;
    if (units == eModelUnit::MicroMeter) {
        scale = 0.000001;
    } else if (units == eModelUnit::MilliMeter) {
        scale = 0.001;
    } else if (units == eModelUnit::CentiMeter) {
        scale = 0.01;
    } else if (units == eModelUnit::Meter) {
        scale = 1;
    } else if (units == eModelUnit::Inch) {
        scale = 0.0254;
    } else if (units == eModelUnit::Foot) {
        scale = 0.3048;
    } else { 
        scale = 1;
    }
    return scale;
}
void printVersion(PWrapper wrapper) {
    Lib3MF_uint32 nMajor, nMinor, nMicro;
    wrapper->GetLibraryVersion(nMajor, nMinor, nMicro);
    std::cout << "lib3mf version = " << nMajor << "." << nMinor << "." << nMicro;
    std::string sReleaseInfo, sBuildInfo;
    if (wrapper->GetPrereleaseInformation(sReleaseInfo)) {
        std::cout << "-" << sReleaseInfo;
    }
    if (wrapper->GetBuildInformation(sBuildInfo)) {
        std::cout << "+" << sBuildInfo;
    }
    std::cout << std::endl;
    return;
}

void loadSimFrom3MF(const string& inputLattice, Simulation* sim) {
	PWrapper wrapper = CWrapper::loadLibrary();
	//printVersion(wrapper);
	PModel model = wrapper->CreateModel();

	// import from file
	{ 
		Lib3MF::PReader reader = model->QueryReader("3mf");
		reader->SetStrictModeActive(false);
		reader->ReadFromFile(inputLattice);

		for (Lib3MF_uint32 iWarning = 0; iWarning < reader->GetWarningCount(); iWarning++) {
			Lib3MF_uint32 nErrorCode;
			std::string sWarningMessage = reader->GetWarning(iWarning, nErrorCode);
			std::cout << "Encountered warning #" << nErrorCode << " : " << sWarningMessage << std::endl;
		}	
	}

	eModelUnit units = model->GetUnit();
	float scale = getScaleFromUnit(units);
	std::cout <<"Scaling: " << scale << std::endl;

	PObjectIterator objectIterator = model->GetObjects();
	while (objectIterator->MoveNext()) {
		Lib3MF::PObject object = objectIterator->GetCurrentObject();
		if (object->IsMeshObject()) {
			Lib3MF::PMeshObject mesh = model->GetMeshObjectByID(object->GetResourceID());
			// vertices
			Lib3MF_uint32 nVertices = mesh->GetVertexCount();
			cout << "Loading " << nVertices << " vertices from 3MF\n";
			std::vector<Lib3MF::sPosition> vertices(nVertices);
			mesh->GetVertices(vertices);
			// pushing masses to sim
			for (Lib3MF::sPosition item : vertices) {
				sim->createMass(Vec(item.m_Coordinates[0]*scale,item.m_Coordinates[1]*scale,item.m_Coordinates[2]*scale));
			}
			// putting springs to sim
			Lib3MF::PBeamLattice beamset = mesh->BeamLattice();
			Lib3MF_uint32 nBeams = beamset->GetBeamCount();
			std::vector<Lib3MF::sBeam> beams(nBeams);
			beamset->GetBeams(beams);
			cout << "Loading " << nBeams << " beams from 3MF\n";
			for (Lib3MF::sBeam item : beams) {
				Mass *m1 = sim->masses[item.m_Indices[0]];
				Mass *m2 = sim->masses[item.m_Indices[1]];
				sim->createSpring(m1, m2);
			}
			for (int i = 0; i < sim->springs.size(); i++) {
				Spring *s = sim->springs[i];
				s->_diam = beams[i].m_Radii[0];
    		}
		}
	}
	return;
}

int main(int argc, char *argv[]) {
	Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
	string inFile = "";
	string outFile = "";
	double dt = 0;	
	double T = 0;
	double k = 10000;
	int nSteps = 10;
	if (argc == 1) {
		cerr << "OOPS: You need to provide the arguments for the input file, dt, and T!\n";
	}
	else if (argc > 3) {
		inFile = argv[1];
		dt = atof(argv[2]);
		T = atof(argv[3]);
	}

	std::cout << "Starting performance analysis with: " << inFile << " dt: "<< dt << " T: " << T << std::endl;
	
	Simulation sim;
	sim.createLattice(Vec(0, 0, 10), Vec(5, 5, 5), 5, 5, 5); // create lattice with center at (0, 0, 10) and given dimensions
  	sim.createPlane(Vec(0, 0, 1), 0); // create constraint plane

	cout << "TITAN simulation now has " << sim.masses.size() << " masses and " << sim.springs.size() << " springs.\n";
	cout << "Stepping " << nSteps << " timesteps...\n";
	cout << "Default dt: " << sim.gdt() << endl;
	sim.start();
	cout << "Sim time: " << sim.time() << endl;
	auto start = std::chrono::high_resolution_clock::now(); 
	sim.step(nSteps);
	auto stop = std::chrono::high_resolution_clock::now(); 
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
	// Runtime: 12271059 seconds for original Titan library
	double d = duration.count()/(10*pow(10,5));
	cout << "Runtime: " << d << " seconds\n";\
	double speed = sim.springs.size()*(sim.time()/sim.gdt())/d;
	std::cout << "SPEED: " << speed << " springs/second.\n";
	cout << "Default dt: " << sim.gdt() << endl;
	cout << "Sim time: " << sim.time() << endl;
	std::cout << "All done, no runtime issues.\n";

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