#include "3MFutils.h"
sLib3MFPosition fnCreateVertex(float x, float y, float z)
{
    sLib3MFPosition result;
    result.m_Coordinates[0] = x;
    result.m_Coordinates[1] = y;
    result.m_Coordinates[2] = z;
    return result;
}

sLib3MFBeam fnCreateBeam(int v0, int v1, double r0, double r1, eLib3MFBeamLatticeCapMode c0, eLib3MFBeamLatticeCapMode c1)
{
    sLib3MFBeam result;
    result.m_Indices[0] = v0;
    result.m_Indices[1] = v1;
    result.m_Radii[0] = r0;
    result.m_Radii[1] = r1;
    result.m_CapModes[0] = c0;
    result.m_CapModes[1] = c1;
    return result;
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

void ShowObjectProperties(PObject object)
{
    std::cout << "   Name:            \"" << object->GetName() << "\"" << std::endl;
    std::cout << "   PartNumber:      \"" << object->GetPartNumber() << "\"" << std::endl;

    switch (object->GetType()) {
    case eObjectType::Model:
        std::cout << "   Object type:     model" << std::endl;
        break;
    case eObjectType::Support:
        std::cout << "   Object type:     support" << std::endl;
        break;
    case eObjectType::SolidSupport:
        std::cout << "   Object type:     solidsupport" << std::endl;
        break;
    case eObjectType::Other:
        std::cout << "   Object type:     other" << std::endl;
        break;
    default:
        std::cout << "   Object type:     invalid" << std::endl;
        break;
    }

    if (object->HasSlices(false)) {
        PSliceStack sliceStack = object->GetSliceStack();
        ShowSliceStack(sliceStack, "   ");
    }

    if (object->GetMetaDataGroup()->GetMetaDataCount() > 0) {
        ShowMetaDataInformation(object->GetMetaDataGroup());
    }
}

void ShowMeshObjectInformation(PMeshObject meshObject)
{
    std::cout << "mesh object #" << meshObject->GetResourceID() << ": " << std::endl;

    ShowObjectProperties(meshObject);

    Lib3MF_uint64 nVertexCount = meshObject->GetVertexCount();
    Lib3MF_uint64 nTriangleCount = meshObject->GetTriangleCount();
    PBeamLattice beamLattice = meshObject->BeamLattice();

    // Output data
    std::cout << "   Vertex count:    " << nVertexCount << std::endl;
    std::cout << "   Triangle count:  " << nTriangleCount << std::endl;

    Lib3MF_uint64 nBeamCount = beamLattice->GetBeamCount();
    if (nBeamCount > 0) {
        std::cout << "   Beam count:  " << nBeamCount << std::endl;
        Lib3MF_uint32 nRepresentationMesh;
        if (beamLattice->GetRepresentation(nRepresentationMesh))
            std::cout << "   |_Representation Mesh ID:  " << nRepresentationMesh << std::endl;
        eLib3MFBeamLatticeClipMode eClipMode;
        Lib3MF_uint32 nClippingMesh;
        beamLattice->GetClipping(eClipMode, nClippingMesh);
        if (eClipMode != eBeamLatticeClipMode::NoClipMode)
            std::cout << "   |_Clipping Mesh ID:  " << nClippingMesh << "(mode=" << (int)eClipMode << ")" << std::endl;
        if (beamLattice->GetBeamSetCount() > 0) {
            std::cout << "   |_BeamSet count:  " << beamLattice->GetBeamSetCount() << std::endl;
        }
    }
}

void ShowSliceStack(PSliceStack sliceStack, std::string indent)
{
    std::cout << indent << "SliceStackID:  " << sliceStack->GetResourceID() << std::endl;
    if (sliceStack->GetSliceCount() > 0) {
        std::cout << indent << "  Slice count:  " << sliceStack->GetSliceCount() << std::endl;
    }
    if (sliceStack->GetSliceRefCount() > 0) {
        std::cout << indent << "  Slice ref count:  " << sliceStack->GetSliceRefCount() << std::endl;
        for (Lib3MF_uint64 iSliceRef = 0; iSliceRef < sliceStack->GetSliceRefCount(); iSliceRef++) {
            std::cout << indent << "  Slice ref :  " << sliceStack->GetSliceStackReference(iSliceRef)->GetResourceID() << std::endl;
        }
    }
}

void ShowMetaDataInformation(PMetaDataGroup metaDataGroup)
{
    Lib3MF_uint32 nMetaDataCount = metaDataGroup->GetMetaDataCount();

    for (Lib3MF_uint32 iMeta = 0; iMeta < nMetaDataCount; iMeta++) {

        PMetaData metaData = metaDataGroup->GetMetaData(iMeta);
        std::string sMetaDataValue = metaData->GetValue();
        std::string sMetaDataName = metaData->GetName();
        std::cout << "Metadatum: " << iMeta << ":" << std::endl;
        std::cout << "Name  = \"" << sMetaDataName << "\"" << std::endl;
        std::cout << "Value = \"" << sMetaDataValue << "\"" << std::endl;
    }
}

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