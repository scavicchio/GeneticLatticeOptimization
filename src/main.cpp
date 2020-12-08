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
	// Runtime: 12.271059 seconds for original Titan library
	double d = duration.count()/(10*pow(10,5));
	cout << "Runtime: " << d << " seconds\n";\
	double speed = sim.springs.size()*(sim.time()/sim.gdt())/d;
	std::cout << "SPEED: " << speed << " springs/second.\n";
	cout << "Default dt: " << sim.gdt() << endl;
	cout << "Sim time: " << sim.time() << endl;
	std::cout << "All done, no runtime issues.\n";
	exit(1);
    return 1;
}

/* IMPROVEMENT NOTES}

// original NVPROF RESULTS
- for 10 steps
Runtime: 12.1983 seconds
SPEED: 8.49954e+06 springs/second.
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
GPU activities:   82.66%  10.1740s     99722  102.02us  92.985us  1.9848ms  computeSpringForces(CUDA_SPRING**, int, double)
                   17.34%  2.13408s     99722  21.400us  18.028us  1.1473ms  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)

- for 50 steps
Runtime: 61.392 seconds
SPEED: 8.44419e+06 springs/second.
			Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   82.63%  49.8635s    500357  99.655us  97.999us  1.2790ms  computeSpringForces(CUDA_SPRING**, int, double)
                   17.37%  10.4787s    500356  20.942us  20.508us  841.48us  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)


// halfing block size to 256 


- for 10 steps
Runtime: 8.34788 seconds
SPEED: 1.24169e+07 springs/second.
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
GPU activities:   74.22%  6.24244s     99606  62.671us  50.904us  1.3667ms  computeSpringForces(CUDA_SPRING**, int, double)
                   25.78%  2.16875s     99606  21.773us  17.800us  1.1468ms  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)


- for 50 steps
Runtime: 41.4957 seconds
SPEED: 1.2495e+07 springs/second.
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   74.49%  30.7991s    499982  61.600us  57.971us  1.6370ms  computeSpringForces(CUDA_SPRING**, int, double)
                   25.51%  10.5492s    499982  21.099us  20.475us  950.86us  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)


//  combining the if statements in sim.cu for spring_.k and spring_.compute check


- for 50 steps
Runtime: 41.3233 seconds
SPEED: 1.25416e+07 springs/second.
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   74.25%  30.7318s    500768  61.369us  58.405us  1.3420ms  computeSpringForces(CUDA_SPRING**, int, double)
                   25.75%  10.6606s    500768  21.288us  20.610us  262.93us  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)
      API calls:   80.87%  69.7262s   1074339  64.901us  4.0590us  349.08ms  cudaLaunchKernel
                   18.74%  16.1591s      9296  1.7383ms  1.3630us  60.594ms  cudaDeviceSynchronize

// by convertin the constraint if statements in the spring force to arithmatic

- for 50 steps
Runtime: 31.2904 seconds
SPEED: 1.65756e+07 springs/second.
           Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   66.50%  20.7522s    500365  41.474us  38.946us  1.2031ms  computeSpringForces(CUDA_SPRING**, int, double)
                   33.50%  10.4542s    500364  20.893us  20.289us  1.2605ms  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)
      API calls:   84.68%  56.1559s   1109051  50.634us  4.1280us  262.87ms  cudaLaunchKernel
                   14.82%  9.83075s     13635  720.99us  1.4930us  38.011ms  cudaDeviceSynchronize

// one more line converted

- for 50 steps
Runtime: 31.7971 seconds
SPEED: 1.63369e+07 springs/second.
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   66.36%  20.8438s    500927  41.610us  39.298us  1.2051ms  computeSpringForces(CUDA_SPRING**, int, double)
                   33.64%  10.5641s    500926  21.089us  20.545us  1.2961ms  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)
      API calls:   77.05%  51.9536s   1108555  46.866us  4.0170us  263.25ms  cudaLaunchKernel
                   21.40%  14.4279s     13573  1.0630ms  1.3530us  46.942ms  cudaDeviceSynchronize

// one more line

- for 50 steps
Runtime: 31.7291 seconds
SPEED: 1.63656e+07 springs/second.
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   66.38%  20.7533s    500735  41.445us  39.329us  1.3050ms  computeSpringForces(CUDA_SPRING**, int, double)
                   33.62%  10.5124s    500734  20.994us  20.416us  1.1716ms  massForcesAndUpdate(CUDA_MASS**, Vec, CUDA_GLOBAL_CONSTRAINTS, int)
      API calls:   79.32%  53.4157s   1109787  48.131us  4.0370us  262.94ms  cudaLaunchKernel
                   19.09%  12.8539s     13727  936.39us  1.3720us  46.809ms  cudaDeviceSynchronize

*/