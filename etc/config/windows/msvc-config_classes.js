//
// $Id$
//
// file should be included after base_config_classes

// VC compiler, linker and librarian

var varSign = "$$";

// builds standard string of inputs from a collection
function buildOptionsVC(collection, prefix)
{
    var options = "";
    for (i in collection)
    {
        if (typeof(collection[i]) != "function")
        {
            options += " " + prefix + "\"" + i + "\"";
        }
    }
    return options;
}

// returns outputDirectory with the (back)slash at end
function getOutputDir()
{
    return this.outputDirectory;
}

// returns intermDirectory with the (back)slash at end
function getIntermDir()
{
    return this.intermDirectory;
}

//------------------------------------------------
// CompilerVC class
//------------------------------------------------

// VC compiler options
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
function CompilerVC(compilerBase)
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
    this.getCommandLine = getCommandLineCompilerVC;
    this.getPreprocessCommandLine = getPreprocessCommandLineCompilerVC;
}

function getCommandLineCompilerVC()
{
    var cmdLine = "cl /c";
    // nologo
    if (this.nologo)
    {
        cmdLine += " /nologo";
    }
    // defines
    cmdLine += buildOptionsVC(this.defines, "/D ");
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

function writeSolutionSettingsCompilerVC(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCCLCompilerTool\"");
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
    vcproj.WriteLine("\t\t\t\tOptimization=\"" + opt + "\"");
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
    vcproj.WriteLine("\t\t\t\tAdditionalIncludeDirectories=\""
                        + xmlencode(includesArray.join(";")) + "\"");
    
    var defsArray = new Array();
    for (i in this.defines)
    {
        if (!this.defines[i])
        {
            defsArray.push(i);
        }
    }
    vcproj.WriteLine("\t\t\t\tPreprocessorDefinitions=\""
                    + xmlencode(defsArray.join(";")) + "\"");
    vcproj.WriteLine("\t\t\t\tMinimalRebuild=\""
                    + this.minimalRebuild.toString().toUpperCase() + "\"");
    vcproj.WriteLine("\t\t\t\tExceptionHandling=\""
                    + this.exceptions.toString().toUpperCase() + "\"");
                        
    //TODO: customize following 2 lines later
    //vcproj.WriteLine("\t\t\t\tBasicRuntimeChecks=\"3\"");
    vcproj.WriteLine("\t\t\t\tDetect64BitPortabilityProblems=\"FALSE\"");
    
    vcproj.WriteLine("\t\t\t\tSuppressStartupBanner=\""
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
    vcproj.WriteLine("\t\t\t\tRuntimeLibrary=\"" + runtime + "\"");
    if (this.pchBuildFrom.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tPrecompiledHeaderThrough=\""
                            + xmlencode(this.pchBuildFrom) + "\"");
    }
    if (this.pchName.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tPrecompiledHeaderFile=\""
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
    vcproj.WriteLine("\t\t\t\tUsePrecompiledHeader=\"" + usePh + "\"");
    
    if (this.pdbName.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tProgramDataBaseFileName=\""
            + xmlencode(this.getIntermDir() + "\\" + this.pdbName) + "\"");
    }
    
    vcproj.WriteLine("\t\t\t\tWarningLevel=\""
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
    vcproj.WriteLine("\t\t\t\tObjectFile=\""
        + xmlencode(this.getIntermDir() + "\\" + this.outputFile) + "\"");

    vcproj.WriteLine(
        "\t\t\t\tDebugInformationFormat=\"" + diFormat + "\"/>");   
}


function getPreprocessCommandLineCompilerVC()
{
    return this.getCommandLine() + " /E";
}

// compilerVC class methods
CompilerVC.prototype.cloneTo = genericCloneTo;
CompilerVC.prototype.clone = genericClone;
CompilerVC.prototype.getOutputDir = getOutputDir;
CompilerVC.prototype.getIntermDir = getIntermDir;
CompilerVC.prototype.getCommandLine = getCommandLineCompilerVC;
CompilerVC.prototype.getPreprocessCommandLine =
                        getPreprocessCommandLineCompilerVC;
CompilerVC.prototype.writeSolutionSettings =
                        writeSolutionSettingsCompilerVC;

//------------------------------------------------
// LinkerVC class
//------------------------------------------------

// VC linker options
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
function LinkerVC(linkerBase)
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
    this.getCommandLine = getCommandLineLinkerVC;
}

function getCommandLineLinkerVC()
{
    var cmdLine = "link";
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

function writeSolutionSettingsLinkerVC(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCLinkerTool\"");
    
    // additional libraries, if any
    var addlibsArray = new Array();
    for (al in this.libraries)
    {
        if (!this.libraries[al])
        {
            addlibsArray.push(al);
        }
    }
    if (addlibsArray.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tAdditionalDependencies=\""
                        + xmlencode(addlibsArray.join(" ")) + "\"");
    }
    
    var addlibdirsArray = new Array();
    for (ald in this.libdirs)
    {
        if (!this.libdirs[ald])
        {
            addlibdirsArray.push(ald);
        }
    }
    if (addlibdirsArray.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tAdditionalLibraryDirectories=\""
                        + xmlencode(addlibdirsArray.join(";")) + "\"");
    }
    
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
    vcproj.WriteLine("\t\t\t\tLinkIncremental=\"" + inc + "\"");
    vcproj.WriteLine("\t\t\t\tSuppressStartupBanner=\""
                        + this.nologo.toString().toUpperCase() + "\"");
    vcproj.WriteLine("\t\t\t\tGenerateDebugInformation=\""
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
    vcproj.WriteLine("\t\t\t\tSubSystem=\"" + subsys + "\"");
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
    vcproj.WriteLine("\t\t\t\tOptimizeReferences=\"" + opt + "\"");
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
    vcproj.WriteLine("\t\t\t\tEnableCOMDATFolding=\"" + opt + "\"");
    vcproj.WriteLine("\t\t\t\tTargetMachine=\""
                        + (this.machine.toUpperCase() == "X86" ?
                                "1" : "0") + "\""); 
    if (this.pdbFile.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tProgramDatabaseFile=\""
                    + xmlencode(this.getOutputDir() + "\\" + this.pdbFile)
                    + "\"");
    }
    
    vcproj.WriteLine("\t\t\t\tOutputFile=\""
        + xmlencode(this.getOutputDir() + "\\" + this.outputFile)
        + "\"/>");
}

// linkerVC class methods
LinkerVC.prototype.cloneTo = genericCloneTo;
LinkerVC.prototype.clone = genericClone;
LinkerVC.prototype.getOutputDir = getOutputDir;
LinkerVC.prototype.getIntermDir = getIntermDir;
LinkerVC.prototype.getCommandLine = getCommandLineLinkerVC;
LinkerVC.prototype.writeSolutionSettings =
                    writeSolutionSettingsLinkerVC;

//------------------------------------------------
// LibrarianVC class
//------------------------------------------------

// VC librarian options
// nologo = true (TRUE, /nologo) | FALSE (FALSE, "")

// .ctor from base librarian
function LibrarianVC(librarianBase)
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
    this.getCommandLine = getCommandLineLibrarianVC;
}

function getCommandLineLibrarianVC()
{
    var cmdLine = "lib";
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

function writeSolutionSettingsLibrarianVC(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCLibrarianTool\"");
    vcproj.WriteLine("\t\t\t\tOutputFile=\""
        + xmlencode(this.getOutputDir() + "\\" + this.outputFile)
        + "\"/>");
}

// librarianVC class methods
LibrarianVC.prototype.cloneTo = genericCloneTo;
LibrarianVC.prototype.clone = genericClone;
LibrarianVC.prototype.getOutputDir = getOutputDir;
LibrarianVC.prototype.getIntermDir = getIntermDir;
LibrarianVC.prototype.getCommandLine = getCommandLineLibrarianVC;
LibrarianVC.prototype.writeSolutionSettings =
                        writeSolutionSettingsLibrarianVC;


// CustomBuildVC class
// .ctor from base CustomBuild
function CustomBuildVC(customBase)
{
    if (!customBase)
    {
        customBase = new CustomBuild();
    }
    customBase.cloneTo(this);

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
}

function writeSolutionSettingsCustomVC(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCCustomBuildTool\"");
    vcproj.Write("\t\t\t\tCommandLine=\""
                    + xmlencode(this.command) + "\"");
    if (this.output.length > 0)
    {
        vcproj.WriteLine("");
        vcproj.Write("\t\t\t\tOutputs=\"" + this.output + "\"");
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
        vcproj.WriteLine("");
        vcproj.Write("\t\t\t\tAdditionalDependencies=\""
                        + deps.join(";") + "\"");
    }
    vcproj.WriteLine("/>");
}

CustomBuildVC.prototype.cloneTo = genericCloneTo;
CustomBuildVC.prototype.clone = genericClone;
CustomBuildVC.prototype.writeSolutionSettings =
                            writeSolutionSettingsCustomVC;

// encodes special xml characters
function xmlencode(str)
{
    return str.replace(/\"/g, "&quot;");
}

//------------------------------------------------
// FakeLinkerVC class
// Used in Configure project just to set "Ignore Import library" option
//------------------------------------------------

// .ctor from base linker
function FakeLinkerVC()
{
    this.base = Tool;
    this.base(linkerToolName);
}

function writeSolutionSettingsFakeLinkerVC(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCLinkerTool\"");
    vcproj.WriteLine("\t\t\t\tIgnoreImportLibrary=\"TRUE\"/>");
}

// FakeLinkerVC class methods
FakeLinkerVC.prototype.cloneTo = genericCloneTo;
FakeLinkerVC.prototype.clone = genericClone;
FakeLinkerVC.prototype.writeSolutionSettings =
            writeSolutionSettingsFakeLinkerVC;
            
            
//------------------------------------------------
// PostBuildVC class
// Used in Examples projects to copy necessary files
//------------------------------------------------
// .ctor from base PostBuild
function PostBuildVC(postBase)
{
    if (!postBase)
    {
        postBase = new PostBuild();
    }
    postBase.cloneTo(this);

    this.cloneTo = genericCloneTo;
    this.clone = genericClone;
}

function writeSolutionSettingsPostBuildVC(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCPostBuildEventTool\"");
    vcproj.Write("\t\t\t\tCommandLine=\"");
    for (i in this.commands)
    {
        var index = i.indexOf("$");
        if (index != -1)
        {
            vcproj.WriteLine(xmlencode(this.commands[i]));
        }
    }
    vcproj.Write("\"");
    vcproj.WriteLine("/>");
}

PostBuildVC.prototype.cloneTo = genericCloneTo;
PostBuildVC.prototype.clone = genericClone;
PostBuildVC.prototype.writeSolutionSettings =
                            writeSolutionSettingsPostBuildVC;
                            
                            
//------------------------------------------------
// CompilerVC71 class
//------------------------------------------------

// VC 7.1 compiler options
// todo: add these options
// basicRuntimeChecks
// detect64BitProblems = true | false

// .ctor from base compiler
function CompilerVC71(compilerVC)
{
    if (!compilerVC)
    {
        compilerVC = new CompilerVC();
    }   
    compilerVC.cloneTo(this);
    //TODO: add
    // basicRuntimeChecks
    // detect64BitProblems = true | false
    
    this.cloneTo = genericCloneTo;
    this.clone = genericClone;  
    this.getOutputDir = getOutputDir;
    this.getIntermDir = getIntermDir;
    //TODO: change this line when new options will be added
    this.getCommandLine = getCommandLineCompilerVC;
    this.getPreprocessCommandLine = getPreprocessCommandLineCompilerVC;
    
}

//compilerVC class methods
CompilerVC71.prototype.cloneTo = genericCloneTo;
CompilerVC71.prototype.clone = genericClone;
CompilerVC71.prototype.getOutputDir = getOutputDir;
CompilerVC71.prototype.getIntermDir = getIntermDir;
//TODO: change this line when new options will be added
CompilerVC71.prototype.getCommandLine = getCommandLineCompilerVC;
CompilerVC71.prototype.getPreprocessCommandLine =
    getPreprocessCommandLineCompilerVC;
CompilerVC71.prototype.writeSolutionSettings =
                        writeSolutionSettingsCompilerVC;
                        
                        
//------------------------------------------------
// CompilerVC80 class
//------------------------------------------------

function getCommandLineCompilerVC80()
{
    var cmdLine = "cl /c";
    // nologo
    if (this.nologo)
    {
        cmdLine += " /nologo";
    }
    // defines
    cmdLine += buildOptionsVC(this.defines, "/D ");
    // includes
    cmdLine += buildOptionsVC(this.includeDirectories, "/I");
    // warnings
    cmdLine += " /W" + this.warningsLevel;
    // exceptions
    if (this.exceptions)
    {
        cmdLine += " /EHa";
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
        cmdLine += " /MT";
        break;
    default: // SingleDebug
        cmdLine += " /MTd";
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

function writeSolutionSettingsCompilerVC80(vcproj, outDir)
{
    vcproj.WriteLine("\t\t\t<Tool")
    vcproj.WriteLine("\t\t\t\tName=\"VCCLCompilerTool\"");
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
    vcproj.WriteLine("\t\t\t\tOptimization=\"" + opt + "\"");
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
    vcproj.WriteLine("\t\t\t\tAdditionalIncludeDirectories=\""
                        + xmlencode(includesArray.join(";")) + "\"");
    
    var defsArray = new Array();
    for (i in this.defines)
    {
        if (!this.defines[i])
        {
            defsArray.push(i);
        }
    }
    vcproj.WriteLine("\t\t\t\tPreprocessorDefinitions=\""
                    + xmlencode(defsArray.join(";")) + "\"");
    vcproj.WriteLine("\t\t\t\tMinimalRebuild=\""
                    + this.minimalRebuild.toString().toUpperCase() + "\"");
    vcproj.WriteLine("\t\t\t\tExceptionHandling=\""
                    + this.exceptions.toString().toUpperCase() + "\"");
                        
    //TODO: customize following 2 lines later
    //vcproj.WriteLine("\t\t\t\tBasicRuntimeChecks=\"3\"");
    vcproj.WriteLine("\t\t\t\tDetect64BitPortabilityProblems=\"FALSE\"");
    
    vcproj.WriteLine("\t\t\t\tSuppressStartupBanner=\""
                        + this.nologo.toString().toUpperCase() + "\"");
    
    var runtime = "0";
    switch (this.runtime.toUpperCase())
    {
        case "SINGLEDEBUG":
            runtime = "1";
            break;
        case "SINGLE":
            runtime = "0";
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
    vcproj.WriteLine("\t\t\t\tRuntimeLibrary=\"" + runtime + "\"");
    if (this.pchBuildFrom.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tPrecompiledHeaderThrough=\""
                            + xmlencode(this.pchBuildFrom) + "\"");
    }
    if (this.pchName.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tPrecompiledHeaderFile=\""
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
    vcproj.WriteLine("\t\t\t\tUsePrecompiledHeader=\"" + usePh + "\"");
    
    if (this.pdbName.length > 0)
    {
        vcproj.WriteLine("\t\t\t\tProgramDataBaseFileName=\""
            + xmlencode(this.getIntermDir() + "\\" + this.pdbName) + "\"");
    }
    
    vcproj.WriteLine("\t\t\t\tWarningLevel=\""
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
    vcproj.WriteLine("\t\t\t\tObjectFile=\""
        + xmlencode(this.getIntermDir() + "\\" + this.outputFile) + "\"");

    vcproj.WriteLine(
        "\t\t\t\tDebugInformationFormat=\"" + diFormat + "\"/>");   
}

// VC 8.0 compiler options
// todo: add these options
// basicRuntimeChecks
// detect64BitProblems = true | false

// .ctor from base compiler
function CompilerVC80(compilerVC)
{
    if (!compilerVC)
    {
        compilerVC = new CompilerVC();
    }   
    compilerVC.cloneTo(this);
    //TODO: add
    // basicRuntimeChecks
    // detect64BitProblems = true | false
    
    this.cloneTo = genericCloneTo;
    this.clone = genericClone;  
    this.getOutputDir = getOutputDir;
    this.getIntermDir = getIntermDir;
    //TODO: change this line when new options will be added
    this.getCommandLine = getCommandLineCompilerVC80;
    this.getPreprocessCommandLine = getPreprocessCommandLineCompilerVC;
    
}

//compilerVC class methods
CompilerVC80.prototype.cloneTo = genericCloneTo;
CompilerVC80.prototype.clone = genericClone;
CompilerVC80.prototype.getOutputDir = getOutputDir;
CompilerVC80.prototype.getIntermDir = getIntermDir;
//TODO: change this line when new options will be added
CompilerVC80.prototype.getCommandLine = getCommandLineCompilerVC80;
CompilerVC80.prototype.getPreprocessCommandLine =
    getPreprocessCommandLineCompilerVC;
CompilerVC80.prototype.writeSolutionSettings =
                        writeSolutionSettingsCompilerVC80;
