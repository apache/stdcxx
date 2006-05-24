// ICC solution configuration for configure script
// not use this for the solution generation

//clone generic solution
var solutionICC90Cfg = solution.clone();
solutionICC90Cfg.name = icc90CfgSolutionName;
configurations.add(icc90CfgSolutionName.name, solutionICC90Cfg);
configureToolsICC90();

function configureToolsICC90()
{
    // ICC compilers
    var compilerICCRelease = new CompilerICC(compilerWin32);
    
    var compilerICCDebug = compilerICCRelease.clone();
    compilerICCDebug.isDebug = true;
    
    compilerICCRelease.optimization = "Size";
    compilerICCRelease.debugInfoFormat = "PDB";
    
    compilerICCDebug.debugInfoFormat = "PDB";
    
    var compilerICC90Release = new CompilerICC90(compilerICCRelease);
    var compilerICC90Debug = new CompilerICC90(compilerICCDebug);
    
    // ICC linkers
    var linkerICCDebug = new LinkerICC(linker);
    linkerICCDebug.subsystem = "Console";
    linkerICCDebug.incremental = "No";
    var linkerICCRelease = linkerICCDebug.clone();
    linkerICCRelease.optimizeRefs = "Remove";
    linkerICCRelease.comdatFolding = "Remove";

    // ICC librarian
    var librarianICC = new LibrarianICC(librarian);
 
    // ICC custom
    var customICC = new CustomBuildICC();
    
    // ICC post build
    var postICC = new PostBuildICC();
    
    // configure projects tools
    configureToolsConfigure(icc90SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);
        
    /* not needed in this pseudo-solution
    configureToolsExamples(icc90SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);

    configureToolsTests(icc90SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);
        
    configureToolsRwTest(icc90SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);
        
    configureToolsRunExamples(icc90SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);
        
    configureToolsRunTests(icc90SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);
    */
}


//clone generic solution
var solutionICC91Cfg = solution.clone();
solutionICC91Cfg.name = icc91CfgSolutionName;
configurations.add(icc91CfgSolutionName.name, solutionICC91Cfg);
configureToolsICC91();

function configureToolsICC91()
{
    // ICC compilers
    var compilerICCRelease = new CompilerICC(compilerWin32);
    
    var compilerICCDebug = compilerICCRelease.clone();
    compilerICCDebug.isDebug = true;
    
    compilerICCRelease.optimization = "Size";
    compilerICCRelease.debugInfoFormat = "PDB";
    
    compilerICCDebug.debugInfoFormat = "PDB";
    
    var compilerICC90Release = new CompilerICC90(compilerICCRelease);
    var compilerICC90Debug = new CompilerICC90(compilerICCDebug);
    
    // ICC linkers
    var linkerICCDebug = new LinkerICC(linker);
    linkerICCDebug.subsystem = "Console";
    linkerICCDebug.incremental = "No";
    var linkerICCRelease = linkerICCDebug.clone();
    linkerICCRelease.optimizeRefs = "Remove";
    linkerICCRelease.comdatFolding = "Remove";

    // ICC librarian
    var librarianICC = new LibrarianICC(librarian);
 
    // ICC custom
    var customICC = new CustomBuildICC();
    
    // ICC post build
    var postICC = new PostBuildICC();
    
    // configure projects tools
    configureToolsConfigure(icc91SolutionName, compilerICC90Debug, 
        compilerICC90Release, linkerICCDebug, linkerICCRelease, 
        librarianICC, customICC, postICC);
}
