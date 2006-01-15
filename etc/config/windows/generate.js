//
// $Id$
//
// shall be included after base_config_classes

var projectStdLibName = ".stdlib";
var stdlibProjectFolder = "src";
var stdlibSrcFolder = ".";
var stdlibIncludeFolder = "..\\include";
var stdlibAnsiIncludeFolder = "..\\include\\ansi";
var winCfgFolder = ".\\etc\\config\\windows";
var testSrcFolder = ".\\etc\\config\\src";

var cfgProjectSrcFolder = "etc\\config\\src";
var cfgProjectIncludeFolder = "etc\\config\\src";

var samplesProjectsPrefix = "example_";
var samplesProjectFolder = "examples";

var rwtestProjectFolder = "tests\\src";
var rwtestSrcFolder = ".";
var rwtestIncludeFolder = "..\\include";

var testsProjectsPrefix = "test_";
var testsProjectFolder = "tests";

var defineRWSHARED = "_RWSHARED";

var samplesDir = "examples";
var testsDir = "tests";
var libOutputDir = "..\\lib";

var logFile = null;
var runExamplesLog = "runexamples.log";
var runTestsLog = "runtests.log";

var iccConversionUtility = "ICProjConvert90.exe";

function fillProjectsFromFolders(result, solution, srcDir, 
                    projectTemplateName, projectNamePrefix, projectFolder, 
                    includeOffset, excludedFolders, excludedFiles,
                    oldProjects)
{
    var folder = fso.GetFolder(srcDir);
    if (! folder)
        return;
        
    var enumSubFolders = new Enumerator(folder.SubFolders);
    for (; !enumSubFolders.atEnd(); enumSubFolders.moveNext())
    {
        var folderName = enumSubFolders.item().Name;
        if (excludedFolders.test(folderName))
        {
            // skip excluded folder
            continue;
        }
        
        fillProjectsFromFolders(result, solution, 
            srcDir + "\\" + folderName, projectTemplateName, 
            projectNamePrefix, projectFolder + "\\" + folderName,
            "..\\" + includeOffset, excludedFolders, excludedFiles,
            oldProjects);
    }
    
    var libPrj = result.projects[projectStdLibName];
    
    var rx = new RegExp("^.+\\.(?:cpp)$", "i");
    var enumFiles = new Enumerator(folder.Files);
    for (; !enumFiles.atEnd(); enumFiles.moveNext())
    {
        var fileName = enumFiles.item().Name;
        if (rx.test(fileName))
        {
            if (excludedFiles.test(fileName))  // we should exclude this file
            {
                continue;
            }
            
            var lastPoint = fileName.lastIndexOf(".");
            var prjName = projectNamePrefix + fileName.substr(0, lastPoint);
            var pureName = fileName.substr(0, lastPoint);
            
            var justUpdate = false;
            if (oldProjects != null)
            {
                var prjOldTmp = oldProjects[pureName];
                if (prjOldTmp != null)
                    justUpdate = true;
            }
            
            if (justUpdate)
            {
                logFile.WriteLine("Checking " + pureName);
                WScript.Echo("  Checking " + pureName + "...");
            }
            else
            {
                logFile.WriteLine("Creating " + pureName);
                WScript.Echo("  Creating " + pureName + "...");
            }
            
            var project = 
                solution.projects[projectTemplateName].shallowClone();
            project.name = prjName;
            project.folder = projectFolder;
            
            if (justUpdate)
                project.id = oldProjects[pureName].id;
            else
                project.id = createUUID();

            project.dependencies.add(projectStdLibName);
            project.platforms = 
                solution.projects[projectTemplateName].platforms.clone();
            
            if (justUpdate)
            {
                result.projects.add(pureName, project); 
                continue;
            }
            
            project.sourceFiles.add(fileName, false);
            
            for (i in project.platforms)
            {
                var platform = project.platforms[i];
                if (! platform || ! platform.configurations)
                {
                    continue;
                }
                
                for (j in platform.configurations)
                {
                    var config = platform.configurations[j];
                    if (! config || ! config.tools)
                    {
                        continue;
                    }
                    
                    var oldCompiler = config.tools[compilerToolName];
                    var newCompiler = oldCompiler.clone();
                    for (k in oldCompiler.includeDirectories)
                    {
                        if (!oldCompiler.includeDirectories[k])
                        {
                            newCompiler.includeDirectories.func_remove(k);
                        }
                    }
                    
                    for (m in oldCompiler.includeDirectories)
                    {
                        if (!oldCompiler.includeDirectories[m])
                        {
                            var findex = m.indexOf(varSign);
                            if (findex == 0)
                            {
                                newCompiler.includeDirectories.add(
                                    varSign + includeOffset + 
                                    m.substr(varSign.length));
                            }
                            else
                            {
                                newCompiler.includeDirectories.add(
                                    includeOffset + m);
                            }
                        }
                    }
                    
                    var postBuildTool = config.tools[postBuildToolName];
                    if (postBuildTool)
                    {
                        if (true == copyDll)
                        {
                            var libPlatform = libPrj.platforms[platform.name];
                            var libConfig = 
                                libPlatform.configurations[config.name];
                            var dllName = 
                                libConfig.tools[linkerToolName].outputFile;
                            var cmdCopyDll = "copy /Y " + includeOffset + 
                                "..\\lib" + "\\" + dllName + " $(OutDir)\\" + 
                                dllName;
                                
                            postBuildTool.commands.func_remove(varCmdCopyDll);
                            postBuildTool.commands.add(
                                varCmdCopyDll, cmdCopyDll);
                        }
                        else
                        {
                            config.tools.func_remove(postBuildToolName);
                        }
                    }
                    
                    config.tools.func_remove(compilerToolName);
                    config.tools.add(compilerToolName, newCompiler);
                }
            }
            
            result.projects.add(pureName, project);     
            logFile.WriteLine(pureName + " created");   
        }   
    }
}

// creates examples projects in a real solution
function fillSolutionWithSamples(result, solution, srcDir, oldProjects)
{
    var examplesDir = srcDir + "\\" + samplesDir;
    var excludeFolders = 
        new RegExp("^(?:\\.svn|Release.*|Debug.*|in|out|CVS)$","i");
    var excludeFiles = 
        new RegExp("^(?:rwstdmessages.cpp)$","i");
    var includeOffset = "";
    
    fillProjectsFromFolders(result, solution, examplesDir, 
            projectExamplesName, samplesProjectsPrefix, samplesDir, 
            includeOffset, excludeFolders, excludeFiles,
            oldProjects);  
}


// creates examples projects in a real solution
function fillSolutionWithTests(result, solution, srcDir, oldProjects)
{
    var tDir = srcDir + "\\" + testsDir;
    var excludeFolders = 
        new RegExp("^(?:\\.svn|Release.*|Debug.*|in|out|CVS|src)$","i");
    var excludeFiles = 
        new RegExp("^(?:rwstdmessages.cpp)$","i");
    var includeOffset = "";
    
    fillProjectsFromFolders(result, solution, tDir, 
                projectTestsName, testsProjectsPrefix, testsDir, 
                includeOffset, excludeFolders, excludeFiles,
                oldProjects); 
}


function updateSolutionProjectConfiguration(result, 
    projectTemplateName, projectNamePrefix) 
{
    for (j in result.projects)
    {
        var somePrj = result.projects[j];
        if (! somePrj || ! somePrj.name)
        {
            continue;
        }
        
        var firstPrefix = somePrj.name.indexOf(projectNamePrefix);
        if (firstPrefix == -1)
        {
            continue;
        }
        
        var pureName = 
            somePrj.name.substr(firstPrefix + projectNamePrefix.length);
        
        if (projectNamePrefix == samplesProjectsPrefix)
        {
            var prjExRunner = result.projects[projectRunExamplesName];
            if (prjExRunner)
                prjExRunner.dependencies.add(pureName, somePrj);
        }
        
        if (projectNamePrefix == testsProjectsPrefix)
        {
            var prjTestsRunner = result.projects[projectRunTestsName];
            if (prjTestsRunner)
                prjTestsRunner.dependencies.add(pureName, somePrj);
        }
            
        for (i in result.configurations)
        {
            var resultCfg = result.configurations[i];
            if (! resultCfg.projectConfigurations)
            {
                continue;
            }
            
            var exmplCfg = 
                resultCfg.projectConfigurations[projectTemplateName];
            if (! exmplCfg || ! exmplCfg.platform)
            {
                continue;
            }
            
            var prjCfg = exmplCfg.clone();
            prjCfg.projectName = pureName;
                
            var platformName = prjCfg.platform;
            var configName = prjCfg.configuration;
                
            var cfg = 
                somePrj.platforms[platformName].configurations[configName];
            if (cfg)
            {
                var linker = cfg.tools[linkerToolName];
                if (linker)
                    linker.outputFile = pureName + ".exe";
                    
                cfg.intermDir = cfg.intermDir + "\\" + pureName;
            }
            
            resultCfg.projectConfigurations.add(prjCfg.projectName, prjCfg);
        }
        
        somePrj.name = pureName;
    }
}

function removeProjectTemplates(result, projectTemplateName)
{
    for (i in result.configurations)
    {
        var resultCfg = result.configurations[i];
        if (! resultCfg || ! resultCfg.projectConfigurations)
        {
            continue;
        }
        
        resultCfg.projectConfigurations.func_remove(projectTemplateName);
    }
}

// creates real solution based on the template
function fillSolutionTemplate(solution, srcDir, outDir, logStream)
{
    logFile = logStream;
    
    // get configre project
    // put it into a new solution
    var configureProjectTemplate = 
        solution.projects.get(projectConfigureName);
    if (!configureProjectTemplate)
    {
        WScript.StdErr.WriteLine(
            "Generate: Fatal error: Configure project not found");
        WScript.Quit(3);
    }

    // create dependencies for Configure project
    var configureDependencies = makeDependencies(srcDir);

    var result = solution.shallowClone();
    result.generateSolution = solution.generateSolution;
    result.checkEnvironment = solution.checkEnvironment;
    result.version = solution.version;
    result.formatVersion = solution.formatVersion;
    // clone build configurations
    result.configurations = solution.configurations.clone();
    
    // append stdlib configurations as a copy of configure configurations
    for (i in solution.configurations)
    {
        var solutionCfg = solution.configurations[i];
        if (!solutionCfg.projectConfigurations)
        {
            // not a project configuration
            continue;
        }
        
        // copy solution configuration for Configure project
        // into stdlib project configuration
        var resultCfg = result.configurations[i];
        var configureCfg = 
            solutionCfg.projectConfigurations[projectConfigureName];
        var newProjCfg = configureCfg.clone();
        newProjCfg.projectName = projectStdLibName;
        resultCfg.projectConfigurations.add(newProjCfg.name, newProjCfg);

        // copy/fill projects that have associated solution configuration
        for (j in resultCfg.projectConfigurations)
        {
            var projCfg = resultCfg.projectConfigurations[j];
            
            // find/copy related project
            var projName = projCfg.projectName;
            if (!projName)
            {
                //not a project
                continue;
            }
            
            if (projName == projectExamplesName || 
                projName == projectTestsName)
            {
                continue;
            }
            
            var project = result.projects[projName];
            // stdlib project will be copied from confgure
            var templateProjName = projName == projectStdLibName ?
                    projectConfigureName : projName;
            if (!project) // project was not copied yet. copy it
            {
                project = solution.projects[templateProjName].shallowClone();
                project.name = projName;
                project.id = createUUID();
                // add files
                switch (projName)
                {
                    
                case projectConfigureName:
                    var filter = createExcludedSourcesFilter(srcDir, 
                        cfgProjectSrcFolder);
                    project.sourceFiles.add(filter.name, filter);
                    
                    filter = createExcludedIncludesFilter(srcDir, 
                        cfgProjectIncludeFolder);                        
                    project.sourceFiles.add(filter.name, filter);
                
                    break;
                    
                case projectStdLibName:
                    project.folder = stdlibProjectFolder;
                    
                    var filter = createCppSourcesFilter(srcDir + "\\"
                        + stdlibProjectFolder, stdlibSrcFolder);
                    project.sourceFiles.add(filter.name, filter);
                    
                    filter = createIncludesFilter(srcDir + "\\"
                        + stdlibProjectFolder, stdlibIncludeFolder);
                    addIncludesToFilter(srcDir + "\\"
                        + stdlibProjectFolder, stdlibSrcFolder, filter);
                        
                    project.sourceFiles.add(filter.name, filter);
                    
                    project.dependencies.add(projectConfigureName);
                    break;
                    
                case projectRwTestName:
                
                    project.folder = rwtestProjectFolder;
                    var filter = createCppSourcesFilter(srcDir + "\\"
                        + rwtestProjectFolder, rwtestSrcFolder);
                    project.sourceFiles.add(filter.name, filter);
                    
                    filter = createIncludesFilter(srcDir + "\\"
                        + rwtestProjectFolder, rwtestIncludeFolder);    
                    addIncludesToFilter(srcDir + "\\"
                        + rwtestProjectFolder, rwtestSrcFolder, filter);
                        
                    project.sourceFiles.add(filter.name, filter);
                    
                    break;
                    
                case projectRunExamplesName:
                    project.folder = samplesProjectFolder;
                    break;
                    
                case projectRunTestsName:
                    project.folder = testsProjectFolder;
                    break;
                    
                default:
                //do nothing. suppose that all files are explicitly specified
                }
                result.projects.add(project.name, project);
            }
            
            // find/copy related platform
            var platformName = projCfg.platform;
            var platform = project.platforms.get(platformName);
            if (!platform) // platform was not copied yet. copy it.
            {
                platform =
                    solution.projects[templateProjName]
                        .platforms[platformName].shallowClone();
                project.platforms.add(platform.name, platform);
            }
            
            // find/copy-fill related configuration
            var configName = projCfg.configuration;
            var config = platform.configurations[configName];
            if (!config) // configuration was not copied yet. copy it
            {
                var oldCfg = solution.projects[templateProjName]
                        .platforms[platformName].configurations[configName];
                switch (projName)
                {
                case projectConfigureName:
                    var newCfg = oldCfg.shallowClone();
                    newCfg.type = configTypeDll;                                        
                    var tool = oldCfg.tools[customBuildToolName].clone();
                    tool.output = ".\\include\\" + 
                        newCfg.outputDir + "\\config.h";
                    
                    tool.command =
                        "cscript .\\etc\\config\\windows\\configure.wsf";                       
                    
                    tool.command += " /SolutionName:\"" + 
                        solution.name + "\"";
                    tool.command += " /ConfigurationName:\"" + 
                        resultCfg.name + "\"";
                    tool.command += " /SrcDir:\"" + srcDir + 
                        "etc\\config\\src\\" + "\"";
                    tool.command += " /OutDir:\"" + outDir + "include\\" + 
                        newCfg.outputDir + "\\" + "\"";
                    tool.command += " /IncludeDir:\"" + srcDir + 
                        "include" + "\"";
                    tool.command += " /OutFile:\"" + outDir + "include\\" + 
                        newCfg.outputDir + "\\" + "config.h" + "\"";    
                    tool.command += " /LogFile:\"" + outDir + "include\\" + 
                        newCfg.outputDir + "\\" + "config.log" + "\"";
                                                    
                    tool.dependencies = configureDependencies;
                    newCfg.tools.add(tool.name, tool);
                    // add a fake linker to suppress dependency on output
                    tool = getFakeLinker(solution.name);
                    newCfg.tools.add(tool.name, tool);
                    
                    newCfg.outputDir = "include\\" + newCfg.outputDir;
                    newCfg.intermDir = "include\\" + newCfg.intermDir;
                    platform.configurations.add(newCfg.name, newCfg);
                    break;
                case projectStdLibName:
                    var newCfg = oldCfg.shallowClone();
                    var tool = oldCfg.tools[compilerToolName].clone();
                    tool.includeDirectories.add(stdlibIncludeFolder);
                    tool.includeDirectories.add(stdlibAnsiIncludeFolder);
                    tool.includeDirectories.add(varSign + 
                        stdlibIncludeFolder + "\\" + newCfg.outputDir);
                                
                    var cfgSuffix = newCfg.outputDir;
                    newCfg.outputDir = libOutputDir;
                                
                    newCfg.tools.add(tool.name, tool);
                    // for lib project add librarian
                    // for dll project add linker
                    // project is a dll project if librarian is absent
                    if (!oldCfg.tools[librarianToolName])
                    {                       
                        tool = oldCfg.tools[linkerToolName].clone();
                        tool.outputFile = 
                            removeLeadingDot(projectStdLibName) + 
                            cfgSuffix + ".dll";
                            
                        tool.pdbFile = removeLeadingDot(projectStdLibName) +
                            cfgSuffix + ".pdb";
                        
                        var toolCmp = newCfg.tools[compilerToolName];
                        toolCmp.defines.add("_RWSHARED");
                        toolCmp.defines.add("_RWBUILD_std");
                    }
                    else
                    {
                        tool = oldCfg.tools[librarianToolName].clone();
                        tool.outputFile = 
                            removeLeadingDot(projectStdLibName) +
                            cfgSuffix + ".lib";
                    }
                    newCfg.tools.add(tool.name, tool);
                    platform.configurations.add(newCfg.name, newCfg);
                    break;
                    
                case projectRwTestName:
                    var newCfg = oldCfg.clone();
                    var tool = newCfg.tools[librarianToolName];
                    if (tool)
                        tool.outputFile = 
                            removeLeadingDot(projectRwTestName) + 
                            ".lib";
                        
                    platform.configurations.add(newCfg.name, newCfg);
                    break;
                    
                case projectRunExamplesName:
                    var newCfg = oldCfg.shallowClone();
                    newCfg.type = configTypeDll;                                        
                    var tool = oldCfg.tools[customBuildToolName].clone();
                    var toolMark = oldCfg.tools[postBuildToolName];
                    tool.output = newCfg.outputDir + "\\" + runExamplesLog;
                    
                    tool.command =
                        "cscript /nologo " +
                        "..\\etc\\config\\windows\\runall.wsf";
                    
                    tool.command += 
                        " /INOUTDIR:\"" + srcDir + "examples" + "\"";
                    tool.command += 
                        " /EXEDIR:\"" + outDir + "examples" + "\"";
                    tool.command += 
                        " /BUILDTYPE:\"" + newCfg.outputDir + "\"";
                    tool.command += 
                        " /CONFIG:\"" + result.name + "\"";
                    tool.command += 
                        " /LOGFILE:\"" + newCfg.outputDir + "\\" 
                        + runExamplesLog + "\"";
                        
                    if (toolMark)
                    {
                        // pass true if it is neccessary to copy dll manually
                        if (copyDll)
                            tool.command += " /COPYDLL:false";
                        else
                        {
                            tool.command += " /COPYDLL:true";
                            tool.command +=  
                                " /LIBDIR:\"" + outDir + "lib\\" + "\"";
                        }
                    }
                                                    
                    newCfg.tools.add(tool.name, tool);
                    // add a fake linker to suppress dependency on output
                    tool = getFakeLinker(solution.name);
                    newCfg.tools.add(tool.name, tool);
                
                    platform.configurations.add(newCfg.name, newCfg);
                    break;
                    
                case projectRunTestsName:
                    var newCfg = oldCfg.shallowClone();
                    newCfg.type = configTypeDll;                                        
                    var tool = oldCfg.tools[customBuildToolName].clone();
                    var toolMark = oldCfg.tools[postBuildToolName];
                    tool.output = newCfg.outputDir + "\\" + runTestsLog;
                    
                    tool.command =
                        "cscript /nologo " + 
                        "..\\etc\\config\\windows\\runall.wsf";                        
                    
                    tool.command += 
                        " /INOUTDIR:\"" + srcDir + "tests" + "\"";
                    tool.command += 
                        " /EXEDIR:\"" + outDir + "tests" + "\"";
                    tool.command += 
                        " /BUILDTYPE:\"" + newCfg.outputDir + "\"";
                    tool.command += 
                        " /CONFIG:\"" + result.name + "\"";
                    tool.command += 
                        " /LOGFILE:\"" + newCfg.outputDir + "\\" 
                        + runTestsLog + "\"";
                        
                    if (toolMark)
                    {
                        // pass true if it is neccessary to copy dll manually
                        if (copyDll)
                            tool.command += " /COPYDLL:false";
                        else
                        {
                            tool.command += " /COPYDLL:true";
                            tool.command +=  
                                " /LIBDIR:\"" + outDir + "lib\\" + "\"";
                        }
                    }
                                                    
                    newCfg.tools.add(tool.name, tool);
                    // add a fake linker to suppress dependency on output
                    tool = getFakeLinker(solution.name);
                    newCfg.tools.add(tool.name, tool);
                
                    platform.configurations.add(newCfg.name, newCfg);
                    break;
                    
                default:
                    var newCfg = oldCfg.clone();
                    platform.configurations.add(newCfg.name, newCfg);
                }
            }
            //else configuration already present. do nothing            
        }
    }
    
    WScript.Echo("Library and configure projects are created");
    logFile.WriteLine("Library and configure projects are created");
    
    logFile.WriteLine("Creating examples");
    WScript.Echo("Creating examples...");
    fillSolutionWithSamples(result, solution, srcDir, null);
    
    WScript.Echo("Configuring examples...");
    logFile.WriteLine("Configuring examples");
    updateSolutionProjectConfiguration(result, 
        projectExamplesName, samplesProjectsPrefix);
    
    logFile.WriteLine("Creating tests");
    WScript.Echo("Creating tests...");
    fillSolutionWithTests(result, solution, srcDir, null);
    
    WScript.Echo("Configuring tests...");
    logFile.WriteLine("Configuring tests");
    updateSolutionProjectConfiguration(result, 
        projectTestsName, testsProjectsPrefix);
    
    logFile.WriteLine("Clearing temporary data");
    WScript.Echo("Clearing temporary data...");
    removeProjectTemplates(result, projectExamplesName);
    removeProjectTemplates(result, projectTestsName);
    
    return result;
}

function createSourcesFilter(projectDir, offsetDir)
{
    var filter = createFilter(projectDir, offsetDir, 
        "Source Files", "cpp;c;cxx;s;cc", /^.+\.cc|.+\.c$/i);
    filter.id = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";
    return filter;
}

function createCppSourcesFilter(projectDir, offsetDir)
{
    var filter = createFilter(projectDir, offsetDir, 
        "Source Files", "cpp;cxx;s", /^.+\.cc|.+\.c$/i);
    filter.id = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";
    return filter;
}

function createIncludesFilter(projectDir, offsetDir)
{
    var filter = createFilter(projectDir, offsetDir, 
        "Header Files", "h;hpp;hxx;c;cc", /^.+\.cc?/i);
    filter.id = "{93995380-89BD-4b04-88EB-625FBE52EBFB}";
    return filter;  
}

function addIncludesToFilter(projectDir, offsetDir, filter)
{
    addToFilter(projectDir, offsetDir, filter, /^.+\.cc?/i);    
}

function createExcludedSourcesFilter(projectDir, offsetDir)
{
    var filter = createFilter(projectDir, offsetDir, 
        "Source Files", "cpp;c;cxx;s;cc", /^.+\.cc|.+\.c|.+\.cpp|.+\.cxx$/i);
    filter.id = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";
    return filter;
}

function createExcludedIncludesFilter(projectDir, offsetDir)
{
    var filter = createFilter(projectDir, offsetDir, 
        "Header Files", "h;hpp;hxx", /^.+\.cc|.+\.c|.+\.hpp|.+\.hxx$/i);
    filter.id = "{93995380-89BD-4b04-88EB-625FBE52EBFB}";
    return filter;
}

var rxExcludedFolders = 
    new RegExp("^(?:\\.svn|Release.*|Debug.*|in|out|CVS)$","i");

function addToFilter(projectDir, offsetDir, filter, excludeRx)
{
    if (! filter)
        return;
        
    var filterExt = filter.filter;
    var extArray = filter.filter.split(";");
    var rxText = "^";
    if (extArray.length != 0)
    {
        rxText += ".+\\.(?:" + extArray[0];
        for (i = 1; i < extArray.length; ++i)
        {
            rxText += "|" + extArray[i];
        }
        rxText += ")";
    }
    rxText += "$";
    var rx = new RegExp(rxText, "i");
    var dir = projectDir + "\\" + offsetDir;
    try
    {
        var folder = fso.GetFolder(dir);
    }
    catch (e)
    {
        WScript.StdErr.WriteLine("Generate: Fatal error: Failed to"
            + " open folder " + dir);
        WScript.Quit(3);
    }
    // add subfolders as own filters
    var enumSubFolders = new Enumerator(folder.SubFolders);
    for (; !enumSubFolders.atEnd(); enumSubFolders.moveNext())
    {
        var folderName = enumSubFolders.item().Name;
        if (rxExcludedFolders.test(folderName))
        {
            // skip excluded folder
            continue;
        }
        
        var subfilter =  createFilter(projectDir
            , offsetDir + "\\" + folderName
            , folderName, filterExt, excludeRx);        
            
        if (subfilter != null)
            filter.sourceFiles.add(subfilter.name, subfilter);
    }
    
    // add files
    var enumFiles = new Enumerator(folder.Files);
    for (; !enumFiles.atEnd(); enumFiles.moveNext())
    {
        var fileName = enumFiles.item().Name;
        if (rx.test(fileName))
        {
            var exclude = excludeRx.test(fileName);
            filter.sourceFiles.add(offsetDir + "\\" + fileName, exclude);
        }   
    }
}

function createFilter(projectDir, offsetDir, filterName, 
    filterExtensions, excludeRx)
{
    var filter = new Filter(filterName);
    filter.filter = filterExtensions;
    // create regexp from extensions
    var extArray = filterExtensions.split(";");
    var rxText = "^";
    if (extArray.length != 0)
    {
        rxText += ".+\\.(?:" + extArray[0];
        for (i = 1; i < extArray.length; ++i)
        {
            rxText += "|" + extArray[i];
        }
        rxText += ")";
    }
    rxText += "$";
    var rx = new RegExp(rxText, "i");
    var dir = projectDir + "\\" + offsetDir;
    try
    {
        var folder = fso.GetFolder(dir);
    }
    catch (e)
    {
        WScript.StdErr.WriteLine("Generate: Fatal error: Failed to"
            + " open folder " + dir);
        WScript.Quit(3);
    }
    // add subfolders as own filters
    var enumSubFolders = new Enumerator(folder.SubFolders);
    for (; !enumSubFolders.atEnd(); enumSubFolders.moveNext())
    {
        var folderName = enumSubFolders.item().Name;
        if (rxExcludedFolders.test(folderName))
        {
            // skip excluded folder
            continue;
        }
        
        var subfilter =  createFilter(projectDir
            , offsetDir + "\\" + folderName
            , folderName, filterExtensions, excludeRx);     
            
        if (subfilter != null)
            filter.sourceFiles.add(subfilter.name, subfilter);
    }
    // add files
    var n = 0;
    var enumFiles = new Enumerator(folder.Files);
    for (; !enumFiles.atEnd(); enumFiles.moveNext())
    {
        var fileName = enumFiles.item().Name;
        if (rx.test(fileName))
        {
            n++;
            var exclude = excludeRx.test(fileName);
            filter.sourceFiles.add(offsetDir + "\\" + fileName, exclude);
        }   
    }
    if (n == 0)
        return null;
        
    return filter;
}

function makeDependencies(srcDir)
{
    var deps = new Collection();
    var folder = fso.GetFolder(srcDir + "\\" + winCfgFolder);
    var rxCfgFiles = /^.+\.(?:js|cpp|wsf)$/i;
    var filesEnum = new Enumerator(folder.Files);
    for (; !filesEnum.atEnd(); filesEnum.moveNext())
    {
        var fileName = filesEnum.item().Name;
        if (rxCfgFiles.test(fileName))
        {
            deps.add(winCfgFolder + "\\" + fileName);
        }
    }
    folder = fso.GetFolder(srcDir + "\\" + testSrcFolder);
    filesEnum = new Enumerator(folder.Files);
    for (; !filesEnum.atEnd(); filesEnum.moveNext())
    {
        deps.add(testSrcFolder + "\\" + filesEnum.item().Name);
    }
    return deps;
}


function getFakeLinker(solutionName)
{
    if (solutionName == vc71SolutionName)
        return new FakeLinkerVC();
        
    if (solutionName == icc90SolutionName)
        return new FakeLinkerICC();
        
    return new FakeLinkerVC();
}

// creates real solution based on the template
function fillSolutionTemplateLight(solution, oldProjects, srcDir, outDir, logStream)
{
    logFile = logStream;
    
    // get configre project
    // put it into a new solution
    var configureProjectTemplate = 
        solution.projects.get(projectConfigureName);
    if (!configureProjectTemplate)
    {
        WScript.StdErr.WriteLine(
            "Update: Fatal error: Configure project not found");
        WScript.Quit(3);
    }

    // create dependencies for Configure project
    var configureDependencies = makeDependencies(srcDir);

    var result = solution.shallowClone();
    result.generateSolution = solution.generateSolution;
    result.checkEnvironment = solution.checkEnvironment;
    result.version = solution.version;
    result.formatVersion = solution.formatVersion;
    // clone build configurations
    result.configurations = solution.configurations.clone();
    
    // append stdlib configurations as a copy of configure configurations
    for (i in solution.configurations)
    {
        var solutionCfg = solution.configurations[i];
        if (!solutionCfg.projectConfigurations)
        {
            // not a project configuration
            continue;
        }
        
        // copy solution configuration for Configure project
        // into stdlib project configuration
        var resultCfg = result.configurations[i];
        var configureCfg = 
            solutionCfg.projectConfigurations[projectConfigureName];
        var newProjCfg = configureCfg.clone();
        newProjCfg.projectName = projectStdLibName;
        resultCfg.projectConfigurations.add(newProjCfg.name, newProjCfg);

        // copy/fill projects that have associated solution configuration
        for (j in resultCfg.projectConfigurations)
        {
            var projCfg = resultCfg.projectConfigurations[j];
            
            // find/copy related project
            var projName = projCfg.projectName;
            if (!projName)
            {
                //not a project
                continue;
            }
            
            if (projName == projectExamplesName || 
                projName == projectTestsName)
            {
                continue;
            }
            
            var project = result.projects[projName];
            // stdlib project will be copied from confgure
            var templateProjName = projName == projectStdLibName ?
                    projectConfigureName : projName;
            if (!project) // project was not copied yet. copy it
            {                
                project = solution.projects[templateProjName].shallowClone();
                project.name = projName;
                    
                var oldProject = oldProjects[projName];
                if (! oldProject)
                {
                    project.id = createUUID();
                }
                else
                    project.id = oldProject.id;                    
                
                // add files
                switch (projName)
                {
                    
                case projectConfigureName:
                    break;
                    
                case projectStdLibName:
                    project.folder = stdlibProjectFolder;
                    project.dependencies.add(projectConfigureName);
                    break;
                    
                case projectRwTestName:                
                    project.folder = rwtestProjectFolder;
                    break;
                    
                case projectRunExamplesName:
                    project.folder = samplesProjectFolder;
                    break;
                    
                case projectRunTestsName:
                    project.folder = testsProjectFolder;
                    break;
                    
                default:
                //do nothing. suppose that all files are explicitly specified
                }
                result.projects.add(project.name, project);
            }
            
            // find/copy related platform
            var platformName = projCfg.platform;
            var platform = project.platforms.get(platformName);
            if (!platform) // platform was not copied yet. copy it.
            {
                platform =
                    solution.projects[templateProjName]
                        .platforms[platformName].shallowClone();
                project.platforms.add(platform.name, platform);
            }
            
            // find/copy-fill related configuration
            var configName = projCfg.configuration;
            var config = platform.configurations[configName];
            if (!config) // configuration was not copied yet. copy it
            {
                var oldCfg = solution.projects[templateProjName]
                        .platforms[platformName].configurations[configName];

                var newCfg = oldCfg.clone();
                if (projName == projectStdLibName)
                {
                    if (!newCfg.tools[librarianToolName])
                    {
                        var tool = newCfg.tools[linkerToolName];
                        tool.outputFile = 
                            removeLeadingDot(projectStdLibName) + 
                            newCfg.outputDir + ".dll";
                    }
                    else
                    {
                        var tool = newCfg.tools[librarianToolName];
                        tool.outputFile = 
                            removeLeadingDot(projectStdLibName) +
                            newCfg.outputDir + ".lib";
                    }
                }
                
                platform.configurations.add(newCfg.name, newCfg);
            }
            //else configuration already present. do nothing            
        }
    }
    
    WScript.Echo("Library amd configure projects are checked");
    logFile.WriteLine("Library amd configure projects are checked");
    
    logFile.WriteLine("Checking examples");
    WScript.Echo("Checking examples...");
    fillSolutionWithSamples(result, solution, srcDir, oldProjects);
    
    WScript.Echo("Configuring examples...");
    logFile.WriteLine("Configuring examples");
    updateSolutionProjectConfiguration(result, 
        projectExamplesName, samplesProjectsPrefix);
    
    logFile.WriteLine("Checking tests");
    WScript.Echo("Checking tests...");
    fillSolutionWithTests(result, solution, srcDir, oldProjects);
    
    WScript.Echo("Configuring tests...");
    logFile.WriteLine("Configuring tests");
    updateSolutionProjectConfiguration(result, 
        projectTestsName, testsProjectsPrefix);
    
    logFile.WriteLine("Clearing temporary data");
    WScript.Echo("Clearing temporary data...");
    removeProjectTemplates(result, projectExamplesName);
    removeProjectTemplates(result, projectTestsName);
    
    return result;
}

// generates solution for the VC
function generateVCPROJ(project, srcDir, outDir, version)
{
    var projectPath = project.folder.length > 0 ?
        outDir + project.folder + "\\" : outDir + "";
    var projectSrcPath = project.folder.length > 0 ?
        srcDir + project.folder + "\\" : srcDir + "";
    var projectFile = projectPath + 
        removeLeadingDot(project.name) + ".vcproj";
    
    if (! fso.FolderExists(projectPath))
    {
        fso.CreateFolder(projectPath);
    }
            
    // write project file
    var vcproj = fso.CreateTextFile(projectFile, true, false);
    vcproj.WriteLine(
        "<?xml version=\"1.0\" encoding=\"windows-1252\"?>");
    vcproj.WriteLine("<VisualStudioProject");
    vcproj.WriteLine("\tProjectType=\"Visual C++\"");
    vcproj.WriteLine("\tVersion=\"" + version + "\"");
    vcproj.WriteLine("\tName=\"" + project.name + "\"");
    vcproj.WriteLine("\tProjectGUID=\"" + project.id +"\"");
    vcproj.WriteLine("\tRootNamespace=\"" + project.name + "\"");
    vcproj.WriteLine("\tKeyword=\"stdcxx\">");
            
    // platforms
    vcproj.WriteLine("\t<Platforms>");
    for (j in project.platforms)
    {
        var platform = project.platforms[j];
        if (platform.name)
        {
            vcproj.WriteLine("\t\t<Platform");
            vcproj.WriteLine("\t\t\tName=\""
                + platform.name + "\"/>");
        }
    }           
    vcproj.WriteLine("\t</Platforms>");
            
    // configurations
    vcproj.WriteLine("\t<Configurations>");
    for (j in project.platforms)
    {
        var platform = project.platforms[j];
        if (platform.configurations)
        {
            for (k in platform.configurations)
            {
                var configuration = platform.configurations[k];
                if (configuration.name)
                {
                    vcproj.WriteLine("\t\t<Configuration");
                    vcproj.WriteLine(
                        "\t\t\tName=\"" + configuration.name
                        + "|" + platform.name + "\"");
                    vcproj.WriteLine(
                        "\t\t\tOutputDirectory=\""
                        + configuration.outputDir + "\"");
                    vcproj.WriteLine(
                        "\t\t\tIntermediateDirectory=\""
                        + configuration.intermDir + "\"");
                    var cfgType = 1;
                    switch (configuration.type.toUpperCase())
                    {
                    case configTypeExe:
                        cfgType = 1;
                        break;
                    case configTypeDll:
                        cfgType = 2;
                        break;
                    case configTypeLib:
                        cfgType = 4;
                        break;
                    default:
                    WScript.StdErr.WriteLine(
                        "Generate: Unknown configuration type for"
                        + " project " + project.name
                        + " platform " + platform.name
                        + " configuration " + configuration.name);
                    WScript.Quit(3);
                    }
                    vcproj.WriteLine("\t\t\tConfigurationType=\""
                                                + cfgType + "\"");
                    // MBCS
                    vcproj.WriteLine("\t\t\tCharacterSet=\"2\">"); 
                            
                    // tools
                    for (l in configuration.tools)
                    {
                        var tool = configuration.tools[l];
                        if (tool.name)
                        {
                            tool.outputDirectory = "$(OutDir)";
                            tool.intermDirectory = "$(IntDir)";
                            tool.writeSolutionSettings(
                                vcproj, projectSrcPath);
                        }
                    }
                    vcproj.WriteLine("\t\t</Configuration>");
                }
            }
        }
    }
    vcproj.WriteLine("\t</Configurations>");
            
    // references (not needed for us)
    // vcproj.WriteLine("\t<References>");
    // vcproj.WriteLine("\t</References>");
            
    // files
    vcproj.WriteLine("\t<Files>");
    writeSourcesVC(project.sourceFiles, vcproj, "\t\t", 
        project, projectSrcPath);
    vcproj.WriteLine("\t</Files>");
            
    // some unknows stuff
    // vcproj.WriteLine("\t<Globals>");
    // vcproj.WriteLine("\t</Globals>");
            
    vcproj.WriteLine("</VisualStudioProject>");
    vcproj.Close();
}

function generateSolutionVCImpl (solution, srcDir, outDir, 
                                 logStream, genVCPROJs)
{
    var slnFileName = outDir + "\\" + solution.name + ".sln";
    var sln = fso.CreateTextFile(slnFileName, true, false);
    // header
    sln.WriteLine(
        "Microsoft Visual Studio Solution File, Format Version " + 
        solution.formatVersion); //8.00");
    for (i in solution.projects)
    {
        var project = solution.projects[i];
        if (project.name) //really a project
        {
            if (true == genVCPROJs)
            {
                logStream.WriteLine("Saving project " + project.name);
                WScript.Echo("  Saving " + project.name + "...");
            }
            
            // project section header
            sln.Write(
                "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = ");
            sln.Write("\"" + project.name + "\"");
            var projectPath = project.folder.length > 0 ?
                outDir + project.folder + "\\" : outDir + "";
            var projectRelPath = project.folder.length > 0 ?
                removeLeadingSymbol(project.folder, "\\") + "\\" : "";
            var projectSrcPath = project.folder.length > 0 ?
                srcDir + project.folder + "\\" : srcDir + "";
            var projectFile = projectRelPath + 
                removeLeadingDot(project.name) + ".vcproj";
            sln.Write(", \"" + projectFile + "\"");
            sln.WriteLine(", \"" + project.id + "\"");
            
            // project dependencies
            sln.WriteLine(
                "\tProjectSection(ProjectDependencies) = postProject");
            for (j in project.dependencies)
            {
                var depProject = solution.projects[j];
                if (depProject.name)
                {
                    sln.WriteLine("\t\t" + depProject.id
                        + " = " + depProject.id);
                }
            }
            sln.WriteLine("\tEndProjectSection");
            // project section end
            sln.WriteLine("EndProject");
            
            if (true == genVCPROJs)
            {
                generateVCPROJ(project, srcDir, outDir, solution.version);
                logStream.WriteLine(project.name + " saved");
            }
        }
    }
    // Global section
    sln.WriteLine("Global");
    // solution configurations
    sln.WriteLine("\tGlobalSection(SolutionConfiguration) = preSolution");
    for (j in solution.configurations)
    {
        var slnCfg = solution.configurations[j];
        if (slnCfg.name)
        {           
            sln.WriteLine("\t\t" + slnCfg.name + " = " + slnCfg.name);
        }
    }
    sln.WriteLine("\tEndGlobalSection");
    // project configurations
    sln.WriteLine("\tGlobalSection(ProjectConfiguration) = postSolution");
    for (j in solution.configurations)
    {
        var slnCfg = solution.configurations[j];
        if (slnCfg.name)
        {
            for (k in slnCfg.projectConfigurations)
            {
                var projectCfg = slnCfg.projectConfigurations[k];
                if (projectCfg.projectName)
                {
                    var project = solution.projects[projectCfg.projectName];
                    if (!project)
                    {
                        WScript.StdErr.WriteLine(
                            "Generate: Fatal error: Project " 
                            + projectCfg.projectName
                            + " mentioned in the configuration " 
                            + slnCfg.name
                            + " is missing.");
                        WScript.Quit(3);
                    }
                    sln.WriteLine("\t\t" + project.id + "." + slnCfg.name
                        + ".ActiveCfg = "
                        + projectCfg.configuration + "|" 
                        + projectCfg.platform);
                    sln.WriteLine("\t\t" + project.id + "." + slnCfg.name
                        + ".Build.0 = "
                        + projectCfg.configuration + "|" 
                        + projectCfg.platform);
                }
            }
        }
    }
    sln.WriteLine("\tEndGlobalSection");

    // some unknown stuff
    sln.WriteLine("\tGlobalSection(ExtensibilityGlobals) = postSolution");
    sln.WriteLine("\tEndGlobalSection");
    sln.WriteLine("\tGlobalSection(ExtensibilityAddIns) = postSolution");
    sln.WriteLine("\tEndGlobalSection");

    sln.WriteLine("EndGlobal");
    sln.Close();
}

function writeSourcesVC(sourceFiles, vcproj, prefix, project, outDir)
{
    var excludeString = "";
    for (i in project.platforms)
    {
        var platform = project.platforms[i];
        if (platform.configurations)
        {
            for (j in platform.configurations)
            {
                var cfg = platform.configurations[j];
                if (cfg.name)
                {
                    excludeString += prefix + "\t<FileConfiguration\r\n"
                                + prefix + "\t\tName=\"" + cfg.name + "|"
                                + platform.name + "\"\r\n"
                                + prefix 
                                + "\t\tExcludedFromBuild=\"TRUE\">\r\n"
                                + prefix 
                                + "\t\t<Tool Name=\"VCCLCompilerTool\"/>\r\n"
                                + prefix + "\t</FileConfiguration>\r\n";
                }
            }
        }
    }

    for (i in sourceFiles)
    {
        var obj = sourceFiles[i];
        if (typeof(obj) == "boolean")
        {
            // file
            vcproj.WriteLine(prefix + "<File");
            vcproj.WriteLine(prefix + 
                "\tRelativePath=\"" + outDir + i + "\">");
            if (obj) //excluded from build
            {
                vcproj.Write(excludeString);
            }
            vcproj.WriteLine(prefix + "</File>");
        }
        else if (obj.filter)
        {
            //filter
            vcproj.WriteLine(prefix + "<Filter");
            vcproj.Write(prefix + "\tName=\"" + obj.name + "\"");
            if (obj.filter.length > 0)
            {
                vcproj.WriteLine("");
                vcproj.Write(prefix + "\tFilter=\""
                    + obj.filter + "\"");
            }
            if (obj.id.length > 0)
            {
                vcproj.WriteLine("");
                vcproj.Write(prefix + "\tUniqueIdentifier=\""
                    + obj.id + "\"");
            }
            vcproj.WriteLine(">");
            writeSourcesVC(obj.sourceFiles, vcproj, 
                prefix + "\t", project, outDir);
            vcproj.WriteLine(prefix + "</Filter>");
        }
    }
}

function generateSolutionVC(srcDir, outDir, logStream, genVCPROJs)
{
    generateSolutionVCImpl(this, srcDir, outDir, logStream, genVCPROJs);
}


function checkEnvironmentVC(logStream)
{
    try
    {
        var testGUID = createUUID();
        if (testGUID == "")
        {
            WScript.Echo("Error: uuidgen failed.");
            logStream.WriteLine("Error: uuidgen failed.");
            return false;
        }
    }
    catch(e)
    {
        WScript.Echo("Error: cannot find the uuidgen utility.");
        logStream.WriteLine("Error: cannot find the uuidgen utility.");
        return false;
    }
    
    return true;
}

////////////////////////////////////////////////////////////////////
// ICC 
function setProjectAdditionalDependencies(solution, outDir, prj)
{
    if (!prj || !solution)
        return;
        
    for (dep in prj.dependencies)
    {
        var depPrj = solution.projects[dep];
        if (! depPrj.name || ! depPrj.platforms)
            continue;

        if (depPrj.name != projectStdLibName && 
            depPrj.name != projectRwTestName)
                continue;
               
        for (j in prj.platforms)
        {
            var platform = prj.platforms[j];
            var depPlatform = depPrj.platforms[j];

            if (!platform.configurations || !depPlatform.configurations)
                continue;
                    
            for (k in platform.configurations)
            {
                var cfg = platform.configurations[k];
                var depCfg = depPlatform.configurations[k];
                if (!cfg.tools || !depCfg.tools)
                    continue;

                var lnkTool = cfg.tools[linkerToolName];
                if (! lnkTool)
                    continue;
                    
                var depLnkTool = depCfg.tools[linkerToolName];
                var depLibTool = depCfg.tools[librarianToolName];
                                  
                if (depLnkTool && depLnkTool.outputFile)
                {
                    var addlib = depLnkTool.outputFile;
                    var idxDll = addlib.lastIndexOf(".dll");
                    if (-1 != idxDll)
                        addlib = addlib.substr(0, idxDll);
                    addlib += ".lib";
                        
                    lnkTool.libraries.add(addlib);
                    lnkTool.libdirs.add(outDir + depPrj.folder + 
                        "\\" + depCfg.outputDir);
                }
                else if (depLibTool && depLibTool.outputFile)
                {                        
                    var addlib = depLibTool.outputFile;
                    lnkTool.libraries.add(addlib);
                    lnkTool.libdirs.add(outDir + depPrj.folder + 
                        "\\" + depCfg.outputDir);
                }
            }
        }
    }
}

function setAdditionalDependencies(solution, outDir)
{
    for (i in solution.projects)
    {
        var prj = solution.projects[i];
        if (! prj.platforms || ! prj.dependencies)
            continue;
            
        setProjectAdditionalDependencies(solution, outDir, prj);
    }
}

function convertSolution(outDir, solName, toICC)
{   
    var slnICFileName = outDir + "\\" + solName + ".sln";
    convertSolutionImpl(slnICFileName, toICC, null);
}

function generateSolutionICC(srcDir, outDir, logStream, genICPROJs)
{
    if (false == genICPROJs)
    {
        generateSolutionVCImpl(this, srcDir, outDir, logStream, genICPROJs);
        return;
    }
            
    setAdditionalDependencies(this, outDir);
    
    generateSolutionVCImpl(this, srcDir, outDir, logStream, genICPROJs);
    
    var slnICFileName = outDir + "\\" + this.name + ".sln";
    convertSolutionImpl(slnICFileName, true, logStream);
}

function checkEnvironmentICC(logStream)
{
    try
    {
        var testGUID = createUUID();
        if (testGUID == "")
        {
            WScript.Echo("Error: uuidgen failed.");
            logStream.WriteLine("Error: uuidgen failed.");
            return false;
        }
    }
    catch(e1)
    {
        WScript.Echo("Error: cannot find the uuidgen utility.");
        logStream.WriteLine("Error: cannot find the uuidgen utility.");
        return false;
    }
    
    try
    {
        var cmdICConvTest = iccConversionUtility + " /?";
        var ret = WshShell.Run(cmdICConvTest, 7, true);
    }
    catch(e2)
    {
        logStream.WriteLine(
            iccConversionUtility + " conversion utility not found");
        
        WScript.Echo("Error: " + 
            iccConversionUtility + " conversion utility not found");
            
        return false;
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////
function readSolutionGUIDs(solFileName)
{
    if (! fso.FileExists(solFileName))
        return null;
        
    var solFile = fso.OpenTextFile(solFileName);
    var solLine = solFile.ReadLine();
    if (solLine != 
            "Microsoft Visual Studio Solution File, Format Version 8.00" &&
        solLine != 
            "Microsoft Visual Studio Solution File, Format Version 9.00")
    {
        solFile.Close();
        return null;
    }
   
    var slnProjects = new Collection();
    while (! solFile.AtEndOfStream)
    {
        solLine = solFile.ReadLine();
        var idx = solLine.indexOf("Project");
        if (idx == 0)
        {
            var tempPrj = readProjectSolLine(solLine);
            slnProjects.add(tempPrj.name, tempPrj);
        }
    }
    
    solFile.Close();
    
    return slnProjects;
}

function readProjectSolLine(solLine)
{
    var eqSignIndex = solLine.indexOf("=");
    var prjInfoStr = solLine.substr(eqSignIndex + 2);
    var rgPrjInfo = prjInfoStr.split(", ");

    var prjName = rgPrjInfo[0].substr(1, rgPrjInfo[0].length - 2);
    var tempProject = new Project(prjName);
    tempProject.folder = rgPrjInfo[1].substr(1, rgPrjInfo[1].length - 2);
    tempProject.id = rgPrjInfo[2].substr(1, rgPrjInfo[2].length - 2);
    
    return tempProject;
}


var solutionVC71 = getSolution(vc71SolutionName);
solutionVC71.generateSolution = generateSolutionVC;
solutionVC71.checkEnvironment = checkEnvironmentVC;

var solutionVC80 = getSolution(vc80SolutionName);
solutionVC80.generateSolution = generateSolutionVC;
solutionVC80.checkEnvironment = checkEnvironmentVC;

var solutionICC90 = getSolution(icc90SolutionName);
solutionICC90.generateSolution = generateSolutionICC;
solutionICC90.checkEnvironment = checkEnvironmentICC;
