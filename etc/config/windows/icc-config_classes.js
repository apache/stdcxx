// file should be included after base_config_classes

// IC compiler, linker and librarian

//------------------------------------------------
// CompilerICC class
//------------------------------------------------

// ICC compiler options
// optimization = Disabled (0, /Od) | Size (1, /O1) | Speed (2, /O2)
//                  | Full (3, /Ox) (custom is not supported)
// minimalRebuild = true (TRUE, /Gm)| false (FALSE, "")
// runtime = 
//      SingleDebug (5, /MLd) | Single (4, /ML) | MultiDebugDll (3, /MDd)
//              | MultiDll (2, /MD) | MultiDebug (1, MTd) | Multi (0, /MT)
// exceptions = true (TRUE, /EHsc) | false (FALSE, "")
// precompiledHeader = 
//      Use (3, /Yu) | Auto (2, /YX) | Create (1, /Yc) | No (0, "")
// pchName = <name of the pchfile>
// pchBuildFrom = <name of the .h file>
// warningsLevel = 0 (/W0) | 1 (/W1) | 2 (/W2)| 3 (/W3)| 4 (/W4)
// debugInfoFormat = Disabled (0, "") | C7 (1, /Z7) | LinesOnly (2, /Zd)
//                      | PDB (3, /Zi) | EditContinue (4, /ZI)
// pdbName = <name of pdb file>
// nologo = true (TRUE, /nologo) | FALSE (FALSE, "")

//.ctor from base compiler
function CompilerICC(compilerBase)
{
    if (!compilerBase)
    {
        compilerBase = new Compiler();
    }
    compilerBase.cloneTo(this);
    this.optimization = "Disabled";
    this.minimalRebuild = true;
    this.runtime = "MultiDebugDll";
    this.exceptions = true;
    this.precompiledHeader = "No";
    this.pchName = "";
    this.pchBuildFrom = "";
    this.warningsLevel = 3;
    this.debugInfoFormat = "PDB";
    this.pdbName = "";
    this.nologo = true;

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
    this.getOutputDir = getOutputDir;
    this.getIntermDir = getIntermDir;
    this.getCommandLine = getCommandLineCompilerICC;
    this.getPreprocessCommandLine = getPreprocessCommandLineCompilerICC;
}

function getCommandLineCompilerICC()
{
    var cmdLine = "icl /c";
    // nologo
    if (this.nologo)
    {
        cmdLine += " /nologo";
    }
    // defines
    cmdLine += buildOptionsVC(this.defines, "/D");
    // includes
    cmdLine += buildOptionsVC(this.includeDirectories, "/I");
    // warnings
    cmdLine += " /W" + this.warningsLevel;
    // exceptions
    if (this.exceptions)
    {
        cmdLine += " /EHsc";
    }
    // runtime
    switch (this.runtime.toUpperCase())
    {
    case "MULTI":
        cmdLine += " /MT";
        break;
    case "MULTIDEBUG":
        cmdLine += " /MTd";
        break;
    case "MULTIDLL":
        cmdLine += " /MD";
        break;
    case "MULTIDEBUGDLL":
        cmdLine += " /MDd";
        break;
    case "SINGLE":
        cmdLine += " /ML";
        break;
    default: // SingleDebug
        cmdLine += " /MLd";
    }
    // minimalRebuild
    if (this.minimalRebuild)
    {
        cmdLine += " /Gm";
    }
    switch (this.optimization.toUpperCase())
    {
    case "FULL":
        cmdLine += " /Ox";
        break;
    case "SPEED":
        cmdLine += " /O2";
        break;
    case "SIZE":
        cmdLine += " /O1";
        break;
    default: // Diasbled
        cmdLine += " /Od";
    }
    /*
    // outfolder
    var outDir = this.getOutputDir() + "\\";
    if (outDir.length > 0)
    {
        cmdLine += " /Fo\"" + outDir + "\\\"";
    }
    */
    // outfolder
    var outDir = this.getIntermDir() + "\\";
    if (outDir.length > 0)
    {
        cmdLine += " /Fo\"" + outDir + "\\\"";
    }
    // pch
    var usePCH = true;
    switch (this.precompiledHeader.toUpperCase())
    {
    case "USE":
        cmdLine += " /Yu";
        break;
    case "AUTO":
        cmdLine += " /YX";
        break;
    case "CREATE":
        cmdLine += " /Yc";
        break;
    default:
        usePCH = false;
    }
    if (usePCH)
    {
        if (this.pchBuildFrom.length > 0)
        {
            cmdLine += "\"" + this.pchBuildFrom + "\"";
        }
        else
        {
            cmdLine += "\"stdafx.h\"";
        }
        if (this.pchName.length > 0)
        {
            cmdLine += " /Fp\"" + outFir + this.pchName + "\"";
        }
    }
    // pdb
    var usePDB = true;
    switch (this.debugInfoFormat.toUpperCase())
    {
    case "C7":
        cmdLine += " /Z7";
        break;
    case "LINESONLY":
        cmdLine += " /Zd";
        break;
    case "PDB":
        cmdLine += " /Zi";
        break;
    case "EDITCONTINUE":
        cmdLine += " /ZI";
        break;
    default:
        usePDB = false;
    }
    if (usePDB && this.pdbName.length > 0)
    {
        cmdLine += " /Fd\"" + outDir + this.pdbName + "\"";
    }
    // output
    if (this.outputFile.length > 0)
    {
        cmdLine += " /Fe\"" + outDir + this.outputFile + "\"";
    }
    // inputs
    cmdLine += buildOptionsVC(this.inputFiles, "");
    return cmdLine;
}

function writeSolutionSettingsCompilerICC(icproj, outDir)
{
    icproj.WriteLine("\t\t\t<Tool")
    icproj.WriteLine("\t\t\t\tName=\"CppCmplrTool\"");
    var opt = 0;
    switch (this.optimization.toUpperCase())
    {
    case "SIZE":
        opt = 1;
        break;
    case "SPEED":
        opt = 2;
        break;
    case "FULL":
        opt = 3;
        break;
    default:
        // leave 0 (Disabled)
    }
    icproj.WriteLine("\t\t\t\tOptimization=\"" + opt + "\"");
    var includesArray = new Array();
    for (i in this.includeDirectories)
    {
        if (!this.includeDirectories[i])
        {
            var findex = i.indexOf(varSign);
            if (findex == 0)
                includesArray.push(i.substr(varSign.length));
            else
                includesArray.push(outDir + i);
        }
    }
    icproj.WriteLine("\t\t\t\tAdditionalIncludeDirectories=\""
                        + xmlencode(includesArray.join(";")) + "\"");
    
    var defsArray = new Array();
    for (i in this.defines)
    {
        if (!this.defines[i])
        {
            defsArray.push(i);
        }
    }
    icproj.WriteLine("\t\t\t\tPreprocessorDefinitions=\""
                    + xmlencode(defsArray.join(";")) + "\"");
    icproj.WriteLine("\t\t\t\tMinimalRebuild=\""
                    + this.minimalRebuild.toString().toUpperCase() + "\"");
    icproj.WriteLine("\t\t\t\tExceptionHandling=\""
                    + this.exceptions.toString().toUpperCase() + "\"");
                        
    //TODO: customize following 2 lines later
    //icproj.WriteLine("\t\t\t\tBasicRuntimeChecks=\"3\"");
    icproj.WriteLine("\t\t\t\tDetect64BitPortabilityProblems=\"FALSE\"");
    
    icproj.WriteLine("\t\t\t\tSuppressStartupBanner=\""
                        + this.nologo.toString().toUpperCase() + "\"");
    
    var runtime = "0";
    switch (this.runtime.toUpperCase())
    {
        case "SINGLEDEBUG":
            runtime = "5";
            break;
        case "SINGLE":
            runtime = "4";
            break;
        case "MULTIDEBUGDLL":
            runtime = "3";
            break;
        case "MULTIDLL":
            runtime = "2";
            break;
        case "MULTIDEBUG":
            runtime = "1";
            break;
        default:
            // Multi, 0
    }
    icproj.WriteLine("\t\t\t\tRuntimeLibrary=\"" + runtime + "\"");
    if (this.pchBuildFrom.length > 0)
    {
        icproj.WriteLine("\t\t\t\tPrecompiledHeaderThrough=\""
                            + xmlencode(this.pchBuildFrom) + "\"");
    }
    if (this.pchName.length > 0)
    {
        icproj.WriteLine("\t\t\t\tPrecompiledHeaderFile=\""
                            + xmlencode(this.pchName) + "\"");
    }
    var usePh = "0";
    switch (this.precompiledHeader.toUpperCase())
    {
    case "USE":
        usePh = "3";
        break;
    case "AUTO":
        usePh = "2";
        break;
    case "CREATE":
        usePh = "1";
        break;
    default:
        // No, 0        
    }
    icproj.WriteLine("\t\t\t\tUsePrecompiledHeader=\"" + usePh + "\"");
    
    if (this.pdbName.length > 0)
    {
        icproj.WriteLine("\t\t\t\tProgramDataBaseFileName=\""
            + xmlencode(this.getIntermDir() + "\\" + this.pdbName) + "\"");
    }
    
    icproj.WriteLine("\t\t\t\tWarningLevel=\""
                        + this.warningsLevel + "\"");
    var diFormat = "0";
    switch (this.debugInfoFormat.toUpperCase())
    {
        case "C7":
            diFormat = "1";
            break;
        case "LINESONLY":
            diFormat = "2";
            break;
        case "PDB":
            diFormat = "3";
            break;
        case "EDITCONTINUE":
            diFormat = "4";
            break;
        default:
            // Disabled, 0
    }   
    icproj.WriteLine("\t\t\t\tObjectFile=\""
        + xmlencode(this.getIntermDir() + "\\" + this.outputFile) + "\"");

    icproj.WriteLine(
        "\t\t\t\tDebugInformationFormat=\"" + diFormat + "\"/>");   
}


function getPreprocessCommandLineCompilerICC()
{
    return this.getCommandLine() + " /E";
}

// compilerICC class methods
CompilerICC.prototype.cloneTo = genericCloneTo;
CompilerICC.prototype.clone = genericClone;
CompilerICC.prototype.getOutputDir = getOutputDir;
CompilerICC.prototype.getIntermDir = getIntermDir;
CompilerICC.prototype.getCommandLine = getCommandLineCompilerICC;
CompilerICC.prototype.getPreprocessCommandLine =
                        getPreprocessCommandLineCompilerICC;
CompilerICC.prototype.writeSolutionSettings =
                        writeSolutionSettingsCompilerICC;

//------------------------------------------------
// LinkerICC class
//------------------------------------------------

// ICC linker options
// incremental = Yes (2, /INCREMENTAL) | 
//      No (1, /INCREMENTAL:NO) | Default (0, "")
// debugInfo = true (TRUE, /DEBUG) | false (FALSE, "")
// pdbFile = <file name w/o directory>
// subsystem = Default (0, "") | Console (1, /SUBSYSTEM:CONSOLE)
//              | Windows (2, /SUBSYSTEM:WINDOWS)
// optimizeRefs = Remove (2, /OPT:REF) | 
//      Leave (1, /OPT:NOREF) | Default (0, "")
// comdatFolding = Remove (2, /OPT:ICF) | 
//      Leave (1, /OPT:NOICF) | Default (0, "")
// machine = X86 (1, /MACHINE:X86) | Default (0, "")
// nologo = true (TRUE, /nologo) | FALSE (FALSE, "")
// isDLL = true (?, /DLL) | false (?, "") 
//determined by a configuration type in solution!

// .ctor from base linker
function LinkerICC(linkerBase)
{
    if (!linkerBase)
    {
        linkerBase = new Linker();
    }
    linkerBase.cloneTo(this);
    this.incremental = "Default";
    this.debugInfo = true;
    this.pdbFile = "";
    this.subsystem = "";
    this.optimizeRef = "";
    this.comdatFolding = "";
    this.machine = "X86";
    this.nologo = true;
    this.isDLL = false;

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
    this.getOutputDir = getOutputDir;
    this.getIntermDir = getIntermDir;
    this.getCommandLine = getCommandLineLinkerICC;
}

function getCommandLineLinkerICC()
{
    var cmdLine = "xilink -qnoipo";
    // nologo
    if (this.nologo)
    {
        cmdLine += " /NOLOGO";
    }
    if (this.isDLL)
    {
        cmdLine += " /DLL";
    }
    // machine
    switch (this.machine.toUpperCase())
    {
    case "X86":
        cmdLine += " /MACHINE:X86";
        break;
    default:
        // do nothing
    }
    // comdatFolding
    switch (this.comdatFolding.toUpperCase())
    {
    case "REMOVE":
        cmdLine += " /OPT:ICF";
        break;
    case "LEAVE":
        cmdLine += " /OPT:NOICF";
        break;
    default:
        // do nothing
    }
    // optimizeRef
    switch (this.optimizeRef.toUpperCase())
    {
    case "REMOVE":
        cmdLine += " /OPT:REF";
        break;
    case "LEAVE":
        cmdLine += " /OPT:NOREF";
        break;
    default:
        // do nothing
    }
    // subsystem
    switch (this.subsystem.toUpperCase())
    {
    case "WINDOWS":
    case "CONSOLE":
        cmdLine += " /SUBSYSTEM:" + this.subsystem.toUpperCase();
        break;
    default:
        // do nothing
    }
    // debug info
    if (this.debugInfo)
    {
        cmdLine += " /DEBUG";
    }
    // incremental
    switch (this.incremental.toUpperCase())
    {
    case "YES":
        cmdLine += " /INCREMENTAL";
        break;
    case "NO":
        cmdLine += " /INCREMENTAL:NO";
        break;
    default:
        // do nothing
    }
    // PDB
    var intDir = this.getIntermDir() + "\\";
    if (this.pdbFile.length > 0)
    {
        cmdLine += " /PDB:\"" + intDir + this.pdbFile + "\"";
    }
    // output
    var outDir = this.getOutputDir() + "\\";
    if (this.outputFile.length > 0)
    {
        cmdLine += " /OUT:\"" + outDir + this.outputFile + "\"";
    }
    // inputs
    cmdLine += buildOptionsVC(this.inputFiles, "");
    cmdLine += buildOptionsVC(this.libraries, "");
    return cmdLine;
}

function writeSolutionSettingsLinkerICC(icproj, outDir)
{
    icproj.WriteLine("\t\t\t<Tool")
    icproj.WriteLine("\t\t\t\tName=\"LinkerTool\"");
    var inc = "0";
    switch (this.incremental.toUpperCase())
    {
        case "YES":
            inc = "2";
            break;
        case "NO":
            inc = "1";
            break;
        default:        
            // leave 0
    }
    icproj.WriteLine("\t\t\t\tLinkIncremental=\"" + inc + "\"");
    icproj.WriteLine("\t\t\t\tSuppressStartupBanner=\""
                        + this.nologo.toString().toUpperCase() + "\"");
    icproj.WriteLine("\t\t\t\tGenerateDebugInformation=\""
                        + this.debugInfo.toString().toUpperCase() + "\"");
    var subsys = "0";
    switch (this.subsystem.toUpperCase())
    {
        case "WINDOWS":
            subsys = "2";
            break;
        case "CONSOLE":
            subsys = "1";
            break;
        default:
            // leave as it is
    }
    icproj.WriteLine("\t\t\t\tSubSystem=\"" + subsys + "\"");
    var opt = "0";
    switch (this.optimizeRef.toUpperCase())
    {
        case "REMOVE":
            opt = "2";
            break;
        case "LEAVE":
            opt = "1";
            break;
        default:
            // leave as it is
    }   
    icproj.WriteLine("\t\t\t\tOptimizeReferences=\"" + opt + "\"");
    opt = "0";
    switch (this.comdatFolding.toUpperCase())
    {
        case "REMOVE":
            opt = "2";
            break;
        case "LEAVE":
            opt = "1";
            break;
        default:
            // leave as it is
    }   
    icproj.WriteLine("\t\t\t\tEnableCOMDATFolding=\"" + opt + "\"");
    icproj.WriteLine("\t\t\t\tTargetMachine=\""
                        + (this.machine.toUpperCase() == "X86" ?
                                "1" : "0") + "\""); 
    if (this.pdbFile.length > 0)
    {
        icproj.WriteLine("\t\t\t\tProgramDatabaseFile=\""
                    + xmlencode(this.getOutputDir() + "\\" + this.pdbFile)
                    + "\"");
    }
    
    icproj.WriteLine("\t\t\t\tOutputFile=\""
        + xmlencode(this.getOutputDir() + "\\" + this.outputFile)
        + "\"/>");
}

// linkerICC class methods
LinkerICC.prototype.cloneTo = genericCloneTo;
LinkerICC.prototype.clone = genericClone;
LinkerICC.prototype.getOutputDir = getOutputDir;
LinkerICC.prototype.getIntermDir = getIntermDir;
LinkerICC.prototype.getCommandLine = getCommandLineLinkerICC;
LinkerICC.prototype.writeSolutionSettings =
                    writeSolutionSettingsLinkerICC;

//------------------------------------------------
// LibrarianICC class
//------------------------------------------------

// ICC librarian options
// nologo = true (TRUE, /nologo) | FALSE (FALSE, "")

// .ctor from base librarian
function LibrarianICC(librarianBase)
{
    if (!librarianBase)
    {
        librarianBase = new Librarian();
    }
    librarianBase.cloneTo(this);
    this.nologo = true;

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
    this.getOutputDir = getOutputDir;
    this.getCommandLine = getCommandLineLibrarianICC;
}

function getCommandLineLibrarianICC()
{
    var cmdLine = "xilib";
    //nologo
    if (this.nologo)
    {
        cmdLine += " /NOLOGO";
    }
    //output
    if (this.outputFile.length > 0)
    {
        cmdLine += " /OUT:\"" + this.getOutputDir() + "\\"
            + this.outputFile + "\"";
    }
    //inputs
    cmdLine += buildOptionsVC(this.inputFiles, "");
    return cmdLine;
}

function writeSolutionSettingsLibrarianICC(icproj, outDir)
{
    icproj.WriteLine("\t\t\t<Tool")
    icproj.WriteLine("\t\t\t\tName=\"LibrarianTool\"");
    icproj.WriteLine("\t\t\t\tOutputFile=\""
        + xmlencode(this.getOutputDir() + "\\" + this.outputFile)
        + "\"/>");
}

// librarianICC class methods
LibrarianICC.prototype.cloneTo = genericCloneTo;
LibrarianICC.prototype.clone = genericClone;
LibrarianICC.prototype.getOutputDir = getOutputDir;
LibrarianICC.prototype.getIntermDir = getIntermDir;
LibrarianICC.prototype.getCommandLine = getCommandLineLibrarianICC;
LibrarianICC.prototype.writeSolutionSettings =
                        writeSolutionSettingsLibrarianICC;


// CustomBuildICC class
// .ctor from base CustomBuild
function CustomBuildICC(customBase)
{
    if (!customBase)
    {
        customBase = new CustomBuild();
    }
    customBase.cloneTo(this);

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
}

function writeSolutionSettingsCustomICC(icproj, outDir)
{
    icproj.WriteLine("\t\t\t<Tool")
    icproj.WriteLine("\t\t\t\tName=\"CustomTool\"");
    icproj.Write("\t\t\t\tCommandLine=\""
                    + xmlencode(this.command) + "\"");
    if (this.output.length > 0)
    {
        icproj.WriteLine("");
        icproj.Write("\t\t\t\tOutputs=\"" + this.output + "\"");
    }
    var deps = new Array();
    for (i in this.dependencies)
    {
        if (!this.dependencies[i])
        {
            deps.push(i);
        }
    }
    if (deps.length > 0)
    {
        icproj.WriteLine("");
        icproj.Write("\t\t\t\tAdditionalDependencies=\""
                        + deps.join(";") + "\"");
    }
    icproj.WriteLine("/>");
}

CustomBuildICC.prototype.cloneTo = genericCloneTo;
CustomBuildICC.prototype.clone = genericClone;
CustomBuildICC.prototype.writeSolutionSettings =
                            writeSolutionSettingsCustomICC;

// encodes special xml characters
function xmlencode(str)
{
    return str.replace(/\"/g, "&quot;");
}

//------------------------------------------------
// FakeLinkerICC class
// Used in Configure project just to set "Ignore Import library" option
//------------------------------------------------

// .ctor from base linker
function FakeLinkerICC()
{
    this.base = Tool;
    this.base(linkerToolName);
}

function writeSolutionSettingsFakeLinkerICC(icproj, outDir)
{
    icproj.WriteLine("\t\t\t<Tool")
    icproj.WriteLine("\t\t\t\tName=\"LinkerTool\"");
    icproj.WriteLine("\t\t\t\tIgnoreImportLibrary=\"TRUE\"/>");
}

// FakeLinkerICC class methods
FakeLinkerICC.prototype.cloneTo = genericCloneTo;
FakeLinkerICC.prototype.clone = genericClone;
FakeLinkerICC.prototype.writeSolutionSettings =
            writeSolutionSettingsFakeLinkerICC;
            
            
//------------------------------------------------
// PostBuildICC class
// Used in Examples projects to copy necessary files
//------------------------------------------------
// .ctor from base PostBuild
function PostBuildICC(postBase)
{
    if (!postBase)
    {
        postBase = new PostBuild();
    }
    postBase.cloneTo(this);

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
}

function writeSolutionSettingsPostBuildICC(icproj, outDir)
{
    icproj.WriteLine("\t\t\t<Tool")
    icproj.WriteLine("\t\t\t\tName=\"PostBuildEventTool\"");
    icproj.Write("\t\t\t\tCommandLine=\"");
    for (i in this.commands)
    {
        var index = i.indexOf("$");
        if (index != -1)
        {
            icproj.WriteLine(xmlencode(this.commands[i]));
        }
    }
    icproj.Write("\"");
    icproj.WriteLine("/>");
}

PostBuildICC.prototype.cloneTo = genericCloneTo;
PostBuildICC.prototype.clone = genericClone;
PostBuildICC.prototype.writeSolutionSettings =
                            writeSolutionSettingsPostBuildICC;
                            
                            
//------------------------------------------------
// CompilerICC90 class
//------------------------------------------------

// ICC 9.0 compiler options

// .ctor from base compiler
function CompilerICC90(compilerICC)
{
    if (!compilerICC)
    {
        compilerICC = new CompilerICC();
    }   
    compilerICC.cloneTo(this);
    //TODO: add
    // basicRuntimeChecks
    // detect64BitProblems = true | false
    
    this.cloneTo = genericCloneTo;
    this.clone = genericClone;  
    this.getOutputDir = getOutputDir;
    this.getIntermDir = getIntermDir;
    //TODO: change this line when new options will be added
    this.getCommandLine = getCommandLineCompilerICC;
    this.getPreprocessCommandLine = getPreprocessCommandLineCompilerICC;
    
}

//compilerICC90 class methods
CompilerICC90.prototype.cloneTo = genericCloneTo;
CompilerICC90.prototype.clone = genericClone;
CompilerICC90.prototype.getOutputDir = getOutputDir;
CompilerICC90.prototype.getIntermDir = getIntermDir;
//TODO: change this line when new options will be added
CompilerICC90.prototype.getCommandLine = getCommandLineCompilerICC;
CompilerICC90.prototype.getPreprocessCommandLine =
    getPreprocessCommandLineCompilerICC;
CompilerICC90.prototype.writeSolutionSettings =
                        writeSolutionSettingsCompilerICC;
