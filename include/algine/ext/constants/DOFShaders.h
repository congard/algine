#ifndef ALGINE_DOFSHADERS_H
#define ALGINE_DOFSHADERS_H

// TODO: split constants in 2 separate files

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace DOFShaders {
        namespace Settings {
            constant(OutputType, "vecout")
            constant(TexComponent, "texComponent")

            constant(Dof, "ALGINE_LINEAR_DOF")
            constant(CinematicDof, "ALGINE_CINEMATIC_DOF")
            constant(DofCocMap, "ALGINE_DOF_FROM_COC_MAP")
            constant(BleedingMinDeltaZ, "ALGINE_BLEEDING_ELIM_DZ")
            constant(BleedingMinDeltaCoC, "ALGINE_BLEEDING_ELIM_DCOC")
            constant(BleedingMaxFocusCoC, "ALGINE_BLEEDING_ELIM_FCOC")
        }

        namespace Vars {
            constant(BaseImage, "baseImage")
            constant(PositionMap, "positionMap")
            constant(CoCMap, "cocMap")
            constant(MinSigma, "minSigma")
            constant(MaxSigma, "maxSigma")
            constant(FocalDepth, "focalDepth")
            constant(FocalRange, "focalRange")
            constant(PlaneInFocus, "cinematicDOF.p")
            constant(Aperture, "cinematicDOF.a")
            constant(ImageDistance, "cinematicDOF.i")
            constant(BleedingMinDeltaZ, "bleedingMinDeltaZ") // Bleeding* - bleeding elimination variables
            constant(BleedingMinDeltaCoC, "bleedingMinDeltaCoC")
            constant(BleedingMaxFocusCoC, "bleedingMaxFocusCoC")
        }
    }
}

#undef constant

#endif //ALGINE_DOFSHADERS_H
