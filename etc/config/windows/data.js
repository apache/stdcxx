//
// $Id$
//
// file should be included after base_config_classes

// base configuration settings

//------------------------------------------------
// global object storing all configurations
//------------------------------------------------
var configurations = new Collection();

// configuration names
var confDebugStaticName = "11s Debug Static";
var confReleaseStaticName = "8s Optimized Static";
var confMTDebugStaticName = "15s Debug Thread-safe Static";
var confMTReleaseStaticName = "12s Optimized Thread-safe Static";
var confDebugDllName = "11d Debug Dll";
var confReleaseDllName = "8d Optimized Dll";
var confMTDebugDllName = "15d Debug Thread-safe Dll";
var confMTReleaseDllName = "12d Optimized Thread-safe Dll";

// configuration output directories
var confDebugStaticOut = "11s";
var confReleaseStaticOut = "8s";
var confMTDebugStaticOut = "15s";
var confMTReleaseStaticOut = "12s";
var confDebugDllOut = "11d";
var confReleaseDllOut = "8d";
var confMTDebugDllOut = "15d";
var confMTReleaseDllOut = "12d";

// map between short and long configuration names
var configsShortToLong = new Collection();
configsShortToLong.add(confDebugStaticOut, confDebugStaticName);
configsShortToLong.add(confReleaseStaticOut, confReleaseStaticName);
configsShortToLong.add(confMTDebugStaticOut, confMTDebugStaticName);
configsShortToLong.add(confMTReleaseStaticOut, confMTReleaseStaticName);
configsShortToLong.add(confDebugDllOut, confDebugDllName);
configsShortToLong.add(confReleaseDllOut, confReleaseDllName);
configsShortToLong.add(confMTDebugDllOut, confMTDebugDllName);
configsShortToLong.add(confMTReleaseDllOut, confMTReleaseDllName);

// platform names
var platformWin32Name = "Win32";

// projects configurations

///////////////////////////////////////////////////////////////////
// Configure project
var projectConfigureName = ".configure";

var projectConfConfigureDebugStatic =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confDebugStaticName);
                            
var projectConfConfigureReleaseStatic =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confReleaseStaticName);
                            
var projectConfConfigureMTDebugStatic =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confMTDebugStaticName);
                            
var projectConfConfigureMTReleaseStatic =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confMTReleaseStaticName);
                            
var projectConfConfigureDebugDll =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confDebugDllName);

var projectConfConfigureReleaseDll =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confReleaseDllName);
                            
var projectConfConfigureMTDebugDll =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confMTDebugDllName);

var projectConfConfigureMTReleaseDll =
    new ProjectConfiguration(projectConfigureName, platformWin32Name
                            , confMTReleaseDllName);
                            
///////////////////////////////////////////////////////////////////
// Examples template project
var projectExamplesName = "Examples";

var projectConfExamplesDebugStatic =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confDebugStaticName);
                            
var projectConfExamplesReleaseStatic =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confReleaseStaticName);
                            
var projectConfExamplesMTDebugStatic =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confMTDebugStaticName);
                            
var projectConfExamplesMTReleaseStatic =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confMTReleaseStaticName);
                            
var projectConfExamplesDebugDll =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confDebugDllName);

var projectConfExamplesReleaseDll =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confReleaseDllName);
                            
var projectConfExamplesMTDebugDll =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confMTDebugDllName);

var projectConfExamplesMTReleaseDll =
    new ProjectConfiguration(projectExamplesName, platformWin32Name
                            , confMTReleaseDllName);
                            
                            
///////////////////////////////////////////////////////////////////
// Tests template project
var projectTestsName = "Tests";

var projectConfTestsDebugStatic =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confDebugStaticName);
                            
var projectConfTestsReleaseStatic =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confReleaseStaticName);
                            
var projectConfTestsMTDebugStatic =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confMTDebugStaticName);
                            
var projectConfTestsMTReleaseStatic =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confMTReleaseStaticName);
                            
var projectConfTestsDebugDll =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confDebugDllName);

var projectConfTestsReleaseDll =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confReleaseDllName);
                            
var projectConfTestsMTDebugDll =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confMTDebugDllName);

var projectConfTestsMTReleaseDll =
    new ProjectConfiguration(projectTestsName, platformWin32Name
                            , confMTReleaseDllName);
                            
///////////////////////////////////////////////////////////////////
// rwtest project
var projectRwTestName = ".rwtest";

var projectConfRwTestDebugStatic =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confDebugStaticName);
                            
var projectConfRwTestReleaseStatic =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confReleaseStaticName);
                            
var projectConfRwTestMTDebugStatic =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confMTDebugStaticName);
                            
var projectConfRwTestMTReleaseStatic =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confMTReleaseStaticName);
                            
var projectConfRwTestDebugDll =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confDebugDllName);

var projectConfRwTestReleaseDll =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confReleaseDllName);
                            
var projectConfRwTestMTDebugDll =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confMTDebugDllName);

var projectConfRwTestMTReleaseDll =
    new ProjectConfiguration(projectRwTestName, platformWin32Name
                            , confMTReleaseDllName);
                            

///////////////////////////////////////////////////////////////////
// run_examples project
var projectRunExamplesName = ".stdlib_examples";

var projectConfRunExamplesDebugStatic =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confDebugStaticName);
                            
var projectConfRunExamplesReleaseStatic =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confReleaseStaticName);
                            
var projectConfRunExamplesMTDebugStatic =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confMTDebugStaticName);
                            
var projectConfRunExamplesMTReleaseStatic =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confMTReleaseStaticName);
                            
var projectConfRunExamplesDebugDll =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confDebugDllName);

var projectConfRunExamplesReleaseDll =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confReleaseDllName);
                            
var projectConfRunExamplesMTDebugDll =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confMTDebugDllName);

var projectConfRunExamplesMTReleaseDll =
    new ProjectConfiguration(projectRunExamplesName, platformWin32Name
                            , confMTReleaseDllName);
                            
///////////////////////////////////////////////////////////////////
// run_tests project
var projectRunTestsName = ".stdlib_tests";

var projectConfRunTestsDebugStatic =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confDebugStaticName);
                            
var projectConfRunTestsReleaseStatic =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confReleaseStaticName);
                            
var projectConfRunTestsMTDebugStatic =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confMTDebugStaticName);
                            
var projectConfRunTestsMTReleaseStatic =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confMTReleaseStaticName);
                            
var projectConfRunTestsDebugDll =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confDebugDllName);

var projectConfRunTestsReleaseDll =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confReleaseDllName);
                            
var projectConfRunTestsMTDebugDll =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confMTDebugDllName);

var projectConfRunTestsMTReleaseDll =
    new ProjectConfiguration(projectRunTestsName, platformWin32Name
                            , confMTReleaseDllName);
                            
////////////////////////////////////////////////////////////////////////
// solution configurations

// debug static configuration
var solutionConfDebugStatic = 
    new SolutionConfiguration(confDebugStaticName);

solutionConfDebugStatic.projectConfigurations.add(
    projectConfConfigureDebugStatic.projectName, 
    projectConfConfigureDebugStatic);
    
solutionConfDebugStatic.projectConfigurations.add(
    projectConfExamplesDebugStatic.projectName, 
    projectConfExamplesDebugStatic);
    
solutionConfDebugStatic.projectConfigurations.add(
    projectConfRwTestDebugStatic.projectName, 
    projectConfRwTestDebugStatic);
    
solutionConfDebugStatic.projectConfigurations.add(
    projectConfTestsDebugStatic.projectName, 
    projectConfTestsDebugStatic);
    
solutionConfDebugStatic.projectConfigurations.add(
    projectConfRunExamplesDebugStatic.projectName, 
    projectConfRunExamplesDebugStatic);
    
solutionConfDebugStatic.projectConfigurations.add(
    projectConfRunTestsDebugStatic.projectName, 
    projectConfRunTestsDebugStatic);
    
    
// release static configuration
var solutionConfReleaseStatic = 
    new SolutionConfiguration(confReleaseStaticName);

solutionConfReleaseStatic.projectConfigurations.add(
    projectConfConfigureReleaseStatic.projectName, 
    projectConfConfigureReleaseStatic);
    
solutionConfReleaseStatic.projectConfigurations.add(
    projectConfExamplesReleaseStatic.projectName, 
    projectConfExamplesReleaseStatic);
    
solutionConfReleaseStatic.projectConfigurations.add(
    projectConfRwTestReleaseStatic.projectName, 
    projectConfRwTestReleaseStatic);
    
solutionConfReleaseStatic.projectConfigurations.add(
    projectConfTestsReleaseStatic.projectName, 
    projectConfTestsReleaseStatic);
    
solutionConfReleaseStatic.projectConfigurations.add(
    projectConfRunExamplesReleaseStatic.projectName, 
    projectConfRunExamplesReleaseStatic);
    
solutionConfReleaseStatic.projectConfigurations.add(
    projectConfRunTestsReleaseStatic.projectName, 
    projectConfRunTestsReleaseStatic);
    
// debug multi-threaded static configuration
var solutionConfMTDebugStatic = 
    new SolutionConfiguration(confMTDebugStaticName);

solutionConfMTDebugStatic.projectConfigurations.add(
    projectConfConfigureMTDebugStatic.projectName, 
    projectConfConfigureMTDebugStatic);
    
solutionConfMTDebugStatic.projectConfigurations.add(
    projectConfExamplesMTDebugStatic.projectName, 
    projectConfExamplesMTDebugStatic);
    
solutionConfMTDebugStatic.projectConfigurations.add(
    projectConfRwTestMTDebugStatic.projectName, 
    projectConfRwTestMTDebugStatic);
    
solutionConfMTDebugStatic.projectConfigurations.add(
    projectConfTestsMTDebugStatic.projectName, 
    projectConfTestsMTDebugStatic);
    
solutionConfMTDebugStatic.projectConfigurations.add(
    projectConfRunExamplesMTDebugStatic.projectName, 
    projectConfRunExamplesMTDebugStatic);
    
solutionConfMTDebugStatic.projectConfigurations.add(
    projectConfRunTestsMTDebugStatic.projectName, 
    projectConfRunTestsMTDebugStatic);
    
// release multi-threaded static configuration
var solutionConfMTReleaseStatic = 
    new SolutionConfiguration(confMTReleaseStaticName);

solutionConfMTReleaseStatic.projectConfigurations.add(
    projectConfConfigureMTReleaseStatic.projectName, 
    projectConfConfigureMTReleaseStatic);
    
solutionConfMTReleaseStatic.projectConfigurations.add(
    projectConfExamplesMTReleaseStatic.projectName, 
    projectConfExamplesMTReleaseStatic);
    
solutionConfMTReleaseStatic.projectConfigurations.add(
    projectConfRwTestMTReleaseStatic.projectName, 
    projectConfRwTestMTReleaseStatic);
    
solutionConfMTReleaseStatic.projectConfigurations.add(
    projectConfTestsMTReleaseStatic.projectName, 
    projectConfTestsMTReleaseStatic);
    
solutionConfMTReleaseStatic.projectConfigurations.add(
    projectConfRunExamplesMTReleaseStatic.projectName, 
    projectConfRunExamplesMTReleaseStatic);
    
solutionConfMTReleaseStatic.projectConfigurations.add(
    projectConfRunTestsMTReleaseStatic.projectName, 
    projectConfRunTestsMTReleaseStatic);
    
// debug dll configuration
var solutionConfDebugDll = 
    new SolutionConfiguration(confDebugDllName);
    
solutionConfDebugDll.projectConfigurations.add(
    projectConfConfigureDebugDll.projectName, 
    projectConfConfigureDebugDll);
    
solutionConfDebugDll.projectConfigurations.add(
    projectConfExamplesDebugDll.projectName, 
    projectConfExamplesDebugDll);
    
solutionConfDebugDll.projectConfigurations.add(
    projectConfRwTestDebugDll.projectName, 
    projectConfRwTestDebugDll);
    
solutionConfDebugDll.projectConfigurations.add(
    projectConfTestsDebugDll.projectName, 
    projectConfTestsDebugDll);
    
solutionConfDebugDll.projectConfigurations.add(
    projectConfRunExamplesDebugDll.projectName, 
    projectConfRunExamplesDebugDll);
    
solutionConfDebugDll.projectConfigurations.add(
    projectConfRunTestsDebugDll.projectName, 
    projectConfRunTestsDebugDll);
    
// release dll configuration
var solutionConfReleaseDll = 
    new SolutionConfiguration(confReleaseDllName);
    
solutionConfReleaseDll.projectConfigurations.add(
    projectConfConfigureReleaseDll.projectName, 
    projectConfConfigureReleaseDll);
    
solutionConfReleaseDll.projectConfigurations.add(
    projectConfExamplesReleaseDll.projectName, 
    projectConfExamplesReleaseDll);
    
solutionConfReleaseDll.projectConfigurations.add(
    projectConfRwTestReleaseDll.projectName, 
    projectConfRwTestReleaseDll);
    
solutionConfReleaseDll.projectConfigurations.add(
    projectConfTestsReleaseDll.projectName, 
    projectConfTestsReleaseDll);
    
solutionConfReleaseDll.projectConfigurations.add(
    projectConfRunExamplesReleaseDll.projectName, 
    projectConfRunExamplesReleaseDll);
    
solutionConfReleaseDll.projectConfigurations.add(
    projectConfRunTestsReleaseDll.projectName, 
    projectConfRunTestsReleaseDll);
    
    
// debug multi-threaded dll configuration
var solutionConfMTDebugDll = 
    new SolutionConfiguration(confMTDebugDllName);
    
solutionConfMTDebugDll.projectConfigurations.add(
    projectConfConfigureMTDebugDll.projectName, 
    projectConfConfigureMTDebugDll);
    
solutionConfMTDebugDll.projectConfigurations.add(
    projectConfExamplesMTDebugDll.projectName, 
    projectConfExamplesMTDebugDll);
    
solutionConfMTDebugDll.projectConfigurations.add(
    projectConfRwTestMTDebugDll.projectName, 
    projectConfRwTestMTDebugDll);
    
solutionConfMTDebugDll.projectConfigurations.add(
    projectConfTestsMTDebugDll.projectName, 
    projectConfTestsMTDebugDll);
    
solutionConfMTDebugDll.projectConfigurations.add(
    projectConfRunExamplesMTDebugDll.projectName, 
    projectConfRunExamplesMTDebugDll);
    
solutionConfMTDebugDll.projectConfigurations.add(
    projectConfRunTestsMTDebugDll.projectName, 
    projectConfRunTestsMTDebugDll);
    
// release multi-threaded dll configuration
var solutionConfMTReleaseDll = 
    new SolutionConfiguration(confMTReleaseDllName);
    
solutionConfMTReleaseDll.projectConfigurations.add(
    projectConfConfigureMTReleaseDll.projectName, 
    projectConfConfigureMTReleaseDll);
    
solutionConfMTReleaseDll.projectConfigurations.add(
    projectConfExamplesMTReleaseDll.projectName, 
    projectConfExamplesMTReleaseDll);
    
solutionConfMTReleaseDll.projectConfigurations.add(
    projectConfRwTestMTReleaseDll.projectName, 
    projectConfRwTestMTReleaseDll);
    
solutionConfMTReleaseDll.projectConfigurations.add(
    projectConfTestsMTReleaseDll.projectName, 
    projectConfTestsMTReleaseDll);
    
solutionConfMTReleaseDll.projectConfigurations.add(
    projectConfRunExamplesMTReleaseDll.projectName, 
    projectConfRunExamplesMTReleaseDll);
    
solutionConfMTReleaseDll.projectConfigurations.add(
    projectConfRunTestsMTReleaseDll.projectName, 
    projectConfRunTestsMTReleaseDll);
    
//////////////////////////////////////////////////////////////////
// base solution
var solution = new Solution("Solution");

solution.configurations.add(
        solutionConfDebugStatic.name, solutionConfDebugStatic);
        
solution.configurations.add(
        solutionConfReleaseStatic.name, solutionConfReleaseStatic);
        
solution.configurations.add(
        solutionConfMTDebugStatic.name, solutionConfMTDebugStatic);
        
solution.configurations.add(
        solutionConfMTReleaseStatic.name, solutionConfMTReleaseStatic);
                            
solution.configurations.add(
        solutionConfDebugDll.name, solutionConfDebugDll);
        
solution.configurations.add(
        solutionConfReleaseDll.name, solutionConfReleaseDll);
        
solution.configurations.add(
        solutionConfMTDebugDll.name, solutionConfMTDebugDll);
        
solution.configurations.add(
        solutionConfMTReleaseDll.name, solutionConfMTReleaseDll);
        
/////////////////////////////////////////////////////////////////
// projects
var projectConfigure = new Project(projectConfigureName);
solution.projects.add(projectConfigure.name, projectConfigure);

var projectExamples = new Project(projectExamplesName);
solution.projects.add(projectExamples.name, projectExamples);

var projectRwTest = new Project(projectRwTestName);
projectRwTest.dependencies.add(projectConfigure.name, projectConfigure);
solution.projects.add(projectRwTest.name, projectRwTest);

var projectTests = new Project(projectTestsName);
projectTests.dependencies.add(projectRwTestName, projectRwTest);
solution.projects.add(projectTests.name, projectTests);

var projectRunExamples = new Project(projectRunExamplesName);
solution.projects.add(projectRunExamples.name, projectRunExamples);

var projectRunTests = new Project(projectRunTestsName);
solution.projects.add(projectRunTests.name, projectRunTests);

/////////////////////////////////////////////////////////////////
// platforms
var platformWin32 = new Platform(platformWin32Name);

var platformWin32Conf = platformWin32.clone();
var platformWin32Ex = platformWin32.clone();
var platformWin32Tst = platformWin32.clone();
var platformWin32Rw = platformWin32.clone();

//////////////////////////////////////////////////////////////////
// platform configurations

// for configure
projectConfigure.platforms.add(platformWin32Conf.name, platformWin32Conf);

var confConfDebugStatic = 
    new Configuration(
        confDebugStaticName, configTypeLib, 
        confDebugStaticOut, confDebugStaticOut);
platformWin32Conf.configurations.add(
    confConfDebugStatic.name, confConfDebugStatic);

var confConfReleaseStatic = 
    new Configuration(
        confReleaseStaticName, configTypeLib,
        confReleaseStaticOut, confReleaseStaticOut);
platformWin32Conf.configurations.add(
    confConfReleaseStatic.name, confConfReleaseStatic);

var confConfMTDebugStatic = 
    new Configuration(
        confMTDebugStaticName, configTypeLib,
        confMTDebugStaticOut, confMTDebugStaticOut);
platformWin32Conf.configurations.add(
    confConfMTDebugStatic.name, confConfMTDebugStatic);

var confConfMTReleaseStatic = 
    new Configuration(
        confMTReleaseStaticName, configTypeLib,
        confMTReleaseStaticOut, confMTReleaseStaticOut);
platformWin32Conf.configurations.add(
    confConfMTReleaseStatic.name, confConfMTReleaseStatic);

var confConfDebugDll = 
    new Configuration(
        confDebugDllName, configTypeDll,
        confDebugDllOut, confDebugDllOut);
platformWin32Conf.configurations.add(
    confConfDebugDll.name, confConfDebugDll);

var confConfReleaseDll = 
    new Configuration(
        confReleaseDllName, configTypeDll,
        confReleaseDllOut, confReleaseDllOut);
platformWin32Conf.configurations.add(
    confConfReleaseDll.name, confConfReleaseDll);
    
var confConfMTDebugDll = 
    new Configuration(
        confMTDebugDllName, configTypeDll,
        confMTDebugDllOut, confMTDebugDllOut);
platformWin32Conf.configurations.add(
    confConfMTDebugDll.name, confConfMTDebugDll);

var confConfMTReleaseDll = 
    new Configuration(
        confMTReleaseDllName, configTypeDll,
        confMTReleaseDllOut, confMTReleaseDllOut);
platformWin32Conf.configurations.add(
    confConfMTReleaseDll.name, confConfMTReleaseDll);


// for examples
projectExamples.platforms.add(platformWin32Ex.name, platformWin32Ex);

var confExDebugStatic = 
    new Configuration(
        confDebugStaticName, configTypeExe,
        confDebugStaticOut, confDebugStaticOut);
platformWin32Ex.configurations.add(
    confExDebugStatic.name, confExDebugStatic);

var confExReleaseStatic = 
    new Configuration(
        confReleaseStaticName, configTypeExe,
        confReleaseStaticOut, confReleaseStaticOut);
platformWin32Ex.configurations.add(
    confExReleaseStatic.name, confExReleaseStatic);

var confExMTDebugStatic = 
    new Configuration(
        confMTDebugStaticName, configTypeExe,
        confMTDebugStaticOut, confMTDebugStaticOut);
platformWin32Ex.configurations.add(
    confExMTDebugStatic.name, confExMTDebugStatic);

var confExMTReleaseStatic = 
    new Configuration(
        confMTReleaseStaticName, configTypeExe,
        confMTReleaseStaticOut, confMTReleaseStaticOut);
platformWin32Ex.configurations.add(
    confExMTReleaseStatic.name, confExMTReleaseStatic);

var confExDebugDll = 
    new Configuration(
        confDebugDllName, configTypeExe,
        confDebugDllOut, confDebugDllOut);
platformWin32Ex.configurations.add(
    confExDebugDll.name, confExDebugDll);

var confExReleaseDll = 
    new Configuration(
        confReleaseDllName, configTypeExe,
        confReleaseDllOut, confReleaseDllOut);
platformWin32Ex.configurations.add(
    confExReleaseDll.name, confExReleaseDll);
    
var confExMTDebugDll = 
    new Configuration(
        confMTDebugDllName, configTypeExe,
        confMTDebugDllOut, confMTDebugDllOut);
platformWin32Ex.configurations.add(
    confExMTDebugDll.name, confExMTDebugDll);

var confExMTReleaseDll = 
    new Configuration(
        confMTReleaseDllName, configTypeExe,
        confMTReleaseDllOut, confMTReleaseDllOut);
platformWin32Ex.configurations.add(
    confExMTReleaseDll.name, confExMTReleaseDll);


// for tests
projectTests.platforms.add(platformWin32Tst.name, platformWin32Tst);

var confTstDebugStatic = 
    new Configuration(
        confDebugStaticName, configTypeExe,
        confDebugStaticOut, confDebugStaticOut);
platformWin32Tst.configurations.add(
    confTstDebugStatic.name, confTstDebugStatic);

var confTstReleaseStatic = 
    new Configuration(
        confReleaseStaticName, configTypeExe,
        confReleaseStaticOut, confReleaseStaticOut);
platformWin32Tst.configurations.add(
    confTstReleaseStatic.name, confTstReleaseStatic);

var confTstMTDebugStatic = 
    new Configuration(
        confMTDebugStaticName, configTypeExe,
        confMTDebugStaticOut, confMTDebugStaticOut);
platformWin32Tst.configurations.add(
    confTstMTDebugStatic.name, confTstMTDebugStatic);

var confTstMTReleaseStatic = 
    new Configuration(
        confMTReleaseStaticName, configTypeExe,
        confMTReleaseStaticOut, confMTReleaseStaticOut);
platformWin32Tst.configurations.add(
    confTstMTReleaseStatic.name, confTstMTReleaseStatic);

var confTstDebugDll = 
    new Configuration(
        confDebugDllName, configTypeExe,
        confDebugDllOut, confDebugDllOut);
platformWin32Tst.configurations.add(
    confTstDebugDll.name, confTstDebugDll);

var confTstReleaseDll = 
    new Configuration(
        confReleaseDllName, configTypeExe,
        confReleaseDllOut, confReleaseDllOut);
platformWin32Tst.configurations.add(
    confTstReleaseDll.name, confTstReleaseDll);
    
var confTstMTDebugDll = 
    new Configuration(
        confMTDebugDllName, configTypeExe,
        confMTDebugDllOut, confMTDebugDllOut);
platformWin32Tst.configurations.add(
    confTstMTDebugDll.name, confTstMTDebugDll);

var confTstMTReleaseDll = 
    new Configuration(
        confMTReleaseDllName, configTypeExe,
        confMTReleaseDllOut, confMTReleaseDllOut);
platformWin32Tst.configurations.add(
    confTstMTReleaseDll.name, confTstMTReleaseDll);


// for rwtest
projectRwTest.platforms.add(platformWin32Rw.name, platformWin32Rw);

var confRwDebugStatic = 
    new Configuration(
        confDebugStaticName, configTypeLib,
        confDebugStaticOut, confDebugStaticOut);
platformWin32Rw.configurations.add(
    confRwDebugStatic.name, confRwDebugStatic);

var confRwReleaseStatic = 
    new Configuration(
        confReleaseStaticName, configTypeLib,
        confReleaseStaticOut, confReleaseStaticOut);
platformWin32Rw.configurations.add(
    confRwReleaseStatic.name, confRwReleaseStatic);

var confRwMTDebugStatic = 
    new Configuration(
        confMTDebugStaticName, configTypeLib,
        confMTDebugStaticOut, confMTDebugStaticOut);
platformWin32Rw.configurations.add(
    confRwMTDebugStatic.name, confRwMTDebugStatic);

var confRwMTReleaseStatic = 
    new Configuration(
        confMTReleaseStaticName, configTypeLib,
        confMTReleaseStaticOut, confMTReleaseStaticOut);
platformWin32Rw.configurations.add(
    confRwMTReleaseStatic.name, confRwMTReleaseStatic);

var confRwDebugDll = 
    new Configuration(
        confDebugDllName, configTypeLib,
        confDebugDllOut, confDebugDllOut);
platformWin32Rw.configurations.add(
    confRwDebugDll.name, confRwDebugDll);

var confRwReleaseDll = 
    new Configuration(
        confReleaseDllName, configTypeLib,
        confReleaseDllOut, confReleaseDllOut);
platformWin32Rw.configurations.add(
    confRwReleaseDll.name, confRwReleaseDll);
    
var confRwMTDebugDll = 
    new Configuration(
        confMTDebugDllName, configTypeLib,
        confMTDebugDllOut, confMTDebugDllOut);
platformWin32Rw.configurations.add(
    confRwMTDebugDll.name, confRwMTDebugDll);

var confRwMTReleaseDll = 
    new Configuration(
        confMTReleaseDllName, configTypeLib,
        confMTReleaseDllOut, confMTReleaseDllOut);
platformWin32Rw.configurations.add(
    confRwMTReleaseDll.name, confRwMTReleaseDll);

// for RunExamples
var platformWin32RunEx = platformWin32Conf.clone();
projectRunExamples.platforms.add(platformWin32RunEx.name, 
    platformWin32RunEx);

// for RunTests
var platformWin32RunTests = platformWin32Conf.clone();
projectRunTests.platforms.add(platformWin32RunTests.name, 
    platformWin32RunTests);

/////////////////////////////////////////////////////////////////////////
// tools

// compilers
var compiler = new Compiler();
compiler.defines.add("_RWSTD_USE_CONFIG");

var compilerWin32 = compiler.clone();
// compilerWin32.defines.add("WIN32");

// linkers
var linker = new Linker();

// librarians
var librarian = new Librarian();

////////////////////////////////////////////////////////////////////////
// functions

// returns solution by its name
function getSolution(name)
{
    return configurations.get(name);
}

