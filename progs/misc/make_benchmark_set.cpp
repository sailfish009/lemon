#include <iostream>
#include <sstream>

#include "lemon/lemon.hpp"

int main(int argc, char* argv[]) {
    lemon::Options o;
    std::string outdir;
    auto distance = 6.0;
    o.add_option("distance,d", distance,
                 "Largest distance between protein and a small molecule.");
    o.add_option("outdir,o", outdir, "output directory");
    o.parse_command_line(argc, argv);

    lemon::Entries entries;
    std::unordered_map<std::string, lemon::ResidueNameSet> rnms;
    std::ifstream is(o.entries());
    lemon::read_entry_file(is, entries, rnms);

    auto worker = [distance, &rnms, &outdir](chemfiles::Frame complex,
                                             const std::string& pdbid) {

        // Selection phase
        std::list<size_t> smallm;
        if (lemon::select::specific_residues(complex, smallm,
                                             rnms[pdbid]) == 0) {
            return "Skipping " + pdbid;
        }

        // Pruning phase
        lemon::prune::identical_residues(complex, smallm);

        // Output phase
        for (auto resid : smallm) {
            chemfiles::Frame prot;
            chemfiles::Frame lig;
            lemon::separate::protein_and_ligand(complex, resid, distance,
                                                prot, lig);

            auto protfile = boost::filesystem::path(outdir);
            protfile /= pdbid + "_" + lig.get("name")->as_string() + ".pdb";
            auto ligfile  = boost::filesystem::path(outdir);
            ligfile /= pdbid + "_" + lig.get("name")->as_string() + ".sdf";

            chemfiles::Trajectory prot_traj(protfile.string(),'w');
            chemfiles::Trajectory lig_traj(ligfile.string(),'w');
            prot_traj.write(prot);
            lig_traj.write(lig);
            prot_traj.close();
            lig_traj.close();
        }

        return pdbid + std::to_string(smallm.size());
    };

    return lemon::launch<lemon::print_combine>(o, worker, std::cout);
}
