//
// $Id$
//
// file should be included after config_base and after msvc_config_classes

// MSVC solution configuration

// includes
var includeFirstParentDir = "../include";
var includeSecondParentDir = "../../include";
var includeSecondParentAnsiDir = "../../include/ansi";

var samplesBaseIncludeDir = "..\\include";
var samplesBaseAnsiIncludeDir = "..\\include\\ansi";
var samplesIncludeDir = "include";

var testsBaseIncludeDir = "..\\include";
var testsBaseAnsiIncludeDir = "..\\include\\ansi";
var testsIncludeDir = "include";

// defines
var defineRWSHARED = "_RWSHARED";
var defineRWSTD_TEST_SRC = "_RWSTD_TEST_SRC";
var defineRWSTDDEBUG = "_RWSTDDEBUG";

var varCmdCopyIn = "$$copy_in";
var varCmdCopyOut = "$$copy_out";
var varCmdCopyDll = "$$copy_dll";

////////////////////////////////////////////////////////////////////
// solutions

// VC71 solution configuration

//clone generic solution
var solutionVC71 = solution.clone();
solutionVC71.name = vc71SolutionName;
configurations.add(solutionVC71.name, solutionVC71);

// configure the solution
configureToolsVC71();

// VC80 solution configuration
var solutionVC80 = solution.clone();
solutionVC80.name = vc80SolutionName;
solutionVC80.version = "8.00";
solutionVC80.formatVersion = "9.00";
configurations.add(solutionVC80.name, solutionVC80);

// configure the solution
configureToolsVC80();

// ICC solution configuration

//clone VC71 solution
var solutionICC90 = solutionVC71.clone();
solutionICC90.name = icc90SolutionName;
configurations.add(solutionICC90.name, solutionICC90);

////////////////////////////////////////////////////////////////////
// implementation

function configureToolsConfigure (solutionName,
            compilerDebug, compilerRelease, 
            linkerDebug, linkerRelease, 
            librarianTool, customTool, 
            postBuildTool)
{
    var compilerDebugConfigure = compilerDebug.clone();
    var compilerReleaseConfigure = compilerRelease.clone();
    var linkerDebugConfigure = linkerDebug.clone();
    var linkerReleaseConfigure = linkerRelease.clone();
    var librarianConfigure = librarianTool.clone();
    var customConfigure = customTool.clone();
    
    compilerDebugConfigure.pdbName = "MSVC.pdb";
    compilerReleaseConfigure.pdbName = "MSVC.pdb";
    
    if (solutionName == icc90SolutionName)
    {
        compilerDebugConfigure.pdbName = "ICC.pdb";
        compilerReleaseConfigure.pdbName = "ICC.pdb";
    }
    
    var solutionCfg = getSolution(solutionName);
    var configureProj = solutionCfg.projects.get(projectConfigureName);
    var platform = configureProj.platforms.get(platformWin32Name);
    
    // debug static
    var compilerDebugStaticConfigure = compilerDebugConfigure.clone();
    compilerDebugStaticConfigure.runtime = "SINGLEDEBUG";
    compilerDebugStaticConfigure.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugStaticName);
    configuration.tools.add(compilerToolName, 
        compilerDebugStaticConfigure);
    configuration.tools.add(linkerToolName, linkerDebugConfigure);  
    // presence of a librarian here allows configure script to determine
    // that this is a static configuration
    configuration.tools.add(librarianToolName, librarianConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);

    // release static
    var compilerReleaseStaticConfigure = compilerReleaseConfigure.clone();
    compilerReleaseStaticConfigure.runtime = "SINGLE";
    
    var configuration = platform.configurations.get(confReleaseStaticName);
    configuration.tools.add(compilerToolName, 
        compilerReleaseStaticConfigure);
    configuration.tools.add(linkerToolName, linkerReleaseConfigure);
    // presence of a librarian here allow configure script to determine
    // that this is a static configuration
    configuration.tools.add(librarianToolName, librarianConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);
    
    // debug multi-threaded static
    var compilerMTDebugStaticConfigure = compilerDebugConfigure.clone();
    compilerMTDebugStaticConfigure.runtime = "MULTIDEBUG";
    compilerMTDebugStaticConfigure.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugStaticName);
    configuration.tools.add(compilerToolName, 
        compilerMTDebugStaticConfigure);
    configuration.tools.add(linkerToolName, linkerDebugConfigure);  
    // presence of a librarian here allows configure script to determine
    // that this is a static configuration
    configuration.tools.add(librarianToolName, librarianConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);

    // release multi-threaded static
    var compilerMTReleaseStaticConfigure = compilerReleaseConfigure.clone();
    compilerMTReleaseStaticConfigure.runtime = "MULTI";
    
    var configuration = platform.configurations.get(confMTReleaseStaticName);
    configuration.tools.add(compilerToolName, 
        compilerMTReleaseStaticConfigure);
    configuration.tools.add(linkerToolName, linkerReleaseConfigure);
    // presence of a librarian here allow configure script to determine
    // that this is a static configuration
    configuration.tools.add(librarianToolName, librarianConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);
        
    // debug dll
    var compilerDebugDllConfigure = compilerDebugConfigure.clone();
    compilerDebugDllConfigure.runtime = "SINGLEDEBUG";
    compilerDebugDllConfigure.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugDllName);
    configuration.tools.add(compilerToolName, compilerDebugDllConfigure);
    configuration.tools.add(linkerToolName, linkerDebugConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);

    // release dll
    var compilerReleaseDllConfigure = compilerReleaseConfigure.clone();
    compilerReleaseDllConfigure.runtime = "SINGLE";
    
    var configuration = platform.configurations.get(confReleaseDllName);
    configuration.tools.add(compilerToolName, compilerReleaseDllConfigure);
    configuration.tools.add(linkerToolName, linkerReleaseConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);
    
    // debug multi-threaded dll
    var compilerMTDebugDllConfigure = compilerDebugConfigure.clone();
    compilerMTDebugDllConfigure.runtime = "MultiDebugDll";
    compilerMTDebugDllConfigure.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugDllName);
    configuration.tools.add(compilerToolName, compilerMTDebugDllConfigure);
    configuration.tools.add(linkerToolName, linkerDebugConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);

    // release multi-threaded dll
    var compilerMTReleaseDllConfigure = compilerReleaseConfigure.clone();
    compilerMTReleaseDllConfigure.runtime = "MULTIDLL";
    
    var configuration = platform.configurations.get(confMTReleaseDllName);
    configuration.tools.add(compilerToolName, compilerMTReleaseDllConfigure);
    configuration.tools.add(linkerToolName, linkerReleaseConfigure);
    configuration.tools.add(customBuildToolName, customConfigure);
}

function configureToolsExamples( solutionName,
            compilerDebug, compilerRelease, 
            linkerDebug, linkerRelease, 
            librarianTool, customTool,
            postBuildTool)
{
    var compilerDebugExamples = compilerDebug.clone();
    var compilerReleaseExamples = compilerRelease.clone();
    var linkerDebugExamples = linkerDebug.clone();
    var linkerReleaseExamples = linkerRelease.clone();
    
    var postExamplesDll = postBuildTool.clone();
    postExamplesDll.commands.add(varCmdCopyDll, varCmdCopyDll);
    
    compilerDebugExamples.includeDirectories.add(samplesBaseIncludeDir);
    compilerDebugExamples.includeDirectories.add(samplesIncludeDir);
    compilerDebugExamples.includeDirectories.add(samplesBaseAnsiIncludeDir);
  
    compilerReleaseExamples.includeDirectories.add(samplesBaseIncludeDir);
    compilerReleaseExamples.includeDirectories.add(samplesIncludeDir);
    compilerReleaseExamples.includeDirectories.add(samplesBaseAnsiIncludeDir);
    
    var solutionCfg = getSolution(solutionName);
    var examplesProj = solutionCfg.projects.get(projectExamplesName);
    var platform = examplesProj.platforms.get(platformWin32Name);
  
    // debug static
    var compilerDebugStaticEx = compilerDebugExamples.clone();
    compilerDebugStaticEx.runtime = "SINGLEDEBUG";
    compilerDebugStaticEx.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugStaticName);
    compilerDebugStaticEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerDebugStaticEx);
    configuration.tools.add(linkerToolName, linkerDebugExamples);   

    // release static
    var compilerReleaseStaticEx = compilerReleaseExamples.clone();
    compilerReleaseStaticEx.runtime = "SINGLE";
    
    var configuration = platform.configurations.get(confReleaseStaticName);
    compilerReleaseStaticEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerReleaseStaticEx);
    configuration.tools.add(linkerToolName, linkerReleaseExamples);
    
    // debug multi-threaded static
    var compilerMTDebugStaticEx = compilerDebugExamples.clone();
    compilerMTDebugStaticEx.runtime = "MULTIDEBUG";
    compilerMTDebugStaticEx.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugStaticName);
    compilerMTDebugStaticEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTDebugStaticEx);
    configuration.tools.add(linkerToolName, linkerDebugExamples);   

    // release multi-threaded static
    var compilerMTReleaseStaticEx = compilerReleaseExamples.clone();
    compilerMTReleaseStaticEx.runtime = "MULTI";
    
    var configuration = platform.configurations.get(confMTReleaseStaticName);
    compilerMTReleaseStaticEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTReleaseStaticEx);
    configuration.tools.add(linkerToolName, linkerReleaseExamples);
        
    // debug dll
    var compilerDebugDllEx = compilerDebugExamples.clone();
    compilerDebugDllEx.runtime = "SINGLEDEBUG";
    compilerDebugDllEx.defines.add(defineRWSHARED);
    compilerDebugDllEx.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugDllName);
    compilerDebugDllEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    var postExamplesDebugDll = postExamplesDll.clone();
    
    configuration.tools.add(compilerToolName, compilerDebugDllEx);
    configuration.tools.add(linkerToolName, linkerDebugExamples);
    configuration.tools.add(postBuildToolName, postExamplesDebugDll);   

    // release dll
    var compilerReleaseDllEx = compilerReleaseExamples.clone();
    compilerReleaseDllEx.runtime = "SINGLE";
    compilerReleaseDllEx.defines.add(defineRWSHARED);
    
    var configuration = platform.configurations.get(confReleaseDllName);
    compilerReleaseDllEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    var postExamplesReleaseDll = postExamplesDll.clone();
    
    configuration.tools.add(compilerToolName, compilerReleaseDllEx);
    configuration.tools.add(linkerToolName, linkerReleaseExamples); 
    configuration.tools.add(postBuildToolName, postExamplesReleaseDll);
    
    // debug multi-threaded dll
    var compilerMTDebugDllEx = compilerDebugExamples.clone();
    compilerMTDebugDllEx.runtime = "MultiDebugDll";
    compilerMTDebugDllEx.defines.add(defineRWSHARED);
    compilerMTDebugDllEx.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugDllName);
    compilerMTDebugDllEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    var postExamplesMTDebugDll = postExamplesDll.clone();
    
    configuration.tools.add(compilerToolName, compilerMTDebugDllEx);
    configuration.tools.add(linkerToolName, linkerDebugExamples);
    configuration.tools.add(postBuildToolName, postExamplesMTDebugDll);

    // release multi-threaded dll
    var compilerMTReleaseDllEx = compilerReleaseExamples.clone();
    compilerMTReleaseDllEx.runtime = "MULTIDLL";
    compilerMTReleaseDllEx.defines.add(defineRWSHARED);
    
    var configuration = platform.configurations.get(confMTReleaseDllName);
    compilerMTReleaseDllEx.includeDirectories.add(
        varSign + samplesBaseIncludeDir + "/" + configuration.outputDir);
        
    var postExamplesMTReleaseDll = postExamplesDll.clone();
        
    configuration.tools.add(compilerToolName, compilerMTReleaseDllEx);
    configuration.tools.add(linkerToolName, linkerReleaseExamples); 
    configuration.tools.add(postBuildToolName, postExamplesMTReleaseDll);
}


function configureToolsTests( solutionName,
            compilerDebug, compilerRelease, 
            linkerDebug, linkerRelease, 
            librarianTool, customTool,
            postBuildTool)
{
    var compilerDebugTests = compilerDebug.clone();
    var compilerReleaseTests = compilerRelease.clone();
    var linkerDebugTests = linkerDebug.clone();
    var linkerReleaseTests = linkerRelease.clone();
    
    var postTestsDll = postBuildTool.clone();
    postTestsDll.commands.add(varCmdCopyDll, varCmdCopyDll);
    
    compilerDebugTests.includeDirectories.add(testsBaseIncludeDir);
    compilerDebugTests.includeDirectories.add(testsIncludeDir);
    compilerDebugTests.includeDirectories.add(testsBaseAnsiIncludeDir);
  
    compilerReleaseTests.includeDirectories.add(testsBaseIncludeDir);
    compilerReleaseTests.includeDirectories.add(testsIncludeDir);
    compilerReleaseTests.includeDirectories.add(testsBaseAnsiIncludeDir);
    
    var solutionCfg = getSolution(solutionName);
    var testsProj = solutionCfg.projects.get(projectTestsName);
    var platform = testsProj.platforms.get(platformWin32Name);
  
    // debug static
    var compilerDebugStaticTst = compilerDebugTests.clone();
    compilerDebugStaticTst.runtime = "SINGLEDEBUG";
    compilerDebugStaticTst.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugStaticName);
    compilerDebugStaticTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerDebugStaticTst);
    configuration.tools.add(linkerToolName, linkerDebugTests);  

    // release static
    var compilerReleaseStaticTst = compilerReleaseTests.clone();
    compilerReleaseStaticTst.runtime = "SINGLE";
    
    var configuration = platform.configurations.get(confReleaseStaticName);
    compilerReleaseStaticTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerReleaseStaticTst);
    configuration.tools.add(linkerToolName, linkerReleaseTests);
    
    // debug multi-threaded static
    var compilerMTDebugStaticTst = compilerDebugTests.clone();
    compilerMTDebugStaticTst.runtime = "MULTIDEBUG";
    compilerMTDebugStaticTst.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugStaticName);
    compilerMTDebugStaticTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTDebugStaticTst);
    configuration.tools.add(linkerToolName, linkerDebugTests);  

    // release multi-threaded static
    var compilerMTReleaseStaticTst = compilerReleaseTests.clone();
    compilerMTReleaseStaticTst.runtime = "MULTI";
    
    var configuration = platform.configurations.get(confMTReleaseStaticName);
    compilerMTReleaseStaticTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTReleaseStaticTst);
    configuration.tools.add(linkerToolName, linkerReleaseTests);
        
    // debug dll
    var compilerDebugDllTst = compilerDebugTests.clone();
    compilerDebugDllTst.runtime = "SINGLEDEBUG";
    compilerDebugDllTst.defines.add(defineRWSHARED);
    compilerDebugDllTst.defines.add(defineRWSTD_TEST_SRC);
    compilerDebugDllTst.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugDllName);
    compilerDebugDllTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    var postTestsDebugDll = postTestsDll.clone();
        
    configuration.tools.add(compilerToolName, compilerDebugDllTst);
    configuration.tools.add(linkerToolName, linkerDebugTests);
    configuration.tools.add(postBuildToolName, postTestsDebugDll);

    // release dll
    var compilerReleaseDllTst = compilerReleaseTests.clone();
    compilerReleaseDllTst.runtime = "SINGLE";
    compilerReleaseDllTst.defines.add(defineRWSHARED);
    compilerReleaseDllTst.defines.add(defineRWSTD_TEST_SRC);
    
    var configuration = platform.configurations.get(confReleaseDllName);
    compilerReleaseDllTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    var postTestsReleaseDll = postTestsDll.clone();
        
    configuration.tools.add(compilerToolName, compilerReleaseDllTst);
    configuration.tools.add(linkerToolName, linkerReleaseTests);    
    configuration.tools.add(postBuildToolName, postTestsReleaseDll);
    
    // debug multi-threaded dll
    var compilerMTDebugDllTst = compilerDebugTests.clone();
    compilerMTDebugDllTst.runtime = "MultiDebugDll";
    compilerMTDebugDllTst.defines.add(defineRWSHARED);
    compilerMTDebugDllTst.defines.add(defineRWSTD_TEST_SRC);
    compilerMTDebugDllTst.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugDllName);
    compilerMTDebugDllTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    var postTestsMTDebugDll = postTestsDll.clone();
        
    configuration.tools.add(compilerToolName, compilerMTDebugDllTst);
    configuration.tools.add(linkerToolName, linkerDebugTests);
    configuration.tools.add(postBuildToolName, postTestsMTDebugDll);

    // release multi-threaded dll
    var compilerMTReleaseDllTst = compilerReleaseTests.clone();
    compilerMTReleaseDllTst.runtime = "MULTIDLL";
    compilerMTReleaseDllTst.defines.add(defineRWSHARED);
    compilerMTReleaseDllTst.defines.add(defineRWSTD_TEST_SRC);
    
    var configuration = platform.configurations.get(confMTReleaseDllName);
    compilerMTReleaseDllTst.includeDirectories.add(
        varSign + testsBaseIncludeDir + "/" + configuration.outputDir);
        
    var postTestsMTReleaseDll = postTestsDll.clone();
        
    configuration.tools.add(compilerToolName, compilerMTReleaseDllTst);
    configuration.tools.add(linkerToolName, linkerReleaseTests);
    configuration.tools.add(postBuildToolName, postTestsMTReleaseDll);
}


function configureToolsRwTest( solutionName,
            compilerDebug, compilerRelease, 
            linkerDebug, linkerRelease, 
            librarianTool, customTool,
            postBuildTool)
{
    var compilerDebugRwTest = compilerDebug.clone();
    var compilerReleaseRwTest = compilerRelease.clone();
    var librarianRwTest = librarianTool.clone();
    
    compilerDebugRwTest.includeDirectories.add(includeFirstParentDir);
    compilerDebugRwTest.includeDirectories.add(includeSecondParentDir);
    compilerDebugRwTest.includeDirectories.add(includeSecondParentAnsiDir);
  
    compilerReleaseRwTest.includeDirectories.add(includeFirstParentDir);
    compilerReleaseRwTest.includeDirectories.add(includeSecondParentDir);
    compilerReleaseRwTest.includeDirectories.add(includeSecondParentAnsiDir);
    
    var solutionCfg = getSolution(solutionName);
    var rwTestProj = solutionCfg.projects.get(projectRwTestName);
    var platform = rwTestProj.platforms.get(platformWin32Name);
    
    librarianRwTest.outputFile = rwTestProj.name + ".lib";
  
    // debug static
    var compilerDebugStaticRw = compilerDebugRwTest.clone();
    compilerDebugStaticRw.runtime = "SINGLEDEBUG";
    compilerDebugStaticRw.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugStaticName);
    compilerDebugStaticRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerDebugStaticRw);
    configuration.tools.add(librarianToolName, librarianRwTest);    

    // release static
    var compilerReleaseStaticRw = compilerReleaseRwTest.clone();
    compilerReleaseStaticRw.runtime = "SINGLE";
    
    var configuration = platform.configurations.get(confReleaseStaticName);
    compilerReleaseStaticRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerReleaseStaticRw);
    configuration.tools.add(librarianToolName, librarianRwTest);    
    
    // debug multi-threaded static
    var compilerMTDebugStaticRw = compilerDebugRwTest.clone();
    compilerMTDebugStaticRw.runtime = "MULTIDEBUG";
    compilerMTDebugStaticRw.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugStaticName);
    compilerMTDebugStaticRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTDebugStaticRw);
    configuration.tools.add(librarianToolName, librarianRwTest);

    // release multi-threaded static
    var compilerMTReleaseStaticRw = compilerReleaseRwTest.clone();
    compilerMTReleaseStaticRw.runtime = "MULTI";
    
    var configuration = platform.configurations.get(confMTReleaseStaticName);
    compilerMTReleaseStaticRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTReleaseStaticRw);
    configuration.tools.add(librarianToolName, librarianRwTest);
        
    // debug dll
    var compilerDebugDllRw = compilerDebugRwTest.clone();
    compilerDebugDllRw.runtime = "SINGLEDEBUG";
    compilerDebugDllRw.defines.add(defineRWSHARED);
    compilerDebugDllRw.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confDebugDllName);
    compilerDebugDllRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerDebugDllRw);
    configuration.tools.add(librarianToolName, librarianRwTest);

    // release dll
    var compilerReleaseDllRw = compilerReleaseRwTest.clone();
    compilerReleaseDllRw.runtime = "SINGLE";
    compilerReleaseDllRw.defines.add(defineRWSHARED);
    
    var configuration = platform.configurations.get(confReleaseDllName);
    compilerReleaseDllRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerReleaseDllRw);
    configuration.tools.add(librarianToolName, librarianRwTest);    
    
    // debug multi-threaded dll
    var compilerMTDebugDllRw = compilerDebugRwTest.clone();
    compilerMTDebugDllRw.runtime = "MultiDebugDll";
    compilerMTDebugDllRw.defines.add(defineRWSHARED);
    compilerMTDebugDllRw.defines.add(defineRWSTDDEBUG);
    
    var configuration = platform.configurations.get(confMTDebugDllName);
    compilerMTDebugDllRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTDebugDllRw);
    configuration.tools.add(librarianToolName, librarianRwTest);

    // release multi-threaded dll
    var compilerMTReleaseDllRw = compilerReleaseRwTest.clone();
    compilerMTReleaseDllRw.runtime = "MULTIDLL";
    compilerMTReleaseDllRw.defines.add(defineRWSHARED);
    
    var configuration = platform.configurations.get(confMTReleaseDllName);
    compilerMTReleaseDllRw.includeDirectories.add(
        varSign + includeSecondParentDir + "/" + configuration.outputDir);
        
    configuration.tools.add(compilerToolName, compilerMTReleaseDllRw);
    configuration.tools.add(librarianToolName, librarianRwTest);    
}


function configureToolsRunExamples( solutionName,
            compilerDebug, compilerRelease, 
            linkerDebug, linkerRelease, 
            librarianTool, customTool,
            postBuildTool)
{
    var customRunEx = customTool.clone();
    
    var solutionCfg = getSolution(solutionName);
    var runExProj = solutionCfg.projects.get(projectRunExamplesName);
    var platform = runExProj.platforms.get(platformWin32Name);
  
    // debug static
    var customDebugStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confDebugStaticName);
    configuration.tools.add(customBuildToolName, customDebugStaticRunEx);   

    // release static
    var customReleaseStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confReleaseStaticName);
    configuration.tools.add(customBuildToolName, customReleaseStaticRunEx); 
    
    // multi-threaded debug static
    var customMTDebugStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTDebugStaticName);
    configuration.tools.add(customBuildToolName, customMTDebugStaticRunEx); 
    
    // multi-threaded release static
    var customMTReleaseStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTReleaseStaticName);
    configuration.tools.add(customBuildToolName, customMTReleaseStaticRunEx);   
    
    // debug dll
    var postTestsDebugDll = postBuildTool.clone();
    var customDebugDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confDebugDllName);
    configuration.tools.add(customBuildToolName, customDebugDllRunEx);  
    configuration.tools.add(postBuildToolName, customDebugDllRunEx);  
    
    // release dll
    var postTestsReleaseDll = postBuildTool.clone();
    var customReleaseDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confReleaseDllName);
    configuration.tools.add(customBuildToolName, customReleaseDllRunEx); 
    configuration.tools.add(postBuildToolName, postTestsReleaseDll);     
    
    // multi-threaded debug dll
    var postTestsDebugMTDll = postBuildTool.clone();
    var customMTDebugDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTDebugDllName);
    configuration.tools.add(customBuildToolName, customMTDebugDllRunEx);
    configuration.tools.add(postBuildToolName, postTestsDebugMTDll);      
    
    // multi-threaded release dll
    var postTestsReleaseMTDll = postBuildTool.clone();
    var customMTReleaseDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTReleaseDllName);
    configuration.tools.add(customBuildToolName, customMTReleaseDllRunEx); 
    configuration.tools.add(postBuildToolName, postTestsReleaseMTDll);  
}


function configureToolsRunTests( solutionName,
            compilerDebug, compilerRelease, 
            linkerDebug, linkerRelease, 
            librarianTool, customTool,
            postBuildTool)
{
    var customRunEx = customTool.clone();
    
    var solutionCfg = getSolution(solutionName);
    var runTestsProj = solutionCfg.projects.get(projectRunTestsName);
    var platform = runTestsProj.platforms.get(platformWin32Name);
  
    // debug static
    var customDebugStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confDebugStaticName);
    configuration.tools.add(customBuildToolName, customDebugStaticRunEx);   

    // release static
    var customReleaseStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confReleaseStaticName);
    configuration.tools.add(customBuildToolName, customReleaseStaticRunEx); 
    
    // multi-threaded debug static
    var customMTDebugStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTDebugStaticName);
    configuration.tools.add(customBuildToolName, customMTDebugStaticRunEx); 
    
    // multi-threaded release static
    var customMTReleaseStaticRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTReleaseStaticName);
    configuration.tools.add(customBuildToolName, customMTReleaseStaticRunEx);   
    
    // debug dll
    var postTestsDebugDll = postBuildTool.clone();
    var customDebugDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confDebugDllName);
    configuration.tools.add(customBuildToolName, customDebugDllRunEx);  
    configuration.tools.add(postBuildToolName, postTestsDebugDll);  
    
    // release dll
    var postTestsReleaseDll = postBuildTool.clone();
    var customReleaseDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confReleaseDllName);
    configuration.tools.add(customBuildToolName, customReleaseDllRunEx);  
    configuration.tools.add(postBuildToolName, postTestsReleaseDll);  
    
    // multi-threaded debug dll
    var postTestsDebugMTDll = postBuildTool.clone();
    var customMTDebugDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTDebugDllName);
    configuration.tools.add(customBuildToolName, customMTDebugDllRunEx);
    configuration.tools.add(postBuildToolName, postTestsDebugMTDll);    
    
    // multi-threaded release dll
    var postTestsReleaseMTDll = postBuildTool.clone();
    var customMTReleaseDllRunEx = customRunEx.clone();
    var configuration = platform.configurations.get(confMTReleaseDllName);
    configuration.tools.add(customBuildToolName, customMTReleaseDllRunEx); 
    configuration.tools.add(postBuildToolName, postTestsReleaseMTDll); 
}


function configureToolsVC71()
{
    // VC compilers
    var compilerVCRelease = new CompilerVC(compilerWin32);
    
    var compilerVCDebug = compilerVCRelease.clone();
    compilerVCDebug.isDebug = true;
    
    compilerVCRelease.optimization = "Size";
    compilerVCRelease.debugInfoFormat = "PDB";
    
    compilerVCDebug.debugInfoFormat = "PDB";
    
    var compilerVC71Release = new CompilerVC71(compilerVCRelease);
    var compilerVC71Debug = new CompilerVC71(compilerVCDebug);
    
    // VC linkers
    var linkerVCDebug = new LinkerVC(linker);
    linkerVCDebug.subsystem = "Console";
    linkerVCDebug.incremental = "No";
    var linkerVCRelease = linkerVCDebug.clone();
    linkerVCRelease.optimizeRefs = "Remove";
    linkerVCRelease.comdatFolding = "Remove";

    // VC librarian
    var librarianVC = new LibrarianVC(librarian);
    
    // VC custom
    var customVC = new CustomBuildVC();
    
    // VC post build
    var postVC = new PostBuildVC();
    
    // configure projects tools
    configureToolsConfigure(vc71SolutionName, compilerVC71Debug, 
        compilerVC71Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsExamples(vc71SolutionName, compilerVC71Debug, 
        compilerVC71Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);

    configureToolsTests(vc71SolutionName, compilerVC71Debug, 
        compilerVC71Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsRwTest(vc71SolutionName, compilerVC71Debug, 
        compilerVC71Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsRunExamples(vc71SolutionName, compilerVC71Debug, 
        compilerVC71Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsRunTests(vc71SolutionName, compilerVC71Debug, 
        compilerVC71Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
}



function configureToolsVC80()
{
    // VC compilers
    var compilerVCRelease = new CompilerVC(compilerWin32);
    
    var compilerVCDebug = compilerVCRelease.clone();
    compilerVCDebug.isDebug = true;
    
    compilerVCRelease.optimization = "Size";
    compilerVCRelease.debugInfoFormat = "PDB";
    
    compilerVCDebug.debugInfoFormat = "PDB";
    
    var compilerVC80Release = new CompilerVC80(compilerVCRelease);
    var compilerVC80Debug = new CompilerVC80(compilerVCDebug);
    
    compilerVC80Release.defines.add("_CRT_SECURE_NO_DEPRECATE");
    compilerVC80Debug.defines.add("_CRT_SECURE_NO_DEPRECATE");
    
    // VC linkers
    var linkerVCDebug = new LinkerVC(linker);
    linkerVCDebug.subsystem = "Console";
    linkerVCDebug.incremental = "No";
    var linkerVCRelease = linkerVCDebug.clone();
    linkerVCRelease.optimizeRefs = "Remove";
    linkerVCRelease.comdatFolding = "Remove";

    // VC librarian
    var librarianVC = new LibrarianVC(librarian);
    
    // VC custom
    var customVC = new CustomBuildVC();
    
    // VC post build
    var postVC = new PostBuildVC();
    
    // configure projects tools
    configureToolsConfigure(vc80SolutionName, compilerVC80Debug, 
        compilerVC80Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsExamples(vc80SolutionName, compilerVC80Debug, 
        compilerVC80Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);

    configureToolsTests(vc80SolutionName, compilerVC80Debug, 
        compilerVC80Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsRwTest(vc80SolutionName, compilerVC80Debug, 
        compilerVC80Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsRunExamples(vc80SolutionName, compilerVC80Debug, 
        compilerVC80Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
        
    configureToolsRunTests(vc80SolutionName, compilerVC80Debug, 
        compilerVC80Release, linkerVCDebug, linkerVCRelease, 
        librarianVC, customVC, postVC);
}

