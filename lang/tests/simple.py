import lemon
class MyWorkflow(lemon.Workflow):
    def worker(self, entry, pdbid):
        return entry.topology().residue(1).get("chainname").get().as_string() + '\n'
    def finalize(self):
        pass
