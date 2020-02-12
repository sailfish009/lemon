#include "lemon/separate.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/Trajectory.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Separate") {
    auto traj = chemfiles::Trajectory("files/1AAQ.mmtf", 'r');
    auto frame = traj.read();

    chemfiles::Frame ligand;
    chemfiles::Frame protein;

    const auto& residues = frame.topology().residues();
    size_t i; 
    for (i = 0; i < residues.size(); ++i) {
        if (residues[i].name() == "PSI") {
            break;
        }
    }

    // NOLINTNEXTLINE allow 15 to be magic
    lemon::separate::protein_and_ligand(frame, i, 15, protein, ligand);

    CHECK(ligand.size() == 41);
    CHECK(protein.size() == 998);
}
