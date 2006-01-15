//
// $Id$
//
// config.js - base classes for configurations support
//
//////////////////////////////////////////////////////////////////////

//------------------------------------------------
//
// Common functions
//
//------------------------------------------------

// creates a copy of an object
function genericClone()
{
    return this.cloneTo(new Object(), true);
}

// copy all properties and optionally methods to a target object
function genericCloneTo(target, cloneMethods)
{
    //WScript.Echo(this);
    for (i in this)
    {
        var type = typeof(this[i]);
        //WScript.Echo(i + " " + type);
        switch (type)
        {
        case "object":
            target[i] = this[i].clone();
            break;
        case "number":
        case "boolean":
        case "undefined":
            target[i] = this[i];
            break;
        case "function":
            if (cloneMethods)
            {
                target[i] = this[i];
            }
            break;
        case "string":
            target[i] = String(this[i]);
            break;
        default:
            throw "Unknown property type.";
        }
    }
    return target;
}

//------------------------------------------------
// Associative Collection class
//------------------------------------------------

// Collection class .ctor
function Collection() {}

// Adds specified element to a collection of solutions
function addElement(name, element)
{
    this[name] = element;
}

// Removes specified element from a collection of solutions
function removeElement(name)
{
    delete this[name];
}

// Gets specified element from a collection of solutions
function getElement(name)
{
    return this[name];
}

// Collection class methods
Collection.prototype.cloneTo = genericCloneTo;
Collection.prototype.clone = genericClone;
Collection.prototype.add = addElement;
Collection.prototype.func_remove = removeElement;
Collection.prototype.get = getElement;

//------------------------------------------------
// Solution class
//------------------------------------------------

// Solution .ctor
function Solution(name)
{
    this.name = String(name);
    this.configurations = new Collection();
    this.projects = new Collection();
    this.version = "7.10";
    this.formatVersion = "8.00";
}

// perform shallow cloning of a solution
function solutionShallowClone()
{
    return new Solution(this.name); 
}

// Solution class methods
Solution.prototype.cloneTo = genericCloneTo;
Solution.prototype.clone = genericClone;
Solution.prototype.shallowClone = solutionShallowClone;

//------------------------------------------------
// SolutionConfiguration class
//------------------------------------------------

// SolutionConfiguration .ctor
function SolutionConfiguration(name)
{
    this.name = String(name);
    this.projectConfigurations = new Collection();
}

// SolutionConfiguration class methods
SolutionConfiguration.prototype.cloneTo = genericCloneTo;
SolutionConfiguration.prototype.clone = genericClone;

//------------------------------------------------
// ProjectConfiguration class
//------------------------------------------------

// ProjectConfiguration .ctor
function ProjectConfiguration(projectName, platform, configuration)
{
    this.projectName = String(projectName);
    this.platform = String(platform);
    this.configuration = String(configuration);
}

// ProjectConfiguration class methods
ProjectConfiguration.prototype.cloneTo = genericCloneTo;
ProjectConfiguration.prototype.clone = genericClone;

//------------------------------------------------
// Project class
//------------------------------------------------

// Project .ctor
function Project(name)
{
    this.name = String(name);
    this.dependencies = new Collection();
    this.platforms = new Collection();
    this.id = "";
    this.sourceFiles = new Collection();
    this.folder = "";
}

// creates a shallow copy of a project
function projectShallowClone()
{
    var result = new Project(this.name);
    result.id = this.id;
    result.dependencies = this.dependencies.clone();
    result.sourceFiles = this.sourceFiles.clone();
    result.folder = this.folder;
    return result;
}

// Project class methods
Project.prototype.cloneTo = genericCloneTo;
Project.prototype.clone = genericClone;
Project.prototype.shallowClone = projectShallowClone;


// Filter class
function Filter(name)
{
    this.name = String(name);
    this.sourceFiles = new Collection();
    this.filter = "";
    this.id = "";
}

Filter.prototype.cloneTo = genericCloneTo;
Filter.prototype.clone = genericClone;

//------------------------------------------------
// Platform class
//------------------------------------------------

// Platform .ctor
function Platform(name)
{
    this.name = String(name);
    this.configurations = new Collection();
}

// creates a shallow copy of a platform
function platformShallowClone()
{
    return new Platform(this.name);
}

// Platform class methods
Platform.prototype.cloneTo = genericCloneTo;
Platform.prototype.clone = genericClone;
Platform.prototype.shallowClone = platformShallowClone;

//------------------------------------------------
// Configuration class
//------------------------------------------------

// possible configuration types are EXE, LIB and DLL
var configTypeExe = "EXE";
var configTypeDll = "DLL";
var configTypeLib = "LIB";


// Configuration .ctor
function Configuration(name, type, outputDir, intermDir)
{
    this.name = String(name);
    this.type = type;   
    this.tools = new Collection();
    
    if (! outputDir || outputDir == "")
        this.outputDir = String(name);
    else
        this.outputDir = String(outputDir);
        
    if (! intermDir || intermDir == "")
        this.intermDir = String(name);
    else
        this.intermDir = String(intermDir);
}

// creates a shallow copy of Configuration
function configurationShallowClone()
{
    return new Configuration(this.name, this.type, 
        this.outputDir, this.intermDir);
}

// Configuration class methods
Configuration.prototype.cloneTo = genericCloneTo;
Configuration.prototype.clone = genericClone;
Configuration.prototype.shallowClone = configurationShallowClone;

//------------------------------------------------
// Tool class
//------------------------------------------------

// Tool .ctor
function Tool(name)
{
    this.name = String(name);
    this.inputFiles =  new Collection();
    this.outputDirectory = "";
    this.intermDirectory = "";
    this.outputFile = "";
}

// getCommandLine could not be implemented here

// Tool class methods
Tool.prototype.cloneTo = genericCloneTo;
Tool.prototype.clone = genericClone;

// Tools names
var compilerToolName = "compiler";
var linkerToolName = "linker";
var librarianToolName = "librarian";
var customBuildToolName = "custom";
var postBuildToolName = "postbuild";

//------------------------------------------------
// Compiler class
//------------------------------------------------

// Compiler .ctor
function Compiler()
{
    this.base = Tool;
    this.base(compilerToolName);
    this.isDebug = false;
    this.defines = new Collection();
    this.includeDirectories = new Collection();
}

// getPreprocessCommandLine could not be implemented here

// Compiler class methods
Compiler.prototype.cloneTo = genericCloneTo;
Compiler.prototype.clone = genericClone;

//------------------------------------------------
// Linker class
//------------------------------------------------

// Linker .ctor
function Linker()
{
    this.base = Tool;
    this.base(linkerToolName);
    this.libraries = new Collection();
    this.libdirs = new Collection();
}

// Linker class methods
Linker.prototype.cloneTo = genericCloneTo;
Linker.prototype.clone = genericClone;

//------------------------------------------------
// Librarian class
//------------------------------------------------

// Librarian .ctor
function Librarian()
{
    this.base = Tool;
    this.base(librarianToolName);
    this.libraries = new Collection();
}

// Librarian class methods
Librarian.prototype.cloneTo = genericCloneTo;
Librarian.prototype.clone = genericClone;

//------------------------------------------------
// CustomBuild class
//------------------------------------------------

//CustomBuild .ctor
function CustomBuild()
{
    this.base = Tool;
    this.base(customBuildToolName);
    this.command = "";
    this.output = "";
    this.dependencies = new Collection();
}

//CustomBuild class methods
CustomBuild.prototype.cloneTo = genericCloneTo;
CustomBuild.prototype.clone = genericClone;


//------------------------------------------------
// PostBuild class
//------------------------------------------------

//PostBuild .ctor
function PostBuild()
{
    this.base = Tool;
    this.base(postBuildToolName);
    this.commands = new Collection();
}

//PostBuild class methods
PostBuild.prototype.cloneTo = genericCloneTo;
PostBuild.prototype.clone = genericClone;


//------------------------------------------------
// ItemBuildInfo class
//------------------------------------------------

// ItemBuildInfo .ctor
function ItemBuildInfo(name)
{
    this.name = String(name);
    this.buildCmdLog = "";
    this.buildOutLog = "";
    this.errorsCnt = "0";
    this.warningsCnt = "0";
    this.linkerErrors = false;
    
    this.runOutput = "";
    this.runReqOutput = "";
    this.runDiff = "";
    this.exitCode = 0;
}

// ItemBuildInfo class methods
ItemBuildInfo.prototype.cloneTo = genericCloneTo;
ItemBuildInfo.prototype.clone = genericClone;

